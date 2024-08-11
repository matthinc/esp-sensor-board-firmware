#include <Arduino.h>
#include <memory>
#include <WiFiManager.h>

#include "sensor_registry.h"
#include "eeprom/expansion_eeprom.h"
#include "eeprom/eeprom_state.h"
#include "sensors/DS18B20.h"
#include "sleep_behaviors/IntervalSleeper.h"
#include "constants.h"
#include "utils.h"
#include "mqtt.h"
#include "sensor.h"
#include "esp_eeprom.h"

std::shared_ptr<ExpansionEeprom> eeprom;
std::shared_ptr<EepromState> eepromState;
std::unique_ptr<BaseSensor> attachedSensor;
std::unique_ptr<SleepBehavior> sleeper;
std::unique_ptr<WiFiManager> wifiManager;
std::shared_ptr<Mqtt> mqtt;

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
    #ifdef FLASH_SENSOR_TYPE
    Serial.println("write sensor id to eprom");
    EepromState h{SENSOR_TYPE_DS18B20, SLEEP_TYPE_INTERVAL, 600, 15, 0};
    h.writeSensorHeaderToEeprom(eeprom);
    h.print();
    #endif

    Serial.println("read saved Sensor state..");
    eepromState = std::make_shared<EepromState>(eeprom);
    Serial.print("found sensor state: ");
    eepromState->print();
    //todo: if sensor state is invalid open Webserver for config


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
    mqtt = std::shared_ptr<Mqtt>{
        new Mqtt{
            eepromWrapper.readValue("mqtt_host"),
            eepromWrapper.readValue("mqtt_username"),
            eepromWrapper.readValue("mqtt_password"),
            {"ESP32_" + std::to_string(ESP.getEfuseMac())},
            eepromWrapper.readValue("mqtt_topic"),
            client }};

    // Choose sensor
    bool ret;
    std::tie(ret, attachedSensor) = sensorRegistry->createItem(eepromState->getSensorId());
    if (!ret){
        Serial.printf("error: can not create sensor with sensorId: %d based on sensor state: ", eepromState->getSensorId());
        eepromState->print();
    }
    std::tie(ret, sleeper) = sleeperRegistry->createItem(eepromState->getSleeperId());
    if (!ret){
        Serial.printf("error: can not create sleeper with sleeperId: %d based on sensor state: ", eepromState->getSensorId());
        eepromState->print();
    }

    attachedSensor->init();
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
    attachedSensor->printCurrentState();
    Serial.printf("loop: save last value...\n");
    attachedSensor->save_current_value(eeprom, eepromState);
    Serial.printf("loop: save last done...\n");
    attachedSensor->printFullState(eeprom, eepromState);
    attachedSensor->send_mqtt(mqtt, eeprom, eepromState);
    ledOff();

    delay((uint32_t)sleep);
}