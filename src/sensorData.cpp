#include <Arduino.h>
#include "SensorData.h"
#include "constants.h"

void SensorHeader::print() const {
    Serial.printf("SensorHeader[sensorId: %d, sleeperId: %d, sendInterval: %d, dataQueueLength: %d]\n", sensorId, sleeperId, sendInterval, dataQueueLength);
}

SensorHeader readSensorHeader(std::shared_ptr<ExpansionEeprom> eeprom){
    int current_addr = EEPROM_ADDR_SENSOR_TYPE;
    SensorHeader h{};
    if (auto [ret, data] = eeprom->read(current_addr); ret){
        h.sensorId = data;
        ++current_addr;
        Serial.printf("read sensorId: %d\n", (int)h.sensorId);
    } else {
        Serial.println("error creating sleeper");
    }

    if (auto [ret, data] = eeprom->read(current_addr); ret){
        h.sleeperId = data;
        ++current_addr;
        Serial.printf("read sleeperId: %d\n", (int)h.sensorId);
    } else {
        Serial.println("error creating sleeper");
    }


    if (auto [ret, data] = eeprom->readInt(current_addr); ret){
        h.sendInterval = data;
        current_addr += sizeof(int)/sizeof(uint8_t);
        Serial.printf("read sendInterval: %d\n", (int)h.sendInterval);
    } else {
        Serial.println("error creating sleeper");
    }

    if (auto [ret, data] = eeprom->readInt(current_addr); ret){
        h.dataQueueLength = data;
        current_addr += sizeof(int)/sizeof(uint8_t);
        Serial.printf("read dataQueueLength: %d\n", (int)h.dataQueueLength);
    } else {
        Serial.println("error creating sleeper");
    }
    return h;
}

void writeSensorHeaderToEeprom(const SensorHeader& header, std::shared_ptr<ExpansionEeprom> eeprom){
    auto current_addr = eeprom->write(EEPROM_ADDR_SENSOR_TYPE, header.sensorId);
    current_addr = eeprom->write(current_addr, header.sleeperId);
    current_addr = eeprom->write(current_addr, header.sendInterval);
    current_addr = eeprom->write(current_addr, header.dataQueueLength);
}