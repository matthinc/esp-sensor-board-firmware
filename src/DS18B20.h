#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20 {
public:
    DS18B20(gpio_num_t gpio);
    virtual ~DS18B20() = default;

    float getTemperature(int index = 0);

private:
    OneWire oneWire;
    DallasTemperature dallas;

};
