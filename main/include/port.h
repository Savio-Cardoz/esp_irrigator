#pragma once

#include "esp_log.h"

typedef enum {
    PORT_1,
    PORT_2,
    PORT_3,
    PORT_4
} portMap_t;

typedef enum {
    CLOSED = 0,
    OPEN,
    ERROR
}portState_t;
