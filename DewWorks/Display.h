#ifndef _Display_h
#define _Display_h

#include "Types.h"
#include "Tools.h"

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

namespace DisplayMode{
    enum Mode{
        Measurement,
        Menu,
        ValueChange
    };
}

class Display
{
public:
    Display();
    void begin();
    void update(long rotaryPos);
    void setMeasurement(ControlInput ci);

    void buttonPressed();
    void buttonPressedLong();
    
private:
    hd44780_I2Cexp lcd;
    ControlInput currentMeas;
    Timer timer;
    char lcdBuffer[40];
    char numberBuffer[10];

    void showMeasurement(char *id, EnvironmentInfo  envInfo);
    void clearBuffer();
    void lightOn();
    
    bool lightIsOn = true;
    unsigned long lightOnTime;
    unsigned long lightOnLimit = 15000;

    long lastRotaryPos=-1;
    DisplayMode::Mode mode = DisplayMode::Measurement;
};



#endif
