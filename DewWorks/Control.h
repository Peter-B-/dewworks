#ifndef _Control_h
#define _Control_h

#include "Types.h"

class ControlLogic
{
public:
    ControlLogic(State &state);

    void begin(Config config);
    bool getState();
    bool update();
    void setConfig(Config config);
    void setMeasurement(Measurement &meas);

    static Config getDefaultConfig();
private:
    State &state;
    Config config;
};

#endif