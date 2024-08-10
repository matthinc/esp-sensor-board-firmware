#include <Arduino.h>
#include <memory>
#include <WiFiManager.h>
#include <EEPROM.h>

#include "SensorRegistry.h"
#include "constants.h"
#include "expansion_eeprom.h"
#include "sensors/DS18B20.h"
#include "sleep_behaviors/IntervalSleeper.h"
#include "SensorData.h"
#include "utils.h"
#include "mqtt.h"
#include "esp_eeprom.h"

std::shared_ptr<ExpansionEeprom> eeprom;
std::unique_ptr<Sensor> attachedSensor;
std::unique_ptr<SleepBehavior> sleeper;
std::unique_ptr<WiFiManager> wifiManager;
std::unique_ptr<Mqtt> mqtt;

#define FLASH_SENSOR_TYPE SENSOR_TYPE_DS18B20

std::unique_ptr<SensorRegistry> sensorRegistry = std::unique_ptr<SensorRegistry> { new SensorRegistry() } ;
std::unique_ptr<SleepBehaviorRegistry> sleeperRegistry = std::unique_ptr<SleepBehaviorRegistry> { new SleepBehaviorRegistry() } ;

WiFiClient client;

void setup()
{
    pinMode(5, OUTPUT);
    ledOn();

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
    eeprom = std::make_shared<ExpansionEeprom>( ExpansionEeprom::DEFAULT_ADDR, GPIO_NUM_19, GPIO_NUM_18 );

    // Wifi
    wifiManager = std::unique_ptr<WiFiManager>{ new WiFiManager {} };

    auto mqttServerHost = WiFiManagerParameter{"mqtt_server", "Mqtt Server", "185.244.192.40", 32};
    wifiManager->addParameter(&mqttServerHost);

    auto mqttUsername = WiFiManagerParameter{"mqtt_user", "Mqtt Username", "", 32};
    wifiManager->addParameter(&mqttUsername);

    auto mqttPassword = WiFiManagerParameter{"mqtt_password", "Mqtt Password", "", 32};
    wifiManager->addParameter(&mqttPassword);

    auto mqttBaseTopic = WiFiManagerParameter{"base_topic", "Base Topic", "", 32};
    wifiManager->addParameter(&mqttBaseTopic);

    wifiManager->autoConnect();

    EspEeprom eepromWrapper {
            { "mqtt_host", 32 },
            { "mqtt_username", 32 },
            { "mqtt_password", 32 },
            { "mqtt_topic", 32 },
    };

    if (!std::string{ mqttUsername.getValue() }.empty())
    {
        eepromWrapper.storeValue("mqtt_host", mqttServerHost.getValue());
        eepromWrapper.storeValue("mqtt_username", mqttUsername.getValue());
        eepromWrapper.storeValue("mqtt_password", mqttPassword.getValue());
        eepromWrapper.storeValue("mqtt_topic", mqttBaseTopic.getValue());
        delay(200);
    }

    // Init Mqtt
    mqtt = std::unique_ptr<Mqtt>{
        new Mqtt{
            eepromWrapper.readValue("mqtt_host"),
            eepromWrapper.readValue("mqtt_username"),
            eepromWrapper.readValue("mqtt_password"),
            {"ESP32_" + std::to_string(ESP.getEfuseMac())},
            eepromWrapper.readValue("mqtt_topic"),
            client }};

    #ifdef FLASH_SENSOR_TYPE
    SensorHeader h{SENSOR_TYPE_DS18B20, SLEEP_TYPE_INTERVAL, 100, 131415};
    writeSensorHeaderToEeprom(h, eeprom);
    Serial.println("write sensor id to eprom");
    h.print();
    #endif

    SensorHeader header = readSensorHeader(eeprom);
    header.print();


    // Choose sensor
    bool ret;
    if (auto [ available, data ] = eeprom->read(EEPROM_ADDR_SENSOR_TYPE);
            available && sensorRegistry->hasKey(data) ) {
        std::tie(ret, attachedSensor) = sensorRegistry->createItem(data);
        if (ret){
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
        std::tie(ret, sleeper) = sleeperRegistry->createItem(data);
        if (ret){
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
    ledOff();
}

void loop()
{
    Serial.println("Loop start");
    auto sleep =  sleeper->getSleepTime();
    // auto sleep =  500;
    Serial.printf("sleep for %d ms\n",sleep);

    ledOn();
    attachedSensor->update();
    Serial.println(attachedSensor->getValue());
    mqtt->publish("matthi", attachedSensor->getValue());
    mqtt->loop();
    ledOff();

    delay((uint32_t)sleep);
}