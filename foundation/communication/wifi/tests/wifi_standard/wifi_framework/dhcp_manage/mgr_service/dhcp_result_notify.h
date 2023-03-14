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

#ifndef OHOS_DHCP_RESULT_NOTIFY_H
#define OHOS_DHCP_RESULT_NOTIFY_H

#include "i_dhcp_result_notify.h"
#include "dhcp_define.h"


namespace OHOS {
namespace Wifi {
class DhcpResultNotify : public OHOS::Wifi::IDhcpResultNotify {
public:
    /**
     * @Description : Construct a new dhcp result notify object
     *
     */
    DhcpResultNotify();

    /**
     * @Description : Destroy the dhcp result notify object
     *
     */
    ~DhcpResultNotify() override;

    /**
     * @Description：asyn get dhcp result of specified interface success notify
     *
     * @param status - int
     * @param ifname - interface name,eg:wlan0
     * @param result - dhcp result
     */
    void OnSuccess(int status, const std::string& ifname, DhcpResult& result) override;

    /**
     * @Description：asyn get dhcp result of specified interface failed notify
     *
     * @param status - int
     * @param ifname - interface name,eg:wlan0
     * @param reason - failed reason
     */
    void OnFailed(int status, const std::string& ifname, const std::string& reason) override;

    /**
     * @Description : Get the abnormal exit notify of dhcp server process.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     */
    void OnSerExitNotify(const std::string& ifname) override;
};
}  // namespace Wifi
}  // namespace OHOS
#endif