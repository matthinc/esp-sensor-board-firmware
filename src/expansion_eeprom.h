#pragma once

#include <Wire.h>
#include <tuple>
#include <vector>

class ExpansionEeprom {
public:
    ExpansionEeprom(int addr, gpio_num_t sda, gpio_num_t scl);
    virtual ~ExpansionEeprom() = default;

    int write(int daddr, char data) const;
    int write(int daddr, int data) const;

    // Optional is only available in c++ 17+
    std::tuple<bool, char> read(int daddr) const;
    std::tuple<bool, int> readInt(int daddr) const;



    // Docs
    static const int DEFAULT_ADDR = 0b1010000;

private:
    int addr;

};

