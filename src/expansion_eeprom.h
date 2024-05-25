#pragma once

#include <Wire.h>
#include <tuple>
#include <vector>

class ExpansionEeprom {
public:
    ExpansionEeprom(int addr, gpio_num_t sda, gpio_num_t scl);
    virtual ~ExpansionEeprom() = default;

    void write(char daddr, char data) const;
    // Optional is only available in c++ 17+
    std::tuple<bool, char> read(char daddr) const;

    // Docs
    static const int DEFAULT_ADDR = 0b1010000;

private:
    int addr;

};

