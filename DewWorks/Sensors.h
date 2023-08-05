#ifndef _SENSORS_h
#define _SENSORS_h

#include "Types.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class DhtSensor
{
public:
    DhtSensor(uint8_t pin);
    void begin();
    Measurement measure();
private:
    DHT_Unified dht;
    float getTemperature();
    float getHumidity();
    static void setDewTemperature(Measurement& meas);
};

class Relais
{
public:
    Relais(uint8_t pin);
    void begin();
    void set(bool on);
private:
    uint8_t pin;
};

#endif
