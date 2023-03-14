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

#ifndef GATT_SERVICE_BASE_H
#define GATT_SERVICE_BASE_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <stddef.h>
#include "buffer.h"
#include "gatt_defines.h"

namespace OHOS {
namespace bluetooth {
class GattServiceBase {
public:
    static const uint8_t MAXIMUM_NUMBER_APPLICATION;

    GattServiceBase() : runningState_(false), mutexRunningState_()
    {}
    virtual ~GattServiceBase()
    {}
    bool InRunningState()
    {
        std::lock_guard<std::mutex> lck(mutexRunningState_);
        return runningState_;
    }
    void Start()
    {
        std::lock_guard<std::mutex> lck(mutexRunningState_);
        runningState_ = true;
    }
    void Stop()
    {
        std::lock_guard<std::mutex> lck(mutexRunningState_);
        runningState_ = false;
    }

    static int ConvertConnectionState(const std::string &state);

    static Buffer *BuildBuffer(const uint8_t *value, size_t length);
    static Buffer *BuildBuffer(const uint8_t *value, size_t offset, size_t length);

    static GattValue MoveToGattValue(std::unique_ptr<uint8_t[]> &value);
    static GattValue BuildGattValue(const uint8_t *value, size_t length);
    static int GetApplicationId();

private:
    bool runningState_;
    std::mutex mutexRunningState_;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !GATT_SERVICE_BASE_H
