#ifndef _TYPES_h
#define _TYPES_h

struct SensorMeasurement{
    float Temperature{};
    float Humidity{};
    float DewTemperature{};
};

struct Measurement{
    SensorMeasurement Inside{};
    SensorMeasurement Outside{};
};

struct Config{
    float DeltaDewTempMin{5};
    float DeltaDewTempHyst{1};
    
    float HumInMin{40};
    float HumInHyst{5};

    float TempInMin{10};
    float TempOutMin{-10};
    float TempHyst{1};
};

#endif
