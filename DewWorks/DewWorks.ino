#include <EEPROM.h>
#include <avr/wdt.h>

#include <Encoder.h>    // library Encoder
#include <EasyButton.h> // library EasyButton

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
const int RELAIS = 8;
const int SENSOR_IN = 6;
const int SENSOR_OUT = 7;

Encoder encoder(DT, CLK);
EasyButton button(SW);

DhtSensor dhtIn(SENSOR_IN);
DhtSensor dhtOut(SENSOR_OUT);
Relais relais(RELAIS);

Measurement measurement{};
State state;
Config config;

Display display(state, config);
ControlLogic control(state);

Timer timerMeasure(500);

void setup()
{
    wdt_enable(WDTO_2S); // Set watchdog to 2 seconds

    Serial.begin(115200);
    Serial.println("Starting");

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
    button.onPressedFor(500, onPressedLong);
    button.onPressed(onPressed);
}

void onPressedLong()
{
    display.buttonPressedLong();
}
void onPressed()
{
    display.buttonPressed();
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
    Serial.print("TempInOffset     ");
    formatNumber(config.TempInOffset, 8, 1);
    Serial.println(" °C");
    Serial.print("TempOutOffset    ");
    formatNumber(config.TempOutOffset, 8, 1);
    Serial.println(" °C");
    Serial.print("HumInOffset      ");
    formatNumber(config.HumInOffset, 8, 1);
    Serial.println(" °C");
    Serial.print("HumOutOffset     ");
    formatNumber(config.HumOutOffset, 8, 1);
    Serial.println(" °C");
    Serial.println();
}


int32_t lastPos =0;
void loop()
{
    auto now = millis();
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    int32_t enc = encoder.read();
    int32_t newPosition = enc >> 2;

    if (timerMeasure.ShouldRun(now))
    {
        measurement.Inside = dhtIn.measure();
        measurement.Outside = dhtOut.measure();

         control.setMeasurement(measurement);
         //bool output = control.update();
        relais.set(now % 2 == 0);
     }

    Serial.print(now);
    Serial.print("   ");
    Serial.print(enc);
    Serial.print("   ");
    Serial.println(newPosition);
    if (newPosition != lastPos)
    {
        Serial.println(newPosition);
        lastPos = newPosition;
    }
    //display.update(newPosition);
}
