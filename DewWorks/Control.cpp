#include "Control.h"

#include "arduino.h"

void ControlLogic::begin(Config config)
{
    setConfig(config);
}

bool ControlLogic::getState()
{
    return this->state;
}

bool ControlLogic::update(Measurement meas)
{
    float deltaDewTemp = meas.Inside.DewTemperature - meas.Outside.DewTemperature;
    if (isnanf(deltaDewTemp))
        this->state = false;

    if (
        deltaDewTemp > (config.DeltaDewTempMin + config.DeltaDewTempHyst) &&
        meas.Inside.Temperature > (config.TempInMin + config.TempHyst) &&
        meas.Outside.Temperature > (config.TempOutMin + config.TempHyst) &&
        meas.Inside.Humidity > (config.HumInMin + config.HumInHyst))
    {
        if (!this->state)
            Serial.println("Schalte an");
        this->state = true;
    }
    if (deltaDewTemp < config.DeltaDewTempMin)
    {
        if (this->state)
            Serial.println("Schalte ab wegen Taupunkt");
        this->state = false;
    }
    if (meas.Inside.Temperature < config.TempInMin)
    {
        if (this->state)
            Serial.println("Schalte ab wegen Innentemperatur");
        this->state = false;
    }
    if (meas.Outside.Temperature < config.TempOutMin)
    {
        if (this->state)
            Serial.println("Schalte ab wegen Aussentemperatur");
        this->state = false;
    }
    if (meas.Inside.Humidity < config.HumInMin)
    {
        if (this->state)
            Serial.println("Schalte ab wegen Luftfeuchte");
        this->state = false;
    }

    return this->state;
}

void ControlLogic::setConfig(Config config)
{
    this->config = config;
}

Config ControlLogic::getDefaultConfig()
{
    return Config();
}
