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

#ifndef GATT_CONNECTION_OBSERVER_H
#define GATT_CONNECTION_OBSERVER_H

#include <cstdint>
#include "gatt_data.h"

namespace OHOS {
namespace bluetooth {
class GattConnectionObserver {
public:
    virtual void OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret)
    {}
    virtual void OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret)
    {}
    virtual void OnConnectionChanged(const GattDevice &device, uint16_t connectionHandle, int state)
    {}
    virtual void OnConnectionParameterChanged(
        const GattDevice &device, int interval, int latency, int timeout, int status)
    {}

    virtual void OnReconnect(const GattDevice &device, uint16_t connectionHandle, int ret)
    {}

    virtual void OnDisconnectInter(const GattDevice &device, uint16_t connectionHandle, int ret)
    {}

    virtual void OnShutDown()
    {}
    virtual ~GattConnectionObserver()
    {}
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GATT_CONNECTION_OBSERVER_H
