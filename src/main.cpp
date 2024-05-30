#include <Arduino.h>
#include <memory>
#include <WiFiManager.h>

#include "SensorRegistry.h"
#include "constants.h"
#include "expansion_eeprom.h"
#include "sensors/DS18B20.h"
#include "sleep_behaviors/IntervalSleeper.h"
#include "utils.h"

std::unique_ptr<ExpansionEeprom> eeprom;
std::unique_ptr<Sensor> attachedSensor;
std::unique_ptr<SleepBehavior> sleeper;
std::unique_ptr<WiFiManager> wifiManager;

// #define FLASH_SENSOR_TYPE SENSOR_TYPE_DS18B20

std::unique_ptr<SensorRegistry> sensorRegistry = std::unique_ptr<SensorRegistry> { new SensorRegistry() } ;
std::unique_ptr<SleepBehaviorRegistry> sleeperRegistry = std::unique_ptr<SleepBehaviorRegistry> { new SleepBehaviorRegistry() } ;


void setup()
{
    Serial.begin(115200);
    Serial.printf("Init...\n");
    Serial.printf("pointer: %d\n", (int)&sleeperRegistry);
    sensorRegistry->registerItem(
        SENSOR_TYPE_DS18B20,
        []() { 
                Serial.printf("create sensor\n");
                return std::unique_ptr<DS18B20> { new DS18B20 { GPIO_NUM_14 } }; 
        }
    );   
    sleeperRegistry->registerItem(
        SLEEP_TYPE_INTERVAL,
        []() { 
                Serial.printf("create sleeper\n");
                return std::unique_ptr<IntervalSleeper> { new IntervalSleeper { 5 } }; 
        }
    );


    // Initialize Hardware
    eeprom = std::unique_ptr<ExpansionEeprom>{
        new ExpansionEeprom { ExpansionEeprom::DEFAULT_ADDR, GPIO_NUM_19, GPIO_NUM_18 } };

    // wifiManager = std::unique_ptr<WiFiManager>{ new WiFiManager {} };
    // wifiManager->autoConnect();

    #ifdef FLASH_SENSOR_TYPE
    eeprom->write(EEPROM_ADDR_SENSOR_TYPE, FLASH_SENSOR_TYPE);
    Serial.println("write sensor id to eprom");
    #endif

    // Choose sensor
    if (auto [ available, data ] = eeprom->read(EEPROM_ADDR_SENSOR_TYPE);
            available && sensorRegistry->hasKey(data) ) {
        attachedSensor = sensorRegistry->createItem(data);
        if (attachedSensor){
            Serial.println("all good for sensor");
        } else {
            Serial.println("error creating sensor");
        }
        attachedSensor->init();
    } else {
        // todo: Open webserver to allow user to flash sensor type 
        Serial.println("No sensor found  open webserver");
    }
    
    // Choose sleeper
    if (auto data = SLEEP_TYPE_INTERVAL;
            data>0 && sleeperRegistry->hasKey(data) ) {
        Serial.println("found sleeper in registry. create one");
        sleeper = sleeperRegistry->createItem(data);
        if (sleeper){
            Serial.println("all good for sleeper");
        } else {
            Serial.println("error creating sleeper");
        }
    } else {
        // todo: Open webserver to allow user to flash sensor type 
        Serial.println("No sleeper found  open webserver");
    }

    // attachedSensor->init();
    Serial.println("Init done.");

    Serial.printf("Number of sensors registered: %d\n", sensorRegistry->size());
}

void loop()
{
    Serial.println("Loop start");
    auto sleep =  sleeper->getSleepTime();
    // auto sleep =  500;
    Serial.printf("sleep for %d ms\n",sleep);
    delay((uint32_t)sleep);
    attachedSensor->update();
    Serial.println(attachedSensor->getValue());
}