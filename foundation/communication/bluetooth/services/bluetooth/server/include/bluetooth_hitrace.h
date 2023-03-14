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

#ifndef OHOS_BLUETOOTH_HITRACE_H
#define OHOS_BLUETOOTH_HITRACE_H

#include <vector>
#include <string>

#include "hitrace_meter.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothHiTrace {
public:
    static void BluetoothStartSyncTrace(const std::string& value);
    static void BluetoothFinishSyncTrace(const std::string& value);
    static void BluetoothStartAsyncTrace(const std::string& value, int32_t taskId);
    static void BluetoothFinishAsyncTrace(const std::string& value, int32_t taskId);

private:
    BluetoothHiTrace() = default;
    ~BluetoothHiTrace() = default;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_HITRACE_H