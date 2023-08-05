#include <EEPROM.h>
#include <avr/wdt.h>

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>


#include <Encoder.h>
#include <EasyButton.h>

#include "Types.h"
#include "Hardware.h"
#include "Control.h"

const int LCD_COLS = 16;
const int LCD_ROWS = 2;

const int CLK = 3;
const int DT = 2;
const int SW = 4;

Encoder encoder(DT, CLK);
EasyButton button(SW);
hd44780_I2Cexp lcd;

DhtSensor dhtIn(6);
DhtSensor dhtOut(7);
Relais relais(8);

ControlLogic control;

void setup()
{
    wdt_enable(WDTO_2S); // Set watchdog to 2 seconds

    Serial.begin(115200);
    Serial.println("Starting");

    Config config;
    EEPROM.get(0, config);
    if (isnanf(config.DeltaDewTempMin))
        config = control.getDefaultConfig();
    print(config);

    control.begin(config);

    auto status = lcd.begin(LCD_COLS, LCD_ROWS);
    if (status)
        hd44780::fatalError(status);

    byte deg[8] = {B00111, B00101, B00111, B0000, B00000, B00000, B00000, B00000}; // character °
    lcd.createChar(0, deg);
    byte tau[8] = {B00000, B00000, B00000, B11111, B00100, B00100, B00100, B00110}; // character tau
    lcd.createChar(1, tau);

    dhtIn.begin();
    dhtOut.begin();
    relais.begin();

    button.begin();
    button.onPressedFor(500, onPressedForDuration);
    button.onPressed(onPressed);
}

unsigned int period_check = 2000;
unsigned long lastMs_check = -600;

long oldPosition = -999;

void onPressedForDuration()
{
    Serial.println("Button has been pressed for the given duration!");
}
void onPressed()
{
    Serial.println("Button has been pressed!");
}


void formatNumber(float input, byte columns, byte places)
{
  char buffer[20];
  dtostrf(input, columns, places, buffer);
  Serial.print(buffer);
}

void print(Config &config)
{
    Serial.println();
    Serial.println("Configuration");
    Serial.print("DeltaDewTempMin  ");
    formatNumber(config.DeltaDewTempMin, 8, 1);
    Serial.println(" °C");
    Serial.print("DeltaDewTempHyst ");
    formatNumber(config.DeltaDewTempHyst, 8, 1);
    Serial.println(" °C");
    Serial.print("HumInMin         ");
    formatNumber(config.HumInMin, 8, 0);
    Serial.println(" %");
    Serial.print("HumInHyst        ");
    formatNumber(config.HumInHyst, 8, 0);
    Serial.println(" %p");
    Serial.print("TempInMin        ");
    formatNumber(config.TempInMin, 8, 1);
    Serial.println(" °C");
    Serial.print("TempOutMin       ");
    formatNumber(config.TempOutMin, 8, 1);
    Serial.println(" °C");
    Serial.print("TempHyst         ");
    formatNumber(config.TempHyst, 8, 1);
    Serial.println(" °C");
    Serial.println();
}


Measurement measurement{};

void loop()
{
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    long newPosition = encoder.read();

    if (millis() - lastMs_check > period_check)
    {
        lastMs_check = millis();

        measurement.Inside = dhtIn.measure();
        measurement.Outside = dhtOut.measure();

        auto output = control.update(measurement);
        relais.set(output);

        lcd.setCursor(0, 0);
        lcd.print("Innen  ");
        lcd.write((uint8_t)1);
        lcd.print(measurement.Inside.DewTemperature, 1);
        lcd.write((uint8_t)0);


        lcd.setCursor(0, 1);
        lcd.print(measurement.Inside.Temperature, 1);
        lcd.write((uint8_t)0);
        lcd.write(" ");
        lcd.print(measurement.Inside.Humidity, 0);
        lcd.print("% ");


    }

    if (newPosition != oldPosition)
    {
        oldPosition = newPosition;
        Serial.println(newPosition);
        relais.set(newPosition % 4 == 0);
    }
}

