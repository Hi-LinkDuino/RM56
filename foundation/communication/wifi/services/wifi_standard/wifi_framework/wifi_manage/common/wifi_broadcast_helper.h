/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_WIFI_BROADCAST_HELPER_H
#define OHOS_WIFI_BROADCAST_HELPER_H

#include <string>
#include <map>
#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
/* It is only for testing and will be replaced with a real broadcast system later. */
class WifiBroadCastHelper {
public:
    static void Show(const char v);
    static void Show(const std::string &v);
    static void Show(const std::vector<std::string> &v);
    static void Show(const std::map<std::string, std::string> &v);
    static void Show(const int v);
    static void Show(const WifiP2pDevice &v);
    static void Show(const WifiP2pLinkedInfo &v);
    static void Show(const std::vector<WifiP2pDevice> &v);
    static void Show(const std::vector<WifiP2pLinkedInfo> &v);
    static void Show(const WifiP2pServiceInfo &v);
    static void Show(const std::vector<WifiP2pServiceInfo> &v);

    template<typename T, typename... Args>
    static void Send(T t)
    {
        Handle(t);
    }

    template<typename T, typename... Args>
    static void Send(T t, Args... args)
    {
        Handle(t);
        Send(args...);
    }

private:
    template<typename Th>
    static void Handle(Th things)
    {
        Show(things);
    }
};
}  // namespace Wifi
}  // namespace OHOS
#endif