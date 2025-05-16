#include "main.h"
#include "portSupervisor.h"
#include "vault.h"
#include "sntp.h"
#include "mqtt.h"
#include "esp_log.h"

#include <iostream>
#include "dht22.hpp"
#include "rom/ets_sys.h"
#include "cJSON.h"
#include "pinmap.h"
#include "vault.h"

#include <map>
#include <vector>
#include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "port.h"

#include <time.h>
#include <sys/time.h>
#include "soc/rtc.h"

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_sleep.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "http_server.h"

#include "mdns.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define MAIN_LOOP_INTERVAL 100
#define MDNS_INSTANCE "esp home web server"

uint32_t flowPulseCount{0};

PortSupervisor::Supervisor portMan;
PortSupervisor::Result result = PortSupervisor::Result::ERR;
Vault::Result res = Vault::Result::ERR;
const std::map<int, std::vector<int>> portMap{
    {0, {2, 18, 19}}};

static QueueHandle_t gpio_evt_queue = NULL;

time_t now;

PortSupervisor::Supervisor *activePorts;

http_server *http_server::httpServerInstance = nullptr;
std::mutex http_server::mutexHttpServer;
bool f_systemTimeUpdated = false;

// Function prototypes
void configurationParser(const char *buf);

static void IRAM_ATTR
gpio_isr_handler(void *arg)
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
}

uint32_t getNowTime()
{
    auto timeNow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock().now().time_since_epoch()).count();
    return static_cast<uint32_t>(timeNow);
}

void timeUpdateNotif(struct timeval *newTime)
{
    f_systemTimeUpdated = true;
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
            registerEventHandler(&timeUpdateNotif);
            obtain_time();
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

void configurationParser(const char *buf)
{
    // cJSON *root = cJSON_Parse(buf);
    // int red = cJSON_GetObjectItem(root, "red")->valueint;
    // int green = cJSON_GetObjectItem(root, "green")->valueint;
    // int blue = cJSON_GetObjectItem(root, "blue")->valueint;
    // printf("Light control: red = %d, green = %d, blue = %d", red, green, blue);
    // cJSON_Delete(root);

    // st_portConfig config[1]; // For a single port

    // config[0].e_portState = port.portState_e;
    // config[0].u8_portNumber = port.portNumber_u8;
    // config[0].u32_switchTime = port.switchTime_u32;
    // config[0].u32_interval = port.interval;
    // config[0].u32_duration = port.duration;
    // config[0].u16_flowLitresRequired = port.flowLitresRequired_u16;

    // nvs_write_chunk(VAULT_CONFIG_KEY, config, sizeof(config));

    // nvs_write_u32(VAULT_INITIALIZED, initializationKey);
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

static void initialise_mdns(void)
{
    // initialize mDNS service
    esp_err_t err = mdns_init();
    if (err)
    {
        printf("MDNS Init failed: %d\n", err);
        return;
    }

    // set hostname
    mdns_hostname_set("my-esp32");
    // set default instance
    mdns_instance_name_set("Jhon's ESP32 Thing");
}

void initHttpServer()
{
    // Iniialize a singleton of the http_server
    http_server &server = http_server::getInstance();
    server.init();
    server.registerConfigParser(configurationParser);
}

void setup(Irrigator::System &system)
{
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

    // if (ESP_RST_DEEPSLEEP != esp_reset_reason())
    // {
    /** setup time*/
    setenv("TZ", "IST-5:30", 1);
    tzset();
    // int rc = settimeofday(&nowTime, NULL);
    // if (rc == 0)
    // {
    //     // printf("settimeofday() successful.\n");
    // }
}

extern "C" [[noreturn]] void app_main(void)
{
    const uint32_t interval = 10 * (MAIN_LOOP_INTERVAL * 10);
    const uint32_t LOOPS = 60 * (MAIN_LOOP_INTERVAL * 10);
    uint32_t sleepInterval = 3000;
    std::string config;
    Irrigator::System application;
    uint32_t loopCounter = 0;
    uint32_t prevTicks{0};
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

    initialise_mdns();

    /** Setup Wifi  */
    WIFI::Wifi Wifi;
    Wifi.SetCredentials("Cardoz", "8828385089");
    Wifi.Init();

    // Create a output port
    PortSupervisor::Supervisor ports;
    Vault::getVaultData(ports);
    epaperDisp.portSwitchTime(ports.portList[0].switchTime_u32);
    epaperDisp.updateDesiredFlow(ports.portList[0].flowLitresRequired_u16);
    activePorts = &ports;

    initHttpServer();

    http_server &server = http_server::getInstance();
    server.registerObserver(&ports);

    while (true)
    {
        if (true == f_systemTimeUpdated)
        {
            printf("\nUpdating port switch Times\n");
            ports.updateSwitchTimes();
            f_systemTimeUpdated = false;
            epaperDisp.updateOutputState(false);
            epaperDisp.portSwitchTime(ports.portList[0].switchTime_u32);
            Vault::setVaultData(ports);
        }

        // Check flow rate
        auto nowTicks = getNowTime();
        printf("now: %u, prev: %u, Flow: %d\n", static_cast<unsigned int>(nowTicks), static_cast<unsigned int>(prevTicks), ports.portList[0].flowLitresCurrent_u16);

        // Update flow quantity every 60 seconds
        if ((nowTicks - prevTicks) > 60)
        {
            countPerSecond = flowPulseCount;
            flowPulseCount = 0;
            flowRate = ((1.0 / (getNowTime() - prevTicks)) * countPerSecond) / calibrationFactor;
            prevTicks = getNowTime();
            ports.portList[0].flowLitresCurrent_u16 += static_cast<uint16_t>(flowRate);
            epaperDisp.updateFlow(static_cast<double>(ports.portList[0].flowLitresCurrent_u16));
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
            epaperDisp.portSwitchTime(ports.portList[0].switchTime_u32);
            Vault::setVaultData(ports);
        }

        // If elements need to be updated, refresh the display
        epaperDisp.displayRefresh();
        loopCounter--;
        sleepInterval--;

        vTaskDelay(pdMS_TO_TICKS(MAIN_LOOP_INTERVAL));

        printf("sleep %u", static_cast<unsigned int>(sleepInterval));

        // delay sleep if a station is connected to the ESP
        if (0 != WIFI::Wifi::isStationConnected())
        {
            sleepInterval = 1200;
        }

        if (0 == sleepInterval)
        {
            auto timeNow = getNowTime();
            auto triggerTime = ports.getNextPortTriggerTime();

            ESP_LOGI("Main", "Time now: %lu, Trigger at: %lu", timeNow, triggerTime);

            if ((triggerTime > timeNow) && (3600 > (triggerTime - timeNow))) //  Possibly find the greater and then substract
            {
                // printf("Going to sleep for: %u secs", static_cast<unsigned int>(triggerTime - timeNow));
                ESP_LOGI("Main", "Sleep:  %u secs", static_cast<unsigned int>(triggerTime - timeNow));
                application.deepSleep(triggerTime - timeNow);
            }
            else if (triggerTime < timeNow)
            {
                ports.updateSwitchTimes();
                sleepInterval = 3000;
            }
            else
            {
                ESP_LOGI("Main", "Sleep: 3600 secs");
                application.deepSleep(3600);
            }
        }
    }
}
