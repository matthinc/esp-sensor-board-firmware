#include <Arduino.h>
#include "eeprom/eeprom_state.h"
#include "constants.h"

void EepromState::print() const {
    Serial.printf("SensorHeader[sensorId: %d, sleeperId: %d, sendInterval: %d, maxQueueLength: %d, dataQueueLength: %d]\n", sensorId, sleeperId, sendInterval, maxQueueLength, dataQueueLength);
}

int EepromState::getValueListStartAddress() const {
    return  eeprom_start_offset + 2*sizeof(char) + 2*sizeof(int) + sizeof(TimeStamp);
}

EepromState::EepromState(std::shared_ptr<ExpansionEeprom> eeprom){
    auto current_addr = eeprom_start_offset;
    std::tie(sensorId, current_addr) = get_or_error<char>(eeprom, current_addr);
    std::tie(sleeperId, current_addr) = get_or_error<char>(eeprom, current_addr);
    std::tie(sendInterval, current_addr) = get_or_default<int>(eeprom, current_addr, -1);
    std::tie(maxQueueLength, current_addr) = get_or_default<int>(eeprom, current_addr, 3);
    std::tie(dataQueueLength, current_addr) = get_or_default<int>(eeprom, current_addr, 0);
    // std::tie(lastSuccessfulConnection, current_addr) = get_or_error<TimeStamp>(eeprom, current_addr);
}

void EepromState::writeSensorHeaderToEeprom(std::shared_ptr<ExpansionEeprom> eeprom) const {
    Serial.println("write1");
    auto current_addr = eeprom->write_byte(eeprom_start_offset, sensorId);
    current_addr = eeprom->write_byte(current_addr, sleeperId);
    current_addr = eeprom->write<int>(current_addr, sendInterval);
    current_addr = eeprom->write<int>(current_addr, maxQueueLength);
    current_addr = eeprom->write<int>(current_addr, dataQueueLength);
}


bool EepromState::queueFull() const {
    return maxQueueLength == dataQueueLength;
}

void EepromState::incrQueue() {
    dataQueueLength = (dataQueueLength + 1) % maxQueueLength;
}

const int EepromState::getQueueLength() const{
    return dataQueueLength;
}