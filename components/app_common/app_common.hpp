#ifndef APP_COMMON_HPP
#define APP_COMMON_HPP

#include "non_volatile_storage.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

enum class appWifiState
{
    CONNECTED,
    DISCONNECTED,
    CONNECTING,
    DISCONNECTING
};

struct configData
{
    unsigned int interval;
    unsigned int duration;
    unsigned int quantity;
    unsigned long long nextAlarmTime;
    unsigned long long lastAlarmTime;
};

class application
{
private:
    static appWifiState _wifiState;
    static application *_instance;

    // Private constructor to prevent direct instantiation
    application()
    {
        _wifiState = appWifiState::DISCONNECTED;
    }

public:
    // Delete copy constructor and assignment operator
    application(const application &) = delete;
    application &operator=(const application &) = delete;

    ~application() = default;

    static application &getInstance()
    {
        static application instance;
        return instance;
    }

    void setWifiState(appWifiState state)
    {
        _wifiState = state;
    }

    appWifiState getWifiState()
    {
        return _wifiState;
    }

    void readConfig(configData &portConfig)
    {
        esp_err_t err = nvs_read_chunk("config", &portConfig, sizeof(portConfig));
        if (err != ESP_OK)
        {
            ESP_LOGE("app_common", "Failed to read config: %s", esp_err_to_name(err));
        }
    }

    void writeConfig(const configData &portConfig)
    {
        nvs_write_chunk("config", &portConfig, sizeof(portConfig));
    }
};

#endif // APP_COMMON_HPP