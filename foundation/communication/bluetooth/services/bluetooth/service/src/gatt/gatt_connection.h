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

#ifndef GATT_CONNECTION_H
#define GATT_CONNECTION_H

#include <cstdint>
#include "gatt_data.h"

namespace OHOS {
namespace bluetooth {
class GattConnection {
    uint16_t connectionHandle_ = 0;
    uint16_t mtu_ = 0;
    GattDevice device_;

public:
    // GattConnection()
    // {}
    ~GattConnection()
    {}
    explicit GattConnection(const GattDevice &device) : device_(device)
    {}
    GattConnection(const GattDevice &device, uint16_t mtu, uint16_t handle)
        : connectionHandle_(handle), mtu_(mtu), device_(device)
    {}

    const GattDevice &GetDevice() const
    {
        return device_;
    }

    const uint16_t &GetHandle() const
    {
        return connectionHandle_;
    }

    void SetHandle(uint16_t handle)
    {
        connectionHandle_ = handle;
    }

    const uint16_t &GetMtu() const
    {
        return mtu_;
    }

    void SetMtu(uint16_t mtu)
    {
        mtu_ = mtu;
    }
};
}  // namespace bluetooth
}  // namespace OHOS

#endif // GATT_CONNECTION_OBSERVER_H
