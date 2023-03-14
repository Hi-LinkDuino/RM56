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
#include "wifi_p2p_dns_sd_service_response.h"
#include <climits>
#include "wifi_p2p_dns_sd_service_info.h"

namespace OHOS {
namespace Wifi {
WifiP2pDnsSdServiceResponse::WifiP2pDnsSdServiceResponse(
    P2pServiceStatus status, int tranId, const std::vector<unsigned char> &data)
    : WifiP2pServiceResponse(P2pServicerProtocolType::SERVICE_TYPE_BONJOUR, status, tranId, data),
      dnsQueryName(), instanceName(), dnsType(-1), version(-1), txtRecord(), sVmpack()
{
    Init();
};

WifiP2pDnsSdServiceResponse::WifiP2pDnsSdServiceResponse(const WifiP2pServiceResponse &resp)
    : WifiP2pServiceResponse(resp),
      dnsQueryName(), instanceName(), dnsType(-1), version(-1), txtRecord(), sVmpack()
{
    Init();
};

void WifiP2pDnsSdServiceResponse::Init()
{
    sVmpack.insert(std::make_pair(0x0c, "_tcp.local."));
    sVmpack.insert(std::make_pair(0x11, "local."));
    sVmpack.insert(std::make_pair(0x1c, "_udp.local."));
}

bool WifiP2pDnsSdServiceResponse::FetchTxtData(std::istringstream &istream)
{
    unsigned char t = istream.get();
    while (!istream.eof()) {
        if (t == 0) {
            break;
        }
        if (t > (istream.str().size() - istream.tellg())) {
            return false;
        }
        std::unique_ptr<char[]> ptr = std::make_unique<char[]>(t + 1);
        istream.read(ptr.get(), t);
        ptr[t] = '\0';
        std::istringstream iss(ptr.get());
        std::string keyVal[2];
        getline(iss, keyVal[0], '=');
        getline(iss, keyVal[1], '=');
        txtRecord.insert(std::make_pair(keyVal[0], keyVal[1]));
        t = istream.get();
    }
    return true;
}

bool WifiP2pDnsSdServiceResponse::FetchDnsName(std::istringstream &istream, std::string &dnsName)
{
    if (!dnsQueryName.empty()) {
        sVmpack.insert(std::make_pair(0x27, dnsQueryName));
    }
    dnsName = "";

    unsigned char t = istream.get();
    while (!istream.eof()) {
        if (t == 0x00) {
            return true;
        } else if (t == 0xc0) {
            t = istream.get();
            auto it = sVmpack.find(t);
            if (it == sVmpack.end()) {
                return false;
            }
            dnsName.append(it->second);
            return true;
        } else {
            if (t > (istream.str().size() - istream.tellg())) {
                return false;
            }
            std::unique_ptr<char[]> ptr = std::make_unique<char[]>(t);
            istream.read(ptr.get(), t);
            dnsName.append(ptr.get(), 0, t);
            dnsName.append(".");
        }
        t = istream.get();
    }
    return false;
}

bool WifiP2pDnsSdServiceResponse::ParseData()
{
    std::vector<unsigned char> data = GetData();
    if (data.empty()) {
        return true; /* empty is ok */
    }
    std::string strData;
    strData.insert(strData.begin(), data.begin(), data.end());
    std::istringstream istream(strData);
    std::string dnsName;
    if (!FetchDnsName(istream, dnsName) || dnsName.empty()) {
        return false;
    }
    dnsQueryName = dnsName;

    const int typeAndVersionSize = 3;
    if ((istream.str().size() - istream.tellg()) <= typeAndVersionSize) {
        return false;
    }
    dnsType = istream.get() << CHAR_BIT; /* 2 Byte of high bit */
    dnsType = istream.get();
    version = istream.get(); /* 1 Byte */

    if (dnsType == WifiP2pDnsSdServiceInfo::DNS_PTR_TYPE) {
        std::string rData;
        if (!FetchDnsName(istream, rData) || rData.empty()) {
            return false;
        }
        if (rData.length() <= dnsQueryName.length()) {
            return false;
        }
        instanceName.insert(instanceName.begin(), rData.begin(), rData.end() - dnsQueryName.length() - 1);
    } else if (dnsType == WifiP2pDnsSdServiceInfo::DNS_TXT_TYPE) {
        if (!FetchTxtData(istream)) {
            return false;
        }
    } else {
        return false;
    }
    if (!istream.eof() && istream.get() == ';') {
        std::string svrName("");
        unsigned char svrNameLength = istream.get();
        std::unique_ptr<char[]> ptr = std::make_unique<char[]>(svrNameLength);
        istream.read(ptr.get(), svrNameLength);
        svrName.append(ptr.get(), 0, svrNameLength);
        mSvrName = svrName;
    }
    return true;
}

const std::string &WifiP2pDnsSdServiceResponse::GetQueryName() const
{
    return dnsQueryName;
}

const std::string &WifiP2pDnsSdServiceResponse::GetInstanceName() const
{
    return instanceName;
}

int WifiP2pDnsSdServiceResponse::GetDnsType() const
{
    return dnsType;
}

int WifiP2pDnsSdServiceResponse::GetVersion() const
{
    return version;
}

const std::map<std::string, std::string> &WifiP2pDnsSdServiceResponse::GetTxtRecord() const
{
    return txtRecord;
}
}  // namespace Wifi
}  // namespace OHOS
