#ifndef _Hardware_h
#define _Hardware_h

#include "Types.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class DhtSensor
{
public:
    DhtSensor(uint8_t pin);
    void begin();
    SensorMeasurement measure();
private:
    DHT_Unified dht;
    float getTemperature();
    float getHumidity();
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
