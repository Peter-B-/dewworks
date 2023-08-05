#include <EEPROM.h>
#include <avr/wdt.h>

#include <Encoder.h>
#include <EasyButton.h>

#include "Types.h"
#include "Hardware.h"
#include "Control.h"
#include "Display.h"
#include "Tools.h"

const int LCD_COLS = 16;
const int LCD_ROWS = 2;

const int CLK = 3;
const int DT = 2;
const int SW = 4;

Encoder encoder(DT, CLK);
EasyButton button(SW);

DhtSensor dhtIn(6);
DhtSensor dhtOut(7);
Relais relais(8);
Display display;

ControlLogic control;

Timer timerMeasure(500);

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
 
    dhtIn.begin();
    dhtOut.begin();
    relais.begin();
    display.begin();

    button.begin();
    button.onPressedFor(500, onPressedForDuration);
    button.onPressed(onPressed);
}


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
    auto now = millis();
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    long newPosition = encoder.read();

    if (timerMeasure.ShouldRun(now))
    {
        measurement.Inside = dhtIn.measure();
        measurement.Outside = dhtOut.measure();

        auto output = control.update(measurement);
        relais.set(output);

        display.setMeasurement(measurement);
    }

    if (newPosition != oldPosition)
    {
        oldPosition = newPosition;

        char lcdBuffer[20];
        for (size_t i = 0; i < 16; i++)
        {
            lcdBuffer[i]='_';
        }
        lcdBuffer[16]='\0';

        auto c = dtostrf(newPosition * 5.1, 5, 1, lcdBuffer + 4 );
        Serial.println(lcdBuffer);
        Serial.println(c);



    }

    display.update();
}

