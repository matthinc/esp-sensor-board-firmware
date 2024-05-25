#include "DS18B20.h"



DS18B20::DS18B20(gpio_num_t gpio) : pin { gpio }, oneWire { static_cast<unsigned char>(gpio) }, dallas { &oneWire }
{

}

void DS18B20::init() {
    pinMode(pin, INPUT_PULLUP);
    dallas.begin();
}

void DS18B20::update() {
    dallas.requestTemperatures();
    lastReading = dallas.getTempCByIndex(0);
}

bool DS18B20::hasValue() const {
    return lastReading > -127;
}

double DS18B20::getValue() const {
    return lastReading;
}
