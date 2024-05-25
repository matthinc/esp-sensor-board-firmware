#pragma once

#include <Arduino.h>
#include <cstdint>

namespace {

    void deep_sleep_for(std::uint32_t duration) {
        esp_sleep_enable_timer_wakeup(duration * 1000000);
        esp_deep_sleep_start();
    }

}