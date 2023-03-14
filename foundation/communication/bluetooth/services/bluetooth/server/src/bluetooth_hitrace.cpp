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

#include "bluetooth_hitrace.h"

#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
static constexpr uint64_t BLUETOOTH_BYTRACE_LABEL = HITRACE_TAG_BLUETOOTH;
void BluetoothHiTrace::BluetoothStartSyncTrace(const std::string& value)
{
    StartTrace(BLUETOOTH_BYTRACE_LABEL, value);
    HILOGI("value:%{public}s", value.c_str());
}

void BluetoothHiTrace::BluetoothFinishSyncTrace(const std::string& value)
{
    FinishTrace(BLUETOOTH_BYTRACE_LABEL);
    HILOGI("value:%{public}s", value.c_str());
}

void BluetoothHiTrace::BluetoothStartAsyncTrace(const std::string& value, int32_t taskId)
{
    StartAsyncTrace(BLUETOOTH_BYTRACE_LABEL, value, taskId);
    HILOGI("value:%{public}s, taskId:%{public}d", value.c_str(), taskId);
}

void BluetoothHiTrace::BluetoothFinishAsyncTrace(const std::string& value, int32_t taskId)
{
    FinishAsyncTrace(BLUETOOTH_BYTRACE_LABEL, value, taskId);
    HILOGI("value:%{public}s, taskId:%{public}d", value.c_str(), taskId);
}
}  // namespace Bluetooth
}  // namespace OHOS
