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



Output::Output(uint8_t pin, Config &configuration)
    : pin(pin), config(configuration)
{
}

void Output::begin() const
{
    //pinMode(this->pin, OUTPUT);
}

void Output::set(const bool on) const
{
    //digitalWrite(LED_BUILTIN, on);

    //analogWrite(this->pin, on ? this->config.PwmRatio * 255 / 100 : 0);
}



ModeSwitch::ModeSwitch(uint8_t pin)
    : pin(pin)
{
}

void ModeSwitch::begin() const
{
    pinMode(this->pin, INPUT);
}

OperationMode ModeSwitch::read() const
{
    const int switchValue = analogRead(this->pin);

    if (switchValue < 341)
        return OperationMode::Off;
    else if (switchValue < 682)
        return OperationMode::Auto;
    else
        return OperationMode::On;
}
