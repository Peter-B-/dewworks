#include "Display.h"

Display::Display():timer(200)
{
}

void Display::begin()
{
    auto status = lcd.begin(16, 2);
    if (status)
        hd44780::fatalError(status);

    byte deg[8] = {B00111, B00101, B00111, B0000, B00000, B00000, B00000, B00000}; // character °
    lcd.createChar(0, deg);
    byte tau[8] = {B00000, B00000, B00000, B11111, B00100, B00100, B00100, B00110}; // character tau
    lcd.createChar(1, tau);
}

void Display::update()
{
    auto now = millis();
    if (timer.ShouldRun(now))
    {
        int page = (now / 3000) % 2;

        if (page == 0)
            showMeasurement("Innen", this->currentMeas.Inside);
        else
            showMeasurement("Aussen", this->currentMeas.Outside);
    }
}

void Display::setMeasurement(Measurement meas)
{
    this->currentMeas = meas;
}

void Display::showMeasurement(String id, SensorMeasurement meas)
{

    // for (size_t i = 0; i < 16; i++)
    // {
    //     lcdBuffer[i]='_';
    // }
    

    // lcd.setCursor(0, 0);
    // dtostrf(-meas.DewTemperature, 5, 1, lcdBuffer + 4 );
    // lcd.print(lcdBuffer);
    // Serial.println(lcdBuffer);

    
    lcd.write((uint8_t)1);
    lcd.print( meas.DewTemperature, 1);
    lcd.write((uint8_t)0);

    lcd.setCursor(0, 1);
    lcd.print(meas.Temperature, 1);
    lcd.write((uint8_t)0);
    lcd.write(" ");
    lcd.print(meas.Humidity, 0);
    lcd.print("% ");
}
