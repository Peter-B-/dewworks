#include "Control.h"

#include "arduino.h"

ControlLogic::ControlLogic(State &state) : state(state)
{
}

void ControlLogic::begin(Config config)
{
    setConfig(config);
    strcpy(state.Output.Reason, "Noch aus");
}

bool ControlLogic::update()
{

    bool output = state.Output.State;

    ControlInput &inp = state.Input;
    float deltaDewTemp = inp.Inside.DewPointTemperature - inp.Outside.DewPointTemperature;
    if (isnanf(deltaDewTemp))
    {
        output = false;
        strcpy(state.Output.Reason, "keine Daten");
    }
    if (
        deltaDewTemp > (config.DeltaDewTempMin + config.DeltaDewTempHyst) &&
        inp.Inside.Temperature > (config.TempInMin + config.TempHyst) &&
        inp.Outside.Temperature > (config.TempOutMin + config.TempHyst) &&
        inp.Inside.Humidity > (config.HumInMin + config.HumInHyst))
    {
        output = true;
        strcpy(state.Output.Reason, "An");
    }
    else if (!output)
    {
        // Already switched off.
    }
    else if (deltaDewTemp < config.DeltaDewTempMin)
    {
        output = false;
        strcpy(state.Output.Reason, "Taupunkt");
    }
    else if (inp.Inside.Temperature < config.TempInMin)
    {
        output = false;
        strcpy(state.Output.Reason, "Innentemp");
    }
    else if (inp.Outside.Temperature < config.TempOutMin)
    {
        output = false;
        strcpy(state.Output.Reason, "Aussentemp");
    }
    else if (inp.Inside.Humidity < config.HumInMin)
    {
        output = false;
        strcpy(state.Output.Reason, "Luftfeuchte");
    }

    state.Output.State = output;
    return output;
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

void ControlLogic::setMeasurement(Measurement &meas)
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
    return Config();
}
