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
#ifndef OHOS_P2P_DISABLED_STATE_H
#define OHOS_P2P_DISABLED_STATE_H

#include "state.h"
#include "p2p_macro.h"
#include "wifi_p2p_group_manager.h"
#include "wifi_p2p_device_manager.h"
#include "wifi_p2p_service_manager.h"

namespace OHOS {
namespace Wifi {
class P2pStateMachine;
class P2pDisabledState : public State {
    FRIEND_GTEST(P2pDisabledState);

public:
    /**
     * @Description Construct a new P2pDisabledState object
     * @param None
     * @return None
     */
    P2pDisabledState(P2pStateMachine &, WifiP2pGroupManager &, WifiP2pDeviceManager &, WifiP2pServiceManager &);

    /**
     * @Description Destroy the P2pDisabledState object
     * @param None
     * @return None
     */
    ~P2pDisabledState() = default;

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
    virtual void SetVendorFeatures() const;

private:
    P2pStateMachine &p2pStateMachine;
    WifiP2pGroupManager &groupManager;
    WifiP2pDeviceManager &deviceManager;
    WifiP2pServiceManager &serviceManager;
};
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_P2P_DISABLED_STATE_H */
