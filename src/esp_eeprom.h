#pragma once

#include<string>
#include<map>

class EspEeprom {
public:
    EspEeprom(std::initializer_list<std::tuple<std::string, uint32_t>> layout);

    void storeValue(std::string key, std::string value);
    std::string readValue(std::string key);

private:
    std::map<std::string, uint32_t> dataAddr;
};

