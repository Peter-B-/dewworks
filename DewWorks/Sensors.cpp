#include "Sensors.h"

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

Measurement DhtSensor::measure()
{
    auto meas = Measurement();

    meas.Temperature = getTemperature();
    meas.Humidity = getHumidity();
    
    setDewTemperature(meas);

    return meas;
}

void DhtSensor::setDewTemperature(Measurement &meas)
{
    if (isnanf(meas.Temperature) || isnanf(meas.Humidity))
        meas.DewTemperature = NAN;

    float t = meas.Temperature;
    float r = meas.Humidity;

    float a, b;

    if (t >= 0)
    {
        a = 7.5f;
        b = 237.3f;
    }
    else if (t < 0)
    {
        a = 7.6f;
        b = 240.7f;
    }

    // Sättigungsdampfdruck in hPa
    float sdd = 6.1078f * pow(10, (a * t) / (b + t));

    // Dampfdruck in hPa
    float dd = sdd * r * 0.01;

    // v-Parameter
    float v = log10(dd / 6.1078f);

    // Taupunkttemperatur (°C)
    float tt = (b * v) / (a - v);

    meas.DewTemperature = tt;
}

DhtSensor::DhtSensor(uint8_t pin)
    : dht(pin, DHT22)
{
}

float dewPoint(float t, float r)
{
}
