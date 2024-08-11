#pragma once

#include <chrono>
#include <memory>

#include "eeprom/expansion_eeprom.h"
#include <exception>

using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;

class EepromState {

public:
  EepromState() = default;
  EepromState(char sensorId, char sleeperId, int sendInterval, int maxQueueLength, int dataQueueLength)
      : sensorId{sensorId}, sleeperId{sleeperId},
         sendInterval{sendInterval}, maxQueueLength(maxQueueLength),  dataQueueLength{dataQueueLength} {}
  EepromState(std::shared_ptr<ExpansionEeprom> eeprom);

  void print() const;
  void writeSensorHeaderToEeprom(std::shared_ptr<ExpansionEeprom> eeprom) const;

  char getSensorId() const { return sensorId;}
  char getSleeperId() const { return sleeperId;}
  int getValueListStartAddress() const;
  bool queueFull() const;
  const int getQueueLength() const;

  void updateLastSuccessfulConnection(const TimeStamp &time, std::shared_ptr<ExpansionEeprom> eeprom) const;

  int nextQueueAddress() const;
  void incrQueue();



private:

    template <typename SensorValue>
    std::tuple<SensorValue, int> get_or_default(std::shared_ptr<ExpansionEeprom> eeprom, const int &address, SensorValue&& defaultValue){
        if (auto [ret, data] = eeprom->read<SensorValue>(address); ret){
            Serial.printf("read %d bytes from addr: %d\n", sizeof(SensorValue), (int)address);
            return {data, address + sizeof(SensorValue)};
        } else {
            Serial.printf("error reading %d bytes from addr: %d\n", sizeof(SensorValue), (int)address);
            return {defaultValue, address + sizeof(SensorValue)};
        }
    }

    template <typename SensorValue>
    std::tuple<SensorValue, int> get_or_error(std::shared_ptr<ExpansionEeprom> eeprom, const int &address){
        if (auto [ret, data] = eeprom->read<SensorValue>(address); ret){
            Serial.printf("read %d bytes from addr: %d\n", sizeof(SensorValue), (int)address);
            return {data, address + sizeof(SensorValue)};
        } else {
            Serial.printf("error reading %d bytes from addr: %d\n", sizeof(SensorValue), (int)address);
            assert(false);
        }
    }

private:
  int eeprom_start_offset = 0;
  char sensorId = 0 ;
  char sleeperId = 0;
  int sendInterval = - 1;
  int maxQueueLength = 1;
  int dataQueueLength = 0;
  TimeStamp lastSuccessfulConnection;
};

