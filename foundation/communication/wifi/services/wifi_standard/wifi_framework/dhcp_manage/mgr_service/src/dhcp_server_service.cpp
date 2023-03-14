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

#include "dhcp_server_service.h"

#include <unistd.h>

#include "dhcp_func.h"
#include "wifi_logger.h"
#include "securec.h"

DEFINE_WIFILOG_DHCP_LABEL("DhcpServerService");

namespace OHOS {
namespace Wifi {
std::map<std::string, DhcpServerInfo> DhcpServerService::m_mapDhcpServer;
DhcpServerService::DhcpServerService()
{
    m_setInterfaces.clear();
    m_mapTagDhcpRange.clear();
    m_mapInfDhcpRange.clear();
    m_mapDhcpSerExitNotify.clear();
    bDhcpSerProExitThread = false;
    pDhcpSerProExitThread = nullptr;

    DhcpFunc::CreateDirs(DHCP_SERVER_CONFIG_DIR);
}

DhcpServerService::~DhcpServerService()
{
    WIFI_LOGI("StartDhcpServer: ~DhcpServerService");
    StopDhcpServerOnExit();
    ExitDhcpMgrThreadFunc();
}

int DhcpServerService::StartDhcpServer(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("StartDhcpServer error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    WIFI_LOGI("StartDhcpServer ifname:%{public}s.", ifname.c_str());

    /* Add the specified interface. */
    if (AddSpecifiedInterface(ifname) != DHCP_OPT_SUCCESS) {
        return DHCP_OPT_FAILED;
    }

    if (CreateDefaultConfigFile(DHCP_SERVER_CONFIG_FILE) != DHCP_OPT_SUCCESS) {
        return DHCP_OPT_FAILED;
    }

    if (GetDhcpServerPid(ifname) > 0) {
        WIFI_LOGI("ifname:%{public}s pro already started, now reload config.", ifname.c_str());

        /* reload dhcp server config */
        return ReloadConfig(ifname);
    }

    /* start dhcp server process */
    pid_t pidServer;
    if ((pidServer = vfork()) < 0) {
        WIFI_LOGE("StartDhcpServer() vfork() failed, pidServer:%{public}d!", pidServer);
        return DHCP_OPT_FAILED;
    }
    if (pidServer == 0) {
        /* Child process */
        std::string localIp, netmask, ipRange;
        if ((DhcpFunc::GetLocalIp(ifname, localIp, netmask) != DHCP_OPT_SUCCESS) ||
            (GetUsingIpRange(ifname, ipRange) != DHCP_OPT_SUCCESS)) {
            return DHCP_OPT_FAILED;
        }
        ForkExecProcess(ifname, localIp, netmask, ipRange);
    } else {
        /* Parent process */
        SetDhcpServerInfo(ifname, SERVICE_STATUS_START, pidServer);
        ForkParentProcess();
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::StopDhcpServer(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("StopDhcpServer() error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    pid_t pidServer = GetDhcpServerPid(ifname);
    if (pidServer == 0) {
        WIFI_LOGI("StopDhcpServer() %{public}s, %{public}s already stop.", ifname.c_str(), DHCP_SERVER_FILE.c_str());
        return DHCP_OPT_SUCCESS;
    }
    auto iterRangeMap = m_mapInfDhcpRange.find(ifname);
    if (iterRangeMap != m_mapInfDhcpRange.end()) {
        m_mapInfDhcpRange.erase(iterRangeMap);
    }
    if (RemoveAllDhcpRange(ifname) != DHCP_OPT_SUCCESS) {
        return DHCP_OPT_FAILED;
    }
    if (StopServer(pidServer) != DHCP_OPT_SUCCESS) {
        return DHCP_OPT_FAILED;
    }
    SetDhcpServerInfo(ifname, SERVICE_STATUS_STOP, 0);

    /* Del the specified interface. */
    if (DelSpecifiedInterface(ifname) != DHCP_OPT_SUCCESS) {
        return DHCP_OPT_FAILED;
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::StopDhcpServerOnExit()
{
    for (auto& each: m_setInterfaces) {
        if (each.empty()) {
            WIFI_LOGE("StopDhcpServer() on exit error, ifname is empty!");
            continue;
        }

        pid_t pidServer = GetDhcpServerPid(each);
        if (pidServer == 0) {
            WIFI_LOGI("StopDhcpServer() on exit %{public}s already stop.", each.c_str());
            continue;
        }
        auto iterRangeMap = m_mapInfDhcpRange.find(each);
        if (iterRangeMap != m_mapInfDhcpRange.end()) {
            m_mapInfDhcpRange.erase(iterRangeMap);
        }
        if (RemoveAllDhcpRange(each) != DHCP_OPT_SUCCESS) {
            WIFI_LOGE("StopDhcpServer() on exit, RemoveAllDhcpRange %{public}s error.", each.c_str());
        }
        if (StopServer(pidServer) != DHCP_OPT_SUCCESS) {
            WIFI_LOGE("StopDhcpServer() on exit error, StopServer %{public}s already stop.", each.c_str());
        }
        SetDhcpServerInfo(each, SERVICE_STATUS_STOP, 0);
    }
    m_setInterfaces.clear();
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::GetServerStatus()
{
    return 0;
}

int DhcpServerService::PutDhcpRange(const std::string &tagName, const DhcpRange &range)
{
    if (tagName.empty()) {
        WIFI_LOGE("PutDhcpRange param error, tagName is empty!");
        return DHCP_OPT_ERROR;
    }
    if (!CheckIpAddrRange(range)) {
        WIFI_LOGE("PutDhcpRange tag:%{public}s check range failed.", tagName.c_str());
        return DHCP_OPT_FAILED;
    }

    WIFI_LOGI("PutDhcpRange tag:%{public}s.", tagName.c_str());

    /* add dhcp range */
    auto iterRangeMap = m_mapTagDhcpRange.find(tagName);
    if (iterRangeMap != m_mapTagDhcpRange.end()) {
        int nSize = (int)iterRangeMap->second.size();
        if (nSize > 1) {
            WIFI_LOGE("PutDhcpRange failed, %{public}s range size:%{public}d error!", tagName.c_str(), nSize);
            return DHCP_OPT_FAILED;
        } else if (nSize == 0) {
            WIFI_LOGI("PutDhcpRange m_mapTagDhcpRange find tagName:%{public}s, need push_back.", tagName.c_str());
            iterRangeMap->second.push_back(range);
            return DHCP_OPT_SUCCESS;
        } else {
            for (auto tagRange : iterRangeMap->second) {
                if ((tagRange.iptype != range.iptype) ||
                    (tagRange.strStartip != range.strStartip) || (tagRange.strEndip != range.strEndip)) {
                    continue;
                }
                WIFI_LOGW("PutDhcpRange success, %{public}s range already exist", tagName.c_str());
                return DHCP_OPT_SUCCESS;
            }
            WIFI_LOGE("PutDhcpRange failed, %{public}s range size:%{public}d already exist!", tagName.c_str(), nSize);
            return DHCP_OPT_FAILED;
        }
    } else {
        std::list<DhcpRange> listDhcpRange;
        listDhcpRange.push_back(range);
        m_mapTagDhcpRange.emplace(std::make_pair(tagName, listDhcpRange));
        WIFI_LOGI("PutDhcpRange m_mapTagDhcpRange no find tagName:%{public}s, need emplace.", tagName.c_str());
        return DHCP_OPT_SUCCESS;
    }
}

int DhcpServerService::RemoveDhcpRange(const std::string &tagName, const DhcpRange &range)
{
    if (tagName.empty()) {
        WIFI_LOGE("RemoveDhcpRange param error, tagName is empty!");
        return DHCP_OPT_ERROR;
    }

    /* remove dhcp range */
    auto iterRangeMap = m_mapTagDhcpRange.find(tagName);
    if (iterRangeMap != m_mapTagDhcpRange.end()) {
        auto iterRange = m_mapTagDhcpRange[tagName].begin();
        while (iterRange != m_mapTagDhcpRange[tagName].end()) {
            if ((iterRange->iptype == range.iptype) && (iterRange->strStartip == range.strStartip) &&
                (iterRange->strEndip == range.strEndip)) {
                m_mapTagDhcpRange[tagName].erase(iterRange++);
                WIFI_LOGI("RemoveDhcpRange find tagName:%{public}s, "
                          "range.iptype:%{public}d,strStartip:%{private}s,strEndip:%{private}s, erase.",
                    tagName.c_str(),
                    range.iptype,
                    range.strStartip.c_str(),
                    range.strEndip.c_str());
                return DHCP_OPT_SUCCESS;
            }
            iterRange++;
        }
        WIFI_LOGE("RemoveDhcpRange find tagName:%{public}s, second no find range, erase failed!", tagName.c_str());
    } else {
        WIFI_LOGE("RemoveDhcpRange no find tagName:%{public}s, erase failed!", tagName.c_str());
    }

    return DHCP_OPT_FAILED;
}

int DhcpServerService::RemoveAllDhcpRange(const std::string &tagName)
{
    if (tagName.empty()) {
        WIFI_LOGE("RemoveAllDhcpRange param error, tagName is empty!");
        return DHCP_OPT_ERROR;
    }

    /* remove all dhcp range */
    auto iterRangeMap = m_mapTagDhcpRange.find(tagName);
    if (iterRangeMap != m_mapTagDhcpRange.end()) {
        m_mapTagDhcpRange.erase(iterRangeMap);
        WIFI_LOGI("RemoveAllDhcpRange find tagName:%{public}s, erase success.", tagName.c_str());
    } else {
        WIFI_LOGI("RemoveAllDhcpRange no find tagName:%{public}s, not need erase!", tagName.c_str());
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::SetDhcpRange(const std::string &ifname, const DhcpRange &range)
{
    /* put dhcp range */
    if (PutDhcpRange(ifname, range) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("SetDhcpRange PutDhcpRange failed, ifname:%{public}s.", ifname.c_str());
        return DHCP_OPT_FAILED;
    }

    /* check same network */
    if (DhcpFunc::CheckRangeNetwork(ifname, range.strStartip, range.strEndip) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("SetDhcpRange CheckRangeNetwork failed, ifname:%{public}s.", ifname.c_str());
        RemoveDhcpRange(ifname, range);
        return DHCP_OPT_FAILED;
    }

    /* add dhcp range */
    auto iterRangeMap = m_mapInfDhcpRange.find(ifname);
    if (iterRangeMap != m_mapInfDhcpRange.end()) {
        int nSize = (int)iterRangeMap->second.size();
        if (nSize > 1) {
            WIFI_LOGE("SetDhcpRange failed, %{public}s range size:%{public}d error!", ifname.c_str(), nSize);
            RemoveDhcpRange(ifname, range);
            return DHCP_OPT_FAILED;
        }
        if (nSize == 1) {
            WIFI_LOGW("SetDhcpRange %{public}s range size:%{public}d already exist.", ifname.c_str(), nSize);
            iterRangeMap->second.clear();
        }
        WIFI_LOGI("SetDhcpRange m_mapInfDhcpRange find ifname:%{public}s, need push_back.", ifname.c_str());
        iterRangeMap->second.push_back(range);
    } else {
        std::list<DhcpRange> listDhcpRange;
        listDhcpRange.push_back(range);
        m_mapInfDhcpRange.emplace(std::make_pair(ifname, listDhcpRange));
        WIFI_LOGI("SetDhcpRange m_mapInfDhcpRange no find ifname:%{public}s, need emplace.", ifname.c_str());
    }

    if (CheckAndUpdateConf(ifname) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("SetDhcpRange() CheckAndUpdateConf failed, ifname:%{public}s.", ifname.c_str());
        RemoveDhcpRange(ifname, range);
        return DHCP_OPT_FAILED;
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::SetDhcpRange(const std::string &ifname, const std::string &tagName)
{
    if (ifname.empty() || tagName.empty()) {
        WIFI_LOGE("SetDhcpRange failed, ifname or tagName is empty!");
        return DHCP_OPT_FAILED;
    }

    auto iterTag = m_mapTagDhcpRange.find(tagName);
    if (iterTag == m_mapTagDhcpRange.end()) {
        WIFI_LOGE("SetDhcpRange tag m_mapTagDhcpRange no find tagName:%{public}s.", tagName.c_str());
        return DHCP_OPT_FAILED;
    }
    int nSize = (int)iterTag->second.size();
    if (nSize != 1) {
        WIFI_LOGE("SetDhcpRange tag %{public}s range size:%{public}d error.", tagName.c_str(), nSize);
        return DHCP_OPT_FAILED;
    }

    /* check same network */
    for (auto iterTagValue : iterTag->second) {
        if (DhcpFunc::CheckRangeNetwork(ifname, iterTagValue.strStartip, iterTagValue.strEndip) != DHCP_OPT_SUCCESS) {
            WIFI_LOGE("SetDhcpRange tag CheckRangeNetwork failed, ifname:%{public}s.", ifname.c_str());
            return DHCP_OPT_FAILED;
        }
    }

    /* add dhcp range */
    auto iterRangeMap = m_mapInfDhcpRange.find(ifname);
    if (iterRangeMap != m_mapInfDhcpRange.end()) {
        nSize = (int)iterRangeMap->second.size();
        if (nSize > 1) {
            WIFI_LOGE("SetDhcpRange tag failed, %{public}s range size:%{public}d error!", ifname.c_str(), nSize);
            return DHCP_OPT_FAILED;
        }
        if (nSize == 1) {
            WIFI_LOGW("SetDhcpRange tag %{public}s range size:%{public}d already exist.", ifname.c_str(), nSize);
            iterRangeMap->second.clear();
        }
        WIFI_LOGI("SetDhcpRange tag m_mapInfDhcpRange find ifname:%{public}s, need push_back.", ifname.c_str());
        for (auto iterTagValue : iterTag->second) {
            iterRangeMap->second.push_back(iterTagValue);
        }
    } else {
        m_mapInfDhcpRange.emplace(std::make_pair(ifname, iterTag->second));
        WIFI_LOGI("SetDhcpRange tag no find %{public}s, need emplace %{public}s.", ifname.c_str(), tagName.c_str());
    }

    /* update or reload interface config file */
    if (CheckAndUpdateConf(ifname) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("SetDhcpRange tag CheckAndUpdateConf failed, ifname:%{public}s.", ifname.c_str());
        return DHCP_OPT_FAILED;
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::GetLeases(const std::string& ifname, std::vector<std::string> &leases)
{
    if (ifname.empty()) {
        WIFI_LOGE("DhcpServerService::GetLeases error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }

    std::string strFile = DHCP_SERVER_LEASES_FILE + "." + ifname;
    if (!DhcpFunc::IsExistFile(strFile)) {
        WIFI_LOGE("GetLeases() failed, dhcp leasefile:%{public}s no exist!", strFile.c_str());
        return DHCP_OPT_FAILED;
    }

    leases.clear();

    std::ifstream inFile;
    inFile.open(strFile);
    std::string strTemp = "";
    char tmpLineData[FILE_LINE_MAX_SIZE] = {0};
    while (inFile.getline(tmpLineData, sizeof(tmpLineData))) {
        strTemp = tmpLineData;
        leases.push_back(strTemp);
    }
    inFile.close();

    WIFI_LOGI("GetLeases() leases.size:%{public}d.", (int)leases.size());
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::GetDhcpSerProExit(const std::string &ifname, IDhcpResultNotify *pResultNotify)
{
    if (ifname.empty()) {
        WIFI_LOGE("GetDhcpSerProExit() error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }

    if (pResultNotify == nullptr) {
        WIFI_LOGE("GetDhcpSerProExit() error, pResultNotify = nullptr!");
        return DHCP_OPT_FAILED;
    }

    auto iterExitNotify = m_mapDhcpSerExitNotify.find(ifname);
    if (iterExitNotify == m_mapDhcpSerExitNotify.end()) {
        WIFI_LOGI("GetDhcpSerProExit() SerExitNotify no find ifname:%{public}s, need emplace.", ifname.c_str());
        m_mapDhcpSerExitNotify.emplace(std::make_pair(ifname, pResultNotify));
    } else {
        WIFI_LOGW("GetDhcpSerProExit() SerExitNotify find ifname:%{public}s, not need emplace!", ifname.c_str());
    }

    if (pDhcpSerProExitThread == nullptr) {
        pDhcpSerProExitThread = new std::thread(&DhcpServerService::RunDhcpSerProExitThreadFunc, this);
        if (pDhcpSerProExitThread == nullptr) {
            WIFI_LOGE("GetDhcpSerProExit() init pDhcpSerProExitThread failed!");
            return DHCP_OPT_FAILED;
        }
        WIFI_LOGI("GetDhcpSerProExit() init pDhcpSerProExitThread success.");
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::ReloadConfig(const std::string& ifname)
{
    pid_t pidServer = GetDhcpServerPid(ifname);
    if (pidServer == 0) {
        WIFI_LOGE("failed to reload config %{public}s", DHCP_SERVER_CONFIG_FILE.c_str());
        return DHCP_OPT_FAILED;
    }

    WIFI_LOGI("reload config %{public}s, need restart server:[ %{public}s ], pid:%{public}d.",
        DHCP_SERVER_CONFIG_FILE.c_str(), DHCP_SERVER_FILE.c_str(), pidServer);

    /* stop dhcp server process */
    if (StopServer(pidServer) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("failed to reload config %{public}s, stop pid:%{public}d error",
            DHCP_SERVER_CONFIG_FILE.c_str(), pidServer);
        return DHCP_OPT_FAILED;
    }
    SetDhcpServerInfo(ifname, SERVICE_STATUS_STOP, 0);

    sleep(DHCP_NUM_ONE);

    /* restart dhcp server process for load config */
    if ((pidServer = vfork()) < 0) {
        WIFI_LOGE("failed to reload config %{public}s, vfork %{public}d error!",
            DHCP_SERVER_CONFIG_FILE.c_str(), pidServer);
        return DHCP_OPT_FAILED;
    }
    if (pidServer == 0) {
        /* Child process */
        std::string localIp, netmask, ipRange;
        if ((DhcpFunc::GetLocalIp(ifname, localIp, netmask) != DHCP_OPT_SUCCESS) ||
            (GetUsingIpRange(ifname, ipRange) != DHCP_OPT_SUCCESS)) {
            return DHCP_OPT_FAILED;
        }
        ForkExecProcess(ifname, localIp, netmask, ipRange);
    } else {
        /* Parent process */
        SetDhcpServerInfo(ifname, SERVICE_STATUS_START, pidServer);
        ForkParentProcess();
    }

    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::ForkParentProcess()
{
    RegisterSignal();
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::ForkExecProcess(
    const std::string ifname, const std::string ip, const std::string mask, const std::string pool)
{
    if (ifname.empty()) {
        WIFI_LOGE("ForkExecProcess param error, ifname is empty!");
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("ForkExecProcess %{public}s %{public}s %{public}s %{public}s %{public}s",
        DHCP_SERVER_FILE.c_str(), ifname.c_str(), ip.c_str(), mask.c_str(), pool.c_str());
    char args[DHCP_SER_ARGSNUM][PARAM_MAX_SIZE];
    char* args2[DHCP_SER_ARGSNUM];
    if ((memset_s(args, sizeof(args), 0, sizeof(args)) != EOK) ||
        (memset_s(args2, sizeof(args2), 0, sizeof(args2)) != EOK)) {
        return DHCP_OPT_FAILED;
    }
    int argsIndex = 0;
    if (strncpy_s(args[argsIndex], PARAM_MAX_SIZE, DHCP_SERVER_FILE.c_str(), DHCP_SERVER_FILE.size()) != EOK) {
        return DHCP_OPT_FAILED;
    }
    std::string strArg = "-i" + ifname;
    if (strncpy_s(args[++argsIndex], PARAM_MAX_SIZE, strArg.c_str(), strArg.size()) != EOK) {
        return DHCP_OPT_FAILED;
    }
    if (!ip.empty()) {
        strArg = "-s" + ip;
        if (strncpy_s(args[++argsIndex], PARAM_MAX_SIZE, strArg.c_str(), strArg.size()) != EOK) {
            return DHCP_OPT_FAILED;
        }
    }
    if (!mask.empty()) {
        strArg = "-n" + mask;
        if (strncpy_s(args[++argsIndex], PARAM_MAX_SIZE, strArg.c_str(), strArg.size()) != EOK) {
            return DHCP_OPT_FAILED;
        }
    }
    if (!pool.empty()) {
        strArg = "-P" + pool;
        if (strncpy_s(args[++argsIndex], PARAM_MAX_SIZE, strArg.c_str(), strArg.size()) != EOK) {
            return DHCP_OPT_FAILED;
        }
    }
    for (int i = 0; i <= argsIndex; i++) {
        args2[i] = args[i];
    }
    args2[argsIndex + 1] = nullptr;
    if (execv(args2[0], args2) == -1) {
        WIFI_LOGE("failed to execv %{public}s %{public}d, err:%{public}s", args2[0], errno, ifname.c_str());
    }
    _exit(-1);
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::StopServer(const pid_t &serverPid)
{
    UnregisterSignal();
    if (kill(serverPid, SIGTERM) == -1) {
        if (ESRCH == errno) {
            /* Normal. The subprocess is dead. The SIGCHLD signal triggers the stop hotspot. */
            WIFI_LOGI("StopServer() kill [%{public}d] success, pro pid no exist, pro:%{public}s.",
                serverPid, DHCP_SERVER_FILE.c_str());
            return DHCP_OPT_SUCCESS;
        }
        WIFI_LOGE("StopServer() kill [%{public}d] failed, errno:%{public}d!", serverPid, errno);
        return DHCP_OPT_FAILED;
    }
    if (waitpid(serverPid, nullptr, 0) == -1) {
        WIFI_LOGE("StopServer() waitpid [%{public}d] failed, errno:%{public}d!", serverPid, errno);
        return DHCP_OPT_FAILED;
    }
    WIFI_LOGI("StopServer() waitpid [%{public}d] success, pro:%{public}s!", serverPid, DHCP_SERVER_FILE.c_str());
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::CheckAndUpdateConf(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("CheckAndUpdateConf error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }

    auto iterRangeMap = m_mapInfDhcpRange.find(ifname);
    if ((iterRangeMap == m_mapInfDhcpRange.end()) || (iterRangeMap->second).empty()) {
        return DHCP_OPT_SUCCESS;
    }
    int nSize = (int)iterRangeMap->second.size();
    if (nSize > 1) {
        WIFI_LOGE("CheckAndUpdateConf failed, %{public}s range size:%{public}d error!", ifname.c_str(), nSize);
        return DHCP_OPT_FAILED;
    }

    for (auto iterRange : iterRangeMap->second) {
        if (((iterRange.iptype != 0) && (iterRange.iptype != 1)) || (iterRange.leaseHours <= 0) ||
            (iterRange.strStartip.size() == 0) || (iterRange.strEndip.size() == 0)) {
            WIFI_LOGE("CheckAndUpdateConf failed, "
                      "iptype:%{public}d,leaseHours:%{public}d,strStartip:%{private}s,strEndip:%{private}s error!",
                iterRange.iptype, iterRange.leaseHours, iterRange.strStartip.c_str(), iterRange.strEndip.c_str());
            return DHCP_OPT_FAILED;
        }
    }

    return DHCP_OPT_SUCCESS;
}

bool DhcpServerService::CheckIpAddrRange(const DhcpRange &range)
{
    if (((range.iptype != 0) && (range.iptype != 1)) || range.strStartip.empty() || range.strEndip.empty()) {
        WIFI_LOGE("CheckIpAddrRange range.iptype:%{public}d,strStartip:%{private}s,strEndip:%{private}s error!",
            range.iptype, range.strStartip.c_str(), range.strEndip.c_str());
        return false;
    }

    if (range.iptype == 0) {
        uint32_t uStartIp = 0;
        if (!DhcpFunc::Ip4StrConToInt(range.strStartip, uStartIp)) {
            WIFI_LOGE("CheckIpAddrRange Ip4StrConToInt failed, range.iptype:%{public}d,strStartip:%{private}s!",
                range.iptype, range.strStartip.c_str());
            return false;
        }
        uint32_t uEndIp = 0;
        if (!DhcpFunc::Ip4StrConToInt(range.strEndip, uEndIp)) {
            WIFI_LOGE("CheckIpAddrRange Ip4StrConToInt failed, range.iptype:%{public}d,strEndip:%{private}s!",
                range.iptype, range.strEndip.c_str());
            return false;
        }
        /* check ip4 start and end ip */
        if (uStartIp >= uEndIp) {
            WIFI_LOGE("CheckIpAddrRange failed, start:%{private}u not less end:%{private}u!", uStartIp, uEndIp);
            return false;
        }
    } else {
        uint8_t uStartIp6[sizeof(struct in6_addr)] = {0};
        if (!DhcpFunc::Ip6StrConToChar(range.strStartip, uStartIp6, sizeof(struct in6_addr))) {
            return false;
        }
        uint8_t uEndIp6[sizeof(struct in6_addr)] = {0};
        if (!DhcpFunc::Ip6StrConToChar(range.strEndip, uEndIp6, sizeof(struct in6_addr))) {
            return false;
        }
        /* check ip6 start and end ip */
    }

    return true;
}

int DhcpServerService::AddSpecifiedInterface(const std::string& ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("AddSpecifiedInterface param error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }

    if (m_setInterfaces.find(ifname) == m_setInterfaces.end()) {
        m_setInterfaces.insert(ifname);
        WIFI_LOGI("AddSpecifiedInterface started interfaces add %{public}s success.", ifname.c_str());
    }
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::DelSpecifiedInterface(const std::string& ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("DelSpecifiedInterface param error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }

    auto iterInterfaces = m_setInterfaces.find(ifname);
    if (iterInterfaces != m_setInterfaces.end()) {
        m_setInterfaces.erase(iterInterfaces);
        WIFI_LOGI("DelSpecifiedInterface started interfaces del %{public}s success.", ifname.c_str());
    }
    return DHCP_OPT_SUCCESS;
}

int DhcpServerService::GetUsingIpRange(const std::string ifname, std::string& ipRange)
{
    if (ifname.empty()) {
        WIFI_LOGE("GetUsingIpRange param error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }

    auto iterRangeMap = m_mapInfDhcpRange.find(ifname);
    if (iterRangeMap == m_mapInfDhcpRange.end()) {
        WIFI_LOGE("GetUsingIpRange failed, inf range map no find %{public}s!", ifname.c_str());
        return DHCP_OPT_FAILED;
    }
    int nSize = (int)iterRangeMap->second.size();
    if (nSize != 1) {
        WIFI_LOGE("GetUsingIpRange failed, %{public}s range size:%{public}d error!", ifname.c_str(), nSize);
        return DHCP_OPT_FAILED;
    }

    for (auto iterRange : iterRangeMap->second) {
        if (((iterRange.iptype != 0) && (iterRange.iptype != 1)) || (iterRange.leaseHours <= 0) ||
            (iterRange.strStartip.size() == 0) || (iterRange.strEndip.size() == 0)) {
            WIFI_LOGE("GetUsingIpRange type:%{public}d,lease:%{public}d,start:%{private}s,end:%{private}s error!",
                iterRange.iptype, iterRange.leaseHours, iterRange.strStartip.c_str(), iterRange.strEndip.c_str());
            return DHCP_OPT_FAILED;
        }
        ipRange.clear();
        ipRange = iterRange.strStartip + "," + iterRange.strEndip;
        return DHCP_OPT_SUCCESS;
    }
    WIFI_LOGE("GetUsingIpRange failed, %{public}s range size:%{public}d", ifname.c_str(), nSize);
    return DHCP_OPT_FAILED;
}

int DhcpServerService::CreateDefaultConfigFile(const std::string strFile)
{
    if (strFile.empty()) {
        WIFI_LOGE("CreateDefaultConfigFile param error, strFile is empty!");
        return DHCP_OPT_ERROR;
    }

    if (!DhcpFunc::IsExistFile(strFile)) {
        std::string strData = "leaseTime=" + std::to_string(LEASETIME_DEFAULT * ONE_HOURS_SEC) + "\n";
        DhcpFunc::CreateFile(strFile, strData);
    }
    return DHCP_OPT_SUCCESS;
}

void DhcpServerService::RunDhcpSerProExitThreadFunc()
{
    for (;;) {
        if (bDhcpSerProExitThread) {
            WIFI_LOGI("RunDhcpSerProExitThreadFunc() bDhcpSerProExitThread:true, break!");
            break;
        }
        if (m_mapDhcpSerExitNotify.empty()) {
            sleep(DHCP_NUM_ONE);
            continue;
        }

        /* If the dhcp server process exits abnormally, notify other modules. */
        WIFI_LOGI("RunDhcpSerProExitThreadFunc() other modules have notify reqs, now begin notify...");
        auto iterNotify = m_mapDhcpSerExitNotify.begin();
        while (iterNotify != m_mapDhcpSerExitNotify.end()) {
            std::string ifname = iterNotify->first;
            if (iterNotify->second == nullptr) {
                WIFI_LOGE("RunDhcpSerProExitThreadFunc() ifname:%{public}s error, ptr is nullptr!", ifname.c_str());
                iterNotify = m_mapDhcpSerExitNotify.erase(iterNotify);
                continue;
            }

            if (!DhcpServerService::m_mapDhcpServer[ifname].exitSig ||
                DhcpServerService::m_mapDhcpServer[ifname].normalExit) {
                WIFI_LOGI("RunDhcpSerProExitThreadFunc() ifname:%{public}s exit status not update", ifname.c_str());
                ++iterNotify;
                continue;
            }

            /* notify other modules */
            WIFI_LOGI("RunDhcpSerProExitThreadFunc() notify other modules.");
            iterNotify->second->OnSerExitNotify(ifname);
            iterNotify = m_mapDhcpSerExitNotify.erase(iterNotify);
        }
        WIFI_LOGI("RunDhcpSerProExitThreadFunc() dhcp ser pro exit notify finished.");
        sleep(DHCP_NUM_ONE);
        continue;
    }
    WIFI_LOGI("RunDhcpSerProExitThreadFunc() end!");
}

void DhcpServerService::ExitDhcpMgrThreadFunc()
{
    bDhcpSerProExitThread = true;
    if (pDhcpSerProExitThread != nullptr) {
        pDhcpSerProExitThread->join();
        delete pDhcpSerProExitThread;
        pDhcpSerProExitThread = nullptr;
    }

    if (!m_mapDhcpSerExitNotify.empty()) {
        m_mapDhcpSerExitNotify.clear();
    }
}

void DhcpServerService::RegisterSignal() const
{
    struct sigaction newAction {};

    if (sigfillset(&newAction.sa_mask) == -1) {
        WIFI_LOGE("RegisterSignal() failed, sigfillset error:%{public}d!", errno);
    }

    if (sigdelset(&newAction.sa_mask, SIGCHLD) == -1) {
        WIFI_LOGE("RegisterSignal() sigdelset SIGCHLD error:%{public}d!", errno);
    }

    newAction.sa_handler = SigChildHandler;
    newAction.sa_flags = SA_RESTART;
    newAction.sa_restorer = nullptr;

    if (sigaction(SIGCHLD, &newAction, nullptr) == -1) {
        WIFI_LOGE("RegisterSignal() sigaction SIGCHLD error:%{public}d!", errno);
    }
}

void DhcpServerService::UnregisterSignal() const
{
    struct sigaction newAction {};

    if (sigemptyset(&newAction.sa_mask) == -1) {
        WIFI_LOGE("UnregisterSignal() failed, sigemptyset error:%{public}d!", errno);
    }

    newAction.sa_handler = SIG_DFL;
    newAction.sa_flags = SA_RESTART;
    newAction.sa_restorer = nullptr;

    if (sigaction(SIGCHLD, &newAction, nullptr) == -1) {
        WIFI_LOGE("UnregisterSignal() sigaction SIGCHLD error:%{public}d!", errno);
    }
}

void DhcpServerService::SigChildHandler(int signum)
{
    if (signum == SIGCHLD) {
        for (auto &serverItem : DhcpServerService::m_mapDhcpServer) {
            pid_t pidServer = GetDhcpServerPid(serverItem.first);
            if (pidServer == 0) {
                WIFI_LOGE("SigChildHandler failed to get %{public}s pid.", serverItem.first.c_str());
                continue;
            }
            /* Received signal SIGCHLD, wait the dhcp server process pid status. */
            pid_t childPid = waitpid(pidServer, nullptr, WUNTRACED | WNOHANG);
            if ((childPid == pidServer) && !serverItem.second.exitSig) {
                WIFI_LOGW("SigChildHandler dhcp server %{public}s received SIGCHLD.", serverItem.first.c_str());
                serverItem.second.exitSig = true;
            }
        }
    }
}

pid_t DhcpServerService::GetDhcpServerPid(const std::string &ifname)
{
    if (ifname.empty()) {
        WIFI_LOGE("GetDhcpServerPid error, ifname is empty!");
        return 0;
    }

    auto iter = DhcpServerService::m_mapDhcpServer.find(ifname);
    if (iter == DhcpServerService::m_mapDhcpServer.end()) {
        WIFI_LOGI("Not get dhcp server ifname:%{public}s, pid:0", ifname.c_str());
        return 0;
    }

    WIFI_LOGI("Get dhcp server ifname:%{public}s, pid:%{public}d", ifname.c_str(), (int)iter->second.proPid);
    return iter->second.proPid;
}

int DhcpServerService::SetDhcpServerInfo(const std::string &ifname, int status, const pid_t& serverPid)
{
    if (ifname.empty()) {
        WIFI_LOGE("SetDhcpServerInfo error, ifname is empty!");
        return DHCP_OPT_ERROR;
    }
    if (status == SERVICE_STATUS_INVALID) {
        WIFI_LOGE("SetDhcpServerInfo error, status is invalid!");
        return DHCP_OPT_ERROR;
    }

    DhcpServerService::m_mapDhcpServer[ifname].proPid = serverPid;
    if (status == SERVICE_STATUS_START) {
        DhcpServerService::m_mapDhcpServer[ifname].normalExit = false;
        DhcpServerService::m_mapDhcpServer[ifname].exitSig = false;
    } else {
        DhcpServerService::m_mapDhcpServer[ifname].normalExit = true;
    }
    return DHCP_OPT_SUCCESS;
}
}  // namespace Wifi
}  // namespace OHOS
