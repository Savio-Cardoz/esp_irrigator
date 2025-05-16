#include <cstdio>
#include <iostream>
#include "portSupervisor.h"
#include "vault.h"
#include <stdio.h>
#include <chrono>

#include "cJSON.h"

static uint32_t getNowTime()
{
    auto timeNow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock().now().time_since_epoch()).count();
    return static_cast<uint32_t>(timeNow);
}

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
    auto timeNow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock().now().time_since_epoch()).count();

    if (portState_e == CLOSED)
    {
        portState_e = OPEN;
        flowLitresCurrent_u16 = 0;
        openPort(PORT_1);
        // switchTime_u32 = timeNow + duration;     // Disabled as shifting to using quantity as the OFF condition
        // time_t rawtime = static_cast<unsigned int>(switchTime_u32);
        // struct tm ts;
        // char buf[80];

        // ts = *localtime(&rawtime);
        // strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        // printf("Next switch at: %u | %s\n", static_cast<unsigned int>(switchTime_u32), buf);
    }
    else if (portState_e == OPEN)
    {
        portState_e = CLOSED;
        closePort(PORT_1);
        // switchTime_u32 = timeNow + interval;     // Disabled as shifting to using quantity as the OFF condition. else the ON time was drifting by the duration time
        switchTime_u32 = switchTime_u32 + interval;
        time_t rawtime = static_cast<unsigned int>(switchTime_u32);
        struct tm ts;
        char buf[80];

        // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
        ts = *localtime(&rawtime);
        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        printf("Next switch at: %u | %s\n", static_cast<unsigned int>(switchTime_u32), buf);
    }
    return portState_e;
}

void PortSupervisor::Supervisor::addPort(uint8_t portNum, uint16_t reqFlow, uint32_t interval, uint32_t duration, void (*enPort)(portMap_t), void (*disPort)(portMap_t))
{
    time_t now;
    uint32_t switchTime;
    time(&now);
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
            printf("swtime: %lu timenow: %lu\n", port.getSwitchTime(), (uint32_t)now);
            if ((port.b_portEnabled == true) && (portState_t::CLOSED == port.portState_e) && (port.getSwitchTime() <= (uint32_t)now))
            {
                result = port.portSwitchState();
            }
            if ((portState_t::OPEN == port.portState_e) && (port.flowLitresCurrent_u16 >= port.flowLitresRequired_u16))
            {
                result = port.portSwitchState();
            }
        }
    }
    return result;
}

uint32_t
PortSupervisor::Supervisor::getNextPortTriggerTime()
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

void PortSupervisor::Supervisor::updatePortConfig(const char *buffer)
{
    cJSON *root = cJSON_Parse(buffer);
    if (nullptr != root)
    {
        portList[0].interval = cJSON_GetObjectItem(root, "interval")->valueint;
        portList[0].duration = cJSON_GetObjectItem(root, "duration")->valueint;
        portList[0].flowLitresRequired_u16 = cJSON_GetObjectItem(root, "quantity")->valueint;

        printf("Updated config: Interval: %u, Duration: %u, Qty: %d\r\n", static_cast<unsigned int>(portList[0].interval), static_cast<unsigned int>(portList[0].duration), portList[0].flowLitresRequired_u16);
        Vault::setVaultData(*this);
    }
}

void PortSupervisor::Supervisor::updateSwitchTimes()
{
    auto timeNow = static_cast<uint32_t>(getNowTime());

    for (auto &port : this->portList)
    {
        if (portState_t::OPEN == port.portState_e)
        {
            port.portSwitchState();
        }

        auto portSwitchTime = port.getSwitchTime();

        printf("\nportSwitchTime: %lu timenow: %lu\n", portSwitchTime, timeNow);
        while (portSwitchTime < timeNow)
        {
            portSwitchTime = portSwitchTime + port.interval;
        }
        printf("portSwitchTime updated: %lu \n", portSwitchTime);
        port.setSwitchTime(portSwitchTime);
    }
}