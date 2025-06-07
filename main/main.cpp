/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

extern "C"
{
#include "display.h"
}

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include <nvs_flash.h>
#include "lvgl.h"

#include "app_wifi.hpp"
#include "app_clock.hpp"
#include "app_common.hpp"

#include <queue>

static const char *const TAG = "Main";
appWifiState application::_wifiState = appWifiState::DISCONNECTED;
std::queue<struct tm> timeQueue;

[[noreturn]] void run_clock_task(void *pvParameters)
{
    Clock &clock = Clock::getInstance();
    auto app = static_cast<application *>(pvParameters);
    bool isSNTPTimeSet = false;
    float temp;

    // Initialize the DS3231 hardware
    clock.init_clock_hardware();

    // Update system time from DS3231 RTC time
    struct tm time;
    clock.clock_get_time(&time);
    struct timeval tv;
    tv.tv_sec = mktime(&time);
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);

    while (1)
    {
        clock.clock_get_hardware_temp(&temp);
        if (temp == -1)
        {
            printf("Could not get temperature\n");
            continue;
        }

        clock.clock_get_time(&time);

        if ((nullptr != app) && (appWifiState::CONNECTED == app->getWifiState()) && !isSNTPTimeSet)
        {
            isSNTPTimeSet = true;
            ESP_LOGI(TAG, "WiFi is connected and SNTP time can be used");
            sntp_get_time();
        }

        /* float is used in printf(). you need non-default configuration in
         * sdkconfig for ESP8266, which is enabled by default for this
         * example. see sdkconfig.defaults.esp8266
         */
        printf("%04d-%02d-%02d %02d:%02d:%02d, %.2f deg Cel\n", time.tm_year + 1900 /*Add 1900 for better readability*/, time.tm_mon + 1,
               time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, temp);

        timeQueue.push(time);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" [[noreturn]] void app_main(void)
{
    application &app = application::getInstance();

    ESP_ERROR_CHECK(i2cdev_init());

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

    WIFI::Wifi app_wifi;

    app_wifi.SetCredentials(CONFIG_ESP_WIFI_STA_SSID, CONFIG_ESP_WIFI_STA_PASSWORD);
    app_wifi.Init();

    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    time(&now);
    // Set timezone to China Standard Time
    setenv("TZ", "IST-5:30", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);

    xTaskCreate(run_clock_task, "ds3231_test", configMINIMAL_STACK_SIZE * 3, &app, 5, NULL);

    int counter = 0;
    display_init();

    lv_obj_t *timer_label = lv_label_create(lv_scr_act());
    lv_obj_align(timer_label, LV_ALIGN_TOP_LEFT, 0, 0);

    while (1)
    {
        checkWifiState(&app_wifi);

        if (WIFI::Wifi::state_e::CONNECTED == WIFI::Wifi::GetState())
        {
            app.setWifiState(appWifiState::CONNECTED);
        }
        else
        {
            app.setWifiState(appWifiState::DISCONNECTED);
        }

        if (!timeQueue.empty())
        {
            struct tm currentTime = timeQueue.front();
            timeQueue.pop();

            if (currentTime.tm_sec % 59 == 0) // Update every minute
            {
                ESP_LOGI(TAG, "Current time: %04d-%02d-%02d %02d:%02d:%02d", currentTime.tm_year + 1900, currentTime.tm_mon + 1,
                         currentTime.tm_mday, currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
                lv_label_set_text_fmt(timer_label, "%04d-%02d-%02d\n%02d:%02d", currentTime.tm_year + 1900, currentTime.tm_mon + 1,
                                      currentTime.tm_mday, currentTime.tm_hour, currentTime.tm_min);
            }
        }
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
