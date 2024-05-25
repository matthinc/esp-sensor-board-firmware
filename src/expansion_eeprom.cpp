#include "expansion_eeprom.h"

ExpansionEeprom::ExpansionEeprom(int addr, gpio_num_t sda, gpio_num_t scl) : addr { addr }
{
    Wire.begin(sda, scl);
}

void ExpansionEeprom::write(char daddr, char data) const
{
    Wire.beginTransmission(addr);
    Wire.write(daddr);
    Wire.write(data);
    Wire.endTransmission();

    delay(100);
}

std::tuple<bool, char> ExpansionEeprom::read(char daddr) const
{
    Wire.beginTransmission(addr);
    Wire.write(daddr);
    Wire.endTransmission();

    Wire.requestFrom(addr, 1);

    if(Wire.available())
    {
        return { true, Wire.read() };
    }

    return { false, 0 };
}