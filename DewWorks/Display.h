#ifndef _Display_h
#define _Display_h

#include "Types.h"
#include "Tools.h"

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

namespace DisplayMode
{
    enum Mode
    {
        Measurement,
        Menu,
        ValueChange
    };
}

class MenuItem
{
public:
    MenuItem(const __FlashStringHelper* name, float &value, float min, float max, float factor);

    void select(long rotaryPos);

    void update(long rotrayPos) const;
    void printHeader(char *buffer) const;
    void printValue(char *buffer) const;

private:
    String name;
    float &value;
    float initialValue;
    float minimum, maximum, factor;
    long initialRotaryPos;
};

class Display
{
public:
    Display(State &state, Config &config);
    void begin();
    void update(long rotaryPos);

    void buttonPressed();
    void buttonPressedLong();

private:
    State &state;

    hd44780_I2Cexp lcd;
    Timer timer;
    char lcdBuffer[20];
    Config& config;

    void showMeasurementPage(long rotaryPos);
    void showMeasurement(const char *id, EnvironmentInfo envInfo);
    void showState();

    void showMenu(long rotaryPos);

    void clearBuffer();
    void lightOn();

    bool lightIsOn = true;
    unsigned long lightOnTime;
    unsigned long lightOnLimit = 15000;

    long lastRotaryPos = 0;
    DisplayMode::Mode mode = DisplayMode::Measurement;

    MenuItem* currentMenuItem;
    const size_t menuItemCount = 11;
    MenuItem menuItems[11];
};

#endif
