#ifndef APP_CLOCK_HPP
#define APP_CLOCK_HPP

#include "i2cdev.h"
#include "ds3231.h"

/**
 * @file app_clock.hpp
 * @brief Clock abstraction for DS3231 RTC and SNTP time synchronization.
 *
 * This header provides the Clock class, which wraps DS3231 RTC operations
 * (time set/get, temperature read) and exposes a simple interface for use in
 * ESP-IDF projects. It also declares the SNTP time synchronization function.
 */

/**
 * @brief Synchronize system time using SNTP.
 *
 * This function initializes SNTP and waits for the system time to be set
 * from an NTP server.
 */
void sntp_get_time(void);

/**
 * @class Clock
 * @brief Abstraction for DS3231 RTC device.
 *
 * The Clock class provides methods to initialize the DS3231 hardware,
 * set and get the RTC time, read the temperature from the RTC, and
 * access the underlying I2C device descriptor.
 */
class Clock
{
private:
    i2c_dev_t _dev; ///< I2C device descriptor for DS3231
    // Private constructor for singleton
    Clock() = default;

public:
    // Delete copy constructor and assignment operator
    Clock(const Clock &) = delete;
    Clock &operator=(const Clock &) = delete;

    /**
     * @brief Get the singleton instance of Clock.
     * @return Reference to the singleton Clock instance.
     */
    static Clock &getInstance()
    {
        static Clock instance;
        return instance;
    }

    /**
     * @brief Default destructor.
     */
    ~Clock() = default;

    /**
     * @brief Initialize the DS3231 hardware (I2C and device descriptor).
     */
    void init_clock_hardware(void);

    /**
     * @brief Set the DS3231 RTC time.
     * @param timeinfo Pointer to a struct tm with the desired time.
     */
    void clock_set_time(struct tm *timeinfo)
    {
        // Set the time on the DS3231 RTC
        ds3231_set_time(&_dev, timeinfo);
    }

    /**
     * @brief Get the current time from the DS3231 RTC.
     * @param timeinfo Pointer to a struct tm to store the read time.
     */
    void clock_get_time(struct tm *timeinfo)
    {
        ds3231_get_time(&_dev, timeinfo);
    }

    /**
     * @brief Read the temperature from the DS3231 RTC.
     * @param temp Pointer to a float to store the temperature in Celsius.
     */
    void clock_get_hardware_temp(float *temp)
    {
        ds3231_get_temp_float(&_dev, temp);
    }
};

#endif // APP_CLOCK_HPP