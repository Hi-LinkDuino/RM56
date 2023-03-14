/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef I_BLE_SCAN_FILTER_H
#define I_BLE_SCAN_FILTER_H

#include <cstdint>
#include <string>
#include <vector>

#include "bt_uuid.h"
#include "btstack.h"

namespace OHOS {
namespace bluetooth {
static const uint16_t FILTER_FLAG_ADDRESS = 0x0001;
static const uint16_t FILTER_FLAG_SERVICE_UUID = 0x0002;
static const uint16_t FILTER_FLAG_SOLICIT_UUID = 0x0004;
static const uint16_t FILTER_FLAG_NAME = 0x0008;
static const uint16_t FILTER_FLAG_MANUFACTURER_DATA = 0x0010;
static const uint16_t FILTER_FLAG_SERVICE_DATA = 0x0020;

static const uint8_t BLE_SCAN_FILTER_FINISHED = 0;
static const uint8_t BLE_SCAN_FILTER_FAILD = 1;

static const uint8_t BT_DEVICE_ADDRESS_TYPE_ALL = 0x02;

struct BleScanFilterParam {
    BtAddr address;
    Uuid serviceUuid;
    Uuid serviceUuidMask;
    Uuid solicitationUuid;
    Uuid solicitationUuidMask;
    std::string name;
    uint16_t manufacturerId = 0;
    uint16_t manufacturerIdMask = 0xFFFF;
    std::vector<uint8_t> manufacturerData;
    std::vector<uint8_t> manufacturerDataMask;
    std::vector<uint8_t> serviceData;
    std::vector<uint8_t> serviceDataMask;
    uint16_t filterFlag = 0;
    uint8_t filtIndex = 0;
};

struct BleScanFilterCallback {
    void (*addBleScanFilterResult)(uint8_t result, void *context) = nullptr;
    void (*deleteBleScanFilterResult)(uint8_t result, void *context) = nullptr;
    void (*startBleScanFilterResult)(uint8_t result, void *context) = nullptr;
    void (*stopBleScanFilterResult)(uint8_t result, void *context) = nullptr;
    void *context = nullptr;
};

class IBleScanFilter {
public:
    virtual ~IBleScanFilter() = default;
    virtual uint8_t GetMaxFilterNumber() = 0;
    virtual int AddBleScanFilter(BleScanFilterParam filter, BleScanFilterCallback callback) = 0;
    virtual int DeleteBleScanFilter(BleScanFilterParam filter, BleScanFilterCallback callback) = 0;
    virtual int StartBleScanFilter(BleScanFilterCallback callback) = 0;
    virtual int StopBleScanFilter(BleScanFilterCallback callback) = 0;
};

using createBleScanFilter = IBleScanFilter* (*)(void);
using destroyBleScanFilter = void (*)(IBleScanFilter* p);
}  // namespace bluetooth
}  // namespace OHOS
#endif  // I_BLE_SCAN_FILTER_H
