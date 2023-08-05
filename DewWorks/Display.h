#ifndef _Display_h
#define _Display_h

#include "Types.h"
#include "Tools.h"

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

class Display
{
public:
    Display();
    void begin();
    void update();
    void setMeasurement(ControlInput ci);

    void lightOn();
private:
    hd44780_I2Cexp lcd;
    ControlInput currentMeas;
    Timer timer;
    char lcdBuffer[40];
    char numberBuffer[10];

    void showMeasurement(char *id, EnvironmentInfo  envInfo);
    void clearBuffer();
    
    bool lightIsOn = true;
    unsigned long lightOnTime;
};

#endif
