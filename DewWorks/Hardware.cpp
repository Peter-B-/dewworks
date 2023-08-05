#include "Hardware.h"

void DhtSensor::begin()
{
    this->dht.begin();
}

float DhtSensor::getTemperature()
{
    sensors_event_t event;
    this->dht.temperature().getEvent(&event);
    return event.temperature;
}

float DhtSensor::getHumidity()
{
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    return event.relative_humidity;
}

SensorMeasurement DhtSensor::measure()
{
    auto meas = SensorMeasurement();

    meas.Temperature = getTemperature();
    meas.Humidity = getHumidity();

    return meas;
}

DhtSensor::DhtSensor(uint8_t pin)
    : dht(pin, DHT22)
{
}

float dewPoint(float t, float r)
{
}

Relais::Relais(uint8_t pin)
{
    this->pin=pin;
}

void Relais::begin()
{
    pinMode(this->pin, OUTPUT);
}

void Relais::set(bool on)
{
    digitalWrite(LED_BUILTIN, on);
    digitalWrite(this->pin, on);
}
