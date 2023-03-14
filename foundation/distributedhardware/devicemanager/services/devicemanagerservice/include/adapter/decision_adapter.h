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

#ifndef OHOS_DM_DECISION_ADAPTER_H
#define OHOS_DM_DECISION_ADAPTER_H

#include <string>
#include <vector>

#include "dm_device_info.h"

namespace OHOS {
namespace DistributedHardware {
class IDecisionAdapter {
public:
    virtual ~IDecisionAdapter() = default;
    virtual std::string GetName() = 0;
    virtual std::string GetVersion() = 0;
    virtual int32_t FilterDeviceList(std::vector<DmDeviceInfo> &infoList, const std::string &filterOptions) = 0;
    virtual int32_t SortDeviceList(std::vector<DmDeviceInfo> &infoList, const std::string &sortOptions) = 0;
};

using CreateIDecisionAdapterFuncPtr = IDecisionAdapter *(*)(void);
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_DECISION_ADAPTER_H
