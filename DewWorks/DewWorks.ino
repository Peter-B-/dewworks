
#include <EEPROM.h>
#include <avr/wdt.h>

#include <EasyButton.h>   // library EasyButton
#include <RotaryEncoder.h>

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

RotaryEncoder encoder(DT, CLK, RotaryEncoder::LatchMode::FOUR3);
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

void formatNumber(const float input, const byte columns, const byte places)
{
    char buffer[20];
    dtostrf(input, columns, places, buffer);
    Serial.print(buffer);
}

void print(const Config &configuration)
{
    Serial.println();
    Serial.println("Configuration");
    Serial.print("DeltaDewTempMin  ");
    formatNumber(configuration.DeltaDewTempMin, 8, 1);
    Serial.println(" °C");
    Serial.print("DeltaDewTempHyst ");
    formatNumber(configuration.DeltaDewTempHyst, 8, 1);
    Serial.println(" °C");
    Serial.print("HumInMin         ");
    formatNumber(configuration.HumInMin, 8, 0);
    Serial.println(" %");
    Serial.print("HumInHyst        ");
    formatNumber(configuration.HumInHyst, 8, 0);
    Serial.println(" %p");
    Serial.print("TempInMin        ");
    formatNumber(configuration.TempInMin, 8, 1);
    Serial.println(" °C");
    Serial.print("TempOutMin       ");
    formatNumber(configuration.TempOutMin, 8, 1);
    Serial.println(" °C");
    Serial.print("TempHyst         ");
    formatNumber(configuration.TempHyst, 8, 1);
    Serial.println(" °C");
    Serial.println();
    Serial.print("TempInOffset     ");
    formatNumber(configuration.TempInOffset, 8, 1);
    Serial.println(" °C");
    Serial.print("TempOutOffset    ");
    formatNumber(configuration.TempOutOffset, 8, 1);
    Serial.println(" °C");
    Serial.print("HumInOffset      ");
    formatNumber(configuration.HumInOffset, 8, 1);
    Serial.println(" °C");
    Serial.print("HumOutOffset     ");
    formatNumber(configuration.HumOutOffset, 8, 1);
    Serial.println(" °C");
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

void setup()
{
    wdt_enable(WDTO_2S); // Set watchdog to 2 seconds

    Serial.begin(115200);
    Serial.println("Starting");

    EEPROM.get(0, config);
    if (isnanf(config.DeltaDewTempMin))
        config = ControlLogic::getDefaultConfig();
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

void loop()
{
    auto now = millis();
    wdt_reset(); // Watchdog zurücksetzen

    button.read();
    encoder.tick();
    int newPosition = -encoder.getPosition();

    if (timerMeasure.shouldRun(now))
    {
        measurement.Inside = dhtIn.measure();
        measurement.Outside = dhtOut.measure();

        control.setMeasurement(measurement);
        bool output = false;
        output = control.update();
        relais.set(output);
    }
    
    display.update(newPosition);
}
