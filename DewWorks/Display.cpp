#include "Display.h"

Display::Display(State &state) : timer(300), state(state)
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

void Display::update(long rotaryPos)
{
    auto now = millis();
    bool shouldRefresh = timer.ShouldRun(now);
    if (shouldRefresh)
    {
        if (lightIsOn && now - lightOnTime > lightOnLimit)
        {
            lcd.noBacklight();
            lightIsOn = false;
        }
    }

    if (rotaryPos != lastRotaryPos || shouldRefresh)
    {
        lastRotaryPos = rotaryPos;
        lightOn();

        if (mode == DisplayMode::Measurement)
        {
            int page = rotaryPos % 3;
            if (page == 0)
                showMeasurement("Innen", this->state.Input.Inside);
            else
                showMeasurement("Aussen", this->state.Input.Outside);
        }

        Serial.println(rotaryPos);
    }
}

void Display::buttonPressed()
{
    Serial.println("Short press");
}

void Display::buttonPressedLong()
{
    Serial.println("Long press");
}

void Display::lightOn()
{
    lcd.backlight();
    lightIsOn = true;
    lightOnTime = millis();
}

void printNumber(char *dest, float no, int8_t digitsBeforeDot, int8_t digitsAfterDot)
{
    char buffer[20];

    int8_t size;
    if (digitsAfterDot == 0)
        size = digitsBeforeDot;
    else
        size = digitsBeforeDot + digitsAfterDot + 1;

    dtostrf(no, size, digitsAfterDot, buffer);
    strncpy(dest, buffer, size);
}

void Display::showMeasurement(char *id, EnvironmentInfo ei)
{
    clearBuffer();

    strncpy(lcdBuffer, id, strlen(id));
    printNumber(lcdBuffer + 9, ei.DewPointTemperature, 3, 1);
    lcdBuffer[8] = 1;
    lcdBuffer[14] = 0;
    lcdBuffer[15] = 'C';

    lcd.setCursor(0, 0);
    lcd.write(lcdBuffer, 16);

    clearBuffer();

    lcdBuffer[0] = 'r';
    lcdBuffer[1] = 'H';
    printNumber(lcdBuffer + 2, ei.Humidity, 3, 0);
    lcdBuffer[5] = '%';

    lcdBuffer[8] = 'T';
    printNumber(lcdBuffer + 9, ei.Temperature, 3, 1);
    lcdBuffer[14] = 0;
    lcdBuffer[15] = 'C';

    lcd.setCursor(0, 1);
    lcd.write(lcdBuffer, 16);
}

void Display::clearBuffer()
{
    for (size_t i = 0; i < 16; i++)
        lcdBuffer[i] = ' ';
}
