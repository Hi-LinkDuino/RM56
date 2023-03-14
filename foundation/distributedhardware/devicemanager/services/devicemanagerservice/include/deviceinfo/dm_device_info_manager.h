/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_DEVICEINFO_MANAGER_H
#define OHOS_DM_DEVICEINFO_MANAGER_H

#include <string>
#include <vector>

#include "dm_adapter_manager.h"
#include "dm_device_info.h"
#include "softbus_connector.h"

namespace OHOS {
namespace DistributedHardware {
class DmDeviceInfoManager {
public:
    explicit DmDeviceInfoManager(std::shared_ptr<SoftbusConnector> &softbusConnectorPtr);
    int32_t GetTrustedDeviceList(const std::string &pkgName, const std::string &extra,
                                std::vector<DmDeviceInfo> &deviceList);
    int32_t GetLocalDeviceInfo(DmDeviceInfo &info);

private:
    std::shared_ptr<SoftbusConnector> softbusConnector_;
    std::string decisionSoName_;
};
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DM_DEVICEINFO_MANAGER_H
