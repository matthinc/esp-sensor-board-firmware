#pragma once

#include <cstdint>
#include <memory>
#include <vector>

class ExpansionEeprom;
class EepromState;
class Mqtt;

class SleepBehavior
{
public:
    SleepBehavior() = default;
    virtual ~SleepBehavior() = default;

    virtual std::uint64_t getSleepTime() const = 0;
};


class BaseSensor
{
public:
    BaseSensor() = default;
    virtual ~BaseSensor() = default;

    virtual void init() = 0;
    virtual void update() = 0;
    virtual bool hasValue() const = 0;

    virtual void printCurrentState() const = 0;
    virtual void printFullState(std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<const EepromState> state) const = 0;
    virtual void save_current_value(
            std::shared_ptr<ExpansionEeprom> eeprom,
            std::shared_ptr<EepromState> state) const = 0;
    virtual void send_mqtt(std::shared_ptr<Mqtt> mqtt, std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<EepromState> state) = 0;
};

template <typename SensorValue>
class Sensor : public BaseSensor{
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual SensorValue getValue() const = 0;
    virtual std::vector<SensorValue> get_saved_values(std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<const EepromState> state) const;
    virtual void save_current_value(
            std::shared_ptr<ExpansionEeprom> eeprom,
            std::shared_ptr<EepromState> state) const override;
    virtual void send_mqtt(std::shared_ptr<Mqtt> mqtt, std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<EepromState> state) override;
};

#include "sensor.tcc"