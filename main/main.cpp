#include "main.h"
#include "portSupervisor.h"
#include "vault.h"

/** @brief Private Variables */
static const char *TAG = "MAIN";


/***** create a sample configuration *************/
// struct config_st config = { .portnumber = 1, .startTime = 1670000, .stopTime = 1671000, .flowQuantity = 200 };
// struct config_st config = { 1, 1000, 1000, 100 };       // This initialization worked ??
/************************************************/

extern "C" void app_main(void);
Main App;
PortSupervisor::Supervisor portMan;
PortSupervisor::Result result = PortSupervisor::Result::ERR;
Vault::Result res = Vault::Result::ERR;
Irrigator::System application;
time_t now;
static struct timeval nowTime = {.tv_sec = 1683994547,};
char strftime_buf[64];
struct tm timeinfo;
int64_t* ptr_timeSeconds = NULL;

void Irrigator::System::initialize() {
    /** Setup Wifi  */
    Wifi.SetCredentials("Samsung_ap", "0987654321");
    Wifi.Init();

    /** Setup Humidity and Temperature sensor   */
    I2C i2c_comm;   
    i2c_comm.init(I2C_NUM_0, 5, 4, 100000);
    env_sensor.init(i2c_comm);

}

void Irrigator::System::checkWifiState() {
    wifiState = Wifi.GetState();

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

void Irrigator::System::checkEnvState() {
    env_sensor.read();
    ESP_LOGI(TAG," Temp: %0.2f | Humidity: %0.2f", env_sensor.temperature, env_sensor.humidity);
}

void Irrigator::System::setSleepEnterTime(uint32_t time) {
    sleepEnterTime = time;
}

uint32_t Irrigator::System::getSleepEnterTime() {
    return sleepEnterTime;
}

void Irrigator::System::deepSleep(uint32_t sleepDuration) {
    ESP_LOGI(TAG, "Entering Deep Sleep for: %d secs", sleepDuration);
    esp_sleep_enable_timer_wakeup(sleepDuration * 1000000);
    esp_deep_sleep_start();
}

void enableSolenoid(portMap_t num) {
    ESP_LOGI(TAG, "Open Port: %d", (int)num);
}

void disableSolenoid(portMap_t num) {
    ESP_LOGI(TAG, "Close Port: %d", (int)num);
}

void Main::run(void)
{
    application.checkWifiState();
    application.checkEnvState();

    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time is: %s ", strftime_buf);

    ESP_LOGI(TAG, "time %lld", (long long)now);
    uint32_t loop = 50;
    while(loop--) {
        result = portMan.runPortCheck();
        if(result == PortSupervisor::Result::ERR) {
            ESP_LOGI(TAG, "Error running portCheck");
        }
        else if(result == PortSupervisor::Result::OK_PORTLIST_NEEDS_SAVING) {
            res = Vault::setVaultData(portMan);
            ESP_LOGI(TAG, "Result of Storage Write: %d", (unsigned int)res);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    time(&now);
    application.deepSleep(portMan.getNextPortTriggerTime() - now);

    // display.displayRefresh();    // Use when needed i.e. if not debugging display
}

void Main::setup(void)
{
    Vault::Result res = Vault::Result::ERR;
    esp_event_loop_create_default();
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    application.initialize();
    
    if(ESP_RST_DEEPSLEEP != esp_reset_reason())
    {
        /** setup time*/
        setenv("TZ", "IST-5:30", 1);
        tzset();
        int rc = settimeofday(&nowTime, NULL);
        if(rc == 0) {
            printf("settimeofday() successful.\n");
        }

        /**
         * Get Port list from config. Update port list into object instance
         * Pass the parameters from a config file readout here
         */
        portMan.addPort(1, 200, 200, 100, enableSolenoid, disableSolenoid);
        
        res = Vault::setVaultData(portMan);
        ESP_LOGI(TAG, "Result of Storage Write: %d", (unsigned int)res);

    }

    res = Vault::getVaultData(portMan);
    ESP_LOGI(TAG, "Result of Storage Read: %d", (unsigned int)res);
    
    rtc_gpio_isolate(GPIO_NUM_12);
}

void app_main(void)
{
    App.setup();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        App.run();
    }
}
