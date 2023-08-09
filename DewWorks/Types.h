#ifndef _TYPES_h
#define _TYPES_h

struct SensorMeasurement{
    float Temperature{};
    float Humidity{};
};

struct Measurement{
    SensorMeasurement Inside{};
    SensorMeasurement Outside{};
};

struct EnvironmentInfo
{
    float Temperature{};
    float Humidity{};
    float DewPointTemperature{};
};

struct ControlInput
{
    EnvironmentInfo Inside{};
    EnvironmentInfo Outside{};
};

struct ControlOutput
{
    bool State{};
    char Reason[20]{};
};

struct State
{
    ControlInput Input{};
    ControlOutput Output{};
};

struct Config{
    float DeltaDewTempMin{5};
    float DeltaDewTempHyst{1};
    
    float HumInMin{40};
    float HumInHyst{5};

    float TempInMin{10};
    float TempOutMin{-10};
    float TempHyst{1};

    float TempInOffset{0};
    float TempOutOffset{0};
    float HumInOffset{0};
    float HumOutOffset{0};
};

#endif
