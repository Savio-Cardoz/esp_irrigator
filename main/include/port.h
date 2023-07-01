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

struct st_portConfig {
    uint8_t     u8_portNumber;
    uint32_t    u32_switchTime;
    portState_t e_portState;
    uint16_t    u16_flowLitresRequired;
    uint16_t    u16_flowLitresCurrent;
    uint32_t    u32_interval;
    uint32_t    u32_duration;
    // struct st_portConfig* ptr_nextConfig;
};

void enableSolenoid(portMap_t num);

void disableSolenoid(portMap_t num);