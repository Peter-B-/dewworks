#ifndef _Control_h
#define _Control_h

#include "Types.h"

class ControlLogic
{
public:
    void begin(Config config);
    bool getState();
    bool update(Measurement meas);
    void setConfig(Config config);

    static Config getDefaultConfig();
private:
    bool state = false;
    Config config;
};

#endif