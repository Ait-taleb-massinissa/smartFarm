#ifndef SERVER_H
#define SERVER_H
#include "esp_http_server.h"

esp_err_t sensor_get_handler(httpd_req_t *req);

void start_webserver(void);
float read_sensor(void);
#endif // SERVER_H