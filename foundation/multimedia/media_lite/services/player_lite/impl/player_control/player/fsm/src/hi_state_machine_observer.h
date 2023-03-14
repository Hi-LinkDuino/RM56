/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef HI_STATE_MACHINE_OBSERVER_H
#define HI_STATE_MACHINE_OBSERVER_H

#include "hi_state.h"

namespace OHOS {
class HiStateMachine;

class HiStateMachineObserver {
public:
    HiStateMachineObserver();
    virtual ~HiStateMachineObserver();

    virtual void OnFinalState(const HiState &state);
    virtual void OnEventHandled(const HiStateMachine &stateMachine, int32_t event, int32_t result) = 0;
};
};
#endif
