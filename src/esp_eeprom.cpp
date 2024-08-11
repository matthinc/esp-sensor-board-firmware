#include "esp_eeprom.h"

#include <EEPROM.h>

EspEeprom::EspEeprom(std::initializer_list<std::tuple<std::string, uint32_t>> layout) {
    uint32_t size = 0;
    for (const auto& [key, length] : layout) {
        dataAddr[key] = size;
        size += length;
    }

    EEPROM.begin(size);
}

void EspEeprom::storeValue(std::string key, std::string value) {
    if (dataAddr.count(key)) {
        EEPROM.writeString(dataAddr.at(key), value.c_str());
        EEPROM.commit();
    }
}

std::string EspEeprom::readValue(std::string key) {
    if (dataAddr.count(key)) {
        return { EEPROM.readString(dataAddr.at(key)).c_str() };
    }
    return {};
}