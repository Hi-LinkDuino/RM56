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

#ifndef OHOS_DHCP_CLIENT_SERVICE_IMPL_H
#define OHOS_DHCP_CLIENT_SERVICE_IMPL_H

#include <map>
#include <list>
#include <thread>
#include <mutex>

#include "i_dhcp_client_service.h"
#include "dhcp_define.h"


namespace OHOS {
namespace Wifi {
struct DhcpResultReq {
    int timeouts;
    int getTimestamp;
    IDhcpResultNotify *pResultNotify;

    DhcpResultReq()
    {
        timeouts = RECEIVER_TIMEOUT;
        getTimestamp = 0;
        pResultNotify = nullptr;
    }
};
class DhcpEventSubscriber;
class DhcpClientServiceImpl : public IDhcpClientService {
public:
    /**
     * @Description : Construct a new dhcp client service object.
     *
     */
    DhcpClientServiceImpl();

    /**
     * @Description : Destroy the dhcp client service object.
     *
     */
    ~DhcpClientServiceImpl() override;

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
     * @Description : Get dhcp client service running status of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : 0 - not start, 1 - normal started, -1 - not normal.
     */
    int GetDhcpStatus(const std::string& ifname) override;

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
    int ReleaseDhcpClient(const std::string &ifname) override;

    /**
     * @Description : Handle dhcp result.
     *
     * @param second - sleep second number [out]
     */
    void DhcpResultHandle(uint32_t &second);

    /**
     * @Description : Get the dhcp client process pid of specified interface.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : The dhcp client process pid.
     */
    pid_t GetDhcpClientProPid(const std::string &ifname);

    /**
     * @Description : Check the dhcp client process of specified interface is or not running.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int CheckDhcpClientRunning(const std::string &ifname);

    /**
     * @Description : Get dhcp event success ipv4 result.
     *
     * @param splits - dhcp event result vector [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    static int GetSuccessIpv4Result(const std::vector<std::string> &splits);

    /**
     * @Description : Get dhcp event ipv4 result.
     *
     * @param code - dhcp event result code [in]
     * @param splits - dhcp event result vector [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    static int GetDhcpEventIpv4Result(const int code, const std::vector<std::string> &splits);

    /**
     * @Description : Handle dhcp event result string.
     *
     * @param code - dhcp event result code [in]
     * @param data - dhcp event result string [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    static int DhcpEventResultHandle(const int code, const std::string &data);

public:
    static std::map<std::string, DhcpResult> m_mapDhcpResult;
    static std::map<std::string, DhcpServiceInfo> m_mapDhcpInfo;

private:
    /**
     * @Description : Start dhcp result handle threads.
     *
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int InitDhcpMgrThread();
    /**
     * @Description : Exit dhcp result handle threads and recv msg threads.
     *
     */
    void ExitDhcpMgrThread();
    /**
     * @Description : Check dhcp result req is or not timeout.
     *
     */
    void CheckTimeout();
    /**
     * @Description : Dhcp result handle threads execution function.
     *
     */
    void RunDhcpResultHandleThreadFunc();
    /**
     * @Description : Fork child process function for start or stop dhcp process.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param bIpv6 - can or not get ipv6 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int ForkExecChildProcess(const std::string& ifname, bool bIpv6, bool bStart = false);
    /**
     * @Description : Fork parent process function for handle dhcp function.
     *
     * @param ifname - interface name, eg:wlan0 [in]
     * @param bIpv6 - can or not get ipv6 [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int ForkExecParentProcess(const std::string& ifname, bool bIpv6, bool bStart = false, pid_t pid = 0);

    /**
     * @Description : Subscribe dhcp event.
     *
     * @param strAction - event action [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int SubscribeDhcpEvent(const std::string &strAction);
    /**
     * @Description : Unsubscribe dhcp event.
     *
     * @param strAction - event action [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int UnsubscribeDhcpEvent(const std::string &strAction);

    /**
     * @Description : release result notify memory.
     *
     */
    void ReleaseResultNotifyMemory();

    /**
     * @Description : Unsubscribe all dhcp event.
     *
     * @param strAction - event action [in]
     * @Return : success - DHCP_OPT_SUCCESS, failed - others.
     */
    int UnsubscribeAllDhcpEvent();

private:
    std::mutex mResultNotifyMutex;
    bool isExitDhcpResultHandleThread;
    std::thread *pDhcpResultHandleThread;

    std::map<std::string, std::list<DhcpResultReq*>> m_mapDhcpResultNotify;
    std::map<std::string, std::shared_ptr<OHOS::Wifi::DhcpEventSubscriber>> m_mapEventSubscriber;
};
}  // namespace Wifi
}  // namespace OHOS
#endif