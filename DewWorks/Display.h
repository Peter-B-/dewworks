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
    MenuItem(String name, float &value, float min, float max, float factor);
    
    void select(long rotaryPos);

    void update(long rotrayPos);
    void printHeader(char* buffer);
    void printValue(char* buffer);

private:
    String name;
    float& value;
    float initialValue;
    float min, max, factor;
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
    char lcdBuffer[40];
    char numberBuffer[10];

    void showMeasurementPage(long rotaryPos);
    void showMeasurement(char *id, EnvironmentInfo envInfo);
    void showState();

    void showMenu(long rotaryPos);

    void clearBuffer();
    void lightOn();

    bool lightIsOn = true;
    unsigned long lightOnTime;
    unsigned long lightOnLimit = 15000;

    long lastRotaryPos = -1;
    DisplayMode::Mode mode = DisplayMode::Measurement;

    MenuItem* currentMenuItem;
    MenuItem menuItems[4];
};

#endif
