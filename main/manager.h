#pragma once

#include <Arduino.h>
#include "fixed_vec.h"
#include "i_update.h"


class Manager {
public:
  using update_ptr = unique_ptr<IUpdate>;

  ~Manager() {
    
  }

  // using f_ptr = void(*)();
  void addItem(IUpdate& ptr) {
    pin_listeners_.push_back(&ptr);
  }

  void remoteItem(IUpdate& ptr) {
    pin_listeners_.remove_first(&ptr);
  }

  void init() {
  }

  void poll() {
    for (auto &i : pin_listeners_) {
      i->update();
    }
  }

private:
  FixedVec<IUpdate*, 10> pin_listeners_;
};
