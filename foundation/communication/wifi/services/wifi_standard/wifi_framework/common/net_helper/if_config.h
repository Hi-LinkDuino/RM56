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
#ifndef OHOS_WIFI_IF_CONFIG_H
#define OHOS_WIFI_IF_CONFIG_H

#include <memory>
#include "wifi_log.h"
#include "dhcp_define.h"

namespace OHOS {
namespace Wifi {
typedef enum IpType {
    IPTYPE_IPV4,
    IPTYPE_IPV6,
    IPTYPE_MIX,
    IPTYPE_BUTT,
} IpType;

class IfConfig {
public:
    IfConfig();
    ~IfConfig();

    static IfConfig &GetInstance();

    void FlushIpAddr(const std::string &ifName, const int &ipType);

    void AddIpAddr(const std::string &ifName, const std::string &ipAddr, const std::string &mask, const int &ipType);

    void SetProxy(bool isAuto, const std::string &proxy, const std::string &port, const std::string &noProxys,
        const std::string &pac);

    bool ExecCommand(const std::vector<std::string> &vecCommandArg);

    bool GetIpAddr(const std::string& ifName, std::string& ipAddr);

private:
    /**
     * @Description : Use synchronous mode to execute the command, the current thread will be blocked
     *    until the command execution is complete.
     *
     * @param cmd - command
     * @return bool - true: success, false: failed
     */
    bool SyncExecuteCommand(const std::string& cmd);

    /**
     * @Description : Start a new thread to execute the command, the current thread will not be blocked
     *
     * @param cmd - command
     * @return bool - true: success, false: failed
     */
    bool AsyncExecuteCommand(const std::string& cmd);
};
}  // namespace Wifi
}  // namespace OHOS
#endif