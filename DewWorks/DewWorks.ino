#include <EEPROM.h>
#include <avr/wdt.h>

#include <EasyButton.h> // library EasyButton
#include <Encoder.h>    // library Encoder

#include "Types.h"
#include "Hardware.h"
#include "Control.h"
#include "Display.h"
#include "Tools.h"

constexpr int LCD_COLS = 16;
constexpr int LCD_ROWS = 2;

constexpr int ENC_CLK = 3;
constexpr int ENC_DT = 2;
constexpr int ENC_SW = 4;
constexpr int OUTPUT_PIN = 9;
constexpr int SENSOR_IN = 6;
constexpr int SENSOR_OUT = 7;
constexpr int SWITCH = A0;

Measurement measurement{};
State state;
Config config;

Encoder encoder(ENC_DT, ENC_CLK);
EasyButton button(ENC_SW);
DhtSensor dhtIn(SENSOR_IN);
DhtSensor dhtOut(SENSOR_OUT);
ModeSwitch modeSwitch(SWITCH);

Output output(OUTPUT_PIN, config);
Display display(state, config);
ControlLogic control(state, config);

Timer timerMeasure(300);
Timer timerLog(5000);

void formatNumber(const float input, const byte columns, const byte places)
{
    char buffer[20];
    dtostrf(input, columns, places, buffer);
    Serial.print(buffer);
}

void print(const Config &configuration)
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

    Serial.print(F("PWM              "));
    Serial.print(configuration.PwmRatio);
    Serial.println(F("%"));
    Serial.println();
}

void Log()
{
    Serial.println(F("Measurement:"));
    Serial.print(F(" In  T: "));
    formatNumber(measurement.Inside.Temperature, 6, 1);
    Serial.print(F(" °C  H: "));
    formatNumber(measurement.Inside.Humidity, 6, 0);
    Serial.println(F(" %"));

    Serial.print(F(" Out T: "));
    formatNumber(measurement.Outside.Temperature, 6, 1);
    Serial.print(F(" °C  H: "));
    formatNumber(measurement.Outside.Humidity, 6, 0);
    Serial.println(F(" %"));

    Serial.println(state.Output.Reason);

    // Serial.print(F(" Free memory: "));
    // Serial.print(freeMemory());
    // Serial.println(F(" bytes"));
    Serial.println();
}

void onPressedLong()
{
    Serial.println(F("Button pressed long"));
    display.buttonPressedLong();
}

void onPressed()
{
    Serial.println(F("Button pressed"));
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
    Serial.println(F("Save config"));
    EEPROM.put(0, config);
}

void setup()
{
    wdt_enable(WDTO_2S); // Set watchdog to 2 seconds

    Serial.begin(115200);

    EEPROM.get(0, config);
    if (config.Version != CONFIG_VERSION || isnan(config.DeltaDewTempMin))
    {
        Serial.println(F("Invalid or missing config, loading defaults"));
        config = ControlLogic::getDefaultConfig();
        saveConfig();
    }

    print(config);

    control.begin();

    dhtIn.begin();
    dhtOut.begin();
    output.begin();
    modeSwitch.begin();
    display.begin();
    display.onSaveConfig(saveConfig);

    button.begin();
    button.onPressedFor(500, onPressedLong);
    button.onPressed(onPressed);
}

void loop()
{
    const auto now = millis();
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    const auto newPosition = -(encoder.read() >> 1);

    display.update(newPosition);

    if (timerMeasure.shouldRun(now))
    {
        const auto mode = modeSwitch.read();

        measurement.Inside = dhtIn.measure();
        measurement.Outside = dhtOut.measure();

        control.setMeasurement(measurement);
        const auto outputEnabled = control.update(mode);
        output.set(outputEnabled);
    }

    if (timerLog.shouldRun(now))
        Log();
}
