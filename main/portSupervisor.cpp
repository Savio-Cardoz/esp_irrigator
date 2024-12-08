#include <cstdio>
#include <iostream>
#include "portSupervisor.h"
#include "vault.h"
#include <stdio.h>

static const char *TAG = "PortSupervisor";

PortSupervisor::Port::Port(uint8_t x, uint16_t y, uint32_t a, uint32_t b, void (*f1)(portMap_t), void (*f2)(portMap_t))
{
    portNumber_u8 = x,
    flowLitresRequired_u16 = y;
    openPort = f1;
    closePort = f2;
    portState_e = CLOSED;
    interval = a;
    duration = b;
    b_portEnabled = true;
}

void PortSupervisor::Port::setSwitchTime(uint32_t swTime)
{
    switchTime_u32 = swTime;
    // printf("Port %u, Updated switchTime: %lu", portNumber_u8, switchTime_u32);
    //  ESP_LOGI(TAG, "Port %d, Updated switchTime: %d", portNumber_u8, switchTime_u32);
}

uint32_t PortSupervisor::Port::getSwitchTime()
{
    return switchTime_u32;
}

portState_t PortSupervisor::Port::portSwitchState()
{
    time_t now;
    time(&now);

    if (portState_e == CLOSED)
    {
        portState_e = OPEN;
        openPort(PORT_1);
        switchTime_u32 = now + duration;
    }
    else if (portState_e == OPEN)
    {
        portState_e = CLOSED;
        closePort(PORT_1);
        switchTime_u32 = now + interval;
    }
    return portState_e;
}

void PortSupervisor::Supervisor::addPort(uint8_t portNum, uint16_t reqFlow, uint32_t interval, uint32_t duration, void (*enPort)(portMap_t), void (*disPort)(portMap_t))
{
    time_t now;
    uint32_t switchTime;
    time(&now);
    // ESP_LOGI(TAG, "Port %d added to List at time: %d", portNum, (uint32_t)now);
    // printf("Port %d added to List at time: %lu", portNum, (uint32_t)now);
    switchTime = (uint32_t)now + interval;
    portList.push_back(Port{portNum, reqFlow, interval, duration, enPort, disPort});
    portList.back().setSwitchTime(switchTime);
}

portState_t PortSupervisor::Supervisor::runPortCheck()
{
    time_t now;
    portState_t result = portState_t::ERROR;

    if (!portList.empty())
    {
        time(&now);
        for (auto &port : portList)
        {
            printf("Time now: %u, switch at: %u", static_cast<unsigned int>(now), static_cast<unsigned int>(port.getSwitchTime()));
            if ((port.b_portEnabled == true) && (port.getSwitchTime() <= (uint32_t)now))
            {
                result = port.portSwitchState();
            }
        }
    }
    return result;
}

uint32_t PortSupervisor::Supervisor::getNextPortTriggerTime()
{
    uint32_t nextTriggerTime = 0xFFFFFFFF;
    uint32_t portSwitchTime = 0;
    for (auto port : portList)
    {
        portSwitchTime = port.getSwitchTime();
        if (portSwitchTime < nextTriggerTime)
        {
            nextTriggerTime = portSwitchTime;
        }
    }
    return nextTriggerTime;
}