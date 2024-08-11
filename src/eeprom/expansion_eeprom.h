#pragma once

#include <Wire.h>
#include <tuple>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <Arduino.h>

class ExpansionEeprom {
public:
    ExpansionEeprom(int addr, gpio_num_t sda, gpio_num_t scl);
    virtual ~ExpansionEeprom() = default;

    int write_byte(int daddr, char data) const;

    // Optional is only available in c++ 17+
    std::tuple<bool, char> read(int daddr) const;

    template <typename SensorValue, typename std::enable_if<std::is_arithmetic<SensorValue>::value, SensorValue>::type* = nullptr>
    int write(const int &daddr, const SensorValue &data){
        constexpr const int numBytes = sizeof(SensorValue) / sizeof(u_int8_t);
        u_int8_t bytesData[numBytes];

        std::copy(reinterpret_cast<const u_int8_t*>(&data), reinterpret_cast<const u_int8_t*>(&data) + sizeof(SensorValue), &bytesData[0]);

        // Serial.printf("write %.2f with %d bytes to address %d\n", data, numBytes, daddr);
        for(int i=0; i< numBytes; i++){
            write_byte(daddr+i, bytesData[i]);
            Serial.printf("i: %d write %d into addr: %d \n", i, bytesData[i], daddr+i);
        }

        return daddr + numBytes;
    }

    template <typename SensorValue, typename std::enable_if<std::is_arithmetic<SensorValue>::value, SensorValue>::type* = nullptr>
    std::tuple<bool, SensorValue> read(const int &daddr) const{

        constexpr const int numBytes = sizeof(SensorValue) / sizeof(u_int8_t);
        u_int8_t val[numBytes];

        // Serial.printf("read %d bytes\n", numBytes);
        for(int i=0; i< numBytes; i++){
            if( auto [ret, data] = read(daddr + i); ret){
                val[i] = static_cast<uint8_t>(data);
                // Serial.printf("i: %d read %d\n", i, val[i]);
            } else {
                Serial.printf("error reading from addr [%d, %d] at byte %d\n", daddr, daddr+numBytes-1, i);
                return {false, static_cast<SensorValue>(-1)};
            }
        }
        SensorValue retVal;
        std::copy(std::begin(val), std::end(val), reinterpret_cast<uint8_t*>(&retVal));
        // Serial.printf("value read from addr [%d, %d] read %d\n", daddr, daddr+numBytes-1, retVal);
        return {true, retVal};
    }

    template <typename SensorValue, typename std::enable_if<std::is_arithmetic<SensorValue>::value, SensorValue>::type* = nullptr>
    std::vector<SensorValue> read_vector(const int &daddr, const size_t num) const {
        constexpr const int numBytes = sizeof(SensorValue);
        std::vector<SensorValue> retValue;
        retValue.reserve(num);

        // Serial.printf("read_vector: with %d elements (%d bytes each) with starting address %d\n", num, numBytes, daddr);
        for(int i=0; i< num; i++){
            if(auto [ret, data] = read<SensorValue>(daddr + i*numBytes); ret){
                // Serial.printf("i: %d read %d bytes from address %d which is %f\n", i, sizeof(SensorValue), daddr+i*numBytes, data);
                retValue.emplace_back(data);
            } else {
                Serial.printf("i: %d cannot read % bytes from address %d\n", i, sizeof(SensorValue), daddr+i*numBytes);
                return {};
            }
        }
        return retValue;
    }

    // Docs
    static const int DEFAULT_ADDR = 0b1010000;

private:
    int addr;

};

