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

#ifndef CLASSIC_DATA_TYPE_DEFINE_H
#define CLASSIC_DATA_TYPE_DEFINE_H

namespace OHOS {
namespace bluetooth {
/// Flags
static const int BLUETOOTH_DATA_TYPE_FLAGS = 0x01;
/// Incomplete List of 16-bit Service Class UUIDs
static const int BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS = 0x02;
/// Complete List of 16-bit Service Class UUIDs
static const int BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS = 0x03;
/// Incomplete List of 32-bit Service Class UUIDs
static const int BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS = 0x04;
/// Complete List of 32-bit Service Class UUIDs
static const int BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS = 0x05;
/// Incomplete List of 128-bit Service Class UUIDs
static const int BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS = 0x06;
/// Complete List of 128-bit Service Class UUIDs
static const int BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS = 0x07;
/// Shortened Local Name
static const int BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME = 0x08;
/// Complete Local Name
static const int BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME = 0x09;
/// Tx Power Level
static const int BLUETOOTH_DATA_TYPE_TX_POWER_LEVEL = 0x0A;
/// Class of Device
static const int BLUETOOTH_DATA_TYPE_CLASS_OF_DEVICE = 0x0D;
/// Simple Pairing Hash C
static const int BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_HASH_C = 0x0E;
/// Simple Pairing Hash C-192
static const int BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_HASH_C_192 = 0x0E;
/// Simple Pairing Randomizer R
static const int BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_RANDOMIZER_R = 0x0F;
/// Simple Pairing Randomizer R-192
static const int BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_RANDOMIZER_R_192 = 0x0F;
/// Device ID
static const int BLUETOOTH_DATA_TYPE_DEVICE_ID = 0x10;
/// Security Manager TK Value
static const int BLUETOOTH_DATA_TYPE_SECURITY_MANAGER_TK_VALUE = 0x10;
/// Security Manager Out of Band Flags
static const int BLUETOOTH_DATA_TYPE_SECURITY_MANAMGER_OUT_OF_BAND_FLAGS = 0x11;
/// Connection Interval Range
static const int BLUETOOTH_DATA_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE = 0x12;
/// List of 16-bit Service Solicitation UUIDs
static const int BLUETOOTH_DATA_TYPE_LIST_OF_16_BIT_SERVICE_SOLICITATION_UUIDS = 0x14;
/// List of 128-bit Service Solicitation UUIDs
static const int BLUETOOTH_DATA_TYPE_LIST_OF_128_BIT_SERVICE_SOLICITATION_UUIDS = 0x15;
/// Service Data
static const int BLUETOOTH_DATA_TYPE_SERVICE_DATA = 0x16;
/// Service Data - 16-bit UUID
static const int BLUETOOTH_DATA_TYPE_SERVICE_DATA_16_BIT_UUID = 0x16;
/// Public Target Address
static const int BLUETOOTH_DATA_TYPE_PUBLIC_TARGET_ADDRESS = 0x17;
/// Random Target Address
static const int BLUETOOTH_DATA_TYPE_RANDOM_TARGET_ADDRESS = 0x18;
/// Appearance
static const int BLUETOOTH_DATA_TYPE_APPEARANCE = 0x19;
/// Advertising Interval
static const int BLUETOOTH_DATA_TYPE_ADVERTISING_INTERVAL = 0x1A;
/// LE Bluetooth Device Address
static const int BLUETOOTH_DATA_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS = 0x1B;
/// LE Role
static const int BLUETOOTH_DATA_TYPE_LE_ROLE = 0x1C;
/// Simple Pairing Hash C-256
static const int BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_HASH_C_256 = 0x1D;
/// Simple Pairing Randomizer R-256
static const int BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_RANDOMIZER_R_256 = 0x1E;
/// List of 32-bit Service Solicitation UUIDs
static const int BLUETOOTH_DATA_TYPE_LIST_OF_32_BIT_SERVICE_SOLICITATION_UUIDS = 0x1F;
/// Service Data - 32-bit UUID
static const int BLUETOOTH_DATA_TYPE_SERVICE_DATA_32_BIT_UUID = 0x20;
/// Service Data - 128-bit UUID
static const int BLUETOOTH_DATA_TYPE_SERVICE_DATA_128_BIT_UUID = 0x21;
/// LE Secure Connections Confirmation Value
static const int BLUETOOTH_DATA_TYPE_LE_SECURE_CONNECTIONS_CONFIRMATION_VALUE = 0x22;
/// LE Secure Connections Random Value
static const int BLUETOOTH_DATA_TYPE_LE_SECURE_CONNECTIONS_RANDOM_VALUE = 0x23;
/// URI
static const int BLUETOOTH_DATA_TYPE_URI = 0x24;
/// Indoor Positioning
static const int BLUETOOTH_DATA_TYPE_INDOOR_POSITIONING = 0x25;
/// Transport Discovery Data
static const int BLUETOOTH_DATA_TYPE_TRANSPORT_DISCOVERY_DATA = 0x26;
/// LE Supported Features
static const int BLUETOOTH_DATA_TYPE_LE_SUPPORTED_FEATURES = 0x27;
/// Channel Map Update Indication
static const int BLUETOOTH_DATA_TYPE_CHANNEL_MAP_UPDATE_INDICATION = 0x28;
/// PB-ADV
static const int BLUETOOTH_DATA_TYPE_PB_ADV = 0x29;
/// Mesh Message
static const int BLUETOOTH_DATA_TYPE_MESH_MESSAGE = 0x2A;
/// Mesh Beacon
static const int BLUETOOTH_DATA_TYPE_MESH_BEACON = 0x2B;
/// 3D Information Data
static const int BLUETOOTH_DATA_TYPE_3D_INFORMATION_DATA = 0x3D;
/// Manufacturer Specific Data
static const int BLUETOOTH_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA = 0xFF;
}  // namespace bluetooth
}  // namespace OHOS
#endif  /// CLASSIC_DATA_TYPE_DEFINE_H