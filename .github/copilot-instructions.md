Project: smartFarm — AI assistant instructions

Purpose
- Help AI agents be immediately productive in this ESP-IDF-based embedded C project.

Big picture (what matters)
- Platform: ESP-IDF (FreeRTOS + ESP32 network stack). Entry point: `main/main.c`.
- Modules: each sensor/actuator lives in `main/*.c` with a corresponding header. Examples: `gas_sensor.c`/`gas_sensor.h`, `TempSensor.c`/`TempSensor.h`.
- Runtime model: most modules expose an `*_init()` (returns `esp_err_t`) and `*_start()` (creates a FreeRTOS task). See `gas_sensor_init()` / `gas_sensor_start()` in `main/gas_sensor.c`.
- Inter-task comms: queues are created in `main/Queues.c` and used to pass sensor values; producers push to queues, the webserver (`main/Sever.c`) pulls them.
- Web UI: a small HTTP server implemented in `main/Sever.c` serves a dashboard and `/data` JSON endpoint.

Build & common developer workflows
- This repository is an ESP-IDF project. Build/test steps (Windows PowerShell with ESP-IDF environment loaded):

```powershell
# Run ESP-IDF environment setup (path depends on your ESP-IDF install)
# Example: & C:\esp\esp-idf\export.ps1

# Build
idf.py build

# Flash and monitor (replace COM3 with device port)
idf.py -p COM3 flash monitor
```

- If unsure of the target, run `idf.py set-target esp32` before `build` (project CMake suggests ESP32).
- CMake files: modify `main/CMakeLists.txt` to add/remove source files — the component lists explicit SRCS.

Project-specific conventions & patterns
- Source layout: put `.c` and `.h` for a component in `main/`. Add the `.c` filename to `main/CMakeLists.txt`.
- Naming: prefer `<module>_init()` and `<module>_start()` for initialization + task start. Task functions often named `get_*` or `*_task` and use `xTaskCreate`.
- Queues: use the queue handles from `Queues.h` (e.g., `TempQueue`, `LightQueue`) and `xQueueSend`/`xQueueReceive` for cross-task data.
- Logging: use `ESP_LOGI/E/...` for integrated logging; some files also use `printf` — prefer `ESP_LOG*` when adding or adjusting logs.

Integration points & external dependencies
- ESP-IDF components: `esp_wifi`, `esp_http_server`, `nvs_flash`, `esp_adc_cal`, FreeRTOS primitives.
- Hardware: ADC channels (sensor reads) and GPIO-driven actuators (relays, servos, stepper). Check sensor headers for pin/channel constants (e.g., `GAS_SENSOR_ADC_CHANNEL`).

Known inconsistencies (be cautious)
- File/name mismatches: `main/Sever.c` (server implementation) vs `main/Server.h` — header and implementation signatures differ (e.g., `read_sensor()` prototype mismatch). Review before refactoring.
- Duplicate entries and inconsistent casing in `main/CMakeLists.txt` (files repeated, mixed `CamelCase` and `snake_case` filenames). When adding files, keep existing style but prefer consistent names.

How to add a new sensor module (concrete recipe)
1. Add `MySensor.c` and `MySensor.h` in `main/`.
2. Export `esp_err_t my_sensor_init(void);` and `void my_sensor_start(void);` from the header.
3. Implement ADC/GPIO setup in `*_init()` and create a FreeRTOS task in `*_start()` that sends readings to an existing queue or add a new queue in `Queues.c`.
4. Append `MySensor.c` to the `SRCS` list in `main/CMakeLists.txt`.
5. Update `main/main.c` to call `my_sensor_init()` and `my_sensor_start()` (respecting init order: peripheral init before task start).

Debugging tips specific to this repo
- Use `idf.py monitor` to view `ESP_LOG*` output and `printf` statements.
- If webserver routes return bad JSON, check `main/Sever.c` `read_sensor()` and queue usage — the server blocks on `xQueueReceive`.
- If a module doesn't start, check `CMakeLists.txt` for missing SRCS or symbol mismatches between header and implementation.

When editing files, be conservative: fix obvious header/signature mismatches (e.g., `Server.h` vs `Sever.c`) but do not rename files without syncing `CMakeLists.txt` and includes.

Files to inspect first (quick jump)
- `main/main.c` — startup and commented examples of tasks and wifi/webserver initialization
- `main/CMakeLists.txt` — component SRCS list (must be updated for new sources)
- `main/Queues.c`, `main/Sever.c`, `main/gas_sensor.c` — examples of queues, webserver, and sensor task patterns

If anything here is unclear or you want the instructions tailored (for example to a single developer style, or to normalize naming), tell me what to adjust and I'll update this file.
