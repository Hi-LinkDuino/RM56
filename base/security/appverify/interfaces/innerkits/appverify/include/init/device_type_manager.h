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

#ifndef OHOS_DEVICE_TYPE_MANAGER_H
#define OHOS_DEVICE_TYPE_MANAGER_H

#include "common/export_define.h"

#include <mutex>

namespace OHOS {
namespace Security {
namespace Verify {
class DeviceTypeManager {
public:
    DLL_EXPORT static DeviceTypeManager& GetInstance();
    DLL_EXPORT bool GetDeviceTypeInfo();
    /* Forbid replication constructs and replication */
    DeviceTypeManager(const DeviceTypeManager& deviceTypeManager) = delete;
    DeviceTypeManager& operator = (const DeviceTypeManager& deviceTypeManager) = delete;

private:
    DeviceTypeManager();
    ~DeviceTypeManager();

    bool GetDeviceType() const;

private:
    /* true: debugging type; false: commercial type */
    bool deviceType;
    std::mutex getDeviceTypeMtx;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // OHOS_DEVICE_TYPE_MANAGER_H
