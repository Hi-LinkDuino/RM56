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

#ifndef PBAP_PCE_BASE_STATE_H
#define PBAP_PCE_BASE_STATE_H

#include <cstring>
#include "base_observer_list.h"
#include "interface_profile_pbap_pce.h"
#include "message.h"
#include "pbap_pce_state_machine.h"

namespace OHOS {
namespace bluetooth {
/// pbap pce state mathine identification
static const std::string PCE_DISCONNECTED_STATE = "PceDisconnectedState";
static const std::string PCE_CONNECTING_STATE = "PceConnectingState";
static const std::string PCE_CONNECTED_STATE = "PceConnectedState";
static const std::string PCE_DISCONNECTING_STATE = "PceDisconnectingState";
class PceBaseState : public utility::StateMachine::State {
public:
    /**
     * @brief constructor
     * @details constructor
     */
    explicit PceBaseState(
        const std::string &name,
        PbapPceStateMachine &stm,
        BaseObserverList<IPbapPceObserver> &observerMgrList);

    /**
     * @brief destructor
     * @details destructor
     */
    virtual ~PceBaseState() = default;

protected:
    PbapPceStateMachine &stm_;
    BaseObserverList<IPbapPceObserver> &observerMgrList_;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PBAP_PCE_BASE_STATE_H
