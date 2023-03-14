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

#ifndef OHOS_DHCP_RESULT_NOTIFY_INTERFACE_H
#define OHOS_DHCP_RESULT_NOTIFY_INTERFACE_H

#include "dhcp_define.h"


namespace OHOS {
namespace Wifi {
class IDhcpResultNotify {
public:
    /**
     * @Description : Destroy the dhcp result notify base object.
     *
     */
    virtual ~IDhcpResultNotify() {}

    /**
     * @Description : Get success the dhcp result of specified interface.
     *
     * @param status - success : DHCP_OPT_SUCCESS, failed : DHCP_OPT_FAILED, timeout : DHCP_OPT_TIMEOUT [in]
     * @param ifname - interface name, eg:wlan0 [in]
     * @param result - dhcp result [in]
     */
    virtual void OnSuccess(int status, const std::string& ifname, DhcpResult& result) = 0;

    /**
     * @Description : Get failed the dhcp result of specified interface.
     *
     * @param status - success : DHCP_OPT_SUCCESS, failed : DHCP_OPT_FAILED, timeout : DHCP_OPT_TIMEOUT [in]
     * @param ifname - interface name, eg:wlan0 [in]
     * @param reason - failed reason [in]
     */
    virtual void OnFailed(int status, const std::string& ifname, const std::string& reason) = 0;

    /**
     * @Description : Get the abnormal exit notify of dhcp server process.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     */
    virtual void OnSerExitNotify(const std::string& ifname) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif