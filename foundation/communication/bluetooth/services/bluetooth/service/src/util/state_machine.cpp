/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "state_machine.h"

namespace utility {
void StateMachine::InitState(const std::string &name)
{
    auto it = states_.find(name);
    if (it != states_.end()) {
        current_ = it->second.get();
        Transition(it->first);
    }
}

void StateMachine::Move(std::unique_ptr<State> &state)
{
    if (state != nullptr) {
        auto it = states_.find(state->Name());
        if (it == states_.end()) {
            states_[state->Name()] = std::move(state);
        }
    }
}

bool StateMachine::ProcessMessage(const Message &msg) const
{
    State *current = current_;
    if (current == nullptr) {
        return false;
    } else {
        while (!current->Dispatch(msg)) {
            current = current->parent_;
            if (current == nullptr) {
                return false;
            }
        }
    }

    return true;
}

const StateMachine::State *StateMachine::GetState() const
{
    return current_;
}

void StateMachine::Transition(const std::string &name)
{
    auto it = states_.find(name);
    if (it != states_.end()) {
        std::array<State *, STACK_DEPTH> dstStack { nullptr, nullptr, nullptr, nullptr, nullptr };

        int dstDepth = GetStateDepth(*it->second);
        State *tmp = it->second.get();
        for (int i = 0; i < dstDepth; i++) {
            dstStack[dstDepth - i - 1] = tmp;
            tmp = tmp->parent_;
        }

        int sameDepth;
        for (sameDepth = 0; sameDepth < STACK_DEPTH; sameDepth++) {
            if (dstStack[sameDepth] != stack_[sameDepth]) {
                break;
            }
        }

        for (int i = top_; i > sameDepth; i--) {
            stack_[i - 1]->Exit();
        }
        current_ = it->second.get();

        stack_ = dstStack;
        for (int i = sameDepth; i < dstDepth; i++) {
            stack_[i]->Entry();
        }
        top_ = dstDepth;
    }
}

inline int StateMachine::GetStateDepth(State &state)
{
    int depth = 0;
    State *tmp = &state;
    while (tmp != nullptr) {
        depth++;
        tmp = tmp->parent_;
    }

    return depth;
}
}  // namespace utility