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

#ifndef OHOS_DHCP_CLIENT_SERVICE_INTERFACE_H
#define OHOS_DHCP_CLIENT_SERVICE_INTERFACE_H

#include "i_dhcp_result_notify.h"
#include "dhcp_define.h"

namespace OHOS {
namespace Wifi {
class IDhcpClientService {
public:
    /**
     * @Description : Construct a new dhcp client base service object.
     *
     */
    IDhcpClientService()
    {
    }

    /**
     * @Description : Destroy the dhcp client base service object.
     *
     */
    virtual ~IDhcpClientService()
    {
    }

    /**
     * @Description : Start dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param bIpv6 - can or not get ipv6 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    virtual int StartDhcpClient(const std::string& ifname, bool bIpv6) = 0;

    /**
     * @Description : Stop dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param bIpv6 - can or not get ipv6 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    virtual int StopDhcpClient(const std::string& ifname, bool bIpv6) = 0;

    /**
     * @Description : Get dhcp client service running status of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : 0 - not start, 1 - normal started, -1 - not normal.
     */
    virtual int GetDhcpStatus(const std::string& ifname) = 0;

    /**
     * @Description : Obtain the dhcp result of specified interface asynchronously.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param dhcp - dhcp result notify [in]
     * @param timeouts - timeout interval [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    virtual int GetDhcpResult(const std::string& ifname, IDhcpResultNotify *pResultNotify, int timeouts) = 0;

    /**
     * @Description : Obtain the dhcp info of specified interface synchronously.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param dhcp - dhcp info [out]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    virtual int GetDhcpInfo(const std::string& ifname, DhcpServiceInfo& dhcp) = 0;

    /**
     * @Description : Renew dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    virtual int RenewDhcpClient(const std::string& ifname) = 0;

    /**
     * @Description : Release dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    virtual int ReleaseDhcpClient(const std::string& ifname) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif