#include "main.h"
#include "portSupervisor.h"
#include "vault.h"
#include "sntp.h"
#include "mqtt.h"

#include <iostream>
#include "dht22.hpp"
#include "rom/ets_sys.h"
#include "cJSON.h"
#include "sampleComponent.hpp"
#include "pinmap.h"
#include <map>
#include <vector>
#include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/***** create a sample configuration *************/
// struct config_st config = { .portnumber = 1, .startTime = 1670000, .stopTime = 1671000, .flowQuantity = 200 };
// struct config_st config = { 1, 1000, 1000, 100 };       // This initialization worked ??
/************************************************/
#define ESP_INTR_FLAG_DEFAULT 0

uint32_t flowPulseCount{0};

PortSupervisor::Supervisor portMan;
PortSupervisor::Result result = PortSupervisor::Result::ERR;
Vault::Result res = Vault::Result::ERR;
const std::map<int, std::vector<int>> portMap{
    {0, {2, 18, 19}}};

static QueueHandle_t gpio_evt_queue = NULL;

time_t now;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    flowPulseCount++;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

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

WIFI::Wifi::state_e checkWifiState(WIFI::Wifi &Wifi)
{
    static WIFI::Wifi::state_e prevState;
    auto wifiState = WIFI::Wifi::GetState();

    if (prevState != wifiState)
    {
        prevState = wifiState;
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

    return wifiState;
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
    printf("Open Port: %d", (int)num);
    auto pins = portMap.find((int)num);
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(1)), 1);
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(2)), 0);
    // Enable output
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(0)), 1);
    ets_delay_us(6000);
    // Disable output
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(0)), 0);
}

void disableSolenoid(portMap_t num)
{
    printf("Close Port: %d", (int)num);
    auto pins = portMap.find((int)num);
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(1)), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(2)), 1);
    // Enable output
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(0)), 1);
    ets_delay_us(6000);
    // Disable output
    gpio_set_level(static_cast<gpio_num_t>(pins->second.at(0)), 0);
}

static void gpioTask(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            printf("GPIO intr, val: %d\n", gpio_get_level(static_cast<gpio_num_t>(io_num)));
        }
    }
}

void initGpio()
{
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((uint64_t)1 << (uint64_t)DRIVER_SLEEP) | ((uint64_t)1 << (uint64_t)DRIVER_IN_1) | ((uint64_t)1 << (uint64_t)DRIVER_IN_2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    io_conf = {0};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = ((uint64_t)1 << (uint64_t)FLOW_IN);
    io_conf.pull_up_en = gpio_pullup_t::GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpioTask, "gpioTask", 2048, NULL, 10, NULL);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(static_cast<gpio_num_t>(FLOW_IN), gpio_isr_handler, (void *)FLOW_IN);
}

uint64_t getNowTime()
{
    auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();
    return static_cast<uint64_t>(timeNow);
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
    uint64_t prevTicks{0};
    const uint32_t interval = 1000;
    uint32_t countPerSecond{0};
    double flowRate{0};
    const float calibrationFactor = 4.5;
    double totalFlow{0.0};

    setup(application);

    initGpio();

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

    // Create a output port
    PortSupervisor::Supervisor ports;
    ports.addPort(0, 10, 1200, 30, enableSolenoid, disableSolenoid);

    while (true)
    {
        // Check flow rate
        auto nowTicks = getNowTime();
        printf("now: %u, prev: %u, count: %u\n", static_cast<unsigned int>(nowTicks), static_cast<unsigned int>(prevTicks), static_cast<unsigned int>(flowPulseCount));
        if ((nowTicks - prevTicks) > interval)
        {
            countPerSecond = flowPulseCount;
            flowPulseCount = 0;
            flowRate = ((1000.0 / (getNowTime() - prevTicks)) * countPerSecond) / calibrationFactor;
            prevTicks = getNowTime();
            totalFlow += flowRate;
            printf("flow: %f\n", totalFlow);
        }
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

        // Manage wifi states
        if (WIFI::Wifi::state_e::CONNECTED == checkWifiState(Wifi))
        {
            epaperDisp.updateWifiState(true);
        }
        else
        {
            epaperDisp.updateWifiState(false);
        }

        // Check timings for port control
        auto portState = ports.runPortCheck();
        if (portState_t::OPEN == portState)
        {
            epaperDisp.updateOutputState(true);
        }
        if (portState_t::CLOSED == portState)
        {
            epaperDisp.updateOutputState(false);
        }

        // If elements need to be updated, refresh the display
        epaperDisp.displayRefresh();
        loopCounter--;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
