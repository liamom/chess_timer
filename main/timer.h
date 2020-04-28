#pragma once

#include <Arduino.h>
#include "i_update.h"
#include "debounce_timer.h"

class Timer : public IUpdate
{
    DebouncedValue<bool> value_;
    const uint32_t fequency_;
    uint32_t last_emit_ = millis();
    uint32_t start_time_ = last_emit_;

    bool is_paused_ = true;

    static_assert(sizeof(unsigned long) == sizeof(int32_t));

public:
    using call_back_t = void (*)(void*);
    const call_back_t call_back_;
    void* callback_data_;

    Timer(uint32_t fequency, call_back_t callback, void* callback_data)
        : fequency_(fequency)
        , call_back_(callback)
        , callback_data_(callback_data)
    {
    }

    void pause() {
        is_paused_ = true;
    }

    void resume() {
        is_paused_ = false;
    }

    void reset()
    {
        start_time_ = millis();
    }

    void update() override
    {
        if (is_paused_) return;

        auto cur_time = millis();
        if (cur_time >= (last_emit_ + fequency_))
        {
            last_emit_ = cur_time;
            (*call_back_)(callback_data_);
        }
    }
};
