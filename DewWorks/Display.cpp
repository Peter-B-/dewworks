#include "Display.h"

Display::Display(State &state, Config &config)
    : timer(1000),
      state(state),
      menuItems({
          MenuItem("d Temp Innen", config.TempInOffset, -5.0, 5.0, 0.1),
          MenuItem("d Temp Aussen", config.TempOutOffset, -5.0, 5.0, 0.1),
          MenuItem("d Feuchte Inn.", config.HumInOffset, -10, 10.0, 1),
          MenuItem("d Feuchte Aus.", config.HumOutOffset, -10.0, 10.0, 1),
      })
{
    currentMenuItem = &menuItems[0];
    currentMenuItem->select(lastRotaryPos);
};

void Display::begin()
{
    auto status = lcd.begin(16, 2);
    if (status)
        hd44780::fatalError(status);

    byte deg[8] = {
        B00111,
        B00101,
        B00111,
        B0000,
        B00000,
        B00000,
        B00000,
        B00000}; // character °
    lcd.createChar(0, deg);

    byte tau[8] = {
        B00000,
        B00000,
        B00000,
        B11111,
        B00100,
        B00100,
        B00100,
        B00110}; // character tau
    lcd.createChar(1, tau);

    byte left[8] = {
        B00001,
        B00011,
        B00111,
        B01111,
        B01111,
        B00111,
        B00011,
        B00001}; // left arrow
    lcd.createChar(2, left);

    byte right[8] = {
        B10000,
        B11000,
        B11100,
        B11110,
        B11110,
        B11100,
        B11000,
        B10000}; // right arrow
    lcd.createChar(3, right);
}

void Display::update(long rotaryPos)
{
    auto now = millis();
    bool shouldRefresh = timer.ShouldRun(now);
    bool rotationInput = rotaryPos != lastRotaryPos;

    if (shouldRefresh)
    {
        if (lightIsOn && now - lightOnTime > lightOnLimit)
        {
            lcd.noBacklight();
            lightIsOn = false;
        }
    }

    if (rotationInput)
    {
        lastRotaryPos = rotaryPos;
        lightOn();

        Serial.println(rotaryPos);
    }

    if (rotationInput && mode == DisplayMode::Menu)
    {
        currentMenuItem = &menuItems[rotaryPos % menuItemCount];
        currentMenuItem->select(rotaryPos);
    }

    if (shouldRefresh || rotationInput)
    {
        switch (mode)
        {
        case DisplayMode::Menu:
        case DisplayMode::ValueChange:
            showMenu(rotaryPos);
            break;

        case DisplayMode::Measurement:
        default:
            showMeasurementPage(rotaryPos);
            break;
        }
    }
}

void Display::buttonPressed()
{
    Serial.println("pressed");

    if (mode == DisplayMode::Menu)
        mode = DisplayMode::ValueChange;
    else if (mode == DisplayMode::ValueChange)
        mode = DisplayMode::Menu;
}

void Display::buttonPressedLong()
{
    Serial.println("long pressed");

    if (mode == DisplayMode::Measurement)
        mode = DisplayMode::Menu;
    else if (mode == DisplayMode::Menu)
        mode = DisplayMode::Measurement;
}

void Display::lightOn()
{
    lcd.backlight();
    lightIsOn = true;
    lightOnTime = millis();
}

void printNumber(char *dest, float no, int8_t digitsBeforeDot, int8_t digitsAfterDot)
{
    char buffer[40];

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

void Display::showMeasurementPage(long rotaryPos)
{
    int page = rotaryPos % 3;

    if (page == 0)
        showMeasurement("Innen", this->state.Input.Inside);
    else if (page == 1)
        showMeasurement("Aussen", this->state.Input.Outside);
    else
        showState();
}

void Display::showState()
{
    clearBuffer();
    if (state.Output.State)
        strncpy(lcdBuffer, "An", 2);
    else
        strncpy(lcdBuffer, "Aus", 3);

    lcd.setCursor(0, 0);
    lcd.write(lcdBuffer, 16);

    clearBuffer();
    strncpy(lcdBuffer, state.Output.Reason, strlen(state.Output.Reason));
    lcd.setCursor(0, 1);
    lcd.write(lcdBuffer, 16);
}

void Display::showMenu(long rotaryPos)
{

    clearBuffer();
    currentMenuItem->printHeader(lcdBuffer);
    lcd.setCursor(0, 0);
    lcd.write(lcdBuffer, 16);

    clearBuffer();
    currentMenuItem->printValue(lcdBuffer);

    if (mode == DisplayMode::ValueChange)
        {
            // Show arrows
            lcdBuffer[0] = 3;
            lcdBuffer[15] = 2;
        }

    lcd.setCursor(0, 1);
    lcd.write(lcdBuffer, 16);
}

void Display::clearBuffer()
{
    for (size_t i = 0; i < 16; i++)
        lcdBuffer[i] = ' ';
}

MenuItem::MenuItem(const char *name, float &value, float min, float max, float factor) : value(value)
{
    this->name = name;
    this->min = min;
    this->max = max;
    this->factor = factor;
}

void MenuItem::select(long rotaryPos)
{
    this->initialRotaryPos = initialRotaryPos;
    this->initialValue = value;
}

void MenuItem::update(long rotrayPos)
{
    float v = (rotrayPos - initialRotaryPos) * factor + initialValue;

    if (v > max)
        v = max;
    if (v < min)
        v = min;

    value = v;
}

void MenuItem::printHeader(char *buffer)
{
    strncpy(buffer, name, strlen(name));
}

void MenuItem::printValue(char *buffer)
{
    printNumber(buffer, value, 5, 1);
}
