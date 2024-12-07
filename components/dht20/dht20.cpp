#include <cmath>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dht20.h"

void DHT20::init(I2C i2c_comm)
{
    i2c_instance = i2c_comm;
    humidity = 0;
    temperature = 0;
}

int DHT20::read()
{
    uint8_t *buff = (uint8_t *)malloc(20);
    new_value_available = false;

    buff[0] = 0x71;
    i2c_instance.write(0x38, buff, 1);
    i2c_instance.read(0x38, buff, 1);

    // ESP_LOGI("Read:", "0x%x", buff[0]);

    buff[0] = 0xAC;
    buff[1] = 0x33;
    buff[2] = 0x00;
    i2c_instance.write(0x38, buff, 1);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    i2c_instance.read(0x38, buff, 6);
    // ESP_LOGI(TAG, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]);

    uint32_t humidity_sens = (buff[1] << 12) | (buff[2] << 4) | (buff[3] >> 4);
    humidity = std::round(((float)humidity_sens / 1048576) * 100);

    uint32_t temp_sens = ((buff[3] & 0x0F) << 16) | (buff[4] << 8) | buff[5];
    temperature = std::round((((float)temp_sens / 1048576) * 200) - 50);

    if (humidity_prev != humidity)
    {
        humidity_prev = humidity;
        new_value_available = true;
    }

    if (temperature_prev != temperature)
    {
        temperature_prev = temperature;
        new_value_available = true;
    }

    free(buff);
    return 0;
}