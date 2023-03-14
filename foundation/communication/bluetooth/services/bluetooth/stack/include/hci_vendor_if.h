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

/**
 * @file hci_if.h
 *
 * @brief Interface of bluetooth hci vendor cmd
 *
 */

#ifndef HIC_VENDOR_IF_H
#define HIC_VENDOR_IF_H

#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define HCI_COMMAND_OGF_VENDOR_SPECIFIC 0x3F

typedef struct {
    void (*hciCommandCompleteEvent)(uint16_t opCode, const void *param, uint8_t paramLength, void *context);
    void (*hciVendorClosed)(void *context);
} HCIVendorEventCallback;

/**
 * @brief       Register hci vendor event callback
 * @param       callback            hci vendor event callback
 * @param[in]   context             hci vendor event parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int HCIVIF_RegisterVendorEventCallback(
    const HCIVendorEventCallback *callback, void *context);

/**
 * @brief       Deregister vendor event callback
 * @param       callback            hci vendor event callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int HCIVIF_DeregisterVendorEventCallback(const HCIVendorEventCallback *callback);

/**
 * @brief       Send hci vendor cmd
 *
 * @param       opCode              The cmd opcode.
 * @param       param               The cmd parameter.
 * @param       paramLength         The cmd parameter length.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int BTSTACK_API HCIVIF_SendCmd(uint16_t opCode, const void *param, size_t paramLength);

/**
 * @brief       Receive a vendor commannd complete event
 *
 * @param       opCode              The event opcode.
 * @param       param               The event parameter.
 * @param       paramLength         The event parameter length.
 */
void HciEventOnVendorCommandComplete(uint16_t opCode, const void *param, uint8_t paramLength);

/**
 * @brief       Initialize function for hci vendor cmd/event.
 */
void HciVendorInit(void);

/**
 * @brief       Close function for hci vendor cmd/event.
 */
void HciVendorClose(void);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // HIC_VENDOR_IF_H
