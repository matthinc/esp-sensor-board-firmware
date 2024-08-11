#include <Arduino.h>
#include "DS18B20.h"
#include "constants.h"
#include <memory>
#include <vector>
#include "eeprom/expansion_eeprom.h"
#include "eeprom/eeprom_state.h"
#include "mqtt.h"

DS18B20::DS18B20(gpio_num_t gpio) : pin { gpio }, oneWire { static_cast<unsigned char>(gpio) }, dallas { &oneWire }
{

}

void DS18B20::init() {
    pinMode(pin, INPUT_PULLUP);
    dallas.begin();
}

void DS18B20::update() {
    dallas.requestTemperatures();
    lastReading = dallas.getTempCByIndex(0);
}

bool DS18B20::hasValue() const {
    return lastReading > -127;
}

double DS18B20::getValue() const {
    return lastReading;
}

void DS18B20::printCurrentState() const {
    Serial.printf("Current Temp: %.2f\n", getValue());
}

void DS18B20::printFullState(std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<const EepromState> state) const{
    auto data = get_saved_values(eeprom, state);
    Serial.printf("Found %d saved values (oldest to newest):\n", data.size());
    Serial.printf("Current Temp: %.2f\n", getValue());
    int i=0;
    for(const auto &val: data){
        Serial.printf("i: %d   Temp: %.2f\n", i, val);
        i++;
    }
}