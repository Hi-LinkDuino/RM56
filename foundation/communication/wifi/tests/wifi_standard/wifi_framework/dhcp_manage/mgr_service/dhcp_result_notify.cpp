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

#include "dhcp_result_notify.h"

#include <iostream>

namespace OHOS {
namespace Wifi {
DhcpResultNotify::DhcpResultNotify()
{
    std::cout << "enter DhcpResultNotify::DhcpResultNotify()..." << std::endl;
}

DhcpResultNotify::~DhcpResultNotify()
{
    std::cout << "enter DhcpResultNotify::~DhcpResultNotify()..." << std::endl;
}

void DhcpResultNotify::OnSuccess(int status, const std::string& ifname, DhcpResult& result)
{
    std::cout << "===================enter DhcpResultNotify::OnSuccess()...status:" << status << ",ifname:" << ifname
        << ", result.iptype:" << result.iptype << ",isOptSuc:" << result.isOptSuc
        << ",strYourCli:" << result.strYourCli << ",strServer:" << result.strServer << ",strSubnet:" << result.strSubnet
        << ",strDns1:" << result.strDns1 << ",strDns2:" << result.strDns2
        << ",strRouter1:" << result.strRouter1 << ",strRouter2:" << result.strRouter2
        << ",strVendor:" << result.strVendor << ",uLeaseTime:" << result.uLeaseTime
        << ",uAddTime:" << result.uAddTime << ",uGetTime:" << result.uGetTime << std::endl;
}

void DhcpResultNotify::OnFailed(int status, const std::string& ifname, const std::string& reason)
{
    std::cout << "===================enter DhcpResultNotify::OnFailed()...status:" << status << ",ifname:" << ifname
        << ", reason:" << reason << std::endl;
}

void DhcpResultNotify::OnSerExitNotify(const std::string& ifname)
{
    std::cout << "===================enter DhcpResultNotify::OnSerExitNotify()...ifname:" << ifname << std::endl;
}
}  // namespace Wifi
}  // namespace OHOS