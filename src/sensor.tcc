#pragma once

#include "eeprom/eeprom_state.h"
#include "eeprom/expansion_eeprom.h"
#include "mqtt.h"
#include <Arduino.h>


template <typename SensorValue>
inline std::vector<SensorValue> Sensor<SensorValue>::get_saved_values(
    std::shared_ptr<ExpansionEeprom> eeprom,
    std::shared_ptr<const EepromState> state) const{

    return eeprom->read_vector<SensorValue>(state->getValueListStartAddress(), state->getQueueLength());
}

template <typename SensorValue>
inline void Sensor<SensorValue>::save_current_value(
            std::shared_ptr<ExpansionEeprom> eeprom,
            std::shared_ptr<EepromState> state) const {

    auto start_addr = state->getValueListStartAddress() + sizeof(SensorValue)*state->getQueueLength();
    Serial.printf("save_current_value: %.2f to address %d\n", getValue(), start_addr);
    eeprom->write<SensorValue>(start_addr, getValue());
    state->incrQueue();
}



template <typename SensorValue>
inline void Sensor<SensorValue>::send_mqtt(std::shared_ptr<Mqtt> mqtt, std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<EepromState> state) {
    // query time (ntp)

    // send all values saved in eeprom
    // auto sensor_values = get_saved_values(eeprom, state);

    // todo check return type it it worked?
    mqtt->publish("matthi", getValue()); //todo: use last value for now.
    mqtt->loop();

    // state.updateLastSuccessfulConnection(timeStamp)
    // reset EepromState (dataQueue head so that already send values will be overwritten)
}