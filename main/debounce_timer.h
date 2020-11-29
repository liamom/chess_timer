#pragma once

#include "util.h"

template<class T, unsigned long debounceDelay = 50>
class DebouncedValue {
  unsigned long lastDebounceTime = 0;
  T last_reeding_;
  T current_state_;
public:
  template<class ...ARGS>
  DebouncedValue(ARGS ...args):current_state_(forward<ARGS>(args)...) {
    
  }
  
  // to lazy to fix right now
  DebouncedValue(T cpy):current_state_(cpy) {}

  T get() {
    return current_state_;
  }

  template<class U_REF>
  bool set_value(U_REF &&new_state) {
    if (last_reeding_ != new_state) {
      // new_state the debouncing timer
      lastDebounceTime = millis();      
      last_reeding_ = move(new_state);
    }

    if ((millis() - lastDebounceTime) <= debounceDelay) {
      return false;
    }
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (current_state_ != last_reeding_){
      current_state_ = last_reeding_;
      return true;
    }

    // no update
    return false;
  }
};
