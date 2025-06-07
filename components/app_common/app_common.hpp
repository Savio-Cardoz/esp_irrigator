#ifndef APP_COMMON_HPP
#define APP_COMMON_HPP

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
};

#endif // APP_COMMON_HPP