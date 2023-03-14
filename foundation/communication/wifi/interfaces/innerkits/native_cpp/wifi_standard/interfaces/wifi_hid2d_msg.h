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
#ifndef OHOS_WIFI_HID2D_MSG_H
#define OHOS_WIFI_HID2D_MSG_H

#include <string>

#ifndef MAC_LEN
#define MAC_LEN 6
#endif

#ifndef CFG_DATA_MAX_BYTES
#define CFG_DATA_MAX_BYTES 255
#endif

namespace OHOS {
namespace Wifi {
enum class DhcpMode {
    CONNECT_GO_NODHCP = 0,
    CONNECT_AP_DHCP = 1,
    CONNECT_AP_NODHCP = 2,
    CONNECT_MODE_INVALID = 0xff
};

enum class FreqType {
    FREQUENCY_DEFAULT = 0,
    FREQUENCY_160M = 1,
};

enum class SelfCfgType {
    TYPE_OF_GET_SELF_CONFIG = 1,
    TYPE_OF_GET_SELF_CONFIG_WITH_PASSWORD = 2
};

enum class PeerCfgType {
    TYPE_OF_SET_PEER_CONFIG = 1,
    TYPE_OF_SET_PEER_STATE_CHANGE = 2
};

enum class PreferBandwidth {
    /** default */
    BW_DEFAULT,
    /** indicates the ultimate bandwidth, corresponding to 160 Mbit/s or 320 Mbit/s in the future. */
    BW_EXTRAM,
    /** high throughput. The default value is 80 Mbit/s. */
    BW_HIGH_PERF,
    /** low-latency service type, 40 Mbit/s/80 Mbit/s,
     * which needs to be determined based on the current channel status. */
    BW_LOW_LATENCY
};

enum class RecommendStatus {
    RS_SUCCESS,
    RS_LOCAL_ADJUST,
    RS_REMOTE_ADJUST,
    RS_FAILURE
};

class Hid2dConnectConfig {
public:
    Hid2dConnectConfig() : m_ssid(""), m_bssid(""), m_preSharedKey(""),
        m_frequency(-1), m_dhcpMode(DhcpMode::CONNECT_MODE_INVALID) {
    }
    ~Hid2dConnectConfig() {
    }

    void SetSsid(const std::string& ssid);
    std::string GetSsid() const;
    void SetBssid(const std::string& bssid);
    std::string GetBssid() const;
    void SetPreSharedKey(const std::string& preSharedKey);
    std::string GetPreSharedKey() const;
    void SetFrequency(const int frequency);
    int GetFrequency() const;
    void SetDhcpMode(const DhcpMode dhcpMode);
    DhcpMode GetDhcpMode() const;

private:
    std::string m_ssid;
    std::string m_bssid;
    std::string m_preSharedKey;
    int m_frequency;
    DhcpMode m_dhcpMode;
};

class IpAddrInfo {
public:
    std::string ip;
    std::string gateway;
    std::string netmask;
};

class RecommendChannelRequest {
public:
    RecommendChannelRequest() : remoteIfName(""), remoteIfMode(-1), localIfName(""),
        localIfMode(-1), prefBand(0), prefBandwidth(PreferBandwidth::BW_DEFAULT) {
    }

    ~RecommendChannelRequest() {
    }

    /** the interface name of the remote device */
    std::string remoteIfName;
    /**  the mode of the interface on the remote device */
    int remoteIfMode;
    /** interface name of the local device */
    std::string localIfName;
    /** the mode of the interface on the local device */
    int localIfMode;
    /** preferred frequency band */
    int prefBand;
    /** preferred bandwidth type (enumerated) */
    PreferBandwidth prefBandwidth;
};

class RecommendChannelResponse {
public:
    RecommendChannelResponse() : status(RecommendStatus::RS_FAILURE), index(-1),
        centerFreq(0), centerFreq1(0), centerFreq2(0), bandwidth(0) {
    }
    ~RecommendChannelResponse() {
    }

    /** 0: success; 1: local adjustment; 2: remote adjustment; –1: failure */
    RecommendStatus status;
    /* -1 fails. 0-N corresponds to the input array subscript (that is, the interface to be connected) */
    int index;
    /* optional 20 Mbit/s bandwidth */
    int centerFreq;
    /* optional frequency one */
    int centerFreq1;
    /* optional frequency two */
    int centerFreq2;
    /* band width */
    int bandwidth;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
