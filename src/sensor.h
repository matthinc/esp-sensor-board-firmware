#pragma once

#include <cstdint>

class SleepBehavior
{
public:
    SleepBehavior() = default;
    ~SleepBehavior() = default;

    virtual std::uint64_t getSleepTime() const = 0;
};

class Sensor
{
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual void init() = 0;
    virtual void update() = 0;
    virtual bool hasValue() const = 0;
    virtual double getValue() const = 0;
};