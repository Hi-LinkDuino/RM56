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
#include "mock_dhcp_service.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_DHCP_LABEL("DhcpService");

namespace OHOS {
namespace Wifi {
int DhcpService::StartDhcpClient(const std::string &ifname, bool bIpv6)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s, isIpv6:%{public}d", __FUNCTION__, ifname.c_str(),
        bIpv6);
    return 0;
}

int DhcpService::StopDhcpClient(const std::string &ifname, bool bIpv6)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s, isIpv6:%{public}d", __FUNCTION__, ifname.c_str(),
        bIpv6);
    return 0;
}

int DhcpService::GetDhcpResult(const std::string &ifname, IDhcpResultNotify *pResultNotify, int timeouts)
{
    pResultNotify = nullptr;
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s, timeouts:%{public}d", __FUNCTION__, ifname.c_str(),
        timeouts);
    return 0;
}

int DhcpService::GetDhcpInfo(const std::string &ifname, DhcpServiceInfo &dhcp)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s, dhcp.clientRunStatus:%{public}d", __FUNCTION__,
        ifname.c_str(), dhcp.clientRunStatus);
    return 0;
}

int DhcpService::RenewDhcpClient(const std::string &ifname)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s", __FUNCTION__, ifname.c_str());
    return 0;
}

int DhcpService::ReleaseDhcpClient(const std::string &ifname)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s", __FUNCTION__, ifname.c_str());
    return 0;
}

int DhcpService::StartDhcpServer(const std::string &ifname)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s", __FUNCTION__, ifname.c_str());
    return 0;
}

int DhcpService::StopDhcpServer(const std::string &ifname)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s", __FUNCTION__, ifname.c_str());
    return 0;
}

int DhcpService::GetServerStatus(void)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].", __FUNCTION__);
    return 0;
}

int DhcpService::PutDhcpRange(const std::string &tagName, const DhcpRange &range)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].tagName:%{public}s, range.iptype:%{public}d", __FUNCTION__,
        tagName.c_str(), range.iptype);
    return 0;
}

int DhcpService::RemoveDhcpRange(const std::string &tagName, const DhcpRange &range)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].tagName:%{public}s, range.iptype:%{public}d", __FUNCTION__,
        tagName.c_str(), range.iptype);
    return 0;
}

int DhcpService::RemoveAllDhcpRange(const std::string &tagName)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].tagName:%{public}s", __FUNCTION__, tagName.c_str());
    return 0;
}

int DhcpService::SetDhcpRange(const std::string &ifname, const DhcpRange &range)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].tagName:%{public}s, range.iptype:%{public}d", __FUNCTION__,
        ifname.c_str(), range.iptype);
    return 0;
}

int DhcpService::SetDhcpRange(const std::string &ifname, const std::string &tagName)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s, tagName:%{public}s", __FUNCTION__, ifname.c_str(),
        tagName.c_str());
    return 0;
}

int DhcpService::GetLeases(const std::string&, std::vector<std::string>& leases)
{
    WIFI_LOGD("Enter DhcpService::[%{public}s].", __FUNCTION__);
    if (leases.size() == 0) {
        return -1;
    }
    return 0;
}

int DhcpService::GetDhcpSerProExit(const std::string &ifname, IDhcpResultNotify *pResultNotify)
{
    pResultNotify = nullptr;
    WIFI_LOGD("Enter DhcpService::[%{public}s].ifname:%{public}s", __FUNCTION__, ifname.c_str());
    return 0;
}
} // namespace Wifi
} // namespace OHOS
