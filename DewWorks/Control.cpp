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



bool ControlLogic::update()
{
    float deltaDewTemp = currentInput.Inside.DewPointTemperature - currentInput.Outside.DewPointTemperature;
    if (isnanf(deltaDewTemp))
        this->state = false;

    if (
        deltaDewTemp > (config.DeltaDewTempMin + config.DeltaDewTempHyst) &&
        currentInput.Inside.Temperature > (config.TempInMin + config.TempHyst) &&
        currentInput.Outside.Temperature > (config.TempOutMin + config.TempHyst) &&
        currentInput.Inside.Humidity > (config.HumInMin + config.HumInHyst))
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
    if (currentInput.Inside.Temperature < config.TempInMin)
    {
        if (this->state)
            Serial.println("Schalte ab wegen Innentemperatur");
        this->state = false;
    }
    if (currentInput.Outside.Temperature < config.TempOutMin)
    {
        if (this->state)
            Serial.println("Schalte ab wegen Aussentemperatur");
        this->state = false;
    }
    if (currentInput.Inside.Humidity < config.HumInMin)
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

void setDewTemperature(EnvironmentInfo &envInfo)
{
    if (isnanf(envInfo.Temperature) || isnanf(envInfo.Humidity))
        envInfo.DewPointTemperature = NAN;

    float t = envInfo.Temperature;
    float r = envInfo.Humidity;

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

    envInfo.DewPointTemperature = tt;
}


ControlInput ControlLogic::setMeasurement(Measurement& meas)
{
    currentInput.Inside.Temperature = meas.Inside.Temperature + config.TempInOffset;
    currentInput.Inside.Humidity = meas.Inside.Humidity + config.HumInOffset;
    currentInput.Outside.Temperature = meas.Outside.Temperature + config.TempOutOffset;
    currentInput.Outside.Humidity = meas.Outside.Humidity + config.HumOutOffset;

    setDewTemperature(currentInput.Inside);
    setDewTemperature(currentInput.Outside);

    return this->currentInput;
}


Config ControlLogic::getDefaultConfig()
{
    return Config();
}


