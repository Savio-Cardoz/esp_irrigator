#include "app_wifi.h"
#include "dht20.h"
#include "display.h"

namespace Irrigator
{
    class System
    {
    private:
        static uint8_t count;
        uint32_t sleepEnterTime;

    public:
        void initialize();
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
