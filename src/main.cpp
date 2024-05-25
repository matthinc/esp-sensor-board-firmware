#include <Arduino.h>
#include <memory>

#include "expansion_eeprom.h"
#include "DS18B20.h"
#include "utils.h"

std::unique_ptr<ExpansionEeprom> eeprom;
std::unique_ptr<DS18B20> ds18b20;

enum eeprom_addr_t {
    TEST_DATA = 12
};

void setup()
{
    // Initialize Hardware
    eeprom = std::unique_ptr<ExpansionEeprom>{
        new ExpansionEeprom { ExpansionEeprom::DEFAULT_ADDR, GPIO_NUM_19, GPIO_NUM_18 } };

    ds18b20 = std::unique_ptr<DS18B20>{ new DS18B20 { GPIO_NUM_14 } };

    Serial.begin(115200);
    eeprom->write(TEST_DATA, 69);
}

void loop()
{
    delay(500);

    if (auto [ available, data ] = eeprom->read(TEST_DATA); available)
    {
        Serial.println(static_cast<int>(data));
    }

    Serial.println(ds18b20->getTemperature());
}