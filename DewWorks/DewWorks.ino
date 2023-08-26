#include <EEPROM.h>
#include <avr/wdt.h>

#include <EasyButton.h>   // library EasyButton
#include <Encoder.h>      // library Encoder

#include "Types.h"
#include "Hardware.h"
#include "Control.h"
#include "Display.h"
#include "Tools.h"

constexpr int LCD_COLS = 16;
constexpr int LCD_ROWS = 2;

constexpr int CLK = 3;
constexpr int DT = 2;
constexpr int SW = 4;
constexpr int RELAIS = 8;
constexpr int SENSOR_IN = 6;
constexpr int SENSOR_OUT = 7;

Encoder encoder(DT, CLK);
EasyButton button(SW);

DhtSensor dhtIn(SENSOR_IN);
DhtSensor dhtOut(SENSOR_OUT);
Relais relais(RELAIS);

Measurement measurement{};
State state;
Config config;

Display display(state, config);
ControlLogic control(state, config);

Timer timerMeasure(500);

void formatNumber(const float input, const byte columns, const byte places)
{
    char buffer[20];
    dtostrf(input, columns, places, buffer);
    Serial.print(buffer);
}

void print(const Config& configuration)
{
    Serial.println();
    Serial.println(F("Configuration"));
    Serial.print(F("DeltaDewTempMin  "));
    formatNumber(configuration.DeltaDewTempMin, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("DeltaDewTempHyst "));
    formatNumber(configuration.DeltaDewTempHyst, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("HumInMin         "));
    formatNumber(configuration.HumInMin, 8, 0);
    Serial.println(F(" %"));
    Serial.print(F("HumInHyst        "));
    formatNumber(configuration.HumInHyst, 8, 0);
    Serial.println(F(" %p"));
    Serial.print(F("TempInMin        "));
    formatNumber(configuration.TempInMin, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("TempOutMin       "));
    formatNumber(configuration.TempOutMin, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("TempHyst         "));
    formatNumber(configuration.TempHyst, 8, 1);
    Serial.println(F(" °C"));
    Serial.println();
    Serial.print(F("TempInOffset     "));
    formatNumber(configuration.TempInOffset, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("TempOutOffset    "));
    formatNumber(configuration.TempOutOffset, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("HumInOffset      "));
    formatNumber(configuration.HumInOffset, 8, 1);
    Serial.println(F(" °C"));
    Serial.print(F("HumOutOffset     "));
    formatNumber(configuration.HumOutOffset, 8, 1);
    Serial.println(F(" °C"));
    Serial.println();
}

void onPressedLong()
{
    display.buttonPressedLong();
}

void onPressed()
{
    display.buttonPressed();
}

int freeMemory()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == nullptr ? (int)&__heap_start : (int)__brkval);
}


void saveConfig()
{
    Serial.println("Save config");
    EEPROM.put(0, config);
}

void setup()
{
    wdt_enable(WDTO_2S); // Set watchdog to 2 seconds

    Serial.begin(115200);

    EEPROM.get(0, config);
    if (isnanf(config.DeltaDewTempMin))
        config = ControlLogic::getDefaultConfig();

    print(config);

    control.begin();

    dhtIn.begin();
    dhtOut.begin();
    relais.begin();
    display.begin();
    display.onSaveConfig(saveConfig);

    button.begin();
    button.onPressedFor(500, onPressedLong);
    button.onPressed(onPressed);
}


void loop()
{
    auto now = millis();
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    int32_t enc = encoder.read();
    int32_t newPosition = -(enc >> 2);

    display.update(newPosition);

    if (timerMeasure.shouldRun(now))
    {
        // Serial.print(F("free: "));
        // Serial.println(freeMemory());

        measurement.Inside = dhtIn.measure();
        measurement.Outside = dhtOut.measure();

        control.setMeasurement(measurement);
        bool output = false;
        output = control.update();
        relais.set(output);
    }
}
