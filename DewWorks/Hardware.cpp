#include <Arduino.h>
#include <avr/io.h>
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
    pinMode(this->pin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // Set up Timer1 for ~25kHz PWM on Pin 9 or 10
    if (this->pin == 9 || this->pin == 10) {
        // Fast PWM, TOP=ICR1, non-inverting
        TCCR1A = 0;
        TCCR1B = 0;
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS10); // No prescaler
        ICR1 = 639; // 16MHz/(25kHz)-1 = 639
        if (this->pin == 9) {
            TCCR1A |= (1 << COM1A1); // Non-inverting on OC1A (Pin 9)
        } else {
            TCCR1A |= (1 << COM1B1); // Non-inverting on OC1B (Pin 10)
        }
        // Start with 0% duty
        if (this->pin == 9) {
            OCR1A = 0;
        } else {
            OCR1B = 0;
        }
    } else {
        analogWrite(this->pin, 0);
    }
}

void Output::set(const bool on) const
{
    digitalWrite(LED_BUILTIN, on);

    if (this->pin == 9 || this->pin == 10) {
        // Use Timer1 for high-frequency PWM
        
        uint16_t value;
        
        if (!on)
            value = 0;
        else if (this->config.PwmRatio >= 100)
            value = 639;
        else
            value = this->config.PwmRatio * 6; // * 6 is an approximation of 639 / 100;
        
        if (this->pin == 9) {
            OCR1A = value;
        } else {
            OCR1B = value;
        }
    } else {
        const int16_t value = on ? this->config.PwmRatio * 255 / 100 : 0;
        analogWrite(this->pin, value);
    }
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
