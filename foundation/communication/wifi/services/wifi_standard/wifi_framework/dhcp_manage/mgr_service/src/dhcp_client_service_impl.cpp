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

#include "dhcp_client_service_impl.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dhcp_func.h"
#include "securec.h"
#include "wifi_logger.h"
#include "dhcp_event_subscriber.h"

DEFINE_WIFILOG_DHCP_LABEL("DhcpClientServiceImpl");

namespace OHOS {
namespace Wifi {
std::map<std::string, DhcpResult> DhcpClientServiceImpl::m_mapDhcpResult;
std::map<std::string, DhcpServiceInfo> DhcpClientServiceImpl::m_mapDhcpInfo;
DhcpClientServiceImpl::DhcpClientServiceImpl()
{
    isExitDhcpResultHandleThread = false;
    pDhcpResultHandleThread = nullptr;

    if (!m_mapDhcpResultNotify.empty()) {
        ReleaseResultNotifyMemory();
        m_mapDhcpResultNotify.clear();
    }
    m_mapEventSubscriber.clear();
    InitDhcpMgrThread();
    DhcpFunc::CreateDirs(DHCP_WORK_DIR);
}

DhcpClientServiceImpl::~DhcpClientServiceImpl()
{
    if (!m_mapEventSubscriber.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl destructor mapEventSubscriber is not empty!");
        if (UnsubscribeAllDhcpEvent() != DHCP_OPT_SUCCESS) {
            WIFI_LOGE("DhcpClientServiceImpl unregister all dhcp event failed!");
        }
    }

    ExitDhcpMgrThread();
}

void DhcpClientServiceImpl::ReleaseResultNotifyMemory()
{
    for (auto& item : m_mapDhcpResultNotify) {
        auto& secondItem = item.second;
        for (auto& each : secondItem) {
            if (each != nullptr) {
                delete each;
                each = nullptr;
            }
        }
    }
}

int DhcpClientServiceImpl::InitDhcpMgrThread()
{
    pDhcpResultHandleThread = new std::thread(&DhcpClientServiceImpl::RunDhcpResultHandleThreadFunc, this);
    if (pDhcpResultHandleThread == nullptr) {
        WIFI_LOGE("DhcpClientServiceImpl::InitDhcpMgrThread() init pDhcpResultHandleThread failed!");
        return DHCP_OPT_FAILED;
    }

    return DHCP_OPT_SUCCESS;
}

void DhcpClientServiceImpl::ExitDhcpMgrThread()
{
    isExitDhcpResultHandleThread = true;

    if (pDhcpResultHandleThread != nullptr) {
        pDhcpResultHandleThread->join();
        delete pDhcpResultHandleThread;
        pDhcpResultHandleThread = nullptr;
    }

    if (!m_mapDhcpResultNotify.empty()) {
        WIFI_LOGE("ExitDhcpMgrThread() error, m_mapDhcpResultNotify is not empty!");
        ReleaseResultNotifyMemory();
        m_mapDhcpResultNotify.clear();
    }
}

void DhcpClientServiceImpl::CheckTimeout()
{
    uint32_t tempTime = 0;
    uint32_t curTime = (uint32_t)time(NULL);
    for (auto &itemNotify : m_mapDhcpResultNotify) {
        std::string ifname = itemNotify.first;
        WIFI_LOGI("CheckTimeout() ifname:%{public}s, notify1 second size:%{public}d.",
            ifname.c_str(),
            (int)itemNotify.second.size());
        auto iterReq = itemNotify.second.begin();
        while (iterReq != itemNotify.second.end()) {
            if ((*iterReq == nullptr) || ((*iterReq)->pResultNotify == nullptr)) {
                WIFI_LOGE("DhcpClientServiceImpl::CheckTimeout() error, *iterReq or pResultNotify is nullptr!");
                return;
            }
            tempTime = (*iterReq)->getTimestamp + (*iterReq)->timeouts;
            if (tempTime <= curTime) {
                /* get dhcp result timeout */
                WIFI_LOGW("CheckTimeout() ifname:%{public}s get timeout, getTime:%{public}u,timeout:%{public}d, "
                          "curTime:%{public}u!",
                    ifname.c_str(),
                    (*iterReq)->getTimestamp,
                    (*iterReq)->timeouts,
                    curTime);
                (*iterReq)->pResultNotify->OnFailed(DHCP_OPT_TIMEOUT, ifname, "get dhcp result timeout!");
                delete *iterReq;
                *iterReq = nullptr;
                iterReq = itemNotify.second.erase(iterReq);
            } else {
                ++iterReq;
            }
        }
    }
}

void DhcpClientServiceImpl::DhcpResultHandle(uint32_t &second)
{
    std::unique_lock<std::mutex> lock(mResultNotifyMutex);
    if (m_mapDhcpResultNotify.empty()) {
        second = SLEEP_TIME_200_MS;
        return;
    }

    /* Check timeout */
    CheckTimeout();
    auto iterNotify = m_mapDhcpResultNotify.begin();
    while (iterNotify != m_mapDhcpResultNotify.end()) {
        /* Check dhcp result notify size */
        std::string ifname = iterNotify->first;
        if (iterNotify->second.size() <= 0) {
            iterNotify = m_mapDhcpResultNotify.erase(iterNotify);
            WIFI_LOGI("DhcpResultHandle() ifname:%{public}s, dhcp result notify size:0, erase!", ifname.c_str());
            continue;
        }

        /* Check dhcp result */
        auto iterDhcpResult = DhcpClientServiceImpl::m_mapDhcpResult.find(ifname);
        if (iterDhcpResult == DhcpClientServiceImpl::m_mapDhcpResult.end()) {
            WIFI_LOGI("DhcpResultHandle() ifname:%{public}s, dhcp result is getting...", ifname.c_str());
            ++iterNotify;
            continue;
        }

        auto iterReq = iterNotify->second.begin();
        while (iterReq != iterNotify->second.end()) {
            if ((*iterReq == nullptr) || ((*iterReq)->pResultNotify == nullptr)) {
                WIFI_LOGE("DhcpResultHandle() %{public}s iterReq or pResultNotify is nullptr!", ifname.c_str());
                second = SLEEP_TIME_500_MS;
                return;
            }

            /* Handle dhcp result notify */
            WIFI_LOGI("DhcpResultHandle() ifname:%{public}s, isOptSuc:%{public}d.",
                ifname.c_str(), (iterDhcpResult->second).isOptSuc);
            if ((iterDhcpResult->second).isOptSuc) {
                /* get dhcp result success */
                WIFI_LOGI("DhcpResultHandle() ifname:%{public}s get dhcp result success!", ifname.c_str());
                (*iterReq)->pResultNotify->OnSuccess(DHCP_OPT_SUCCESS, ifname, iterDhcpResult->second);
            } else {
                /* get dhcp result failed */
                WIFI_LOGE("DhcpResultHandle() ifname:%{public}s get dhcp result failed!", ifname.c_str());
                (*iterReq)->pResultNotify->OnFailed(DHCP_OPT_FAILED, ifname, "get dhcp result failed!");
            }
            delete *iterReq;
            *iterReq = nullptr;
            iterReq = iterNotify->second.erase(iterReq);
        }

        ++iterNotify;
    }

    WIFI_LOGI("DhcpResultHandle() dhcp result notify finished.");
    second = SLEEP_TIME_500_MS;
}

int DhcpClientServiceImpl::SubscribeDhcpEvent(const std::string &strAction)
{
    if (strAction.empty()) {
        WIFI_LOGE("SubscribeDhcpEvent error, strAction is empty!");
        return DHCP_OPT_ERROR;
    }
    auto iterSubscriber = m_mapEventSubscriber.find(strAction);
    if (iterSubscriber == m_mapEventSubscriber.end()) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(strAction);
        EventFwk::CommonEventSubscribeInfo subInfo(matchingSkills);
        auto dhcpSubscriber = std::make_shared<OHOS::Wifi::DhcpEventSubscriber>(subInfo);
        if (dhcpSubscriber == nullptr) {
            WIFI_LOGE("SubscribeDhcpEvent error, dhcpSubscriber is nullptr!");
            return DHCP_OPT_FAILED;
        }
        m_mapEventSubscriber.emplace(std::make_pair(strAction, dhcpSubscriber));
    }
    if (m_mapEventSubscriber[strAction] == nullptr) {
        WIFI_LOGE("SubscribeDhcpEvent mapEventSubscriber %{public}s nullptr!", strAction.c_str());
        return DHCP_OPT_FAILED;
    }
    if (!DhcpFunc::SubscribeDhcpCommonEvent(m_mapEventSubscriber[strAction])) {
        WIFI_LOGE("SubscribeDhcpEvent SubscribeDhcpCommonEvent %{public}s failed!", strAction.c_str());
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("SubscribeDhcpEvent %{public}s success", strAction.c_str());
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::UnsubscribeDhcpEvent(const std::string &strAction)
{
    if (strAction.empty()) {
        WIFI_LOGE("UnsubscribeDhcpEvent error, strAction is empty!");
        return DHCP_OPT_ERROR;
    }
    auto iterSubscriber = m_mapEventSubscriber.find(strAction);
    if (iterSubscriber == m_mapEventSubscriber.end()) {
        WIFI_LOGI("UnsubscribeDhcpEvent map no exist %{public}s, no need unsubscriber", strAction.c_str());
        return DHCP_OPT_SUCCESS;
    }

    if (m_mapEventSubscriber[strAction] == nullptr) {
        WIFI_LOGE("UnsubscribeDhcpEvent mapEventSubscriber %{public}s nullptr!", strAction.c_str());
        return DHCP_OPT_FAILED;
    }
    if (!DhcpFunc::UnsubscribeDhcpCommonEvent(m_mapEventSubscriber[strAction])) {
        WIFI_LOGE("UnsubscribeDhcpEvent UnsubscribeDhcpCommonEvent %{public}s failed!", strAction.c_str());
        return DHCP_OPT_FAILED;
    }
    m_mapEventSubscriber.erase(iterSubscriber);
    WIFI_LOGI("UnsubscribeDhcpEvent %{public}s success", strAction.c_str());
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::UnsubscribeAllDhcpEvent()
{
    for (auto& e : m_mapEventSubscriber) {
        if (e.second != nullptr) {
            if (!DhcpFunc::UnsubscribeDhcpCommonEvent(e.second)) {
                WIFI_LOGE("UnsubscribeDhcpEvent UnsubscribeDhcpCommonEvent %{public}s failed!", e.first.c_str());
                return DHCP_OPT_FAILED;
            }
        }
    }
    m_mapEventSubscriber.clear();
    WIFI_LOGI("UnsubscribeDhcpEvent all dhcp event success!");
    return DHCP_OPT_SUCCESS;
}

void DhcpClientServiceImpl::RunDhcpResultHandleThreadFunc()
{
    for (; ;) {
        if (isExitDhcpResultHandleThread) {
            WIFI_LOGI("RunDhcpResultHandleThreadFunc() isExitDhcpResultHandleThread:1, break!");
            break;
        }

        uint32_t uSleepSec = SLEEP_TIME_500_MS;
        DhcpResultHandle(uSleepSec);
        usleep(uSleepSec);
    }

    WIFI_LOGI("DhcpClientServiceImpl::RunDhcpResultHandleThreadFunc() end!");
}

int DhcpClientServiceImpl::ForkExecChildProcess(const std::string &ifname, bool bIpv6, bool bStart)
{
    if (bIpv6) {
        /* get ipv4 and ipv6 */
        if (bStart) {
            const char *args[DHCP_CLI_ARGSNUM] = {DHCP_CLIENT_FILE.c_str(), "start", ifname.c_str(), "-a", nullptr};
            if (execv(args[0], const_cast<char *const *>(args)) == -1) {
                WIFI_LOGE("execv start v4 v6 failed,errno:%{public}d,ifname:%{public}s", errno, ifname.c_str());
            }
        } else {
            const char *args[DHCP_CLI_ARGSNUM] = {DHCP_CLIENT_FILE.c_str(), "stop", ifname.c_str(), "-a", nullptr};
            if (execv(args[0], const_cast<char *const *>(args)) == -1) {
                WIFI_LOGE("execv stop v4 v6 failed,errno:%{public}d,ifname:%{public}s", errno, ifname.c_str());
            }
        }
    } else {
        /* only get ipv4 */
        if (bStart) {
            const char *args[DHCP_CLI_ARGSNUM] = {DHCP_CLIENT_FILE.c_str(), "start", ifname.c_str(), "-4", nullptr};
            if (execv(args[0], const_cast<char *const *>(args)) == -1) {
                WIFI_LOGE("execv start v4 failed,errno:%{public}d,ifname:%{public}s", errno, ifname.c_str());
            }
        } else {
            const char *args[DHCP_CLI_ARGSNUM] = {DHCP_CLIENT_FILE.c_str(), "stop", ifname.c_str(), "-4", nullptr};
            if (execv(args[0], const_cast<char *const *>(args)) == -1) {
                WIFI_LOGE("execv stop v4 failed,errno:%{public}d,ifname:%{public}s", errno, ifname.c_str());
            }
        }
    }
    _exit(-1);
}

int DhcpClientServiceImpl::ForkExecParentProcess(const std::string &ifname, bool bIpv6, bool bStart, pid_t pid)
{
    std::string strAction = OHOS::Wifi::COMMON_EVENT_DHCP_GET_IPV4 + "." + ifname;
    if (bStart) {
        /* normal started, update dhcp client service running status */
        auto iter = DhcpClientServiceImpl::m_mapDhcpInfo.find(ifname);
        if (iter != DhcpClientServiceImpl::m_mapDhcpInfo.end()) {
            DhcpClientServiceImpl::m_mapDhcpInfo[ifname].enableIPv6 = bIpv6;
            DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientRunStatus = 1;
            DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientProPid = pid;
        } else {
            DhcpServiceInfo dhcpInfo;
            dhcpInfo.enableIPv6 = bIpv6;
            dhcpInfo.clientRunStatus = 1;
            dhcpInfo.clientProPid = pid;
            DhcpClientServiceImpl::m_mapDhcpInfo.emplace(std::make_pair(ifname, dhcpInfo));
        }
        /* Subscribe dhcp event. */
        if (SubscribeDhcpEvent(strAction) != DHCP_OPT_SUCCESS) {
            return DHCP_OPT_FAILED;
        }
    } else {
        auto iter = DhcpClientServiceImpl::m_mapDhcpInfo.find(ifname);
        if (iter != DhcpClientServiceImpl::m_mapDhcpInfo.end()) {
            /* not start */
            DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientRunStatus = 0;
            DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientProPid = 0;

            auto iterResult = DhcpClientServiceImpl::m_mapDhcpResult.find(ifname);
            if (iterResult != DhcpClientServiceImpl::m_mapDhcpResult.end()) {
                DhcpClientServiceImpl::m_mapDhcpResult.erase(iterResult);
                WIFI_LOGI("ForkExecParentProcess() m_mapDhcpResult erase ifname:%{public}s success.", ifname.c_str());
            }
        }
        /* Unsubscribe dhcp event. */
        if (UnsubscribeDhcpEvent(strAction) != DHCP_OPT_SUCCESS) {
            return DHCP_OPT_FAILED;
        }
    }
    return DHCP_OPT_SUCCESS;
}

pid_t DhcpClientServiceImpl::GetDhcpClientProPid(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("GetDhcpClientProPid() error, ifname is empty!");
        return 0;
    }

    auto iter = DhcpClientServiceImpl::m_mapDhcpInfo.find(ifname);
    if (iter == DhcpClientServiceImpl::m_mapDhcpInfo.end()) {
        WIFI_LOGI("GetDhcpClientProPid() m_mapDhcpInfo no find ifname:%{public}s.", ifname.c_str());
        return 0;
    }

    std::string pidFile = DHCP_WORK_DIR + ifname + DHCP_CLIENT_PID_FILETYPE;
    pid_t newPid = DhcpFunc::GetPID(pidFile);
    if ((newPid > 0) && (newPid != (iter->second).clientProPid)) {
        WIFI_LOGI("GetDhcpClientProPid() GetPID %{public}s new pid:%{public}d, old pid:%{public}d, need update.",
            pidFile.c_str(), newPid, (iter->second).clientProPid);
        DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientProPid = newPid;
    }

    WIFI_LOGI("GetDhcpClientProPid() m_mapDhcpInfo find ifname:%{public}s, pid:%{public}d.",
        ifname.c_str(), DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientProPid);
    return DhcpClientServiceImpl::m_mapDhcpInfo[ifname].clientProPid;
}

int DhcpClientServiceImpl::CheckDhcpClientRunning(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("CheckDhcpClientRunning param error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }

    std::string pidFile = DHCP_WORK_DIR + ifname + DHCP_CLIENT_PID_FILETYPE;
    pid_t pid = DhcpFunc::GetPID(pidFile);
    if (pid > 0) {
        int nRet = DhcpFunc::CheckProRunning(pid, DHCP_CLIENT_FILE);
        if (nRet == -1) {
            WIFI_LOGE("CheckDhcpClientRunning %{public}s failed, pid:%{public}d", ifname.c_str(), pid);
            return DHCP_OPT_FAILED;
        } else if (nRet == 0) {
            WIFI_LOGI("CheckDhcpClientRunning %{public}s, %{public}s is not running, need remove %{public}s",
                ifname.c_str(), DHCP_CLIENT_FILE.c_str(), pidFile.c_str());
            DhcpFunc::RemoveFile(pidFile);
        } else {
            WIFI_LOGI("CheckDhcpClientRunning %{public}s, %{public}s is running, pid:%{public}d",
                ifname.c_str(), DHCP_CLIENT_FILE.c_str(), pid);
        }
    }
    WIFI_LOGI("CheckDhcpClientRunning %{public}s finished, pid:%{public}d, pro:%{public}s",
        ifname.c_str(), pid, DHCP_CLIENT_FILE.c_str());
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::GetSuccessIpv4Result(const std::vector<std::string> &splits)
{
    /* Result format - ifname,time,cliIp,lease,servIp,subnet,dns1,dns2,router1,router2,vendor */
    if (splits.size() != EVENT_DATA_NUM) {
        WIFI_LOGE("GetSuccessIpv4Result() splits.size:%{public}d error!", (int)splits.size());
        return DHCP_OPT_FAILED;
    }

    /* Check field cliIp. */
    if (splits[DHCP_NUM_TWO] == INVALID_STRING) {
        WIFI_LOGE("GetSuccessIpv4Result() cliIp:%{public}s error!", splits[DHCP_NUM_TWO].c_str());
        return DHCP_OPT_FAILED;
    }

    DhcpResult result;
    result.uAddTime = std::stoi(splits[DHCP_NUM_ONE]);
    std::string ifname = splits[DHCP_NUM_ZERO];
    auto iter = DhcpClientServiceImpl::m_mapDhcpResult.find(ifname);
    if ((iter != DhcpClientServiceImpl::m_mapDhcpResult.end()) && ((iter->second).uAddTime == result.uAddTime)) {
        WIFI_LOGI("GetSuccessIpv4Result() %{public}s old %{public}u equal new %{public}u, no need update.",
            ifname.c_str(), (iter->second).uAddTime, result.uAddTime);
        return DHCP_OPT_SUCCESS;
    }

    if (iter != DhcpClientServiceImpl::m_mapDhcpResult.end()) {
        WIFI_LOGI("GetSuccessIpv4Result() DhcpResult %{public}s old %{public}u no equal new %{public}u, need update...",
            ifname.c_str(), (iter->second).uAddTime, result.uAddTime);
    }

    /* Reload dhcp packet info. */
    auto iterInfo = DhcpClientServiceImpl::m_mapDhcpInfo.find(ifname);
    if (iterInfo != DhcpClientServiceImpl::m_mapDhcpInfo.end()) {
        WIFI_LOGI("GetSuccessIpv4Result() m_mapDhcpInfo find ifname:%{public}s.", ifname.c_str());
        DhcpClientServiceImpl::m_mapDhcpInfo[ifname].serverIp = splits[DHCP_NUM_FOUR];
    }

    result.iptype       = 0;
    result.isOptSuc     = true;
    result.strYourCli   = splits[DHCP_NUM_TWO];
    result.uLeaseTime   = std::stoi(splits[DHCP_NUM_THREE]);
    result.strServer    = splits[DHCP_NUM_FOUR];
    result.strSubnet    = splits[DHCP_NUM_FIVE];
    result.strDns1      = splits[DHCP_NUM_SIX];
    result.strDns2      = splits[DHCP_NUM_SEVEN];
    result.strRouter1   = splits[DHCP_NUM_EIGHT];
    result.strRouter2   = splits[DHCP_NUM_NINE];
    result.strVendor    = splits[DHCP_NUM_TEN];
    result.uGetTime     = (uint32_t)time(NULL);
    if (iter != DhcpClientServiceImpl::m_mapDhcpResult.end()) {
        iter->second = result;
    } else {
        DhcpClientServiceImpl::m_mapDhcpResult.emplace(std::make_pair(ifname, result));
    }
    WIFI_LOGI("GetSuccessIpv4Result() %{public}s, %{public}d, opt:%{public}d, cli:%{private}s, server:%{private}s, "
        "strSubnet:%{private}s, strDns1:%{private}s, Dns2:%{private}s, strRouter1:%{private}s, Router2:%{private}s, "
        "strVendor:%{public}s, uLeaseTime:%{public}u, uAddTime:%{public}u, uGetTime:%{public}u.",
        ifname.c_str(), result.iptype, result.isOptSuc, result.strYourCli.c_str(), result.strServer.c_str(),
        result.strSubnet.c_str(), result.strDns1.c_str(), result.strDns2.c_str(), result.strRouter1.c_str(),
        result.strRouter2.c_str(), result.strVendor.c_str(), result.uLeaseTime, result.uAddTime, result.uGetTime);
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::GetDhcpEventIpv4Result(const int code, const std::vector<std::string> &splits)
{
    /* Result format - ifname,time,cliIp,lease,servIp,subnet,dns1,dns2,router1,router2,vendor */
    if (splits.size() != EVENT_DATA_NUM) {
        WIFI_LOGE("GetDhcpEventIpv4Result() splits.size:%{public}d error!", (int)splits.size());
        return DHCP_OPT_FAILED;
    }

    /* Check field ifname and time. */
    if (splits[DHCP_NUM_ZERO].empty() || splits[DHCP_NUM_ONE].empty()) {
        WIFI_LOGE("GetDhcpEventIpv4Result() ifname or time is empty!");
        return DHCP_OPT_FAILED;
    }

    /* Check field cliIp. */
    if (((code == PUBLISH_CODE_SUCCESS) && (splits[DHCP_NUM_TWO] == INVALID_STRING))
    || ((code == PUBLISH_CODE_FAILED) && (splits[DHCP_NUM_TWO] != INVALID_STRING))) {
        WIFI_LOGE("GetDhcpEventIpv4Result() code:%{public}d,%{public}s error!", code, splits[DHCP_NUM_TWO].c_str());
        return DHCP_OPT_FAILED;
    }

    std::string ifname = splits[DHCP_NUM_ZERO];
    if (code == PUBLISH_CODE_FAILED) {
        /* Get failed. */
        DhcpResult result;
        result.iptype = 0;
        result.isOptSuc = false;
        result.uAddTime = std::stoi(splits[DHCP_NUM_ONE]);
        auto iterResult = DhcpClientServiceImpl::m_mapDhcpResult.find(ifname);
        if (iterResult != DhcpClientServiceImpl::m_mapDhcpResult.end()) {
            iterResult->second = result;
        } else {
            m_mapDhcpResult.emplace(std::make_pair(ifname, result));
        }
        WIFI_LOGI("GetDhcpEventIpv4Result() ifname:%{public}s result.isOptSuc:false!", ifname.c_str());
        return DHCP_OPT_SUCCESS;
    }

    /* Get success. */
    if (GetSuccessIpv4Result(splits) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("GetDhcpEventIpv4Result() GetSuccessIpv4Result failed!");
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("GetDhcpEventIpv4Result() ifname:%{public}s result.isOptSuc:true!", ifname.c_str());
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::DhcpEventResultHandle(const int code, const std::string &data)
{
    if (data.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl::DhcpEventResultHandle() error, data is empty!");
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("Enter DhcpEventResultHandle() code:%{public}d,data:%{private}s.", code, data.c_str());

    /* Data format - ipv4:ifname,time,cliIp,lease,servIp,subnet,dns1,dns2,router1,router2,vendor */
    std::string strData(data);
    std::string strFlag;
    std::string strResult;
    if (strData.find(EVENT_DATA_IPV4) != std::string::npos) {
        strFlag = strData.substr(0, (int)EVENT_DATA_IPV4.size());
        if (strFlag != EVENT_DATA_IPV4) {
            WIFI_LOGE("DhcpEventResultHandle() %{public}s ipv4flag:%{public}s error!", data.c_str(), strFlag.c_str());
            return DHCP_OPT_FAILED;
        }
        /* Skip separator ":" */
        strResult = strData.substr((int)EVENT_DATA_IPV4.size() + 1);
    } else if (strData.find(EVENT_DATA_IPV6) != std::string::npos) {
        strFlag = strData.substr(0, (int)EVENT_DATA_IPV6.size());
        if (strFlag != EVENT_DATA_IPV6) {
            WIFI_LOGE("DhcpEventResultHandle() %{public}s ipv6flag:%{public}s error!", data.c_str(), strFlag.c_str());
            return DHCP_OPT_FAILED;
        }
        strResult = strData.substr((int)EVENT_DATA_IPV6.size() + 1);
    } else {
        WIFI_LOGE("DhcpEventResultHandle() data:%{public}s error, no find ipflag!", data.c_str());
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("DhcpEventResultHandle() flag:%{public}s, result:%{private}s.", strFlag.c_str(), strResult.c_str());

    if (strFlag == EVENT_DATA_IPV4) {
        std::vector<std::string> vecSplits;
        if (!DhcpFunc::SplitString(strResult, EVENT_DATA_DELIMITER, EVENT_DATA_NUM, vecSplits)) {
            WIFI_LOGE("DhcpEventResultHandle() SplitString strResult:%{public}s failed!", strResult.c_str());
            return DHCP_OPT_FAILED;
        }

        if (GetDhcpEventIpv4Result(code, vecSplits) != DHCP_OPT_SUCCESS) {
            WIFI_LOGE("DhcpEventResultHandle() GetDhcpEventIpv4Result failed!");
            return DHCP_OPT_FAILED;
        }
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::StartDhcpClient(const std::string &ifname, bool bIpv6)
{
    if (ifname.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl::StartDhcpClient() error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    WIFI_LOGI("enter StartDhcpClient()...ifname:%{public}s, bIpv6:%{public}d.", ifname.c_str(), bIpv6);

    /* check config */
    /* check dhcp client service running status */
    if (CheckDhcpClientRunning(ifname) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("StartDhcpClient CheckDhcpClientRunning ifname:%{public}s failed.", ifname.c_str());
        return DHCP_OPT_FAILED;
    }
    int nStatus = GetDhcpStatus(ifname);
    if (nStatus == 1) {
        WIFI_LOGI("StartDhcpClient() running status:%{public}d, service already started, ifname:%{public}s.",
            nStatus, ifname.c_str());
        /* reload config */
        return DHCP_OPT_SUCCESS;
    }

    /* start dhcp client service */
    pid_t pid;
    if ((pid = vfork()) < 0) {
        WIFI_LOGE("StartDhcpClient() vfork() failed, pid:%{public}d.", pid);
        return DHCP_OPT_FAILED;
    }
    if (pid == 0) {
        /* Child process */
        ForkExecChildProcess(ifname, bIpv6, true);
    } else {
        /* Parent process */
        WIFI_LOGI("StartDhcpClient() vfork %{public}d success, parent:%{public}d, begin waitpid...", pid, getpid());
        pid_t pidRet = waitpid(pid, nullptr, 0);
        if (pidRet == pid) {
            WIFI_LOGI("StartDhcpClient() waitpid child:%{public}d success.", pid);
        } else {
            WIFI_LOGE("StartDhcpClient() waitpid child:%{public}d failed, pidRet:%{public}d!", pid, pidRet);
        }

        return ForkExecParentProcess(ifname, bIpv6, true, pid);
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::StopDhcpClient(const std::string &ifname, bool bIpv6)
{
    if (ifname.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl::StopDhcpClient() error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    WIFI_LOGI("enter StopDhcpClient()...ifname:%{public}s, bIpv6:%{public}d.", ifname.c_str(), bIpv6);

    /* check dhcp client service running status */
    bool bExecParentProcess = true;
    int nStatus = GetDhcpStatus(ifname);
    if (nStatus == 0) {
        WIFI_LOGI("StopDhcpClient() status:%{public}d, service already stopped, ifname:%{public}s.",
            nStatus, ifname.c_str());
        return DHCP_OPT_SUCCESS;
    } else if (nStatus == -1) {
        WIFI_LOGI("StopDhcpClient() status:%{public}d, service not start or started, not need ExecParentProcess, "
                  "ifname:%{public}s.", nStatus, ifname.c_str());
        bExecParentProcess = false;
    }

    /* stop dhcp client service */
    pid_t pid;
    if ((pid = vfork()) < 0) {
        WIFI_LOGE("StopDhcpClient() vfork() failed, pid:%{public}d.", pid);
        return DHCP_OPT_FAILED;
    }
    if (pid == 0) {
        /* Child process */
        ForkExecChildProcess(ifname, bIpv6);
        return DHCP_OPT_SUCCESS;
    } else {
        /* Parent process */
        WIFI_LOGI("StopDhcpClient() vfork %{public}d success, parent:%{public}d, begin waitpid...", pid, getpid());
        pid_t pidRet = waitpid(pid, nullptr, 0);
        if (pidRet == pid) {
            WIFI_LOGI("StopDhcpClient() waitpid child:%{public}d success.", pid);
        } else {
            WIFI_LOGE("StopDhcpClient() waitpid child:%{public}d failed, pidRet:%{public}d!", pid, pidRet);
        }

        return bExecParentProcess ? ForkExecParentProcess(ifname, bIpv6) : DHCP_OPT_SUCCESS;
    }
}

int DhcpClientServiceImpl::GetDhcpStatus(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl::GetDhcpStatus() error, ifname is empty!");
        return -1;
    }

    auto iter = DhcpClientServiceImpl::m_mapDhcpInfo.find(ifname);
    if (iter == DhcpClientServiceImpl::m_mapDhcpInfo.end()) {
        WIFI_LOGI("DhcpClientServiceImpl::GetDhcpStatus() m_mapDhcpInfo no find ifname:%{public}s.", ifname.c_str());
        return -1;
    }

    WIFI_LOGI("GetDhcpStatus() m_mapDhcpInfo find ifname:%{public}s, clientRunStatus:%{public}d.",
        ifname.c_str(),
        (iter->second).clientRunStatus);
    return (iter->second).clientRunStatus;
}

int DhcpClientServiceImpl::GetDhcpResult(const std::string &ifname, IDhcpResultNotify *pResultNotify, int timeouts)
{
    if (ifname.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl::GetDhcpResult() error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    if (pResultNotify == nullptr) {
        WIFI_LOGE("GetDhcpResult() ifname:%{public}s error, pResultNotify is nullptr!", ifname.c_str());
        return DHCP_OPT_FAILED;
    }

    DhcpResultReq *pResultReq = new DhcpResultReq;
    if (pResultReq == nullptr) {
        WIFI_LOGE("GetDhcpResult() new failed! ifname:%{public}s.", ifname.c_str());
        return DHCP_OPT_FAILED;
    }
    pResultReq->timeouts = timeouts;
    pResultReq->getTimestamp = (uint32_t)time(NULL);
    pResultReq->pResultNotify = pResultNotify;

    std::unique_lock<std::mutex> lock(mResultNotifyMutex);
    auto iter = m_mapDhcpResultNotify.find(ifname);
    if (iter != m_mapDhcpResultNotify.end()) {
        iter->second.push_back(pResultReq);
    } else {
        std::list<DhcpResultReq *> listDhcpResultReq;
        listDhcpResultReq.push_back(pResultReq);
        m_mapDhcpResultNotify.emplace(std::make_pair(ifname, listDhcpResultReq));
    }
    WIFI_LOGI("GetDhcpResult() ifname:%{public}s,timeouts:%{public}d, result push_back!", ifname.c_str(), timeouts);
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::GetDhcpInfo(const std::string &ifname, DhcpServiceInfo &dhcp)
{
    if (ifname.empty()) {
        WIFI_LOGE("DhcpClientServiceImpl::GetDhcpInfo() error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    auto iter = DhcpClientServiceImpl::m_mapDhcpInfo.find(ifname);
    if (iter != DhcpClientServiceImpl::m_mapDhcpInfo.end()) {
        dhcp = iter->second;
    } else {
        WIFI_LOGE("GetDhcpInfo() failed, m_mapDhcpInfo no find ifname:%{public}s.", ifname.c_str());
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::RenewDhcpClient(const std::string &ifname)
{
    WIFI_LOGI("enter DhcpClientServiceImpl::RenewDhcpClient()...ifname:%{public}s.", ifname.c_str());
    int nStatus = GetDhcpStatus(ifname);
    if (nStatus != 1) {
        WIFI_LOGW("RenewDhcpClient() dhcp client service not started, now start ifname:%{public}s.", ifname.c_str());

        /* Start dhcp client service */
        return StartDhcpClient(ifname, DhcpClientServiceImpl::m_mapDhcpInfo[ifname].enableIPv6);
    }

    /* Send dhcp renew packet : kill -USR2 <pid> */
    pid_t pid = GetDhcpClientProPid(ifname);
    if (pid <= 0) {
        WIFI_LOGW("RenewDhcpClient() dhcp client process pid:%{public}d error, ifname:%{public}s!",
            pid, ifname.c_str());
        return DHCP_OPT_FAILED;
    }

    if (kill(pid, SIGUSR2) == -1) {
        WIFI_LOGE("RenewDhcpClient() kill [%{public}d] failed:%{public}d, ifname:%{public}s!",
            pid, errno, ifname.c_str());
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("RenewDhcpClient() kill [%{public}d] success, ifname:%{public}s.", pid, ifname.c_str());
    return DHCP_OPT_SUCCESS;
}

int DhcpClientServiceImpl::ReleaseDhcpClient(const std::string &ifname)
{
    WIFI_LOGI("enter DhcpClientServiceImpl::ReleaseDhcpClient()...ifname:%{public}s.", ifname.c_str());
    int nStatus = GetDhcpStatus(ifname);
    if (nStatus != 1) {
        WIFI_LOGE("ReleaseDhcpClient() failed, dhcp client service not started, ifname:%{public}s!", ifname.c_str());
        return DHCP_OPT_FAILED;
    }

    /* Send dhcp release packet : kill -USR1 <pid> */
    pid_t pid = GetDhcpClientProPid(ifname);
    if (pid <= 0) {
        WIFI_LOGW("ReleaseDhcpClient() dhcp client process pid:%{public}d error, ifname:%{public}s!",
            pid, ifname.c_str());
        return DHCP_OPT_FAILED;
    }

    if (kill(pid, SIGUSR1) == -1) {
        WIFI_LOGE("ReleaseDhcpClient() kill [%{public}d] failed:%{public}d, ifname:%{public}s!",
            pid, errno, ifname.c_str());
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("ReleaseDhcpClient() kill [%{public}d] success, ifname:%{public}s.", pid, ifname.c_str());
    return DHCP_OPT_SUCCESS;
}
}  // namespace Wifi
}  // namespace OHOS
