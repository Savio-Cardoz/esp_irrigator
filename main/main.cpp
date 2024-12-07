#include "main.h"
#include "portSupervisor.h"
#include "vault.h"
#include "sntp.h"
#include "mqtt.h"

#include <iostream>
#include "dht22.hpp"

#include "cJSON.h"
#include "sampleComponent.hpp"

/***** create a sample configuration *************/
// struct config_st config = { .portnumber = 1, .startTime = 1670000, .stopTime = 1671000, .flowQuantity = 200 };
// struct config_st config = { 1, 1000, 1000, 100 };       // This initialization worked ??
/************************************************/

PortSupervisor::Supervisor portMan;
PortSupervisor::Result result = PortSupervisor::Result::ERR;
Vault::Result res = Vault::Result::ERR;

time_t now;
static struct timeval nowTime = {
    .tv_sec = 1683994547,
};
char strftime_buf[64];
struct tm timeinfo;
int64_t *ptr_timeSeconds = NULL;

void Irrigator::System::initialize()
{
    /** Setup Wifi  */
    Wifi.SetCredentials("Cardoz", "8828385089");
    Wifi.Init();

    // Setup display
    this->display.displayOutline();

    /** Setup Humidity and Temperature sensor   */
    I2C i2c_comm;
    i2c_comm.init(I2C_NUM_0, 5, 4, 100000);
    this->env_sensor.init(i2c_comm);

    _server.init();
}

// void Irrigator::System::checkWifiState()
// {
//     wifiState = WIFI::Wifi::GetState();

//     switch (wifiState)
//     {
//     case WIFI::Wifi::state_e::READY_TO_CONNECT:
//         std::cout << "Wifi Status: READY_TO_CONNECT\n";
//         Wifi.Begin();
//         break;
//     case WIFI::Wifi::state_e::DISCONNECTED:
//         std::cout << "Wifi Status: DISCONNECTED\n";
//         Wifi.Begin();
//         break;
//     case WIFI::Wifi::state_e::CONNECTING:
//         std::cout << "Wifi Status: CONNECTING\n";
//         break;
//     case WIFI::Wifi::state_e::WAITING_FOR_IP:
//         std::cout << "Wifi Status: WAITING_FOR_IP\n";
//         break;
//     case WIFI::Wifi::state_e::ERROR:
//         std::cout << "Wifi Status: ERROR\n";
//         break;
//     case WIFI::Wifi::state_e::CONNECTED:
//         std::cout << "Wifi Status: CONNECTED\n";
//         break;
//     case WIFI::Wifi::state_e::NOT_INITIALIZED:
//         std::cout << "Wifi Status: NOT_INITIALIZED\n";
//         break;
//     case WIFI::Wifi::state_e::INITIALIZED:
//         std::cout << "Wifi Status: INITIALIZED\n";
//         break;
//     }
// }

void checkWifiState(WIFI::Wifi &Wifi)
{
    auto wifiState = WIFI::Wifi::GetState();

    switch (wifiState)
    {
    case WIFI::Wifi::state_e::READY_TO_CONNECT:
        std::cout << "Wifi Status: READY_TO_CONNECT\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::DISCONNECTED:
        std::cout << "Wifi Status: DISCONNECTED\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::CONNECTING:
        std::cout << "Wifi Status: CONNECTING\n";
        break;
    case WIFI::Wifi::state_e::WAITING_FOR_IP:
        std::cout << "Wifi Status: WAITING_FOR_IP\n";
        break;
    case WIFI::Wifi::state_e::ERROR:
        std::cout << "Wifi Status: ERROR\n";
        break;
    case WIFI::Wifi::state_e::CONNECTED:
        std::cout << "Wifi Status: CONNECTED\n";
        break;
    case WIFI::Wifi::state_e::NOT_INITIALIZED:
        std::cout << "Wifi Status: NOT_INITIALIZED\n";
        break;
    case WIFI::Wifi::state_e::INITIALIZED:
        std::cout << "Wifi Status: INITIALIZED\n";
        break;
    }
}

void Irrigator::System::updateEnvInfo()
{
    this->env_sensor.read();
    if (true == this->env_sensor.new_value_available)
    {
        this->display.updateEnvData(this->env_sensor.humidity, this->env_sensor.temperature);
    }
}

void Irrigator::System::setSleepEnterTime(uint32_t time)
{
    sleepEnterTime = time;
}

uint32_t Irrigator::System::getSleepEnterTime()
{
    return sleepEnterTime;
}

void Irrigator::System::deepSleep(uint32_t sleepDuration)
{
    // ESP_LOGI(TAG, "Entering Deep Sleep for: %lu secs", sleepDuration);
    // printf("Entering Deep Sleep for: %lu secs", sleepDuration);
    esp_sleep_enable_timer_wakeup(sleepDuration * 1000000);
    esp_deep_sleep_start();
}

void enableSolenoid(portMap_t num)
{
    // ESP_LOGI(TAG, "Open Port: %d", (int)num);
    // printf("Open Port: %d", (int)num);
}

void disableSolenoid(portMap_t num)
{
    // ESP_LOGI(TAG, "Close Port: %d", (int)num);
    // printf("Close Port: %d", (int)num);
}

void run(Irrigator::System &system)
{
    // obtain_time();
    // mqttStart();
    // time(&now);
    // localtime_r(&now, &timeinfo);
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    // // ESP_LOGI(TAG, "The current date/time is: %s ", strftime_buf);
    // //printf("The current date/time is: %s ", strftime_buf);

    // // ESP_LOGI(TAG, "time %lld", (long long)now);
    // //printf("time %lld", (long long)now);
    // uint32_t loop = 5;

    // while(loop--) {
    system.checkWifiState();
    system.updateEnvInfo();

    //     result = portMan.runPortCheck();
    //     if(result == PortSupervisor::Result::ERR) {
    //         // ESP_LOGI(TAG, "Error running portCheck");
    //         //printf("Error running portCheck");
    //     }
    //     else if(result == PortSupervisor::Result::OK_PORTLIST_NEEDS_SAVING) {
    //         res = Vault::setVaultData(portMan);
    //         // ESP_LOGI(TAG, "Result of Storage Write: %d", (unsigned int)res);
    //         //printf("Result of Storage Write: %d", (unsigned int)res);
    //     }

    system.display.displayRefresh();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    // }

    // time(&now);
    // application.deepSleep(portMan.getNextPortTriggerTime() - now);
}

void setup(Irrigator::System &system)
{
    Vault::Result res = Vault::Result::ERR;
    esp_event_loop_create_default();
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // system.initialize();

    // if(ESP_RST_DEEPSLEEP != esp_reset_reason())
    // {
    //     /** setup time*/
    //     // setenv("TZ", "IST-5:30", 1);
    //     // tzset();
    //     // int rc = settimeofday(&nowTime, NULL);
    //     // if(rc == 0) {
    //     //     //printf("settimeofday() successful.\n");
    //     // }

    //     /**
    //      * Get Port list from config. Update port list into object instance
    //      * Pass the parameters from a config file readout here
    //      */
    //     portMan.addPort(1, 200, 200, 100, enableSolenoid, disableSolenoid);

    //     res = Vault::setVaultData(portMan);
    //     // ESP_LOGI(TAG, "Result of Storage Write: %d", (unsigned int)res);
    //     //printf("Result of Storage Write: %d", (unsigned int)res);
    // }

    // res = Vault::getVaultData(portMan);
    // // ESP_LOGI(TAG, "Result of Storage Read: %d", (unsigned int)res);
    // //printf("Result of Storage Read: %d", (unsigned int)res);

    // // rtc_gpio_isolate(GPIO_NUM_12);   // Disabled as it is interfering with Epaper display - Check use
}

extern "C" [[noreturn]] void app_main(void)
{
    std::string config;
    Irrigator::System application;
    uint8_t loopCounter = 0;
    const uint8_t LOOPS = 20;

    setup(application);

    // Initialize the DHT22 sensor
    Dht22 dhtSensor(4);
    int humidityOld = 0;
    int temperatureOld = 0;

    // Initialize the display
    Display epaperDisp;
    epaperDisp.displayOutline();
    epaperDisp.displayRefresh();

    /** Setup Wifi  */
    WIFI::Wifi Wifi;
    Wifi.SetCredentials("Cardoz", "8828385089");
    Wifi.Init();

    while (true)
    {
        if (0 == loopCounter)
        {
            loopCounter = LOOPS;
            dhtSensor.readSensor();
            auto humidityNow = static_cast<int>(dhtSensor.getHumidity());
            auto temperatureNow = static_cast<int>(dhtSensor.getTemperature());

            printf("temperature: %f ", dhtSensor.getTemperature());
            printf("humidity: %f\n", dhtSensor.getHumidity());

            if ((humidityOld != humidityNow) || (temperatureOld != temperatureNow))
            {
                humidityOld = humidityNow;
                temperatureOld = temperatureNow;
                epaperDisp.updateEnvData(dhtSensor.getHumidity(), dhtSensor.getTemperature());
                epaperDisp.displayRefresh();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        checkWifiState(Wifi);
        loopCounter--;
    }
}
