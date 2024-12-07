#include <string>
#include <iostream>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "app_wifi.h"
#include "dht20.h"
#include "display.h"

#include <time.h>
#include <sys/time.h>
#include "soc/rtc.h"

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_sleep.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "http_server.h"

namespace Irrigator
{
    class System
    {
    private:
        static uint8_t count;
        uint32_t sleepEnterTime;
        http_server _server;

    public:
        void initialize();
        void checkWifiState();
        void updateEnvInfo();

        /**
         * Reference the element at the front of the portlist to get the switchTime attribute
         * to set the duration of sleep from current tiime
         */
        void deepSleep(uint32_t sleepDuration);
        void setSleepEnterTime(uint32_t);
        uint32_t getSleepEnterTime();

        WIFI::Wifi::state_e wifiState{WIFI::Wifi::state_e::NOT_INITIALIZED};
        WIFI::Wifi Wifi;

        DHT20 env_sensor;
        Display display;
    };
} // namespace Irrigator
