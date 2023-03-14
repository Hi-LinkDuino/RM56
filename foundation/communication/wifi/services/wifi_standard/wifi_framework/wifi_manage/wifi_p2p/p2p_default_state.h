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
#ifndef OHOS_P2P_DEFAULT_STATE_H
#define OHOS_P2P_DEFAULT_STATE_H

#include "state.h"
#include "wifi_p2p_device_manager.h"
#include "wifi_p2p_group_manager.h"


namespace OHOS {
namespace Wifi {
class P2pStateMachine;
class P2pDefaultState : public State {
public:
    /**
     * @Description Construct a new Default State object
     * @param None
     * @return None
     */
    explicit P2pDefaultState(P2pStateMachine &);

    /**
     * @Description Destroy the Default State object
     * @param None
     * @return None
     */
    ~P2pDefaultState() = default;

    /**
     * @Description - Called when entering state
     * @param None
     * @return None
     */
    virtual void GoInState() override;

    /**
     * @Description - Called when exiting state
     * @param None
     * @return None
     */
    virtual void GoOutState() override;

    /**
     * @Description - Message Processing Function
     * @param msg - Message object pointer
     * @return - bool true:success   false:fail
     */
    virtual bool ExecuteStateMsg(InternalMessage *msg) override;

private:
    P2pStateMachine &p2pStateMachine;
};
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_P2P_DEFAULT_STATE_H */
