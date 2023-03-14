/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "wifi_hid2d_msg.h"

namespace OHOS {
namespace Wifi {
    void Hid2dConnectConfig::SetSsid(const std::string& ssid) {
        m_ssid = ssid;
    }

    std::string Hid2dConnectConfig::GetSsid() const {
        return m_ssid;
    }

    void Hid2dConnectConfig::SetBssid(const std::string& bssid) {
        m_bssid = bssid;
    }

    std::string Hid2dConnectConfig::GetBssid() const {
        return m_bssid;
    }

    void Hid2dConnectConfig::SetPreSharedKey(const std::string& preSharedKey) {
        m_preSharedKey = preSharedKey;
    }

    std::string Hid2dConnectConfig::GetPreSharedKey() const {
        return m_preSharedKey;
    }

    void Hid2dConnectConfig::SetFrequency(const int frequency) {
        m_frequency = frequency;
    }

    int Hid2dConnectConfig::GetFrequency() const {
        return m_frequency;
    }

    void Hid2dConnectConfig::SetDhcpMode(const DhcpMode dhcpMode) {
        m_dhcpMode = dhcpMode;
    }

    DhcpMode Hid2dConnectConfig::GetDhcpMode() const {
        return m_dhcpMode;
    }
}  // namespace Wifi
}  // namespace OHOS