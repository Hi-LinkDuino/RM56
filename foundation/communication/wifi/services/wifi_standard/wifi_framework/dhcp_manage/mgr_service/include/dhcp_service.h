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

#ifndef OHOS_DHCP_SERVICE_H
#define OHOS_DHCP_SERVICE_H

#include "i_dhcp_service.h"
#include "dhcp_define.h"
#include "dhcp_client_service_impl.h"
#include "dhcp_server_service.h"


namespace OHOS {
namespace Wifi {
class DhcpService : public IDhcpService {
public:
    /**
     * @Description : Construct a new dhcp service object.
     *
     */
    DhcpService();

    /**
     * @Description : Destroy the dhcp service object.
     *
     */
    ~DhcpService() override;

    /**
     * @Description : Start dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param bIpv6 - can or not get ipv6 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int StartDhcpClient(const std::string& ifname, bool bIpv6) override;

    /**
     * @Description : Stop dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param bIpv6 - can or not get ipv6 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int StopDhcpClient(const std::string& ifname, bool bIpv6) override;

    /**
     * @Description : Obtain the dhcp result of specified interface asynchronously.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param dhcp - dhcp result notify [in]
     * @param timeouts - timeout interval [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int GetDhcpResult(const std::string& ifname, IDhcpResultNotify *pResultNotify, int timeouts) override;

    /**
     * @Description : Obtain the dhcp info of specified interface synchronously.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param dhcp - dhcp info [out]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int GetDhcpInfo(const std::string& ifname, DhcpServiceInfo& dhcp) override;

    /**
     * @Description : Renew dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int RenewDhcpClient(const std::string& ifname) override;

    /**
     * @Description : Release dhcp client service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int ReleaseDhcpClient(const std::string& ifname) override;

    /**
     * @Description : Start dhcp server service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int StartDhcpServer(const std::string& ifname) override;

    /**
     * @Description : Stop dhcp server service of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int StopDhcpServer(const std::string& ifname) override;

    /**
     * @Description : Get dhcp server service running status.
     *
     * @Return : 0 - not start, 1 - normal started.
     */
    int GetServerStatus(void) override;

    /**
     * @Description : Add or update dhcp ip address pool.
     *
     * @param tagName - ip address pool tag name [in]
     * @param range - ip address range [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int PutDhcpRange(const std::string& tagName, const DhcpRange& range) override;

    /**
     * @Description : Remove dhcp ip address pool.
     *
     * @param tagName - ip address pool tag name [in]
     * @param range - ip address range [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int RemoveDhcpRange(const std::string& tagName, const DhcpRange& range) override;

    /**
     * @Description : Remove all dhcp ip address pool.
     *
     * @param tagName - ip address pool tag name [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int RemoveAllDhcpRange(const std::string& tagName) override;

    /**
     * @Description : Set dhcp ip address pool of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param range - ip address range [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int SetDhcpRange(const std::string& ifname, const DhcpRange& range) override;

    /**
     * @Description : Set dhcp ip address pool of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param tagName - ip address pool tag name [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int SetDhcpRange(const std::string& ifname, const std::string& tagName) override;

    /**
     * @Description : Get dhcp server lease info.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param leases - lease info [out]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int GetLeases(const std::string& ifname, std::vector<std::string>& leases) override;

    /**
     * @Description : Obtain the abnormal exit status of dhcp server process.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param pResultNotify - pointer to dhcp result notify [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int GetDhcpSerProExit(const std::string& ifname, IDhcpResultNotify *pResultNotify) override;

private:
    int InitServerService(void);

private:
    std::unique_ptr<IDhcpClientService> m_pClientService;
    std::unique_ptr<IDhcpServerService> m_pServerService;
};
}  // namespace Wifi
}  // namespace OHOS
#endif