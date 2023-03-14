/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef BT_HAL_BLUETOOTH_ADDRESS_H
#define BT_HAL_BLUETOOTH_ADDRESS_H

#include <memory>
#include <string>
#include <vector>

#include "bt_hal_constant.h"

namespace OHOS {
namespace HDI {
namespace BT {
namespace V1_0 {
class BluetoothAddress {
public:
    BluetoothAddress();
    ~BluetoothAddress() = default;
    static std::shared_ptr<BluetoothAddress> GetDeviceAddress(const std::string &path = BT_DEVICE_ADDRESS_PATH);
    static std::shared_ptr<BluetoothAddress> GenerateDeviceAddress(const std::string &prefix = "");
    void ReadAddress(std::vector<uint8_t> &address) const;
    void ReadAddress(std::string &address) const;

private:
    int ParseAddressFromString(const std::string &string);

private:
    std::vector<uint8_t> address_;
};
}  // namespace V1_0
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS
#endif