#ifndef _TYPES_h
#define _TYPES_h

#include <Arduino.h>

struct SensorMeasurement
{
    float Temperature{};
    float Humidity{};
};

struct Measurement
{
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

/// @brief Operation mode is selected by a hardware switch.
enum class OperationMode
{
    Off,
    Auto,
    On
};


struct FanRuntimeStats
{
    uint32_t TotalMinutes{0};

    // 24 slots to store the last 24 hours of fan runtime
    uint8_t Last24hMinutes[24]{};
    // Index of the current slot (0-23)
    uint8_t Last24hIndex{0};
};

struct State
{
    ControlInput Input{};
    ControlOutput Output{};
    OperationMode Mode{OperationMode::Auto};
    FanRuntimeStats FanRuntime{};
};

constexpr uint16_t CONFIG_VERSION = 1;

struct Config
{
    // Version will be used to detect incompatible changes
    // when loading from EEPROM after a firmware update.
    // Increment this if you change the struct layout
    uint16_t Version{CONFIG_VERSION};

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

    float PwmRatio{50};
};

#endif
