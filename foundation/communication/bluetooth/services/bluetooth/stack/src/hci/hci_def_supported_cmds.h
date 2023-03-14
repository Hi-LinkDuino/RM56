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

#ifndef HCI_DEF_SUPPORTED_CMDS_H
#define HCI_DEF_SUPPORTED_CMDS_H

#ifdef __cplusplus
extern "C" {
#endif

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part C
// 6.27 SUPPORTED COMMANDS
#define GET_COMMAND_FLAG(cmds, byteIndex, bitIndex) ((cmds)[(byteIndex)] & 0x01 << (bitIndex))

#define HCI_SUPPORT_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION(x) !!GET_COMMAND_FLAG(x, 29, 3)
#define HCI_SUPPORT_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION(x) !!GET_COMMAND_FLAG(x, 29, 4)
#define HCI_SUPPORT_READ_LOCAL_SUPPORTED_CODECS(x) !!GET_COMMAND_FLAG(x, 29, 5)
#define HCI_SUPPORT_LE_READ_LOCAL_P_256_PUBLIC_KEY(x) !!GET_COMMAND_FLAG(x, 34, 1)
#define HCI_SUPPORT_LE_GENERATE_DH_KEY(x) !!GET_COMMAND_FLAG(x, 34, 2)
#define HCI_SUPPORT_LE_SET_PRIVACY_MODE(x) !!GET_COMMAND_FLAG(x, 39, 2)

#ifdef __cplusplus
}
#endif

#endif