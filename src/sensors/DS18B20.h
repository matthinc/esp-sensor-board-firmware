#pragma once

#include "OneWire.h"
#include "DallasTemperature.h"

#include "sensor.h"


class DS18B20 : public Sensor<double> {
public:
    explicit DS18B20(gpio_num_t gpio);
    ~DS18B20() override = default;

    void init() override;
    void update() override;
    bool hasValue() const override;
    void printCurrentState() const override;
     void printFullState(std::shared_ptr<ExpansionEeprom> eeprom, std::shared_ptr<const EepromState> state) const override;

    double getValue() const override;

private:
    gpio_num_t pin;
    double lastReading = -128;

    OneWire oneWire;
    DallasTemperature dallas;

};
