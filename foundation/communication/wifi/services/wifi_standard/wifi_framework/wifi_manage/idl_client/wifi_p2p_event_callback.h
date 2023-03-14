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

#ifndef OHOS_WIFI_P2P_EVENT_CALLBACK_H
#define OHOS_WIFI_P2P_EVENT_CALLBACK_H

#include <functional>
#include <string>
#include "wifi_idl_struct.h"

namespace OHOS {
namespace Wifi {
struct P2pHalCallback {
    std::function<void(int)> onConnectSupplicant;
    std::function<void(const IdlP2pDeviceFound &)> onDeviceFound;
    std::function<void(const std::string &)> onDeviceLost;
    std::function<void(const std::string &, short)> onGoNegotiationRequest;
    std::function<void(void)> onGoNegotiationSuccess;
    std::function<void(int)> onGoNegotiationFailure;
    std::function<void(const IdlP2pInvitationInfo &)> onInvitationReceived;
    std::function<void(const std::string &, int)> onInvitationResult;
    std::function<void(void)> onGroupFormationSuccess;
    std::function<void(const std::string &)> onGroupFormationFailure;
    std::function<void(const IdlP2pGroupInfo &)> onGroupStarted;
    std::function<void(const std::string &, bool)> onGroupRemoved;
    std::function<void(const std::string &)> onProvisionDiscoveryPbcRequest;
    std::function<void(const std::string &)> onProvisionDiscoveryPbcResponse;
    std::function<void(const std::string &)> onProvisionDiscoveryEnterPin;
    std::function<void(const std::string &, const std::string &)> onProvisionDiscoveryShowPin;
    std::function<void(void)> onProvisionDiscoveryFailure;
    std::function<void(void)> onFindStopped;
    std::function<void(const std::string &, short, const std::vector<unsigned char> &)> onServiceDiscoveryResponse;
    std::function<void(const std::string &)> onStaDeauthorized;
    std::function<void(const std::string &)> onStaAuthorized;
    std::function<void(void)> connectSupplicantFailed;
    std::function<void(const IdlP2pServDiscReqInfo &)> onP2pServDiscReq;
};
}  // namespace Wifi
}  // namespace OHOS

#endif