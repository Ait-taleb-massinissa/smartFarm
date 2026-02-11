#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_server.h"

#include "TempSensor.h"
#include "Queues.h"
#include "Relay.h"
#include "Servo.h"

static const char *TAG = "WEB_SENSOR";

/* ============================================================
   GLOBAL SENSOR CACHE (NON-BLOCKING HTTP)
   ============================================================ */
typedef struct
{
    float temp;
    float light;
    int water;
    char moisture[16];
    uint16_t gas;
} SensorData;
SensorData g_sensor_data = {0};
/* ============================================================
   LOG BUFFER (ESP_LOG → HTTP)
   ============================================================ */
#define LOG_BUFFER_SIZE 4096

static char log_buffer[LOG_BUFFER_SIZE];
static size_t log_index = 0;

/* ============================================================
   ESP_LOG HOOK
   ============================================================ */
static int http_log_vprintf(const char *fmt, va_list args)
{
    char temp[256];
    int len = vsnprintf(temp, sizeof(temp), fmt, args);

    if (len > 0)
    {
        if (log_index + len >= LOG_BUFFER_SIZE)
        {
            log_index = 0; // overwrite old logs
        }
        memcpy(&log_buffer[log_index], temp, len);
        log_index += len;
    }

    // Keep serial output
    return vprintf(fmt, args);
}

/* ============================================================
   DASHBOARD HTML
   ============================================================ */
const char index_html[] =
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>Smart Farm</title>"
    "<style>"
    "body{font-family:Arial;background:#e0f7fa;text-align:center}"
    ".dashboard{display:flex;flex-wrap:wrap;justify-content:center;gap:15px}"
    ".card{background:white;padding:15px;border-radius:10px;width:170px}"
    ".value{font-size:26px;color:#00796b}"
    "button{padding:10px;border:none;border-radius:6px;background:#00796b;color:white}"
    "button.off{background:#c62828}"
    "</style></head><body>"
    "<h1>Smart Farm Dashboard</h1>"
    "<div class='dashboard'>"

    "<div class='card'><h3>Temperature</h3><div id='temp' class='value'>-</div></div>"
    "<div class='card'><h3>Light</h3><div id='light' class='value'>-</div></div>"
    "<div class='card'><h3>Moisture</h3><div id='moisture' class='value'>-</div></div>"
    "<div class='card'><h3>Water</h3><div id='water' class='value'>-</div></div>"
    "<div class='card'><h3>Gas</h3><div id='gas' class='value'>-</div></div>"

    "<div class='card'><h3>Fan</h3><button id='fanBtn' class='off' onclick='toggleFan()'>OFF</button></div>"
    "<div class='card'><h3>Electrovan</h3><button id='valveBtn' class='off' onclick='toggleValve()'>OFF</button></div>"
    "<div class='card'><h3>Window</h3><button id='windowBtn' class='off' onclick='toggleWindow()'>CLOSED</button></div>"
    "<div class='card'><h3>Door</h3>"
    "<button id='doorBtn' class='off' onclick='toggleDoor()'>CLOSED</button>"
    "</div>"

    "<div class='card'><h3>LED</h3><button id='ledBtn' class='off' onclick='toggleLed()'>OFF</button></div>"

    "</div>"

    "<script>"
    "let fanOn=false,valveOn=false,windowOpen=false,ledOn=false,doorOpen=false;"

    "async function updateSensors(){"
    "let r=await fetch('/data');"
    "let d=await r.json();"
    "temp.innerText=d.temperature.toFixed(1)+'C';"
    "light.innerText=d.light+' lx';"
    "moisture.innerText=d.moisture;"
    "water.innerText=d.water;"
    "gas.innerText=d.gas;"
    "}"
    "async function toggleLed(){"
    "ledOn=!ledOn;"
    "await fetch('/led?state='+(ledOn?'on':'off'));"
    "ledBtn.innerText=ledOn?'ON':'OFF';"
    "}"

    "async function toggleFan(){fanOn=!fanOn;"
    "await fetch('/fan?state='+(fanOn?'on':'off'));"
    "fanBtn.innerText=fanOn?'ON':'OFF';}"

    "async function toggleValve(){valveOn=!valveOn;"
    "await fetch('/valve?state='+(valveOn?'on':'off'));"
    "valveBtn.innerText=valveOn?'ON':'OFF';}"

    "async function toggleWindow(){windowOpen=!windowOpen;"
    "await fetch('/window?state='+(windowOpen?'open':'close'));"
    "windowBtn.innerText=windowOpen?'OPEN':'CLOSED';}"

    "async function toggleDoor(){"
    "doorOpen=!doorOpen;"
    "await fetch('/door?state='+(doorOpen?'open':'close'));"
    "doorBtn.innerText=doorOpen?'OPEN':'CLOSED';"
    "}"

    "setInterval(updateSensors,1000);"
    "</script></body></html>";

esp_err_t logs_page_handler(httpd_req_t *req)
{
    const char *html =
        "<!DOCTYPE html><html><head>"
        "<title>ESP32 Logs</title>"
        "<style>"
        "body{background:black;color:#00ff00;font-family:monospace;white-space:pre}"
        "</style></head><body>"
        "<pre id='log'></pre>"
        "<script>"
        "async function refresh(){"
        "let r=await fetch('/logs');"
        "log.innerText=await r.text();"
        "}"
        "setInterval(refresh,1000);"
        "refresh();"
        "</script>"
        "</body></html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* ============================================================
   SENSOR CACHE TASK
   ============================================================ */
void sensor_cache_task(void *pv)
{
    float temp, light;
    uint16_t gas;
    int water;
    char *moisture;

    while (1)
    {
        // Peek at values instead of removing them
        if (xQueuePeek(TempQueue, &temp, portMAX_DELAY))
            g_sensor_data.temp = temp;

        if (xQueuePeek(LightQueue, &light, portMAX_DELAY))
            g_sensor_data.light = light;

        if (xQueuePeek(WaterQueue, &water, portMAX_DELAY))
            g_sensor_data.water = water;

        if (xQueuePeek(MoistureQueue, &moisture, portMAX_DELAY))

            strcpy(g_sensor_data.moisture, moisture);

        if (xQueuePeek(GasQueue, &gas, portMAX_DELAY))
            g_sensor_data.gas = gas;

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/* ============================================================
   HTTP HANDLERS
   ============================================================ */
esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t data_handler(httpd_req_t *req)
{
    char resp[128];

    snprintf(resp, sizeof(resp),
             "{\"temperature\":%.2f,\"light\":%.2f,\"water\":%d,\"moisture\":\"%s\",\"gas\":%d}",
             g_sensor_data.temp, g_sensor_data.light, g_sensor_data.water, g_sensor_data.moisture, g_sensor_data.gas);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t logs_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
    log_buffer[log_index] = '\0';
    httpd_resp_sendstr(req, log_buffer);
    return ESP_OK;
}

/* ==== CONTROL HELPERS ==== */
void parse_state(httpd_req_t *req, char *state, size_t len)
{
    char query[32];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
        httpd_query_key_value(query, "state", state, len);
}

/* ==== LED RELAY GPIO25 ==== */
esp_err_t led_handler(httpd_req_t *req)
{
    char state[8] = {0};
    parse_state(req, state, sizeof(state));

    if (strcmp(state, "on") == 0)
        relay_on(GPIO_NUM_25);
    else
        relay_off(GPIO_NUM_25);

    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ==== FAN ==== */
esp_err_t fan_handler(httpd_req_t *req)
{
    char state[8] = {0};
    parse_state(req, state, sizeof(state));

    if (strcmp(state, "on") == 0)
        relay_on(GPIO_NUM_23);
    else
        relay_off(GPIO_NUM_23);

    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ==== VALVE ==== */
esp_err_t valve_handler(httpd_req_t *req)
{
    char state[8] = {0};
    parse_state(req, state, sizeof(state));

    if (strcmp(state, "on") == 0)
        relay_on(GPIO_NUM_22);
    else
        relay_off(GPIO_NUM_22);

    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ==== WINDOW SERVO ==== */
esp_err_t window_handler(httpd_req_t *req)
{
    char state[8] = {0};
    parse_state(req, state, sizeof(state));

    if (strcmp(state, "open") == 0)
    {
        servo_ouvrir(GPIO_NUM_18);
        servo_ouvrir(GPIO_NUM_19);
        // servo_ouvrir(GPIO_NUM_21);
    }
    else
    {
        servo_fermer(GPIO_NUM_18);
        servo_fermer(GPIO_NUM_19);
        // servo_fermer(GPIO_NUM_21);
    }

    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ==== DOOR SERVO GPIO21 ==== */
esp_err_t door_handler(httpd_req_t *req)
{
    char state[8] = {0};
    parse_state(req, state, sizeof(state));

    if (strcmp(state, "open") == 0)
    {
        servo_ouvrir(GPIO_NUM_21);
    }
    else
    {
        servo_fermer(GPIO_NUM_21);
    }

    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ============================================================
   START WEB SERVER
   ============================================================ */
void start_webserver(void)
{
    // 🔥 Redirect ESP_LOG to HTTP
    esp_log_set_vprintf(http_log_vprintf);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    httpd_start(&server, &config);

    httpd_uri_t index_uri = {.uri = "/", .method = HTTP_GET, .handler = index_handler};
    httpd_uri_t data_uri = {.uri = "/data", .method = HTTP_GET, .handler = data_handler};
    httpd_uri_t logs_uri = {
        .uri = "/logs",
        .method = HTTP_GET,
        .handler = logs_handler};

    httpd_register_uri_handler(server, &logs_uri);

    httpd_uri_t logs_page_uri = {
        .uri = "/logs.html",
        .method = HTTP_GET,
        .handler = logs_page_handler};

    httpd_uri_t led_uri = {
        .uri = "/led",
        .method = HTTP_GET,
        .handler = led_handler};

    httpd_uri_t fan_uri = {.uri = "/fan", .method = HTTP_GET, .handler = fan_handler};
    httpd_uri_t valve_uri = {.uri = "/valve", .method = HTTP_GET, .handler = valve_handler};
    httpd_uri_t window_uri = {.uri = "/window", .method = HTTP_GET, .handler = window_handler};
    httpd_uri_t door_uri = {
        .uri = "/door",
        .method = HTTP_GET,
        .handler = door_handler};

    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &data_uri);
    httpd_register_uri_handler(server, &logs_page_uri);
    httpd_register_uri_handler(server, &fan_uri);
    httpd_register_uri_handler(server, &valve_uri);
    httpd_register_uri_handler(server, &window_uri);
    httpd_register_uri_handler(server, &led_uri);
    httpd_register_uri_handler(server, &door_uri);

    xTaskCreate(sensor_cache_task, "sensor_cache", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Webserver started");
}
