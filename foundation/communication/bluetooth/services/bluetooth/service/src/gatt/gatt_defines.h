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

#ifndef GATT_DEFINES_H
#define GATT_DEFINES_H

#include <cstdint>
#include <memory>

namespace OHOS {
namespace bluetooth {
using GattValue = std::shared_ptr<std::unique_ptr<uint8_t[]>>;

constexpr uint16_t MAX_ATTRIBUTE_HANDLE = 0xFFFF;
constexpr uint16_t MIN_ATTRIBUTE_HANDLE = 0x0001;
constexpr uint16_t INVALID_ATTRIBUTE_HANDLE = 0x0000;
constexpr uint8_t GATT_DEFAULT_MTU = 0x17;
constexpr uint8_t BIT_8 = 0x08;
constexpr uint8_t GATT_VALUE_LEN_MAX = 0xFF;
constexpr uint8_t GATT_CCCD_NUM_MAX = 0xFF;
constexpr uint16_t GATT_NOTIFICATION_VALUE = 0x0001;
constexpr uint16_t GATT_INDICATION_VALUE = 0x0002;

constexpr uint16_t DEFAULT_BLE_MAX_CONNECTED_DEVICES = 0x0007;
constexpr uint16_t DEFAULT_CLASSIC_MAX_CONNECTED_DEVICES = 0x0007;
constexpr uint16_t DEFAULT_BLE_MIN_CONNECTION_INTERVAL = 0x18;
constexpr uint16_t DEFAULT_BLE_MAX_CONNECTION_INTERVAL = 0x28;
constexpr uint16_t DEFAULT_BLE_CONNECTION_LATENCY = 0x0000;
constexpr uint16_t DEFAULT_BLE_CONNECTION_SUPERVISION_TIMEOUT = 0x1F4;
constexpr uint16_t DEFAULT_CLASSIC_CONNECTION_MTU = 0x0200;
constexpr uint8_t DEFAULT_CLASSIC_CONNECTION_MODE = 0x00;
constexpr uint16_t DEFAULT_CLASSIC_CONNECTION_FLUSH_TIMEOUT = 0xFFFF;
constexpr uint8_t DEFAULT_CLASSIC_CONNECTION_SECURITY_MODE = 0x24;
constexpr uint16_t DEFAULT_BLE_GATT_SERVER_EXCHANGE_MTU = 0x0200;

constexpr uint8_t CHARACTERISTIC_PROPERTIE_BROADCAST = 0x01;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_READ = 0x02;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_WRITE_WITHOUT_RESPONSE = 0x04;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_WRITE = 0x08;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_NOTIFY = 0x10;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_INDICATE = 0x20;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_AUTHENTICATED_SIGNED_WRITES = 0x40;
constexpr uint8_t CHARACTERISTIC_PROPERTIE_EXTENDED_PROPERTIES = 0x80;

constexpr uint16_t UUID_PRIMARY_SERVICE = 0x2800;
constexpr uint16_t UUID_SECONDARY_SERVICE = 0x2801;
constexpr uint16_t UUID_INCLUDE_SERVICE = 0x2802;
constexpr uint16_t UUID_CHARACTERISTIC = 0x2803;

constexpr uint16_t UUID_SERVICE_CHANGED = 0x2A05;
constexpr uint16_t UUID_CHARACTERISTIC_EXTENDED_PROPERTIES = 0x2900;
constexpr uint16_t UUID_CHARACTERISTIC_USER_DESCRIPTION = 0x2901;
constexpr uint16_t UUID_CLIENT_CHARACTERISTIC_CONFIGURATION = 0x2902;
constexpr uint16_t UUID_SERVER_CHARACTERISTIC_CONFIGURATION = 0x2903;
constexpr uint16_t UUID_CHARACTERISTIC_FROMAT = 0x2904;
constexpr uint16_t UUID_CHARACTERISTIC_AGGREGATE_FROMAT = 0x2905;
constexpr uint16_t UUID_GENERIC_ATTRIBUTE_SERVICE = 0x1801;
constexpr uint16_t UUID_DEVICE_INFORMATION_SERVICE = 0x180A;

constexpr uint16_t ATT_CONN_HANDLE_RANGE_START = 0x0000;
constexpr uint16_t ATT_CONN_HANDLE_RANGE_END = 0x0EFF;
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !GATT_DEFINES_H
