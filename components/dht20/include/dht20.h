#ifndef DHT20_H
#define DHT20_H

#include <stdio.h>
#include "i2c_comm.h"

class DHT20 {
    private:
        I2C i2c_instance;

    public:
        float humidity, temperature;
        /** @brief  Initializer sets up I2C interface to use. Initializes humidity and temperature variables
         *  @param  i2c_comm    I2C instance to use
        */
        void init(I2C i2c_comm);

        /** @brief  Read humidity and temperature from sensor. Stores values
         *          in public attributes DHT20::humidity and DHT20::temperature
         *  @param  None
         *  @return None
         * 
        */
        int read();
};

#endif