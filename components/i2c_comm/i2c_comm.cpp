#include <esp_system.h>
#include <sys/param.h>
#include "driver/gpio.h"
#include "i2c_comm.h"

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL I2C_MASTER_ACK                  /*!< I2C ack value */
#define NACK_VAL I2C_MASTER_NACK                /*!< I2C nack value */
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */

// int I2C::port_count = 0;


void I2C::init(i2c_port_t port, uint8_t sda, uint8_t scl, uint32_t freq_hz) {
    i2c_port = port;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.sda_pullup_en = 1;
    conf.scl_io_num = scl;
    conf.scl_pullup_en = 1;
    conf.master.clk_speed = freq_hz;
    conf.clk_flags = 0,

    i2c_param_config(port, &conf);
    i2c_driver_install(port, conf.mode, 0, 0, 0);
}

esp_err_t I2C::write(uint8_t slave_address, uint8_t* tx_buff, uint8_t num_of_bytes) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, tx_buff, num_of_bytes, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t I2C::read(uint8_t slave_address, uint8_t* rx_buff, uint8_t num_of_bytes) {
    if (num_of_bytes == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_address << 1) | READ_BIT, ACK_CHECK_EN);
    if (num_of_bytes > 1) {
        i2c_master_read(cmd, rx_buff, num_of_bytes - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, rx_buff + num_of_bytes - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
