#include "vault.h"
#include <chrono>
#include <iomanip>
#include <ctime>

#define VAULT_INITIALIZED "Initialization_Code"
#define NUMBER_OF_PORTS "numberOfPorts"

const char *TAG = "Vault";

Vault::Result Vault::getVaultData(PortSupervisor::Supervisor &portManager)
{
    esp_err_t err;
    uint8_t portListSize = 0;
    // uint32_t initialzationKey = 0;

    // err = nvs_read_u32(VAULT_INITIALIZED, &initialzationKey);
    // if( (err != ESP_OK) || (initialzationKey != 0xAAAAAAAA) ) {
    //     return Vault::Result::ERR_VAULT_NOT_INITIALIZED;
    // }

    err = nvs_read_u8(NUMBER_OF_PORTS, &portListSize);
    if (err != ESP_OK)
    {
        return Vault::Result::ERR_WRITE_NUMBER_OF_PORTS;
    }
    ESP_LOGI(TAG, "Number of ports in storage: %d", portListSize);

    st_portConfig portConfig[portListSize];

    err = nvs_read_chunk(VAULT_CONFIG_KEY, portConfig, sizeof(portConfig));
    if (err != ESP_OK)
    {
        return Vault::Result::ERR_READ;
    }

    for (uint8_t i = 0; i < portListSize; i++)
    {
        portManager.addPort(portConfig[i].u8_portNumber,
                            // portConfig[i].u16_flowLitresRequired,
                            3, // TODO: 3 ltrs test only
                            portConfig[i].u32_interval,
                            // portConfig[i].u32_duration,
                            300,
                            enableSolenoid,
                            disableSolenoid);

        portManager.portList.back().setSwitchTime(portConfig[i].u32_switchTime);
        portManager.portList.back().portState_e = portConfig[i].e_portState;
        portManager.portList.back().flowLitresCurrent_u16 = portConfig[i].u16_flowLitresCurrent;

        time_t rawtime = static_cast<unsigned int>(portConfig[i].u32_switchTime);
        struct tm ts;
        char buf[80];

        // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
        ts = *localtime(&rawtime);
        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        printf("%s\n", buf);

        printf("Added port: %d, Interval: %u, Duration: %u switch at: %u\n",
               portConfig[i].u8_portNumber,
               static_cast<unsigned int>(portConfig[i].u32_interval),
               static_cast<unsigned int>(portConfig[i].u32_duration),
               static_cast<unsigned int>(portConfig[i].u32_switchTime));
    }

    return Vault::Result::OK;
}

Vault::Result Vault::setVaultData(const PortSupervisor::Supervisor &portManager)
{
    uint8_t portListSize = portManager.portList.size();
    st_portConfig config[portListSize];
    uint8_t portCounter = 0;
    uint32_t initializationKey = 0xAAAAAAAA;

    ESP_LOGI(TAG, "Saving port list size: %d", portListSize);
    nvs_write_u8(NUMBER_OF_PORTS, portListSize);

    for (auto &port : portManager.portList)
    {
        config[portCounter].e_portState = port.portState_e;
        config[portCounter].u8_portNumber = port.portNumber_u8;
        config[portCounter].u32_switchTime = port.switchTime_u32;
        config[portCounter].u32_interval = port.interval;
        config[portCounter].u32_duration = port.duration;
        config[portCounter].u16_flowLitresRequired = port.flowLitresRequired_u16;
        config[portCounter].u16_flowLitresCurrent = port.flowLitresCurrent_u16;
        portCounter++;
    }

    nvs_write_chunk(VAULT_CONFIG_KEY, config, sizeof(config));

    nvs_write_u32(VAULT_INITIALIZED, initializationKey);

    return Vault::Result::OK;
}