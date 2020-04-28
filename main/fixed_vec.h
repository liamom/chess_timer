#pragma once

#include <Arduino.h>

#include <assert.h>

template<class T, size_t CAPACITY>
class FixedVec {
  size_t array_size_ = 0;
  union RawData {
      T item;
  };

  RawData data_[CAPACITY];
  
public:
  struct Iterator {
    Iterator(FixedVec &parent, size_t counter = 0):parent_(parent),counter_(counter) {}
    Iterator(const Iterator &) = default;
    Iterator(Iterator &&) = default;

    Iterator& operator++() {
      counter_ = counter_ + 1;
      return *this;
    }
    
    bool operator==(const Iterator &other) {
      return counter_ == other.counter_;
    }

    
    bool operator!=(const Iterator &other) {
      return counter_ != other.counter_;
    }

    T& operator*() {
      return parent_.get(counter_);
    }
  private:
    FixedVec &parent_;
    size_t counter_;
  };

  inline T& get(size_t i) {
    assert(array_size_ < CAPACITY);

    RawData *raw_element = &data_[i];
    return raw_element->item;
  }

  void push_back(T elm) {
    RawData *uninitialized_data = &data_[array_size_];

    ++array_size_;
    assert(array_size_ < CAPACITY);
    
    new (&uninitialized_data->item) T(move(elm));
  }

  void remove_first(const T &elm) {
    for (size_t i = 0 ; i < array_size_; ++i) {
      T &item = get(i);
      if (item == elm) {
        item.~T();

        for (int j = i; j < array_size_ - 1; ++j) {
          data_[j] = data_[j - 1];
        }

        break;
      }
    }
  }

  template<class ...ARGS>
  T& emplace_back(ARGS ...args) {
    RawData *uninitialized_data = &data_[array_size_];
    
    ++array_size_;
    assert(array_size_ < CAPACITY);

    T *ptr = new (&uninitialized_data->item) T(forward(args)...);
    return *ptr;
  }

  Iterator begin() {
    return Iterator{*this};
  }

  Iterator end() {
    return Iterator{*this, array_size_};
  }
  
  ~FixedVec () {
    for(auto &e : *this) {
      e.~T();
    }
  }
};
