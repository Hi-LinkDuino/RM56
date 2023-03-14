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

#include "wifi_common_util.h"
#include <sstream>
#include <iterator>
#include <regex>
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "wifi_logger.h"

namespace OHOS {
namespace Wifi {
DEFINE_WIFILOG_LABEL("WifiCommonUtil");
static std::string DataAnonymize(const std::string str, const char delim,
    const char hiddenCh, const int startIdx = 0)
{
    std::string s = str;
    constexpr auto minDelimSize = 2;
    constexpr auto minKeepSize = 6;
    if (std::count(s.begin(), s.end(), delim) < minDelimSize) {
        if (s.size() <= minKeepSize) {
            return std::string(s.size(), hiddenCh);
        }
        auto idx1 = 2;
        const auto idx2 = s.size() - 4;
        while (idx1++ < idx2) {
            s[idx1] = hiddenCh;
        }
        return s;
    }

    std::string::size_type begin = s.find_first_of(delim);
    std::string::size_type end = s.find_last_of(delim);
    int idx = 0;
    while (idx++ < startIdx && begin < end) {
        begin = s.find_first_of(delim, begin + 1);
    }
    while (begin++ != end) {
        if (s[begin] != delim) {
            s[begin] = hiddenCh;
        }
    }
    return s;
}

std::string MacAnonymize(const std::string str)
{
    return DataAnonymize(str, ':', '*', 1);
}

std::string IpAnonymize(const std::string str)
{
    return DataAnonymize(str, '.', '*');
}

static unsigned char ConvertStrChar(char ch)
{
    constexpr int numDiffForHexAlphabet = 10;
    if (ch >= '0' && ch <= '9') {
        return (ch - '0');
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + numDiffForHexAlphabet);
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + numDiffForHexAlphabet);
    }
    return 0;
}

errno_t MacStrToArray(const std::string& strMac, unsigned char mac[WIFI_MAC_LEN])
{
    constexpr int strMacLen = 18;
    char tempArray[strMacLen] = { 0 };
    errno_t ret = memcpy_s(tempArray, strMacLen, strMac.c_str(), strMac.size() + 1);
    if (ret != EOK) {
        return ret;
    }

    int idx = 0;
    constexpr int bitWidth = 4;
    char *ptr = nullptr;
    char *p = strtok_s(tempArray, ":", &ptr);
    while (p != nullptr) {
        mac[idx++] = (ConvertStrChar(*p) << bitWidth) | ConvertStrChar(*(p + 1));
        p = strtok_s(nullptr, ":", &ptr);
    }
    return EOK;
}

static char ConvertArrayChar(unsigned char ch)
{
    constexpr int maxDecNum = 9;
    constexpr int numDiffForHexAlphabet = 10;
    if (ch >= 0 && ch <= maxDecNum) {
        return '0' + ch;
    }
    if (ch >= 0xa && ch <= 0xf) {
        return ch + 'a' - numDiffForHexAlphabet;
    }
    return '0';
}

std::string MacArrayToStr(const unsigned char mac[WIFI_MAC_LEN])
{
    constexpr int bitWidth = 4;
    constexpr int noColonBit = 5;
    std::stringstream ss;
    for (int i = 0; i != WIFI_MAC_LEN; ++i) {
        ss << ConvertArrayChar(mac[i] >> bitWidth) << ConvertArrayChar(mac[i] & 0xf);
        if (i != noColonBit) {
            ss << ":";
        }
    }
    return ss.str();
}

bool IsMacArrayEmpty(const unsigned char mac[WIFI_MAC_LEN])
{
    for (int i = 0; i != WIFI_MAC_LEN; ++i) {
        if (mac[i] != 0) {
            return false;
        }
    }
    return true;
}

int Ip2Number(const std::string& strIp)
{
    std::string::size_type front = 0;
    std::string::size_type back = 0;
    int number = 0;
    int size = 32;
    constexpr int sectionSize = 8;

    std::string ip(strIp + '.');
    while ((back = ip.find_first_of('.', back)) != (std::string::size_type)std::string::npos) {
        number |= atoi(ip.substr(front, back - front).c_str()) << (size -= sectionSize);
        front = ++back;
    }
    return number;
}

std::string Number2Ip(int intIp)
{
    constexpr int fourthPartMoveLen = 24;
    constexpr int thirdPartMoveLen = 16;
    constexpr int secondPartMoveLen = 8;

    std::string ip;
    ip.append(std::to_string(((unsigned int)intIp & 0xff000000) >> fourthPartMoveLen));
    ip.push_back('.');
    ip.append(std::to_string((intIp & 0x00ff0000) >> thirdPartMoveLen));
    ip.push_back('.');
    ip.append(std::to_string((intIp & 0x0000ff00) >> secondPartMoveLen));
    ip.push_back('.');
    ip.append(std::to_string(intIp & 0x000000ff));
    return ip;
}

std::vector<std::string> StrSplit(const std::string& str, const std::string& delim) {
    std::regex re(delim);
    std::sregex_token_iterator
        first{ str.begin(), str.end(), re, -1 },
        last;
    return { first, last };
}

sptr<AppExecFwk::IBundleMgr> GetBundleManager()
{
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        WIFI_LOGE("Get system ability manager failed!");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(systemManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID));
}

std::string GetBundleName()
{
    sptr<AppExecFwk::IBundleMgr> bundleInstance = GetBundleManager();
    if (bundleInstance == nullptr) {
        return "";
    }

    std::string bundleName;
    int uid = IPCSkeleton::GetCallingUid();
    if (!bundleInstance->GetBundleNameForUid(uid, bundleName)) {
        WIFI_LOGE("Get bundle name failed!");
    }
    WIFI_LOGI("Get bundle name uid[%{public}d]: %{public}s", uid, bundleName.c_str());
    return bundleName;
}

bool IsSystemApp()
{
    sptr<AppExecFwk::IBundleMgr> bundleInstance = GetBundleManager();
    if (bundleInstance == nullptr) {
        return false;
    }

    std::string bundleName;
    int uid = IPCSkeleton::GetCallingUid();
    bool isSysApp = bundleInstance->CheckIsSystemAppByUid(uid);
    WIFI_LOGI("Is system App uid[%{public}d]: %{public}d", uid, isSysApp);
    return isSysApp;
}
}  // namespace Wifi
}  // namespace OHOS