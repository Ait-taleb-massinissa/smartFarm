#include "log_buffer.h"
#include "esp_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define LOG_BUFFER_SIZE 4096

static char log_buffer[LOG_BUFFER_SIZE];
static size_t log_index = 0;

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

    return vprintf(fmt, args); // keep UART logs
}

void log_buffer_init(void)
{
    esp_log_set_vprintf(http_log_vprintf);
}

const char *log_buffer_get(size_t *len)
{
    *len = log_index;
    return log_buffer;
}
