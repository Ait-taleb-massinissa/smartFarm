#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "lwip/inet.h"
#include "TempSensor.h"
#include "Queues.h"

static const char *TAG = "WEB_SENSOR";

typedef struct
{
    float temp;
    int light;
    char *water;
    char *moisture;
} SensorData;

const char index_html[] =
    "<!DOCTYPE html>"
    "<html lang='en'>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Smart Farm Dashboard</title>"
    "<style>"
    "body {"
    "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
    "  background: #e0f7fa;"
    "  margin: 0;"
    "  padding: 0;"
    "}"
    "h1 {"
    "  text-align: center;"
    "  padding: 20px;"
    "  color: #00796b;"
    "}"
    ".dashboard {"
    "  display: flex;"
    "  flex-wrap: wrap;"
    "  justify-content: center;"
    "  gap: 20px;"
    "  padding: 20px;"
    "}"
    ".card {"
    "  background: #ffffff;"
    "  border-radius: 12px;"
    "  box-shadow: 0 4px 6px rgba(0,0,0,0.1);"
    "  padding: 20px;"
    "  width: 180px;"
    "  text-align: center;"
    "  transition: transform 0.2s;"
    "}"
    ".card:hover {"
    "  transform: translateY(-5px);"
    "}"
    ".card h2 {"
    "  margin: 0;"
    "  font-size: 18px;"
    "  color: #004d40;"
    "}"
    ".value {"
    "  font-size: 32px;"
    "  font-weight: bold;"
    "  margin-top: 10px;"
    "  color: #00796b;"
    "}"
    "button {"
    "  margin-top: 15px;"
    "  padding: 10px 20px;"
    "  font-size: 16px;"
    "  border: none;"
    "  border-radius: 8px;"
    "  cursor: pointer;"
    "  background: #00796b;"
    "  color: white;"
    "}"
    "button.off {"
    "  background: #c62828;"
    "}"

    "</style>"
    "</head>"
    "<body>"
    "<h1>Smart Farm Dashboard</h1>"
    "<div class='dashboard'>"
    "  <div class='card'>"
    "    <h2>Temperature</h2>"
    "    <div class='value' id='temp'>-°C</div>"
    "  </div>"
    "  <div class='card'>"
    "    <h2>Light</h2>"
    "    <div class='value' id='light'>-lx</div>"
    "  </div>"
    "  <div class='card'>"
    "    <h2>Moisture</h2>"
    "    <div class='value' id='moisture'>-</div>"
    "  </div>"
    "  <div class='card'>"
    "    <h2>Water</h2>"
    "    <div class='value' id='water'>-</div>"
    "  </div>"
    "  <div class='card'>"
    "    <h2>Relay</h2>"
    "    <button id='relayBtn' class='off' onclick='toggleRelay()'>OFF</button>"
    "  </div>"
    "</div>"
    "<script>"
    "async function updateSensors() {"
    "  try {"
    "    const res = await fetch('/data');"
    "    const d = await res.json();"
    "    document.getElementById('temp').innerText = d.temperature.toFixed(2) + '°C';"
    "    document.getElementById('light').innerText = d.light.toFixed(0) + ' lx';"
    "    document.getElementById('moisture').innerText = String(d.moisture);"
    "    document.getElementById('water').innerText = String(d.water);"
    "  } catch(err) {"
    "    console.log('Error fetching sensor data', err);"
    "  }"
    "}"
    "let relayOn = false;"

    "async function toggleRelay() {"
    "  relayOn = !relayOn;"
    "  const state = relayOn ? 'on' : 'off';"
    "  await fetch('/relay?state=' + state);"
    "  const btn = document.getElementById('relayBtn');"
    "  btn.innerText = relayOn ? 'ON' : 'OFF';"
    "  btn.className = relayOn ? '' : 'off';"
    "}"
    
    "setInterval(updateSensors, 1000);"
    "</script>"
    "</body>"
    "</html>";

SensorData read_sensor(void)
{

    float temperature;
    float light;
    char *water;
    char *moisture;
    xQueueReceive(TempQueue, &temperature, portMAX_DELAY);
    xQueueReceive(LightQueue, &light, portMAX_DELAY);
    xQueueReceive(WaterQueue, &water, portMAX_DELAY);
    xQueueReceive(MoistureQueue, &moisture, portMAX_DELAY);
    SensorData data;
    data.temp = temperature;
    data.light = light;
    data.water = water;
    data.moisture = moisture;
    return data;
}

esp_err_t sensor_get_handler(httpd_req_t *req)
{
    char response[100];
    SensorData sensor = read_sensor();

    snprintf(response, sizeof(response),
             "{ \"temperature\": %.2f, \"light\": %d , \"water\": \"%s\", \"moisture\": \"%s\"}", sensor.temp, sensor.light, sensor.water, sensor.moisture);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_start(&server, &config);

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};

    httpd_register_uri_handler(server, &index_uri);

    httpd_uri_t data_uri = {
        .uri = "/data",
        .method = HTTP_GET,
        .handler = sensor_get_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server, &data_uri);
}
