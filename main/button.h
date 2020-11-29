#pragma once

#include <Arduino.h>
#include "i_update.h"
#include "debounce_timer.h"

// template<class RET, class ...ARGS>
// static RET adaptor(void *data, ARGS ...args) {
//     using FUNC_1 = RET (*) (ARGS...);
//     FUNC_1 original_arg = reinterpret_cast<FUNC_1>(data);
//     return original_arg(args...);
// }

template<class RET, class ...ARGS>
struct CFunctionWData {
    using FUNC_1 = RET (*) (ARGS...);
    using FUNC_2 = RET (*) (void*, ARGS...);

    FUNC_2 func_ = nullptr;
    void* data_ = nullptr;

    CFunctionWData(){}
    CFunctionWData(const CFunctionWData&) = default;
    CFunctionWData(CFunctionWData&&) = default;
    CFunctionWData& operator=(const CFunctionWData&) = default;
    CFunctionWData& operator=(CFunctionWData&&) = default;

    CFunctionWData(void* data, FUNC_2 func):func_(func), data_(data){}

    static RET adaptor(void *data, ARGS ...args) {
        FUNC_1 original_arg = reinterpret_cast<FUNC_1>(data);
        return (*original_arg)(args...);
    }

    // make sure the reinterpret_cast works
    static_assert(sizeof(void*) == sizeof(FUNC_1));

    CFunctionWData(FUNC_1 func)
        :CFunctionWData(reinterpret_cast<void*>(func), adaptor)
    {}

    RET operator()(ARGS ...args) {
        return (*func_)(data_, args...);
    }
};

class CbButton : public IUpdate
{
    const uint8_t pin_;
    CFunctionWData<void, bool> call_back_;

    DebouncedValue<bool> value_;

public:

    CbButton(uint8_t pin, CFunctionWData<void, bool> callback)
        : pin_(pin), call_back_(callback)
    {
        pinMode(pin_, INPUT_PULLUP);
    }

    void update() override
    {
        bool buttonState = digitalRead(pin_) != HIGH;

        if (value_.set_value(buttonState))
        {
            call_back_(buttonState);
        }
    }
};

class ReleaseButton
{
    const uint8_t pin_;
    DebouncedValue<bool> value_;
    bool wasPressed_ = false;

public:

    ReleaseButton(uint8_t pin)
        : pin_(pin)
    {
        pinMode(pin_, INPUT_PULLUP);
    }

    bool processPress()
    {
        bool buttonState = digitalRead(pin_) != HIGH;

        bool last_state = value_.get();        
        if (value_.set_value(buttonState)) {
            return last_state && !value_.get();
        }

        return false;
    }
};

class PushButton
{
    const uint8_t pin_;
    DebouncedValue<bool> value_;

public:

    PushButton(uint8_t pin)
        : pin_(pin)
    {
        pinMode(pin_, INPUT_PULLUP);
    }

    bool processPress()
    {
        bool buttonState = digitalRead(pin_) != HIGH;

        bool last_state = value_.get();        
        if (value_.set_value(buttonState)) {
            return last_state == false && value_.get() == true;
        }

        return false;
    }
};
