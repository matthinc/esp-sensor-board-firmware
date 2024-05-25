#include "DS18B20.h"



DS18B20::DS18B20(gpio_num_t gpio) : oneWire { static_cast<unsigned char>(gpio) }, dallas { &oneWire }
{
    pinMode(gpio, INPUT_PULLUP);
    dallas.begin();
}

float DS18B20::getTemperature(int index)
{
    dallas.requestTemperatures();
    return dallas.getTempCByIndex(index);
}