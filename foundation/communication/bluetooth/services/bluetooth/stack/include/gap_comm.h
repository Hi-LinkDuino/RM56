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

#ifndef GAP_COMM_DEF_H
#define GAP_COMM_DEF_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GAP_SUCCESS BT_NO_ERROR                    /// GAP error code success
#define GAP_ERR_NOT_ENABLE BT_BAD_STATUS           /// GAP error code module not initialize
#define GAP_ERR_INVAL_PARAM BT_BAD_PARAM           /// GAP error code invalid parameter
#define GAP_ERR_INVAL_STATE BT_BAD_STATUS          /// GAP error code invalid state
#define GAP_ERR_REPEATED BT_ALREADY                /// GAP error code repeated action
#define GAP_ERR_OUT_OF_RES BT_NO_MEMORY            /// GAP error code out of resource
#define GAP_ERR_REMOTE_ACTION BT_OPERATION_FAILED  /// GAP error code communication failed
#define GAP_ERR_NOT_SUPPORT BT_NOT_SUPPORT         /// GAP error code not support

#define GAP_STATUS_SUCCESS 0x00  /// GAP success status
#define GAP_STATUS_FAILED 0xFF   /// GAP internal error status

#define GAP_NOT_ACCEPT 0  /// reject
#define GAP_ACCEPT 1      /// accept

#define GAP_LINKKEY_SIZE 0x10           /// bluetooth link key size
#define GAP_PINCODE_SIZE 0x10           /// bluetooth pin code size
#define GAP_OOB_DATA_CONFIRM_SIZE 0x10  /// bluetooth OOB data confirm size
#define GAP_OOB_DATA_RANDOM_SIZE 0x10   /// bluetooth OOB data random size
#define GAP_OOB_DATA_SIZE 0x10          /// bluetooth OOB data size
#define GAP_EIR_SIZE_MAX 240            /// bluetooth extended inquiry response max size
#define GAP_NAME_SIZE_MAX 248           /// bluetooth device name max size

#define GAP_DISCOVERABLE_MODE_NON 0x00      /// Non-discoverable Mode
#define GAP_DISCOVERABLE_MODE_LIMITED 0x01  /// Limited Discoverable Mode
#define GAP_DISCOVERABLE_MODE_GENERAL 0x02  /// General Discoverable Mode

#define GAP_INQUIRY_SCAN_TYPE_GENERAL 0x00     /// Standard Scan
#define GAP_INQUIRY_SCAN_TYPE_INTERLACED 0x01  /// Interlaced Scan

#define GAP_CONNECTABLE_MODE_NON 0x00  /// Non-connectable Mode
#define GAP_CONNECTABLE_MODE 0x01      /// Connectable Mode

#define GAP_PAGE_SCAN_TYPE_GENERAL 0x00     /// Standard Scan
#define GAP_PAGE_SCAN_TYPE_INTERLACED 0x01  /// Interlaced Scan

#define GAP_SCAN_INTERVAL_RANGE_MIN 0x0012  /// Minimum Scan Interval
#define GAP_SCAN_INTERVAL_RANGE_MAX 0x1000  /// Maximum Scan Interval
#define GAP_SCAN_WINDOW_RANGE_MIN 0x0011    /// Minimum Scan Window
#define GAP_SCAN_WINDOW_RANGE_MAX 0x1000    /// Maximum Scan Window

#define GAP_INQUIRY_SCAN_INTERVAL_DEFAULT 0x1000  /// Controller default inquiry scan interval value
#define GAP_INQUIRY_SCAN_WINDOW_DEFAULT 0x0012    /// Controller default inquiry scan window value
#define GAP_PAGE_SCAN_INTERVAL_DEFAULT 0x0800     /// Controller default page scan interval value
#define GAP_PAGE_SCAN_WINDOW_DEFAULT 0x0012       /// Controller default page scan window value

#define GAP_BONDABLE_MODE_NON 0x00  /// Non-bondable Mode
#define GAP_BONDABLE_MODE 0x01      /// Boneable Mode

/// IO capabilities
#define GAP_IO_DISPLAYONLY 0x00
#define GAP_IO_DISPLAYYESNO 0x01
#define GAP_IO_KEYBOARDONLY 0x02
#define GAP_IO_NOINPUTNOOUTPUT 0x03
#define GAP_IO_KEYBOARDDISPLAY 0x04

/// Link key type of BR/EDR
#define GAP_LKEY_TYPE_COMBINATION 0x00
#define GAP_LKEY_TYPE_LOCAL_UNIT 0x01
#define GAP_LKEY_TYPE_REMOTE_UNIT 0x02
#define GAP_LKEY_TYPE_DEBUG_COMB 0x03
#define GAP_LKEY_TYPE_UNAUTH_COMB_P192 0x04
#define GAP_LKEY_TYPE_AUTH_COMB_P192 0x05
#define GAP_LKEY_TYPE_CHANGED_COMB 0x06
#define GAP_LKEY_TYPE_UNAUTH_COMB_P256 0x07
#define GAP_LKEY_TYPE_AUTH_COMB_P256 0x08

/// OOB authentication data from remote device present
#define GAP_OOB_DATA_NOT_PRESENT 0x00
#define GAP_OOB_DATA_PRESENT_P192 0x01
#define GAP_OOB_DATA_PRESENT_P256 0x02
#define GAP_OOB_DATA_PRESENT_P192_P256 0x03

/// MITM protection
#define GAP_MITM_NOT_REQUIRED 0x00
#define GAP_MITM_REQUIRED 0x01

/// Encryption status
#define GAP_ENCRYPTION_OFF 0x00
#define GAP_ENCRYPTION_ON 0x01

/// Key press type
#define GAP_KEY_PRESS_ENTRY_STARTED 0x00
#define GAP_KEY_PRESS_DIGIT_ENTERED 0x01
#define GAP_KEY_PRESS_DIGIT_ERASED 0x02
#define GAP_KEY_PRESS_CLEARED 0x03
#define GAP_KEY_PRESS_ENTRY_COMPLETED 0x04

/**
 * @brief       OOB data structure.
 */
typedef struct {
    uint8_t C[GAP_OOB_DATA_CONFIRM_SIZE];
    uint8_t R[GAP_OOB_DATA_RANDOM_SIZE];
} GapOOBData;

#ifdef __cplusplus
}
#endif

#endif /* GAP_COMM_DEF_H */