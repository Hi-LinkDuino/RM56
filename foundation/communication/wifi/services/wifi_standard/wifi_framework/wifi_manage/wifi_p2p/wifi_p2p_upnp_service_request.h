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
#ifndef OHOS_WIFI_P2P_UPNP_SERVICE_REQUEST_H
#define OHOS_WIFI_P2P_UPNP_SERVICE_REQUEST_H

#include "wifi_p2p_msg.h"
#include "p2p_macro.h"

namespace OHOS {
namespace Wifi {
class WifiP2pUpnpServiceRequest : public WifiP2pServiceRequest {
    FRIEND_GTEST(WifiP2pUpnpServiceRequest);
public:
    /**
     * @description Create an upnp service request class.
     *
     * @return WifiP2pUpnpServiceRequest
     */
    static WifiP2pUpnpServiceRequest Create();
    /**
     * @Description Create a WifiP2pUpnpServiceRequest object.
     *
     * @param searchTarget - request unique UPNP name
     * @return WifiP2pUpnpServiceRequest
     */
    static WifiP2pUpnpServiceRequest Create(std::string searchTarget);

protected:
    /**
     * @Description Construct a new WifiP2pUpnpServiceRequest object.
     *
     */
    WifiP2pUpnpServiceRequest();
    /**
     * @Description Construct a new WifiP2pUpnpServiceRequest object.
     *
     * @param query - unique query string
     */
    explicit WifiP2pUpnpServiceRequest(std::string query);
    /**
     * @Description Destroy the WifiP2pUpnpServiceRequest object.
     *
     */
    ~WifiP2pUpnpServiceRequest() = default;
};
}  // namespace Wifi
}  // namespace OHOS
#endif /* OHOS_WIFI_P2P_UPNP_SERVICE_REQUEST_H */