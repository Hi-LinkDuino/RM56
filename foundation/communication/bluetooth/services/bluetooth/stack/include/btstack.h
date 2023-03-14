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
#ifndef BT_STACK_H
#define BT_STACK_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NO_SANITIZE
#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define NO_SANITIZE(type) __attribute__((no_sanitize(type)))
#endif
#endif
#endif

#ifndef NO_SANITIZE
#define NO_SANITIZE(type)
#endif

#define BT_CONNECT_NUM_MAX 6

#define BT_NO_ERROR 0
#define BT_OPERATION_FAILED (-1)
#define BT_BAD_PARAM (-1000)
#define BT_BAD_STATUS (-1001)
#define BT_TIMEOUT (-1002)
#define BT_OS_ERROR (-1003)
#define BT_NO_MEMORY (-1004)
#define BT_IO_ERROR (-1005)
#define BT_CREATE_FILE (-1006)
#define BT_CONFIG_ERROR (-1007)
#define BT_DEVICE_ERROR (-1008)

#define BT_NOT_SUPPORT (-2000)
#define BT_ALREADY (-2001)

#define BT_ADDRESS_SIZE 6

#define BT_PUBLIC_DEVICE_ADDRESS 0x00
#define BT_RANDOM_DEVICE_ADDRESS 0x01
#define BT_PUBLIC_IDENTITY_ADDRESS 0x02
#define BT_RANDOM_IDENTITY_ADDRESS 0x03

typedef struct {
    uint8_t addr[6];
    uint8_t type;
} BtAddr;

#define BT_UUID_16 0x01
#define BT_UUID_32 0x02
#define BT_UUID_128 0x03

typedef struct {
    uint8_t type;
    union {
        uint16_t uuid16;
        uint32_t uuid32;
        uint8_t uuid128[16];
    };
} BtUuid;

#define L2CAP_MTU_SIZE 1691
#define SCO_HOST_BUFFER_SIZE 255
#define HOST_ACL_DATA_PACKETS 20
#define HOST_SCO_DATA_PACKETS 10

// The Link Manager Version parameter
// The HCI Version
#define BLUETOOTH_CORE_SPECIFICATION_1_0 0   // Bluetooth® Core Specification 1.0b (Withdrawn)
#define BLUETOOTH_CORE_SPECIFICATION_1_1 1   // Bluetooth Core Specification 1.1 (Withdrawn)
#define BLUETOOTH_CORE_SPECIFICATION_1_2 2   // Bluetooth Core Specification 1.2 (Withdrawn)
#define BLUETOOTH_CORE_SPECIFICATION_2_0 3   // Bluetooth Core Specification 2.0 + EDR (Withdrawn)
#define BLUETOOTH_CORE_SPECIFICATION_2_1 4   // Bluetooth Core Specification 2.1 + EDR (Withdrawn)
#define BLUETOOTH_CORE_SPECIFICATION_3_0 5   // Bluetooth Core Specification 3.0 + HS (Withdrawn)
#define BLUETOOTH_CORE_SPECIFICATION_4_0 6   // Bluetooth Core Specification 4.0
#define BLUETOOTH_CORE_SPECIFICATION_4_1 7   // Bluetooth Core Specification 4.1
#define BLUETOOTH_CORE_SPECIFICATION_4_2 8   // ​Bluetooth Core Specification 4.2
#define BLUETOOTH_CORE_SPECIFICATION_5_0 9   // Bluetooth Core Specification 5.0
#define BLUETOOTH_CORE_SPECIFICATION_5_1 10  // Bluetooth Core Specification 5.1
#define BLUETOOTH_CORE_SPECIFICATION_5_2 11  // Bluetooth Core Specification 5.2

// Modules
#define MODULE_NAME_HCI "hci"
#define MODULE_NAME_GAP "gap"
#define MODULE_NAME_L2CAP "l2cap"
#define MODULE_NAME_AVDTP "avdtp"
#define MODULE_NAME_AVCTP "avctp"
#define MODULE_NAME_RFCOMM "rfcomm"
#define MODULE_NAME_SDP "sdp"
#define MODULE_NAME_ATT "att"
#define MODULE_NAME_SMP "smp"

// Transport
#define TRANSPORT_BREDR 1
#define TRANSPORT_LE 2

#ifdef BLUETOOTH_EXPORT
#define BTSTACK_API __attribute__((visibility("default")))
#else
#define BTSTACK_API
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // BT_STACK_H
