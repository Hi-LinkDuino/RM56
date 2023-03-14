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

#ifndef HCI_DEF_FEATURE_H
#define HCI_DEF_FEATURE_H

#ifdef __cplusplus
extern "C" {
#endif

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part C
// 3.3 FEATURE MASK DEFINITION
#define GET_FEATURE_FLAG(feature, byteIndex, bitIndex) ((feature)[(byteIndex)] & 0x01 << (bitIndex))

#define HCI_SUPPORT_3_SLOT_PACKETS(x) !!GET_FEATURE_FLAG(x, 0, 0)
#define HCI_SUPPORT_5_SLOT_PACKETS(x) !!GET_FEATURE_FLAG(x, 0, 1)
#define HCI_SUPPORT_ROLE_SWITCH(x) !!GET_FEATURE_FLAG(x, 0, 5)
#define HCI_SUPPORT_HOLD_MODE(x) !!GET_FEATURE_FLAG(x, 0, 6)
#define HCI_SUPPORT_SNIFF_MODE(x) !!GET_FEATURE_FLAG(x, 0, 7)
#define HCI_SUPPORT_SCO(x) !!GET_FEATURE_FLAG(x, 1, 3)
#define HCI_SUPPORT_HV2_PACKET(x) !!GET_FEATURE_FLAG(x, 1, 4)
#define HCI_SUPPORT_HV3_PACKET(x) !!GET_FEATURE_FLAG(x, 1, 5)
#define HCI_SUPPORT_EDR_ACL_2MBS_MODE(x) !!GET_FEATURE_FLAG(x, 3, 1)
#define HCI_SUPPORT_EDR_ACL_3MBS_MODE(x) !!GET_FEATURE_FLAG(x, 3, 2)
#define HCI_SUPPORT_RSSI_INQUIRY_RESPONSE(x) !!GET_FEATURE_FLAG(x, 3, 6)
#define HCI_SUPPORT_ESCO_EV3(x) !!GET_FEATURE_FLAG(x, 3, 7)
#define HCI_SUPPORT_ESCO_EV4(x) !!GET_FEATURE_FLAG(x, 4, 0)
#define HCI_SUPPORT_ESCO_EV5(x) !!GET_FEATURE_FLAG(x, 4, 1)
#define HCI_SUPPORT_BREDR(x) !GET_FEATURE_FLAG(x, 4, 5)
#define HCI_SUPPORT_LE(x) !!GET_FEATURE_FLAG(x, 4, 6)
#define HCI_SUPPORT_3_SLOT_EDR_PACKET(x) !!GET_FEATURE_FLAG(x, 4, 7)
#define HCI_SUPPORT_5_SLOT_EDR_PACKET(x) !!GET_FEATURE_FLAG(x, 5, 0)
#define HCI_SUPPORT_ESCO_2MBS(x) !!GET_FEATURE_FLAG(x, 5, 5)
#define HCI_SUPPORT_ESCO_3MBS(x) !!GET_FEATURE_FLAG(x, 5, 6)
#define HCI_SUPPORT_3SLOT_ESCO(x) !!GET_FEATURE_FLAG(x, 5, 7)
#define HCI_SUPPORT_EIR_INQUIRY_RESPONSE(x) !!GET_FEATURE_FLAG(x, 6, 0)
#define HCI_SUPPORT_SECURE_SIMPLE_PAIRING(x) !!GET_FEATURE_FLAG(x, 6, 3)
#define HCI_SUPPORT_NON_FLUSHABLE_PACKET_BONDARY_FLAG(x) !!GET_FEATURE_FLAG(x, 6, 6)

// Page 1
#define HCI_SUPPORT_SECURE_SIMPLE_PAIRING_HOST(x) !!GET_FEATURE_FLAG(x, 0, 0)

// Page 2
#define HCI_SUPPORT_SECURE_CONNECTIONS(x) !!GET_FEATURE_FLAG(x, 1, 0)

#ifdef __cplusplus
}
#endif

#endif