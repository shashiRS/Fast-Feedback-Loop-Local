// Copyright (C) 2020  Aleksey Romanov (aleksey at voltanet dot io)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// https://github.com/alex4747-pub/simple_init_chain/

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     simple_init_chain.cpp
 * @brief    init chain
 */

#include <cstddef>
#ifdef _DEBUG
#include <iostream>
#endif
#include <map>
#include <string>

#include <next/sdk/init_chain/simple_init_chain.hpp>

#if __cplusplus < 201703L
#error "At least c++17 is required"
#endif

namespace next {
namespace sdk {

InitPriority operator+(const InitPriority &level, const int &in) {
  return (static_cast<InitPriority>(in + static_cast<int>(level)));
}

bool InitChain::Run(ConfigMap const &config_map /*= ConfigMap()*/) noexcept {
  // Fail repeated calls or calls afer failure
  //
#ifdef _DEBUG
  std::cout << "InitChain::Run()" << std::endl;
#endif
  if (!ready_) {
    return false;
  }
  ready_ = false;

  El *prev = nullptr;
  El *cur = head_;

#ifdef _DEBUG
  if (cur == nullptr) {
    std::cout << "init chain is empty" << std::endl;
  }
#endif

  while (cur != nullptr) {
    // Make sure elements that do not support
    // reset are called only once
    //
    if (!cur->HaveReset()) {
      if (prev != nullptr) {
        // Skip current elemnt the next time
        //
        prev->SetNext(cur->GetNext());
        if (El *next = cur->GetNext(); next != nullptr) {
          next->SetPrev(prev);
        }
      } else {
        // Discard leading element
        //
        head_ = head_->GetNext();
        if (head_ != nullptr) {
          head_->SetPrev(nullptr);
        }
      }
      if (cur == tail_) {
        tail_ = cur->GetPrev();
      }
      // Note we do not advance prev here
      //
    } else {
      prev = cur;
    }

    if (!cur->DoInit(config_map)) {
      // Only fatal failures are returned through this interface
      //
      ready_ = false;
      failed_ = true;
      return false;
    }

    cur = cur->GetNext();
  }

  return true;
}

void InitChain::Reset(ConfigMap const &config_map /*= ConfigMap()*/) noexcept {
  // Reset the run guard
  std::cout << "InitChain::Reset started: " << std::endl;
  failed_ = false;
  ready_ = true;
  try {
    for (El *cur = tail_; cur != nullptr; cur = cur->GetPrev()) {
      cur->DoReset(config_map);
    }
  } catch (std::exception &e) {
    std::cout << "InitChain::Reset failed: " << e.what() << std::endl;
  }
  std::cout << "InitChain::Reset ended: " << std::endl;
}

InitChain::El::El(int level, InitFunc init_func, ResetFunc reset_func /*= nullptr*/) noexcept(false)
    : level_{level}, init_func_{init_func}, reset_func_{reset_func} {
#ifdef _DEBUG
  std::cout << "[InitChain] inserting element with prio " << level << std::endl;
#endif
  if (init_func == nullptr) {
    throw std::invalid_argument("Valid pointer for InitFunc required, nullptr given");
  }
  Insert(this);
}

InitChain::El::El(InitPriority level, InitFunc init_func, ResetFunc reset_func)
    : level_{static_cast<int>(level)}, init_func_{init_func}, reset_func_{reset_func} {
  if (init_func == nullptr) {
    throw std::invalid_argument("Valid pointer for InitFunc required, nullptr given");
  }
  Insert(this);
}
InitChain::El::~El() noexcept {}

int InitChain::El::GetLevel() const noexcept { return level_; }

InitChain::El *InitChain::El::GetNext() const noexcept { return next_; }

InitChain::El *InitChain::El::GetPrev() const noexcept { return prev_; }

void InitChain::El::SetNext(El *next) noexcept { next_ = next; }

void InitChain::El::SetPrev(El *prev) noexcept { prev_ = prev; }

bool InitChain::El::HaveReset() const noexcept { return reset_func_ != nullptr; }

bool InitChain::El::DoInit(ConfigMap const &config_map) noexcept {
  if (!init_func_) {
    return true;
  }

  try {
    return init_func_(config_map, level_);
  } catch (...) {
    return false;
  }
}

void InitChain::El::DoReset(ConfigMap const &config_map) noexcept {
  if (!reset_func_) {
    return;
  }

  try {
    reset_func_(config_map, level_);
  } catch (...) {
  }
}

void InitChain::Insert(El *el) {
  el->SetNext(nullptr);

  El *prev = nullptr;
  El *cur = head_;

  while (cur != nullptr) {
    if (el->GetLevel() <= cur->GetLevel()) {
      el->SetNext(cur);
      cur->SetPrev(el);
      if (head_ == cur) {
        head_ = el;
      }
      break;
    }

    prev = cur;
    cur = cur->GetNext();
  }

  if (prev == nullptr) {
    head_ = el;
    if (tail_ == nullptr) {
      tail_ = el;
    }
  } else {
    prev->SetNext(el);
    el->SetPrev(prev);
    if (tail_ == prev) {
      tail_ = el;
    }
  }
}

bool InitChain::ready_{true};
bool InitChain::failed_{false};

} // namespace sdk
} // namespace next
