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

#ifndef HCI_DEF_LE_FEATURE_H
#define HCI_DEF_LE_FEATURE_H

#ifdef __cplusplus
extern "C" {
#endif

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
// 4.6 FEATURE SUPPORT
#define GET_LINK_LAYER_FEATURE_FLAG(features, bitIndex) ((features)[(bitIndex) / 8] & 0x01 << ((bitIndex) % 8))

#define HCI_SUPPORT_CONNECTION_PARAMETERS_REQUEST_PROCEDURE(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 1)
#define HCI_SUPPORT_LE_PING(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 4)
#define HCI_SUPPORT_LE_DATA_PACKET_LENGTH_EXTENSION(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 5)
#define HCI_SUPPORT_LL_PRIVACY(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 6)
#define HCI_SUPPORT_LE_2M_PHY(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 8)
#define HCI_SUPPORT_LE_CODED_PHY(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 11)
#define HCI_SUPPORT_LE_EXTENDED_ADVERTISING(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 12)
#define HCI_SUPPORT_LE_PERIODIC_ADVERTISING(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 13)
#define HCI_SUPPURT_CHANNEL_SELECTION_ALGORITHM_2(x) !!GET_LINK_LAYER_FEATURE_FLAG(x, 14)

#ifdef __cplusplus
}
#endif

#endif