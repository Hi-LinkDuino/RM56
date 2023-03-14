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

#ifndef OHOS_WIFI_CONFIG_FILE_SPEC_H
#define OHOS_WIFI_CONFIG_FILE_SPEC_H
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "wifi_internal_msg.h"
#include "wifi_p2p_msg.h"
namespace OHOS {
namespace Wifi {
/* ----------------- template function begin ----------------------- */
/**
 * @Description Clear and init item
 *
 * @tparam T - typename
 * @param item - item
 */
template <typename T>
void ClearTClass(T &item)
{
    /* fixed compile warning, -Werror,-Wunused-parameter */
    item;
    return;
}

/**
 * @Description Set item's data, input key is the item's member and input value is the
 *              member's value
 *
 * @tparam T - typename
 * @param item - T &item
 * @param key - Item key
 * @param value - Item value
 */
template <typename T>
void SetTClassKeyValue(T &item, const std::string &key, const std::string &value)
{
    /* fixed compile warning, -Werror,-Wunused-parameter */
    item;
    std::ostringstream ss;
    ss << key << value << std::endl;
    return;
}

/**
 * @Description Output the item's head
 *
 * @tparam T - typename
 * @param item - item
 * @return std::string - the item's type name
 */
template <typename T>
std::string GetTClassName()
{
    return "";
}

/**
 * @Description Output the item, format: item's member = the member value
 *
 * @tparam T - typename
 * @param item - item
 * @return std::string - output item's total member=value string
 */
template <typename T>
std::string OutTClassString(T &item)
{
    /* fixed compile warning, -Werror,-Wunused-parameter */
    item;
    std::string s;
    return s;
}

/* ----------------- template function end --------------------------------- */

/* ------------template function specialization declare begin-------------- */
/**
 * @Description Clear and init WifiDeviceConfig
 *
 * @tparam
 * @param item - WifiDeviceConfig item
 */
template <>
void ClearTClass<WifiDeviceConfig>(WifiDeviceConfig &item);

/**
 * @Description Set WifiDeviceConfig item data
 *
 * @tparam
 * @param item - WifiDeviceConfig &item
 * @param key - WifiDeviceConfig struct member name
 * @param value - the WifiDeviceConfig item member value
 */
template <>
void SetTClassKeyValue<WifiDeviceConfig>(WifiDeviceConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output WifiDeviceConfig class name
 *
 * @tparam
 * @param item - WifiDeviceConfig &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<WifiDeviceConfig>();

/**
 * @Description Output the WifiDeviceConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - WifiDeviceConfig &item
 * @return std::string - output total member=value string about the WifiDeviceConfig item
 */
template <>
std::string OutTClassString<WifiDeviceConfig>(WifiDeviceConfig &item);

/**
 * @Description Clear and init HotspotConfig
 *
 * @tparam
 * @param item - HotspotConfig item
 */
template <>
void ClearTClass<HotspotConfig>(HotspotConfig &item);

/**
 * @Description Set HotspotConfig item data
 *
 * @tparam
 * @param item - HotspotConfig &item
 * @param key - HotspotConfig struct member name
 * @param value - the HotspotConfig item member value
 */
template <>
void SetTClassKeyValue<HotspotConfig>(HotspotConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output HotspotConfig class name
 *
 * @tparam
 * @param item - HotspotConfig &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<HotspotConfig>();

/**
 * @Description Output the HotspotConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - HotspotConfig &item
 * @return std::string - output total member=value string about the HotspotConfig item
 */
template <>
std::string OutTClassString<HotspotConfig>(HotspotConfig &item);

/**
 * @Description Clear and init P2pVendorConfig
 *
 * @tparam
 * @param item - P2pVendorConfig &item
 */
template<>
void ClearTClass<P2pVendorConfig>(P2pVendorConfig &item);

/**
 * @Description Set P2pVendorConfig item data
 *
 * @tparam
 * @param item - P2pVendorConfig &item
 * @param key - P2pVendorConfig struct member name
 * @param value - the P2pVendorConfig item member value
 */
template<>
void SetTClassKeyValue<P2pVendorConfig>(P2pVendorConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output P2pVendorConfig class name
 *
 * @tparam
 * @param item - P2pVendorConfig &item
 * @return std::string - Class name
 */
template<>
std::string GetTClassName<P2pVendorConfig>();

/**
 * @Description Output the P2pVendorConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - P2pVendorConfig &item
 * @return std::string - output total member=value string about the P2pVendorConfig item
 */
template<>
std::string OutTClassString<P2pVendorConfig>(P2pVendorConfig &item);

/**
 * @Description Clear and init StationInfo
 *
 * @tparam
 * @param item - StationInfo &item
 */
template <>
void ClearTClass<StationInfo>(StationInfo &item);

/**
 * @Description Set StationInfo item data
 *
 * @tparam
 * @param item - StationInfo &item
 * @param key - StationInfo struct member name
 * @param value - the StationInfo item member value
 */
template <>
void SetTClassKeyValue<StationInfo>(StationInfo &item, const std::string &key, const std::string &value);

/**
 * @Description Output StationInfo class name
 *
 * @tparam
 * @param item - StationInfo &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<StationInfo>();

/**
 * @Description Output the StationInfo item, format: item's member = the member value
 *
 * @tparam
 * @param item - StationInfo &item
 * @return std::string - output total member=value string about the StationInfo item
 */
template <>
std::string OutTClassString<StationInfo>(StationInfo &item);

/**
 * @Description Clear and init WifiConfig
 *
 * @tparam
 * @param item - WifiConfig &item
 */
template <>
void ClearTClass<WifiConfig>(WifiConfig &item);

/**
 * @Description Set WifiConfig item data
 *
 * @tparam
 * @param item - WifiConfig &item
 * @param key - WifiConfig struct member name
 * @param value - the WifiConfig item member value
 */
template <>
void SetTClassKeyValue<WifiConfig>(WifiConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output WifiConfig class name
 *
 * @tparam
 * @param item - WifiConfig &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<WifiConfig>();

/**
 * @Description Output the WifiConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - WifiConfig &item
 * @return std::string - output total member=value string about the WifiConfig item
 */
template <>
std::string OutTClassString<WifiConfig>(WifiConfig &item);

/**
 * @Description Clear and init WifiP2pGroupInfo
 *
 * @tparam
 * @param item - WifiP2pGroupInfo item
 */
template<>
void ClearTClass<WifiP2pGroupInfo>(WifiP2pGroupInfo &item);

/**
 * @Description Set WifiP2pGroupInfo item data
 *
 * @tparam
 * @param item - WifiP2pGroupInfo &item
 * @param key - WifiP2pGroupInfo struct member name
 * @param value - the WifiP2pGroupInfo item member value
 */
template<>
void SetTClassKeyValue<WifiP2pGroupInfo>(WifiP2pGroupInfo &item, const std::string &key, const std::string &value);

/**
 * @Description Output WifiP2pGroupInfo class name
 *
 * @tparam
 * @param item - WifiP2pGroupInfo &item
 * @return std::string - Class name
 */
template<>
std::string GetTClassName<WifiP2pGroupInfo>();

/**
 * @Description Output the WifiP2pGroupInfo item, format: item's member = the member value
 *
 * @tparam
 * @param item - WifiP2pGroupInfo &item
 * @return std::string - output total member=value string about the WifiP2pGroupInfo item
 */
template<>
std::string OutTClassString<WifiP2pGroupInfo>(WifiP2pGroupInfo &item);

/**
 * @Description Clear and init TrustListPolicy
 *
 * @tparam
 * @param item - TrustListPolicy &item
 */
template <>
void ClearTClass<TrustListPolicy>(TrustListPolicy &item);

/**
 * @Description Set TrustListPolicy item data
 *
 * @tparam
 * @param item - TrustListPolicy &item
 * @param key - TrustListPolicy struct member name
 * @param value - the TrustListPolicy item member value
 */
template <>
void SetTClassKeyValue<TrustListPolicy>(TrustListPolicy &item, const std::string &key, const std::string &value);

/**
 * @Description Output TrustListPolicy class name
 *
 * @tparam
 * @param item - TrustListPolicy &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<TrustListPolicy>();

/**
 * @Description Output the TrustListPolicy item, format: item's member = the member value
 *
 * @tparam
 * @param item - TrustListPolicy &item
 * @return std::string - output total member=value string about the TrustListPolicy item
 */
template <>
std::string OutTClassString<TrustListPolicy>(TrustListPolicy &item);

/**
 * @Description Clear and init MovingFreezePolicy
 *
 * @tparam
 * @param item - MovingFreezePolicy &item
 */
template <>
void ClearTClass<MovingFreezePolicy>(MovingFreezePolicy &item);

/**
 * @Description Set MovingFreezePolicy item data
 *
 * @tparam
 * @param item - MovingFreezePolicy &item
 * @param key - MovingFreezePolicy struct member name
 * @param value - the MovingFreezePolicy item member value
 */
template <>
void SetTClassKeyValue<MovingFreezePolicy>(MovingFreezePolicy &item, const std::string &key, const std::string &value);

/**
 * @Description Output MovingFreezePolicy class name
 *
 * @tparam
 * @param item - MovingFreezePolicy &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<MovingFreezePolicy>();

/**
 * @Description Output the MovingFreezePolicy item, format: item's member = the member value
 *
 * @tparam
 * @param item - MovingFreezePolicy &item
 * @return std::string - output total member=value string about the MovingFreezePolicy item
 */
template <>
std::string OutTClassString<MovingFreezePolicy>(MovingFreezePolicy &item);

/**
 * @Description Clear and init WifiStoreRandomMac
 *
 * @tparam
 * @param item - WifiStoreRandomMac &item
 */
template <>
void ClearTClass<WifiStoreRandomMac>(WifiStoreRandomMac &item);

/**
 * @Description Set WifiStoreRandomMac item data
 *
 * @tparam
 * @param item - WifiStoreRandomMac &item
 * @param key - WifiStoreRandomMac struct member name
 * @param value - the WifiStoreRandomMac item member value
 */
template <>
void SetTClassKeyValue<WifiStoreRandomMac>(WifiStoreRandomMac &item, const std::string &key, const std::string &value);

/**
 * @Description Output WifiStoreRandomMac class name
 *
 * @tparam
 * @param item - WifiStoreRandomMac &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<WifiStoreRandomMac>();

/**
 * @Description Output the WifiStoreRandomMac item, format: item's member = the member value
 *
 * @tparam
 * @param item - WifiStoreRandomMac &item
 * @return std::string - output total member=value string about the WifiStoreRandomMac item
 */
template <>
std::string OutTClassString<WifiStoreRandomMac>(WifiStoreRandomMac &item);
/* ----------template function specialization declare end----------- */
}  // namespace Wifi
}  // namespace OHOS
#endif