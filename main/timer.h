#pragma once

#include <Arduino.h>
#include "i_update.h"
#include "debounce_timer.h"

class CbTimer : public IUpdate
{
    const uint32_t fequency_;
    uint32_t last_emit_ = millis();
    uint32_t start_time_ = last_emit_;

    bool is_paused_ = true;

    static_assert(sizeof(unsigned long) == sizeof(int32_t));

public:
    using call_back_t = void (*)(void*);
    const call_back_t call_back_;
    void* callback_data_;

    CbTimer(uint32_t fequency, call_back_t callback, void* callback_data)
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

class CountdownTimer {
    const uint32_t fequency_;
    uint32_t start_time_;
    uint32_t last_event_ = 0;
    uint32_t total_time_;

    bool is_paused_ = true;
    uint32_t pause_time_ = 0;
public:
    CountdownTimer(uint32_t total_time, uint32_t fequency)
        : fequency_(fequency)
        , total_time_(total_time)
    {
        start_time_ = millis();
        pause();
    }

    void pause() {
        is_paused_ = true;
        pause_time_ = millis();
    }

    void resume() {        
        start_time_ += millis() - pause_time_;
        is_paused_ = false;
    }
    
    void addBonus(uint32_t bonus) {
        start_time_ += bonus;
    }

    uint32_t getCurrentTime() {
        uint32_t now = millis();
        int32_t total_time_elapsed = now - start_time_;
        int32_t countdown_time = total_time_ - total_time_elapsed;

        Serial.print("getCurrentTime: ");
        Serial.println(countdown_time);

        return max(0, countdown_time);
    }

    bool processUpdate(uint32_t *output) {
        bool doUpdate = millis() > (last_event_ + fequency_);

        if (doUpdate) {
            last_event_ = millis();
            int32_t total_time_elapsed = last_event_ - start_time_;
            int32_t countdown_time = total_time_ - total_time_elapsed;

            *output = max(0, countdown_time);
        }

        return doUpdate;
    }
};