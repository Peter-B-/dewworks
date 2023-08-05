#ifndef _Control_h
#define _Control_h

#include "Types.h"

class ControlLogic
{
public:
    void begin(Config config);
    bool getState();
    bool update();
    void setConfig(Config config);
    ControlInput setMeasurement(Measurement &meas);

    static Config getDefaultConfig();
private:
    bool state = false;
    Config config;
    ControlInput currentInput;
};

#endif