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
#include "wifi_ap_nat_manager.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include "network_interface.h"
#include "if_config.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApNatManager");


namespace OHOS {
namespace Wifi {
const std::string SYSTEM_COMMAND_IP = "/system/bin/ip";
const std::string SYSTEM_COMMAND_IPTABLES = "/system/bin/iptables";
const std::string SYSTEM_COMMAND_IP6TABLES = "/system/bin/ip6tables";
const std::string IP_V4_FORWARDING_CONFIG_FILE = "/proc/sys/net/ipv4/ip_forward";
const std::string IP_V6_FORWARDING_CONFIG_FILE = "/proc/sys/net/ipv6/conf/all/forwarding";

bool WifiApNatManager::EnableInterfaceNat(bool enable, std::string inInterfaceName, std::string outInterfaceName) const
{
    WIFI_LOGI("EnableInterfaceNat enable [%{public}s], inInterfaceName [%{private}s]  outInterfaceName "
         "[%{private}s].",
        enable ? "true" : "false",
        inInterfaceName.c_str(),
        outInterfaceName.c_str());

    if (!NetworkInterface::IsValidInterfaceName(inInterfaceName) ||
        !NetworkInterface::IsValidInterfaceName(outInterfaceName)) {
        WIFI_LOGE("Invalid interface name.");
        return false;
    }

    if (inInterfaceName == outInterfaceName) {
        WIFI_LOGE("Duplicate interface name.");
        return false;
    }

    if (!SetForwarding(enable)) {
        WIFI_LOGE("SetForwarding failed.");
        return false;
    }

    if (!SetInterfaceRoute(enable)) {
        WIFI_LOGE("SetInterfaceRoute failed.");
        return false;
    }

    if (!SetInterfaceNat(enable, outInterfaceName)) {
        WIFI_LOGE("SetInterfaceNat failed.");
        return false;
    }

    return true;
}

bool WifiApNatManager::SetForwarding(bool enable) const
{
    WIFI_LOGI("SetForwarding enable = %{public}s.", ((enable) ? "true" : "false"));

    bool bResult = true;
    const std::string content = enable ? "1" : "0";
    bResult = bResult && WriteDataToFile(IP_V4_FORWARDING_CONFIG_FILE, content);
    bResult = bResult && WriteDataToFile(IP_V6_FORWARDING_CONFIG_FILE, content);
    return bResult;
}

bool WifiApNatManager::SetInterfaceRoute(bool enable) const
{
    const std::string natRouteTable = "10";
    std::vector<std::string> ipRouteCmd;

    ipRouteCmd.push_back(SYSTEM_COMMAND_IP);
    ipRouteCmd.push_back("rule");
    ipRouteCmd.push_back(enable ? "add" : "del");
    ipRouteCmd.push_back("fwmark");
    ipRouteCmd.push_back("0x0/0xffff");
    ipRouteCmd.push_back("lookup");
    ipRouteCmd.push_back("254");
    ipRouteCmd.push_back("prio");
    ipRouteCmd.push_back("18000");
    IfConfig::GetInstance().ExecCommand(ipRouteCmd);

    /* Refresh the cache */
    ipRouteCmd.clear();
    ipRouteCmd.push_back(SYSTEM_COMMAND_IP);
    ipRouteCmd.push_back("route");
    ipRouteCmd.push_back("flush");
    ipRouteCmd.push_back("cache");
    IfConfig::GetInstance().ExecCommand(ipRouteCmd);

    return true;
}

bool WifiApNatManager::SetInterfaceNat(bool enable, const std::string &outInterfaceName) const
{
    std::vector<std::string> iptablesCmd;

    /* Clearing the Firewalls */
    iptablesCmd.push_back(SYSTEM_COMMAND_IPTABLES);
    iptablesCmd.push_back("-F");
    IfConfig::GetInstance().ExecCommand(iptablesCmd);

    /* iptable forward ACCEPT */
    iptablesCmd.clear();
    iptablesCmd.push_back(SYSTEM_COMMAND_IPTABLES);
    iptablesCmd.push_back("-P");
    iptablesCmd.push_back("FORWARD");
    iptablesCmd.push_back(enable ? "ACCEPT" : "DROP");
    IfConfig::GetInstance().ExecCommand(iptablesCmd);

    /* Setting NAT Rules */
    iptablesCmd.clear();
    iptablesCmd.push_back(SYSTEM_COMMAND_IPTABLES);
    iptablesCmd.push_back("-t");
    iptablesCmd.push_back("nat");
    iptablesCmd.push_back(enable ? "-A" : "-D");
    iptablesCmd.push_back("POSTROUTING");
    iptablesCmd.push_back("-o");
    iptablesCmd.push_back(outInterfaceName);
    iptablesCmd.push_back("-j");
    iptablesCmd.push_back("MASQUERADE");
    IfConfig::GetInstance().ExecCommand(iptablesCmd);

    return true;
}

bool WifiApNatManager::WriteDataToFile(const std::string &fileName, const std::string &content) const
{
    std::ofstream outf(fileName, std::ios::out);
    if (!outf) {
        WIFI_LOGE("write content [%publics] to file [%publics] failed. error: %{public}d.",
            content.c_str(), fileName.c_str(), errno);
        return false;
    }
    outf.write(content.c_str(), content.length());
    outf.close();
    return true;
}
}  // namespace Wifi
}  // namespace OHOS