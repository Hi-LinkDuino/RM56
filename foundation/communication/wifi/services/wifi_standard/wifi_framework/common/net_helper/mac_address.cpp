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

#include "mac_address.h"
#include <unistd.h>
#include <climits>
#include <algorithm>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include "securec.h"
#include "log_helper.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_UTIL_MacAddress"

static constexpr char INVALID_MAC[MAC_STRING_LENGTH + 1] = "00:00:00:00:00:00"; /* Indicates an invalid MAC address. */
static constexpr int MAC_BLOCK = 2;
constexpr int INTERVAL = 3; /* MAC addresses:character spacing */
constexpr int HEX_BASE = 16;

namespace OHOS {
namespace Wifi {
const MacAddress MacAddress::INVALID_MAC_ADDRESS = MacAddress(INVALID_MAC);

bool MacAddress::IsValidMac(const std::string &mac)
{
    if (mac.length() != MAC_STRING_LENGTH) {
        return false;
    }

    if (INVALID_MAC_ADDRESS.GetMacAddressWifiString() == mac) {
        return false;
    }

    std::string macBuf = mac;
    std::transform(macBuf.begin(), macBuf.end(), macBuf.begin(), ::tolower);

    for (size_t i = 0; i < MAC_STRING_LENGTH; i++) {
        if (i % INTERVAL == MAC_BLOCK) {
            if (macBuf[i] != ':') {
                return false;
            }
        } else {
            if ((macBuf[i] < '0' || macBuf[i] > '9') && (macBuf[i] < 'a' || macBuf[i] > 'f')) {
                return false;
            }
        }
    }

    return true;
}

MacAddress MacAddress::Create(const std::string &mac)
{
    if (IsValidMac(mac)) {
        std::string macBuf = mac;
        std::transform(macBuf.begin(), macBuf.end(), macBuf.begin(), ::tolower);
        return MacAddress(macBuf);
    }

    return INVALID_MAC_ADDRESS;
}

MacAddress MacAddress::Create(const sockaddr &hwAddr)
{
    char mac[MAC_STRING_LENGTH + 1];
    if (snprintf_s(mac,
        sizeof(mac),
        sizeof(mac) - 1,
        "%02x:%02x:%02x:%02x:%02x:%02x",
        (unsigned char)hwAddr.sa_data[ZERO],
        (unsigned char)hwAddr.sa_data[ONE],
        (unsigned char)hwAddr.sa_data[TWO],
        (unsigned char)hwAddr.sa_data[THREE],
        (unsigned char)hwAddr.sa_data[FOUR],
        (unsigned char)hwAddr.sa_data[FIVE]) < 0) {
        return INVALID_MAC_ADDRESS;
    }

    return Create(mac);
}

MacAddress::MacAddress(const std::string &mac) : mac_(mac)
{}

bool MacAddress::operator==(const MacAddress &mac) const
{
    return mac_ == mac.mac_;
}

bool MacAddress::operator==(const struct sockaddr &hwAddr) const
{
    MacAddress mac = Create(hwAddr);
    return mac_ == mac.mac_;
}

bool MacAddress::IsValid() const
{
    return IsValidMac(mac_);
}

void MacAddress::Dump() const
{
    LOGI("MAC: [%{private}s]", mac_.c_str());
}

const std::string &MacAddress::GetMacAddressWifiString() const
{
    return mac_;
}

struct sockaddr MacAddress::GetMacAddressWifiSockaddr() const
{
    struct sockaddr hwAddr = {0};
    hwAddr.sa_family = 1; /* Ethernet 10Mbps */

    for (size_t i = 0; i < mac_.length(); i += INTERVAL) {
        std::string byte = mac_.substr(i, MAC_BLOCK);
        hwAddr.sa_data[i / INTERVAL] = static_cast<char>(strtol(byte.c_str(), nullptr, HEX_BASE));
    }
    return hwAddr;
}

bool MacAddress::GetMacAddr(const std::string& ifName, unsigned char macAddr[MAC_LEN])
{
    struct ifreq ifr;
    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK ||
        strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifName.c_str()) != EOK) {
        LOGE("Init the ifreq struct failed!");
        return false;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        LOGE("get mac addr socket error");
        return false;
    }

    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
        LOGE("get mac addr ioctl SIOCGIFHWADDR error");
        close(fd);
        return false;
    }

    if (memcpy_s(macAddr, ETH_ALEN, ifr.ifr_hwaddr.sa_data, ETH_ALEN) != EOK) {
        LOGE("get mac addr memcpy_s error");
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
} // namespace WiFi
} // namespace OHOS