#include <Arduino.h>
#include <memory>
#include <WiFiManager.h>

#include "constants.h"
#include "expansion_eeprom.h"
#include "sensors/DS18B20.h"
#include "utils.h"

std::unique_ptr<ExpansionEeprom> eeprom;
std::unique_ptr<Sensor> attachedSensor;
std::unique_ptr<WiFiManager> wifiManager;

// #define FLASH_SENSOR_TYPE SENSOR_TYPE_DS18B20

void setup()
{
    // Initialize Hardware
    eeprom = std::unique_ptr<ExpansionEeprom>{
        new ExpansionEeprom { ExpansionEeprom::DEFAULT_ADDR, GPIO_NUM_19, GPIO_NUM_18 } };

    wifiManager = std::unique_ptr<WiFiManager>{ new WiFiManager {} };
    wifiManager->autoConnect();

    #ifdef FLASH_SENSOR_TYPE
    eeprom->write(EEPROM_ADDR_SENSOR_TYPE, FLASH_SENSOR_TYPE);
    #endif

    // Choose sensor
    if (auto [ available, data ] = eeprom->read(EEPROM_ADDR_SENSOR_TYPE);
            available && data == SENSOR_TYPE_DS18B20) {
        attachedSensor = std::unique_ptr<DS18B20> { new DS18B20 { GPIO_NUM_14 } };
    }

    attachedSensor->init();
    Serial.begin(115200);
}

void loop()
{
    delay(500);
    attachedSensor->update();
    Serial.println(attachedSensor->getValue());
}