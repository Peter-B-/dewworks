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
    MenuItem(const char* name, float& value, float min, float max, float factor);
    MenuItem(const char* name);

    void select(long rotaryPos);

    void update(long rotrayPos) const;
    void printHeader(char* buffer) const;
    void printValue(char* buffer) const;

    const bool isResetDefault;

private:
    const char* name;
    float& value;
    float initialValue;
    float minimum, maximum, factor;
    long initialRotaryPos;
};

class MenuSelector
{
public:
    MenuSelector(unsigned int pages);

    unsigned int getPage(long rotaryPos);
    void setOffset(long rotaryPos);

private:
    long rotaryOffset;
    unsigned page;
    const unsigned int pages;
};


class Display
{
public:
    using callback_t = void(*)();

    Display(State& state, Config& config);
    void begin();
    void selectMenuitem(long rotaryPos);
    void lightOff();
    void update(long rotaryPos);

    void buttonPressed();
    void selectMenu();
    void buttonPressedLong();

    void onSaveConfig(callback_t callback);

private:
    State& state;

    hd44780_I2Cexp lcd;
    Timer timer;
    char lcdBuffer[20];
    Config& config;
    volatile bool buttonWasPressed = false;
    callback_t saveConfigCallback = nullptr;

    void showMeasurementPage(long rotaryPos);
    void showMeasurement(const __FlashStringHelper* id, EnvironmentInfo envInfo);
    void showState();

    void showMenu(long rotaryPos);

    void clearBuffer();
    void lightOn();

    bool lightIsOn = true;
    unsigned long lightOnTime;
    unsigned long lightOnLimit = 60000;

    long lastRotaryPos = 0;
    DisplayMode::Mode mode = DisplayMode::Measurement;

    MenuItem* currentMenuItem;
    const size_t menuItemCount = 12;
    MenuItem menuItems[12];

    MenuSelector menuSelector;
};

#endif
