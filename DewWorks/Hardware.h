#ifndef _Hardware_h
#define _Hardware_h

#include "Types.h"

#include <Adafruit_Sensor.h>
#include <DHT_U.h>

class DhtSensor
{
public:
    explicit DhtSensor(uint8_t pin);
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
    explicit Relais(uint8_t pin);
    void begin() const;
    void set(bool on) const;
private:
    uint8_t pin;
};

#endif
