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
#ifndef OHOS_MOCK_DHCPSERVICE_H
#define OHOS_MOCK_DHCPSERVICE_H
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "i_dhcp_service.h"

namespace OHOS {
namespace Wifi {
class DhcpService : public IDhcpService {
public:
    DhcpService() = default;
    ~DhcpService() = default;
    int StartDhcpClient(const std::string& ifname, bool bIpv6);
    int StopDhcpClient(const std::string& ifname, bool bIpv6);
    int GetDhcpResult(const std::string& ifname, IDhcpResultNotify *pResultNotify, int timeouts);
    int GetDhcpInfo(const std::string& ifname, DhcpServiceInfo& dhcp);
    int RenewDhcpClient(const std::string& ifname);
    int ReleaseDhcpClient(const std::string& ifname);
    int StartDhcpServer(const std::string& ifname);
    int StopDhcpServer(const std::string& ifname);
    int GetServerStatus(void);
    int PutDhcpRange(const std::string& tagName, const DhcpRange& range);
    int RemoveDhcpRange(const std::string& tagName, const DhcpRange& range);
    int RemoveAllDhcpRange(const std::string& tagName);
    int SetDhcpRange(const std::string& ifname, const DhcpRange& range);
    int SetDhcpRange(const std::string& ifname, const std::string& tagName);
    int GetLeases(const std::string& ifname, std::vector<std::string>& leases);
    int GetDhcpSerProExit(const std::string& ifname, IDhcpResultNotify *pResultNotify);
};
}  // namespace Wifi
}  // namespace OHOS
#endif
