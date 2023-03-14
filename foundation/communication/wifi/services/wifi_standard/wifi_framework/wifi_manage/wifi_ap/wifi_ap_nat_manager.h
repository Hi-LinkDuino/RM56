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
#ifndef OHOS_AP_NAT_MANAGER_H
#define OHOS_AP_NAT_MANAGER_H

#include <string>
#include "ap_macro.h"

namespace OHOS {
namespace Wifi {
class WifiApNatManager {
    FRIEND_GTEST(WifiApNatManager);

public:
    /**
     * @Description  Using the IFC-to-IFC NAT Function.
     * @param enable - enabled or disabled
     * @param inInterfaceName - Source Network Interface Name
     * @param outInterfaceName - Destination Network Interface Name
     * @return true: success     false: failed
     */
    bool EnableInterfaceNat(bool enable, std::string inInterfaceName, std::string outInterfaceName) const;

private:
    /**
     * @Description  Allow the system to use the forward function.
     * @param enable - enabled or disabled
     * @return true: success     false: failed
     */
    bool SetForwarding(bool enable) const;

    /**
     * @Description  Set route rule.
     * @param enable - enabled or disabled
     * @return true: success     false: failed
     */
    bool SetInterfaceRoute(bool enable) const;

    /**
     * @Description  Set NAT rule.
     * @param enable - enabled or disabled
     * @param outInterfaceName - Destination Network Interface Name
     * @return true: success     false: failed
     */
    bool SetInterfaceNat(bool enable, const std::string &outInterfaceName) const;

    /**
     * @Description  Write data to a file.
     * @param fileName - file name
     * @param content - Character data to be written
     * @return true: success     false: failed
     */
    bool WriteDataToFile(const std::string &fileName, const std::string &content) const;
};
}  // namespace Wifi
}  // namespace OHOS
#endif