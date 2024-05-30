#include <Arduino.h>
#include "IntervalSleeper.h"

IntervalSleeper::IntervalSleeper(int64_t intervalLengthSec): intervalLengthSec{intervalLengthSec}
{
    Serial.printf("Build Sleeper with time %d\n", this->intervalLengthSec);
}

std::uint64_t IntervalSleeper::getSleepTime() const
{
    return (std::uint64_t) (intervalLengthSec * 1000);
}
