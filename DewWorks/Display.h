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
    void setMeasurement(Measurement meas);
private:
    hd44780_I2Cexp lcd;
    Measurement currentMeas;
    Timer timer;
    char lcdBuffer[40];
    char numberBuffer[10];
    void showMeasurement(String id, SensorMeasurement meas);

};

#endif
