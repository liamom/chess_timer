#pragma once

#include <Arduino.h>
#include "i_update.h"
#include "debounce_timer.h"

class Button : public IUpdate {
  const uint8_t pin_;
  DebouncedValue<bool> value_;

public:
  using call_back_t = void(*)(bool);
  const call_back_t call_back_;

  Button(uint8_t pin, call_back_t callback)
    :pin_(pin)
    ,call_back_(callback) {
    pinMode(pin_, INPUT);

  }
  
  void update() override {
    bool buttonState = digitalRead(pin_)  == HIGH;

    if (value_.set_value(buttonState) && call_back_) {
      (*call_back_)(buttonState);
    }
  }
};
