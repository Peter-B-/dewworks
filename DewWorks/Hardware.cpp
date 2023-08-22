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
    SensorMeasurement meas;

    meas.Temperature = getTemperature();
    meas.Humidity = getHumidity();

    return meas;
}

DhtSensor::DhtSensor(const uint8_t pin)
    : dht(pin, DHT22)
{
}

Relais::Relais(const uint8_t pin)
{
    this->pin=pin;
}

void Relais::begin() const
{
    pinMode(this->pin, OUTPUT);
}

void Relais::set(const bool on) const
{
    digitalWrite(LED_BUILTIN, on);
    digitalWrite(this->pin, on);
}
