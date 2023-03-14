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
#ifndef OHOS_MOCK_IF_CONFIG_H
#define OHOS_MOCK_IF_CONFIG_H

#include <gmock/gmock.h>
#include "dhcp_define.h"

namespace OHOS {
namespace Wifi {
typedef enum IpType {
    IPTYPE_IPV4,
    IPTYPE_IPV6,
    IPTYPE_MIX,
    IPTYPE_BUTT,
} IpType;

class MockIfConfig {
public:
    virtual ~MockIfConfig() = default;
    virtual void SetProxy(bool isAuto, const std::string &proxy, const std::string &port, const std::string &noProxys,
        const std::string &pac) = 0;
    virtual void FlushIpAddr(const std::string &ifName, const int &ipType) = 0;
};

class IfConfig : public MockIfConfig {
public:
    IfConfig() = default;
    ~IfConfig() = default;
    static IfConfig &GetInstance(void);
    MOCK_METHOD5(SetProxy, void(bool isAuto, const std::string &proxy, const std::string &port,
        const std::string &noProxys, const std::string &pac));
    MOCK_METHOD2(FlushIpAddr, void(const std::string &ifName, const int &ipType));
};
} // namespace Wifi
} // namespace OHOS
#endif