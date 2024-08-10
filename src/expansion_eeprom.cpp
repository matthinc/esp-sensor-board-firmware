#include "expansion_eeprom.h"
#include <Arduino.h>

ExpansionEeprom::ExpansionEeprom(int addr, gpio_num_t sda, gpio_num_t scl) : addr { addr }
{
    Wire.begin(sda, scl);
}

int ExpansionEeprom::write(int daddr, char data) const
{
    Wire.beginTransmission(addr);
    Wire.write(daddr);
    Wire.write(data);
    Wire.endTransmission();

    delay(100);
    return daddr + 1;
}


int ExpansionEeprom::write(int daddr, int data) const{
    int current_addr = daddr;

    int numBytes = sizeof(data) / sizeof(uint8_t);
    Serial.printf("write %d bytes for value: %d\n", numBytes, data);
    for(int i = 0; i < numBytes; i++){
        char _d = static_cast<char>(data & 0xFF);
        data = data >> 8;

        Serial.printf("%d: write _d: %d with rest data %d into address %d\n", i, _d, data, current_addr);
        current_addr = write(current_addr, _d );
    }
    return current_addr;
}

std::tuple<bool, int> ExpansionEeprom::readInt(int daddr) const{

    int ret = 0;
    int numBytes = sizeof(int) / sizeof(uint8_t);
    Serial.printf("read %d bytes\n", numBytes);
    for(int i = 0; i < numBytes; i++){
        if (auto [ret, data] = read(daddr + i); ret){
            data = data << 8*i;
            ret += data;
            Serial.printf("%d: shifted data %d ret: %d\n", i, data, ret);
        } else {
            return {false, -1};
        }
    }
    return {true, ret};
}

std::tuple<bool, char> ExpansionEeprom::read(int daddr) const
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