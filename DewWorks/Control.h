#ifndef _Control_h
#define _Control_h

#include "Types.h"

class ControlLogic
{
public:
    explicit ControlLogic(State& state, Config& configuration);

    void begin() const;
    bool update() const;
    void setMeasurement(const Measurement& meas) const;

    static Config getDefaultConfig();

private:
    State& state;
    Config& config;
};

#endif
