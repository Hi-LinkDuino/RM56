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

#ifndef OHOS_WIFI_COMMON_UTIL_H
#define OHOS_WIFI_COMMON_UTIL_H

#include <string>
#include <vector>
#include "securec.h"

#ifndef WIFI_MAC_LEN
#define WIFI_MAC_LEN 6
#endif

namespace OHOS {
namespace Wifi {
/**
 * @Description MAC address anonymization
 *
 * <p> eg: a2:7c:b0:98:e3:92 -> a2:7c:**:**:**:92
 *
 * @param str - Input MAC address
 * @return std::string - Processed MAC
 */
std::string MacAnonymize(const std::string str);

/**
 * @Description MAC address anonymization
 *
 * <p> eg: 192.168.0.1 -> 192.***.*.1
 *
 * @param str - Input MAC address
 * @return std::string - Processed MAC
 */
std::string IpAnonymize(const std::string str);

/**
 * @Description Converting string MAC to a C-style MAC address
 *
 * @param strMac - Input MAC address
 * @param mac - conversion result
 * @return errno_t - EOK for success, failure for other values.
 */
errno_t MacStrToArray(const std::string& strMac, unsigned char mac[WIFI_MAC_LEN]);

/**
 * @Description Converting C-style MAC to a string MAC
 *
 * @param mac - Input MAC address
 * @return string - conversion result.
 */
std::string MacArrayToStr(const unsigned char mac[WIFI_MAC_LEN]);

/**
 * @Description Check whether the array of MAC address is empty
 *
 * @param mac - Input MAC address
 * @return bool - true: empty, false: not empty
 */
bool IsMacArrayEmpty(const unsigned char mac[WIFI_MAC_LEN]);

/**
 * @Description Converting a string IP Address to an integer IP address
 *
 * @param strIp - Input string IP address
 * @return int - integer IP address
 */
int Ip2Number(const std::string& strIp);

/**
 * @Description Converting an integer IP address to a string IP Address
 *
 * @param intIp - Input integer IP address
 * @return string - string IP address
 */
std::string Number2Ip(int intIp);

/**
 * @Description Splitting strings by delimiter
 *
 * @param str - Input string
 * @param delim - Split delimiter
 * @return std::vector<std::string> - Split result
 */
std::vector<std::string> StrSplit(const std::string& str, const std::string& delim);

/**
 * @Description get bundle name
 *
 * @return bool - bundle name
 */
std::string GetBundleName();

/**
 * @Description Check whether the app is a system app
 *
 * @return bool - Returns true for yes, false for no.
 */
bool IsSystemApp();
}  // namespace Wifi
}  // namespace OHOS
#endif