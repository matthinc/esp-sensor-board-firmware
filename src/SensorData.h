#pragma once

#include <chrono>
#include <memory>

#include "expansion_eeprom.h"
#include "type_traits"

using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;

struct SensorHeader {
    SensorHeader() = default;
    SensorHeader(char sensorId, char slepperId, int sendInterval, int dataQueueLength)
        : sensorId{sensorId}, sleeperId{slepperId},
            sendInterval{sendInterval},  dataQueueLength{dataQueueLength} {}
  char sensorId = 0 ;
  char sleeperId = 0;
  int sendInterval = - 1;
  int dataQueueLength = - 1;

  void print() const;
};

SensorHeader readSensorHeader(std::shared_ptr<ExpansionEeprom> eeprom);
void writeSensorHeaderToEeprom(const SensorHeader& header, std::shared_ptr<ExpansionEeprom> eeprom);
