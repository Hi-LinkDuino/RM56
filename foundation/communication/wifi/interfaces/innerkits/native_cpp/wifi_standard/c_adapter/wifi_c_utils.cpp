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

#include "wifi_c_utils.h"
#include <map>
#include "wifi_common_util.h"

namespace OHOS {
namespace Wifi {
static std::map<ErrCode, WifiErrorCode> g_ErrCodeMap = {
    {WIFI_OPT_SUCCESS, WIFI_SUCCESS},
    {WIFI_OPT_FAILED, ERROR_WIFI_UNKNOWN},
    {WIFI_OPT_NOT_SUPPORTED, ERROR_WIFI_NOT_SUPPORTED},
    {WIFI_OPT_INVALID_PARAM, ERROR_WIFI_INVALID_ARGS},
    {WIFI_OPT_FORBID_AIRPLANE, ERROR_WIFI_NOT_AVAILABLE},
    {WIFI_OPT_FORBID_POWSAVING, ERROR_WIFI_NOT_AVAILABLE},
    {WIFI_OPT_PERMISSION_DENIED, ERROR_WIFI_UNKNOWN},
    {WIFI_OPT_OPEN_FAIL_WHEN_CLOSING, ERROR_WIFI_BUSY},
    {WIFI_OPT_OPEN_SUCC_WHEN_OPENED, ERROR_WIFI_BUSY},
    {WIFI_OPT_CLOSE_FAIL_WHEN_OPENING, ERROR_WIFI_BUSY},
    {WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED, ERROR_WIFI_BUSY},
    {WIFI_OPT_STA_NOT_OPENED, ERROR_WIFI_NOT_STARTED},
    {WIFI_OPT_SCAN_NOT_OPENED, ERROR_WIFI_NOT_STARTED},
    {WIFI_OPT_AP_NOT_OPENED, ERROR_WIFI_NOT_STARTED},
    {WIFI_OPT_INVALID_CONFIG, ERROR_WIFI_UNKNOWN}
};

WifiErrorCode GetCErrorCode(ErrCode errCode)
{
    std::map<ErrCode, WifiErrorCode>::const_iterator iter = g_ErrCodeMap.find(errCode);
    return iter == g_ErrCodeMap.end() ? ERROR_WIFI_UNKNOWN : iter->second;
}

WifiErrorCode IpStrToArray(const std::string& str, unsigned int ipAddr[IPV4_ARRAY_LEN]) {
    std::vector<std::string> vec = StrSplit(str, "\\.");
    if (vec.size() != IPV4_ARRAY_LEN) {
        return ERROR_WIFI_INVALID_ARGS;
    }
    for (int i = 0; i != IPV4_ARRAY_LEN && i != (int)vec.size(); ++i) {
        ipAddr[i] = std::stoi(vec[i]);
    }
    return WIFI_SUCCESS;
}

std::string IpArrayToStr(const unsigned int ipAddr[IPV4_ARRAY_LEN]) {
    std::string str = "";
    for (int i = 0; i != IPV4_ARRAY_LEN; ++i) {
        str += std::to_string(ipAddr[i]);
        if (i != IPV4_ARRAY_LEN - 1) {
            str += ".";
        }
    }
    return str;
}
}  // namespace Wifi
}  // namespace OHOS
