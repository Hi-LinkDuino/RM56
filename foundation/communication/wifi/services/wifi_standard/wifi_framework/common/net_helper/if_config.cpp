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

#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include "securec.h"
#include "if_config.h"
#include "ip_tools.h"
#include <thread>

namespace OHOS {
namespace Wifi {
const std::string SYSTEM_COMMAND_IP = "/system/bin/ip";
const int SYSTEM_COMMAND_ERR = -1;
const int SYSTEM_NOT_EXECUTED = 127;
const int RECEIVE_BUFFER_LEN = 64;

IfConfig &IfConfig::GetInstance()
{
    static IfConfig ifConfig;
    return ifConfig;
}

IfConfig::IfConfig()
{}

IfConfig::~IfConfig()
{}

bool IfConfig::SyncExecuteCommand(const std::string& cmd)
{
    int ret = system(cmd.c_str());
    if (ret == SYSTEM_COMMAND_ERR || ret == SYSTEM_NOT_EXECUTED) {
        LOGE("exec failed. cmd: %s, error:%{public}d", cmd.c_str(), errno);
        return false;
    }
    LOGI("Exec cmd end - sync");
    return true;
}

bool IfConfig::AsyncExecuteCommand(const std::string& cmd)
{
    std::thread t(
        [cmd]() {
            FILE *fp = nullptr;
            char buffer[RECEIVE_BUFFER_LEN];
            if ((fp = popen(cmd.c_str(), "r")) != nullptr) {
                while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
                    LOGD("exec cmd receive: %{public}s", buffer);
                }
                pclose(fp);
            } else {
                LOGE("exec cmd popen error!");
            }
            LOGI("Exec cmd end - async");
        }
    );
    t.detach();
    return true;
}

/**
 * @Description : Execute script commands
 * @Return success:true failed:false
 */
bool IfConfig::ExecCommand(const std::vector<std::string> &vecCommandArg)
{
    std::string command;
    for (auto iter : vecCommandArg) {
        command += iter;
        command += " ";
    }
    LOGI("Exec cmd start: [%s]", command.c_str());
    return AsyncExecuteCommand(command);
}

/**
 * @Description : Flush the IpAddr
 * @Return None
 */
void IfConfig::FlushIpAddr(const std::string& ifName, const int& ipType)
{
    LOGI("Flush IP, ifName: %{public}s", ifName.c_str());

    if (ipType != static_cast<int>(IpType::IPTYPE_IPV4)) {
        return;
    }
    struct ifreq ifr;
    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK ||
        strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifName.c_str()) != EOK) {
        LOGE("Init the ifreq struct failed!");
        return;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        LOGE("AddIpAddr:socket error");
        return;
    }
    struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
    sin->sin_family = AF_INET;
    /* ipAddr */
    if (inet_aton("0.0.0.0", &(sin->sin_addr)) < 0) {
        LOGE("AddIpAddr:inet_aton error");
        close(fd);
        return;
    }
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        LOGE("AddIpAddr:ioctl SIOCSIFADDR error");
        close(fd);
        return;
    }
    close(fd);
    return;
}

/**
 * @Description : Add the IpAddr
 * @Return None
 */
void IfConfig::AddIpAddr(
    const std::string &ifName, const std::string &ipAddr, const std::string &mask, const int &ipType)
{
    LOGI("Add ip address, ifName = %{public}s", ifName.c_str());

    if (ipType == static_cast<int>(IpType::IPTYPE_IPV4)) {
        struct ifreq ifr;
        if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK ||
            strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifName.c_str()) != EOK) {
            LOGE("set ifr info failed!");
            return;
        }

        struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
        sin->sin_family = AF_INET;

        // ipAddr
        if (inet_aton(ipAddr.c_str(), &(sin->sin_addr)) < 0) {
            LOGE("inet_aton   error\n");
            return;
        }

        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0) {
            LOGE("socket error\n");
            return;
        }

        if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
            LOGE("ioctl   SIOCSIFADDR   error\n");
            close(fd);
            return;
        }

        // netMask
        if (inet_aton(mask.c_str(), &(sin->sin_addr)) < 0) {
            LOGE("inet_pton   error\n");
            close(fd);
            return;
        }

        if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0) {
            LOGE("ioctl SIOCSIFNETMASK error");
            close(fd);
            return;
        }
        close(fd);
    } else {
        std::vector<std::string> ipRouteCmd;
        ipRouteCmd.clear();
        ipRouteCmd.push_back(SYSTEM_COMMAND_IP);
        ipRouteCmd.push_back("-6");
        ipRouteCmd.push_back("addr");
        ipRouteCmd.push_back("add");
        ipRouteCmd.push_back(ipAddr);
        ipRouteCmd.push_back("dev");
        ipRouteCmd.push_back(ifName);
        ExecCommand(ipRouteCmd);
    }

    return;
}

/**
 * @Description : set proxy
 * @param isAuto - whether to automatically proxy[in]
 * @param proxy - proxy host name[in]
 * @param port - port[in]
 * @param noProxys - objects to bypass proxy[in]
 * @Return None
 */
void IfConfig::SetProxy(
    bool isAuto, const std::string &proxy, const std::string &port, const std::string &noProxys, const std::string &pac)
{
    LOGI("SetProxy pac=[%s]\n", pac.c_str());
    std::vector<std::string> ipRouteCmd;

    if (!isAuto) {
        // Add proxy
        if (!proxy.empty()) {
            ipRouteCmd.clear();
            ipRouteCmd.push_back("export");
            ipRouteCmd.push_back("http_proxy=" + proxy + ":" + port);
            ExecCommand(ipRouteCmd);
        }

        // Bypass proxy
        if (!noProxys.empty()) {
            ipRouteCmd.clear();
            ipRouteCmd.push_back("export");
            ipRouteCmd.push_back("no_proxy=" + noProxys);
            ExecCommand(ipRouteCmd);
        }
    }
    return;
}

bool IfConfig::GetIpAddr(const std::string& ifName, std::string& ipAddr)
{
    struct ifreq ifr;
    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK ||
        strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifName.c_str()) != EOK) {
        LOGE("set ifr info failed!");
        return false;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        LOGE("socket error\n");
        return false;
    }
    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
        perror("ioctl error!\n");
        close(fd);
        return false;
    }
    struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
    ipAddr = inet_ntoa(sin->sin_addr);
    close(fd);
    return true;
}
}  // namespace Wifi
}  // namespace OHOS