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
#include "wifi_p2p_dns_sd_service_info.h"

#include <sstream>
#include <algorithm>
#include <iomanip>

namespace OHOS {
namespace Wifi {
inline std::string FindPacket(std::string &dnsName)
{
    if (dnsName == "_tcp.local.") {
        return "c00c";
    }
    if (dnsName == "local.") {
        return "c011";
    }
    if (dnsName == "_udp.local.") {
        return "c01c";
    }
    return "";
}


WifiP2pDnsSdServiceInfo::WifiP2pDnsSdServiceInfo(const std::vector<std::string> &queryList)
    : WifiP2pServiceInfo(queryList)
{}

WifiP2pDnsSdServiceInfo::WifiP2pDnsSdServiceInfo()
{}

WifiP2pDnsSdServiceInfo WifiP2pDnsSdServiceInfo::Create(const std::string &instanceName, const std::string &serviceType,
    const std::map<std::string, std::string> &txtMap, const std::string &svrName)
{
    WifiP2PDnsTxtRecord txtRecord;
    for (auto it = txtMap.begin(); it != txtMap.end(); ++it) {
        txtRecord.SetRecord(it->first, it->second);
    }
    std::vector<std::string> queries;
    queries.push_back(BuildPtrServiceQuery(instanceName, serviceType, svrName));
    queries.push_back(BuildTxtServiceQuery(instanceName, serviceType, txtRecord, svrName));

    WifiP2pDnsSdServiceInfo dnsSdServInfo(queries);
    dnsSdServInfo.SetServiceName(svrName);
    dnsSdServInfo.SetServicerProtocolType(P2pServicerProtocolType::SERVICE_TYPE_BONJOUR);
    return dnsSdServInfo;
}
std::string WifiP2pDnsSdServiceInfo::BuildPtrServiceQuery(
    const std::string &instanceName, const std::string &serviceType, const std::string &svrName)
{
    std::stringstream ret;
    std::vector<unsigned char> buf;

    ret << "bonjour ";
    ret << BuildRequest(serviceType + ".local.", DNS_PTR_TYPE, VERSION_1);
    ret << " ";

    std::stringstream stream(instanceName);
    buf.clear();
    buf.push_back(static_cast<unsigned char>(stream.str().length()));
    ret << Bin2HexStr(buf);

    ret << Bin2HexStr(stream.str());
    ret << "c027";

    ret << Bin2HexStr(std::string(";"));
    buf.clear();
    buf.push_back(static_cast<unsigned char>(svrName.length()));
    ret << Bin2HexStr(buf);
    ret << Bin2HexStr(svrName);
    return ret.str();
}

std::string WifiP2pDnsSdServiceInfo::BuildTxtServiceQuery(const std::string &instanceName,
    const std::string &serviceType, const WifiP2PDnsTxtRecord &txtRecord, const std::string &svrName)
{
    std::string ret("");
    ret += std::string("bonjour ");
    std::string dnsName = instanceName + std::string(".") + serviceType + std::string(".local.");
    ret += BuildRequest(dnsName, DNS_TXT_TYPE, VERSION_1);
    ret += std::string(" ");
    std::vector<unsigned char> rawData = txtRecord.GetData();
    if (rawData.empty()) {
        ret += std::string("00");
    } else {
        ret += Bin2HexStr(rawData);
    }

    if (!svrName.empty()) {
        if (!rawData.empty()) {
            ret += std::string("00");
        }
        ret.append(Bin2HexStr(std::string(";")));
        std::vector<unsigned char> buf;
        buf.push_back(static_cast<unsigned char>(svrName.length()));
        ret.append(Bin2HexStr(buf));
        ret.append(Bin2HexStr(svrName));
    }

    return ret;
}

std::string WifiP2pDnsSdServiceInfo::BuildRequest(const std::string &dnsName, int dnsType, int version)
{
    std::stringstream ret;
    std::string name = dnsName;
    const int stepFour = 4;
    const int stepTwo = 2;
    if (dnsType == WifiP2pDnsSdServiceInfo::DNS_TXT_TYPE) {
        transform(name.begin(), name.end(), name.begin(), ::tolower);
    }
    ret << TurnDnsNameToStream(name);
    ret << std::hex << std::setw(stepFour) << std::setfill('0') << dnsType;
    ret << std::hex << std::setw(stepTwo) << std::setfill('0') << version;
    return ret.str();
}

std::string WifiP2pDnsSdServiceInfo::TurnDnsNameToStream(const std::string &dnsName)
{
    std::stringstream ret;
    std::string name = dnsName;
    const int stepTwo = 2;

    while (true) {
        std::string data = FindPacket(name);
        if (!data.empty()) {
            ret << data;
            break;
        }
        auto i = name.find_first_of('.');
        if (i == std::string::npos) {
            if (name.length() > 0) {
                ret << std::hex << std::setw(stepTwo) << std::setfill('0') << name.length();
                ret << Bin2HexStr(name);
            }
            ret << "00"; /* for *\0 */
            break;
        }
        if (i != 0) { /* for .* create dnsreq with (null, "*") will make the dnsname like ".tcp" */
            std::string splitName = name.substr(0, i);
            ret << std::hex << std::setw(stepTwo) << std::setfill('0') << splitName.length();
            ret << Bin2HexStr(splitName);
        }
        name = name.substr(i + 1);
    }
    return ret.str();
}
}  // namespace Wifi
}  // namespace OHOS