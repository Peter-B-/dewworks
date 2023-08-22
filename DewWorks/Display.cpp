#include "Display.h"

unsigned getPage(const int value, const unsigned pages)
{
    int mod = value % (int)pages;
    if (mod < 0)
    {
        mod += pages;
    }
    return mod;
}

Display::Display(State &state, Config &config)
    : state(state),
      timer(1000),
      config(config),
      menuItems({
          
          MenuItem("K Temp innen", config.TempInOffset, -5.0, 5.0, 0.1),
          MenuItem("K Temp aussen", config.TempOutOffset, -5.0, 5.0, 0.1),
          MenuItem("K Feuchte innen", config.HumInOffset, -10, 10.0, 1),
          MenuItem("K Feuchte aussen", config.HumOutOffset, -10.0, 10.0, 1),
          MenuItem("\4\1. Min", config.DeltaDewTempMin, 0.0, 10.0, 0.2),
          MenuItem("\4\1. Hysterese", config.DeltaDewTempHyst, 0.0, 5.0, 0.2),
          MenuItem("Feuchte in min", config.HumInMin, 10.0, 80.0, 1),
          MenuItem("Feuchte Hyst", config.HumInHyst, 0.0, 10.0, 0.2),
          MenuItem("Temp innen min", config.TempInMin, -20.0, 25.0, 1),
          MenuItem("Temp aussen min", config.TempOutMin, -20.0, 25.0, 1),
          MenuItem("Temp Hysterese", config.TempHyst, 0.0, 5.0, 0.2),
      }) 
{
    currentMenuItem = &menuItems[0];
    currentMenuItem->select(lastRotaryPos);
};

void Display::begin()
{
    const auto status = lcd.begin(16, 2);
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

    byte delta[8] = {
        B00000,
        B00100,
        B01010,
        B01010,
        B10001,
        B10001,
        B11111,
        B00000}; // Delta
    lcd.createChar(4, delta);
}

void Display::update(const long rotaryPos)
{
    const auto now = millis();
    const bool shouldRefresh = timer.shouldRun(now);
    const bool rotationInput = rotaryPos != lastRotaryPos;

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
        auto page = getPage(rotaryPos, menuItemCount);
        currentMenuItem = &menuItems[page];
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

void printNumber(char *dest, const float no, const int8_t digitsBeforeDot, const int8_t digitsAfterDot)
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

void Display::showMeasurement(const char *id, const EnvironmentInfo envInfo)
{
    clearBuffer();

    strncpy(lcdBuffer, id, strlen(id));
    printNumber(lcdBuffer + 9, envInfo.DewPointTemperature, 3, 1);
    lcdBuffer[8] = 1;
    lcdBuffer[14] = 0;
    lcdBuffer[15] = 'C';

    lcd.setCursor(0, 0);
    lcd.write(lcdBuffer, 16);

    clearBuffer();

    lcdBuffer[0] = 'r';
    lcdBuffer[1] = 'H';
    printNumber(lcdBuffer + 2, envInfo.Humidity, 3, 0);
    lcdBuffer[5] = '%';

    lcdBuffer[8] = 'T';
    printNumber(lcdBuffer + 9, envInfo.Temperature, 3, 1);
    lcdBuffer[14] = 0;
    lcdBuffer[15] = 'C';

    lcd.setCursor(0, 1);
    lcd.write(lcdBuffer, 16);
}

void Display::showMeasurementPage(const long rotaryPos)
{
    const auto page = getPage(rotaryPos, 3);

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
    strncpy(lcdBuffer, state.Output.Reason.c_str(), state.Output.Reason.length());
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

MenuItem::MenuItem(const String& name, float &value, const float min, const float max, const float factor) : value(value), initialValue(0), initialRotaryPos(0)
{
    this->name = name;
    this->min = min;
    this->max = max;
    this->factor = factor;
}

void MenuItem::select(const long rotaryPos)
{
    this->initialRotaryPos = rotaryPos;
    this->initialValue = value;
}

void MenuItem::update(const long rotrayPos) const
{
    float v = static_cast<float>(rotrayPos - initialRotaryPos) * factor + initialValue;

    if (v > max)
        v = max;
    if (v < min)
        v = min;

    value = v;
}

void MenuItem::printHeader(char *buffer) const
{
    strncpy(buffer, name.c_str(), name.length());
}

void MenuItem::printValue(char *buffer) const
{
    printNumber(buffer, value, 5, 1);
}
