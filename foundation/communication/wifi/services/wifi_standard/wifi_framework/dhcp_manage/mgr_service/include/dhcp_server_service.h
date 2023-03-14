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

#ifndef OHOS_DHCP_SERVER_SERVICE_H
#define OHOS_DHCP_SERVER_SERVICE_H

#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <thread>

#include "i_dhcp_server_service.h"
#include "dhcp_define.h"


namespace OHOS {
namespace Wifi {
class DhcpServerService : public IDhcpServerService {
public:
    /**
     * @Description : Construct a new dhcp server service object.
     *
     */
    DhcpServerService();

    /**
     * @Description : Destroy the dhcp server service object.
     *
     */
    ~DhcpServerService() override;

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

    /**
     * @Description : Reload dhcp server config.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int ReloadConfig(const std::string& ifname) override;

    /**
     * @Description : Check and update dhcp server config of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int CheckAndUpdateConf(const std::string& ifname);

    /**
     * @Description : Check invalid or already exist in dhcp range.
     *
     * @param range - ip address range [in]
     * @Return : true - yes, false - no.
     */
    bool CheckIpAddrRange(const DhcpRange& range);

    /**
     * @Description : Add the specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int AddSpecifiedInterface(const std::string& ifname);

    /**
     * @Description : Delete the specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int DelSpecifiedInterface(const std::string& ifname);

    /**
     * @Description : Get dhcp ip address pool of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param ipRange - ip address range [out]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int GetUsingIpRange(const std::string ifname, std::string& ipRange);

    /**
     * @Description : Create default config file.
     *
     * @param strFile - config file name [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int CreateDefaultConfigFile(const std::string strFile);

    /**
     * @Description : Stop dhcp server on exit
     *
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int StopDhcpServerOnExit();

private:
    /**
     * @Description : Fork parent process function.
     *
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int ForkParentProcess();

    /**
     * @Description : Fork child process function for start dhcp server process.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int ForkExecProcess(const std::string ifname, const std::string ip, const std::string mask, const std::string pool);

    /**
     * @Description : Stop dhcp server process.
     *
     * @param serverPid - process id [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int StopServer(const pid_t& serverPid);

    /**
     * @Description : Exit dhcp process exit abnormal notify thread.
     *
     */
    void ExitDhcpMgrThreadFunc();

    /**
     * @Description : Dhcp server process exit abnormal notify.
     *
     */
    void RunDhcpSerProExitThreadFunc();

    /**
     * @Description : Register the SIGCHID signal.
     *
     */
    void RegisterSignal() const;

    /**
     * @Description : Unregister the SIGCHID signal.
     *
     */
    void UnregisterSignal() const;

    /**
     * @Description : Receives the SIGCHLD signal of the dhcp server process.
     *
     * @param signum - signal num [in]
     */
    static void SigChildHandler(int signum);

    /**
     * @Description : Get dhcp server process id.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : dhcp server process id.
     */
    static pid_t GetDhcpServerPid(const std::string &ifname);

    /**
     * @Description : Set dhcp server info.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param status - process run status [in]
     * @param serverPid - process id [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    static int SetDhcpServerInfo(const std::string &ifname, int status, const pid_t& serverPid);

private:
    std::set<std::string> m_setInterfaces;                          /* the started specified interfaces */
    std::map<std::string, std::list<DhcpRange>> m_mapTagDhcpRange;  /* dhcp server can be used ip range */
    std::map<std::string, std::list<DhcpRange>> m_mapInfDhcpRange;  /* dhcp server using ip range */
    std::map<std::string, IDhcpResultNotify *> m_mapDhcpSerExitNotify;
    static std::map<std::string, DhcpServerInfo> m_mapDhcpServer;

    bool bDhcpSerProExitThread;
    std::thread *pDhcpSerProExitThread;
};
}  // namespace Wifi
}  // namespace OHOS
#endif