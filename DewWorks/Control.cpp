#include "Control.h"

#include "arduino.h"

ControlLogic::ControlLogic(State &state) : state(state)
{
}

void ControlLogic::begin(const Config& configuration)
{
    this->config = configuration;
    state.Output.Reason = "Noch aus";
}

bool ControlLogic::update() const
{
    bool output = state.Output.State;

    const ControlInput &inp = state.Input;
    const float deltaDewTemp = inp.Inside.DewPointTemperature - inp.Outside.DewPointTemperature;
    if (isnanf(deltaDewTemp))
    {
        output = false;
        state.Output.Reason = "keine Daten";
    }
    if (
        deltaDewTemp > (config.DeltaDewTempMin + config.DeltaDewTempHyst) &&
        inp.Inside.Temperature > (config.TempInMin + config.TempHyst) &&
        inp.Outside.Temperature > (config.TempOutMin + config.TempHyst) &&
        inp.Inside.Humidity > (config.HumInMin + config.HumInHyst))
    {
        output = true;
        state.Output.Reason = "An";
    }
    else if (!output)
    {
        // Already switched off.
    }
    else if (deltaDewTemp < config.DeltaDewTempMin)
    {
        output = false;
        state.Output.Reason = "Taupunkt";
    }
    else if (inp.Inside.Temperature < config.TempInMin)
    {
        output = false;
        state.Output.Reason = "Innentemp";
    }
    else if (inp.Outside.Temperature < config.TempOutMin)
    {
        output = false;
        state.Output.Reason = "Aussentemp";
    }
    else if (inp.Inside.Humidity < config.HumInMin)
    {
        output = false;
        state.Output.Reason = "Luftfeuchte";
    }

    state.Output.State = output;
    return output;
}


void setDewTemperature(EnvironmentInfo &envInfo)
{
    if (isnanf(envInfo.Temperature) || isnanf(envInfo.Humidity))
        envInfo.DewPointTemperature = NAN;

    const float t = envInfo.Temperature;
    const float r = envInfo.Humidity;

    float a, b;

    if (t >= 0)
    {
        a = 7.5f;
        b = 237.3f;
    }
    else
    {
        a = 7.6f;
        b = 240.7f;
    }

    // Sättigungsdampfdruck in hPa
    const float sdd = 6.1078f * static_cast<float>(pow(10, (a * t) / (b + t)));

    // Dampfdruck in hPa
    const float dd = sdd * r * 0.01f;

    // v-Parameter
    const float v = log10(dd / 6.1078f);

    // Taupunkttemperatur (°C)
    const float tt = (b * v) / (a - v);

    envInfo.DewPointTemperature = tt;
}

void ControlLogic::setMeasurement(const Measurement &meas) const
{
    state.Input.Inside.Temperature = meas.Inside.Temperature + config.TempInOffset;
    state.Input.Inside.Humidity = meas.Inside.Humidity + config.HumInOffset;
    state.Input.Outside.Temperature = meas.Outside.Temperature + config.TempOutOffset;
    state.Input.Outside.Humidity = meas.Outside.Humidity + config.HumOutOffset;

    setDewTemperature(state.Input.Inside);
    setDewTemperature(state.Input.Outside);
}

Config ControlLogic::getDefaultConfig()
{
    return {};
}
