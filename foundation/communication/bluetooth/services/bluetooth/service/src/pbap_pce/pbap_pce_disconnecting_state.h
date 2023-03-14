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

#ifndef PBAP_PCE_DISCONNECTING_STATE_H
#define PBAP_PCE_DISCONNECTING_STATE_H

#include <cstring>
#include "base_observer_list.h"
#include "interface_profile_pbap_pce.h"
#include "message.h"
#include "pbap_pce_base_state.h"
#include "pbap_pce_state_machine.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief disconnecting statemachine
 * control disconnecting state
 */
class PceDisconnectingState : public PceBaseState {
public:
    explicit PceDisconnectingState(
        const std::string &name, PbapPceStateMachine &stm, BaseObserverList<IPbapPceObserver> &observerMgrList);

    /**
     * @brief destructor
     * @details destructor
     */
    ~PceDisconnectingState() override = default;

    /**
     * @brief entry
     * @details when become disconnecting, call this
     * @return void
     */
    void Entry() override;

    /**
     * @brief exit
     * @details when leave disconnecting, call this
     * @return void
     */
    void Exit() override;

    /**
     * @brief Dispatch
     * @details when Dispatch PceDisconnectingState, call this
     * @param msg message
     * @return bool true:success false:failure
     */
    bool Dispatch(const utility::Message &msg) override;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PBAP_PCE_DISCONNECTING_STATE_H