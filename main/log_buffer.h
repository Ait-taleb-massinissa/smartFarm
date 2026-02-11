#pragma once
#include <stddef.h>

void log_buffer_init(void);
const char *log_buffer_get(size_t *len);
