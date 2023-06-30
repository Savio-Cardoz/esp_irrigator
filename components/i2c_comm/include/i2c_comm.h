#ifndef I2C_COMM_H
#define I2C_COMM_H

#include <stdio.h>
#include "driver/i2c.h"

#define BUFFER_SIZE 10      // Bytes

// enum {
//     I2C_BUSY = -1,
//     I2C_OK = 0
// }i2c_err_t;

class I2C {
    
    public:
        i2c_port_t i2c_port;
        void init(i2c_port_t port, uint8_t sda, uint8_t scl, uint32_t freq_hz);

        /**
         * @brief   Write a stream of bytes on the interface
         * @param   slave_address 7 bit address of the slave to communicate with
         * @param   tx_buff   A pointer to a uint8_t memory
         * @param   num_of_bytes  Number of bytes to send from the tx_buff memory
         * @return  esp_err_t
         */
        esp_err_t write(uint8_t slave_address, uint8_t* tx_buff, uint8_t num_of_bytes);

        /**
         * @brief   Read a stream of bytes on the interface
         * @param   slave_address 7 bit address of the slave to communicate with
         * @param   rx_buff   A pointer to a uint8_t memory
         * @param   num_of_bytes  Number of bytes to read from the interface into the ex_buff memory
         * @return  esp_err_t
         */
        esp_err_t read(uint8_t slave_address, uint8_t* rx_buff, uint8_t num_of_bytes);

        
};

#endif