#pragma once 

#include "sensor.h"


class IntervalSleeper : public SleepBehavior {
public:
    IntervalSleeper(int64_t intervalLengthSec);

    virtual std::uint64_t getSleepTime() const override;

private:
    int64_t intervalLengthSec;

};