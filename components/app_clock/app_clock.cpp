/**
 * @file app_clock.cpp
 * @brief Implementation of Clock class for DS3231 RTC and SNTP time synchronization.
 *
 * This file provides the implementation for the Clock singleton class, which
 * abstracts DS3231 RTC operations (initialization, time set/get, temperature read)
 * and SNTP time synchronization for ESP-IDF projects.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "app_clock.hpp"
#include "sntp.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>
#include <ds3231.h>

static const char *const TAG = "Clock";
static const char *const SNTP_SERVER = "pool.ntp.org";

/**
 * @brief Initialize the DS3231 hardware (I2C and device descriptor).
 */
void Clock::init_clock_hardware(void)
{
    memset(&_dev, 0, sizeof(i2c_dev_t));
    ESP_ERROR_CHECK(ds3231_init_desc(
        &_dev,
        (i2c_port_t)0,
        (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SDA,
        (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SCL));
}

/**
 * @brief SNTP time synchronization notification callback.
 *
 * This callback is called when the system time is synchronized via SNTP.
 * It sets the ESP system time and updates the DS3231 RTC to match.
 *
 * @param tv Pointer to timeval structure with the new time.
 */
void time_sync_notification_cb(struct timeval *tv)
{
    Clock &clock = Clock::getInstance();
    settimeofday(tv, nullptr);

    struct tm tm_info;
    localtime_r(&tv->tv_sec, &tm_info);
    clock.clock_set_time(&tm_info);

    ESP_LOGI(TAG, "Time synchronized: %04d-%02d-%02d %02d:%02d:%02d",
             tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
             tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
}

/**
 * @brief Initialize SNTP and register the time sync callback.
 */
void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, SNTP_SERVER);
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();
}

/**
 * @brief Synchronize system time using SNTP.
 *
 * This function initializes SNTP, waits for the system time to be set
 * from an NTP server, and updates the DS3231 RTC via the callback.
 */
void sntp_get_time(void)
{
    initialize_sntp();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}