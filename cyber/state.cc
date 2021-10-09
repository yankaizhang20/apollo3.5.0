/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "cyber/state.h"

#include <atomic> //@zyk:原子操作用于线程同步

namespace apollo {
namespace cyber {

namespace {
std::atomic<State> g_cyber_state;
}

State GetState() { return g_cyber_state.load(); }

void SetState(const State& state) { g_cyber_state.store(state); }

} // namespace cyber
} // namespace apollo
