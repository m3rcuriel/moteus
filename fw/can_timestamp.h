// Copyright 2026 Lee Mracek
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstdint>

#include "mbed.h"

namespace moteus {

/// The FDCAN internal timestamp counter counts CAN bit times, which
/// are not constant when bit rate switching is used.  On the STM32G4,
/// the FDCAN external timestamp input is TIM3's counter, so we run
/// TIM3 as a free running 1us counter and use that instead.
///
/// TIM3 is also available to the aux ports.  If one of them takes it
/// over, CanTimestampValid() will return false.
class CanTimestamp {
 public:
  static uint32_t Prescaler() {
    return SystemCoreClock / 1000000u - 1;
  }

  /// Start TIM3 as a 1us free running counter, unless something else
  /// is already using it.
  static void ConfigureTimer() {
    __HAL_RCC_TIM3_CLK_ENABLE();
    if (TIM3->CR1 & TIM_CR1_CEN) { return; }

    TIM3->CR1 = 0;
    TIM3->PSC = Prescaler();
    TIM3->ARR = 0xffff;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->CR1 = TIM_CR1_CEN;
  }

  static bool Valid() {
    return (TIM3->CR1 & TIM_CR1_CEN) &&
        !(TIM3->CR1 & TIM_CR1_DIR) &&
        TIM3->PSC == Prescaler() &&
        TIM3->ARR == 0xffff;
  }

  static uint16_t Read() {
    return static_cast<uint16_t>(TIM3->CNT);
  }
};

}
