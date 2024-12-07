/*

    DHT22 temperature sensor driver

*/

#ifndef DHT22_H_
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =======================================

void setDHTgpio(int gpio);
void errorHandler(int response);
int readDHT();
float getHumidity();
float getTemperature();
int getSignalLevel(int usTimeOut, bool state);

class Dht22
{
private:
    int DHTgpio = 4; // my default DHT pin = 4
    float humidity = 0.;
    float temperature = 0.;

    int getSignalLevel(int usTimeOut, bool state);

public:
    //! \brief  Contructs the DHT22 object
    //! \param  gpioNumber an int i.e. the GPIO number of ESP32
    Dht22(int gpioNumber) : DHTgpio(gpioNumber) {};
    ~Dht22() = default;

    int readSensor();

    float getHumidity() { return humidity; };
    float getTemperature() { return temperature; };
};

#endif