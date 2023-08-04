#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <avr/wdt.h>

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#include <Encoder.h>
#include <EasyButton.h>

const int LCD_COLS = 16;
const int LCD_ROWS = 2;

const int CLK = 3;
const int DT = 2;
const int SW = 4;

Encoder encoder(DT, CLK);
EasyButton button(SW);
hd44780_I2Cexp lcd;
DHT_Unified dhtIn(6, DHT22);
DHT_Unified dhtOut(7, DHT22);

void setup()
{
    wdt_enable(WDTO_2S); // Set watchdog to 2 seconds

    Serial.begin(115200);
    Serial.println("Starting");

    auto status = lcd.begin(LCD_COLS, LCD_ROWS);
    if (status)
        hd44780::fatalError(status);

    byte deg[8] = {B00111, B00101, B00111, B0000, B00000, B00000, B00000, B00000}; // character °
    lcd.createChar(0, deg);
    byte tau[8] = {B00000, B00000, B00000, B11111, B00100, B00100, B00100, B00110}; // character tau
    lcd.createChar(1, tau);

    dhtIn.begin();
    dhtOut.begin();

    button.begin();
    button.onPressedFor(500, onPressedForDuration);
    button.onPressed(onPressed);
}

unsigned int period_check = 500;
unsigned long lastMs_check = -600;
unsigned long lastLoop = 0;

long oldPosition = -999;

void onPressedForDuration()
{
    Serial.println("Button has been pressed for the given duration!");
}
void onPressed()
{
    Serial.println("Button has been pressed!");
}

void loop()
{
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    long newPosition = encoder.read();

    if (millis() - lastMs_check > period_check)
    {
        lastMs_check = millis();
        auto tempIn = GetTemp(dhtIn);
        auto humIn = GetHumidity(dhtIn);
        auto tempOut = GetTemp(dhtOut);
        auto humOut = GetHumidity(dhtOut);
        auto dewIn = taupunkt(tempIn, humIn);
        auto dewOut = taupunkt(tempOut, humOut);

        lcd.setCursor(0, 0);
        lcd.print("I ");
        lcd.print(tempIn, 1);
        lcd.write((uint8_t)0);
        lcd.write(" ");
        lcd.print(humIn, 0);
        lcd.print("% ");
        lcd.write((uint8_t)1);
        lcd.print(dewIn, 1);
        lcd.write((uint8_t)0);

        lcd.setCursor(0, 1);
        lcd.print("A ");
        lcd.print(tempOut, 1);
        lcd.write((uint8_t)0);
        lcd.write(" ");
        lcd.print(humOut, 0);
        lcd.print("% ");
        lcd.write((uint8_t)1);
        lcd.print(dewOut, 1);
        lcd.write((uint8_t)0);
    }

    if (newPosition != oldPosition)
    {
        oldPosition = newPosition;
        Serial.println(newPosition);
    }

    auto now = millis();
    auto diff = now - lastLoop;
    if (diff > 10)
        Serial.println(diff);
    lastLoop = now;
}

float GetTemp(DHT_Unified &dht)
{
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    return event.temperature;
}

float GetHumidity(DHT_Unified &dht)
{
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    return event.relative_humidity;
}

float taupunkt(float t, float r)
{
    float a, b;

    if (t >= 0)
    {
        a = 7.5f;
        b = 237.3f;
    }
    else if (t < 0)
    {
        a = 7.6f;
        b = 240.7f;
    }

    // Sättigungsdampfdruck in hPa
    float sdd = 6.1078f * pow(10, (a * t) / (b + t));

    // Dampfdruck in hPa
    float dd = sdd * r * 0.01;

    // v-Parameter
    float v = log10(dd / 6.1078f);

    // Taupunkttemperatur (°C)
    float tt = (b * v) / (a - v);
    return tt;
}
