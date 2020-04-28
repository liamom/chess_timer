#pragma once

#include "util.h"

template<class T>
class unique_ptr {
  T *data_;
  
public:
  unique_ptr(): data_(nullptr) {}
  unique_ptr(T *ptr): data_(ptr) {}
  unique_ptr(const unique_ptr<T> &o) = delete;
  unique_ptr(unique_ptr<T> && other):data_(other.data_) {
    other.data_ = nullptr;
  }

  T* get() {
    return data_; 
  }

  T* release() {
    auto tmp = data_;
    delete data_;
    return tmp;
  }
  
  ~unique_ptr() {
    delete data_;
  }
};


template<class T, class... ARGS>
unique_ptr<T> make_unique(ARGS &&... args) {
  return unique_ptr<T>(new T(forward(args)...));
}
