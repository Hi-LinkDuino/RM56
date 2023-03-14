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

/**
 * @file l2cap_if.h
 *
 * @brief Interface of bluetooth l2cap protocol BR/EDR part
 *
 */

#ifndef L2CAP_IF_H
#define L2CAP_IF_H

#include "l2cap_def.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Initialize l2cap for BR/EDR
 *
 * @param traceLevel debug log level.
 */
void BTSTACK_API L2CIF_Initialize(int traceLevel);

/**
 * @brief Finalize l2cap for BR/EDR
 *
 */
void BTSTACK_API L2CIF_Finalize();

/**
 * @brief Register l2cap psm
 *
 * @param psm protocol psm
 * @param svc callback for protocol psm
 * @param context context for protocol psm
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API L2CIF_RegisterService(
    uint16_t lpsm, const L2capService *svc, void *context, void (*cb)(uint16_t lpsm, int result));

/**
 * @brief Deregister l2cap psm
 *
 * @param psm protocol psm
 */
void BTSTACK_API L2CIF_DeregisterService(uint16_t lpsm, void (*cb)(uint16_t lpsm, int result));

/**
 * @brief Send Connection Request packets
 *
 * @param addr remote bluetooth address
 * @param lpsm local protocol psm
 * @param rpsm remote protocol psm
 * @param lcid OUT parameter, local channel id
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API L2CIF_ConnectReq(const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, void *context,
    void (*cb)(const BtAddr *addr, uint16_t lcid, int result, void *context));

/**
 * @brief Send Connection Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @param result indicates the outcome of the connection request
 * @param status indicates the status of the connection
 */
void BTSTACK_API L2CIF_ConnectRsp(
    uint16_t lcid, uint8_t id, uint16_t result, uint16_t status, void (*cb)(uint16_t lcid, int result));

/**
 * @brief Send Configuration Request packet
 *
 * @param lcid local channel id
 * @param cfg config parameter
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API L2CIF_ConfigReq(uint16_t lcid, const L2capConfigInfo *cfg, void (*cb)(uint16_t lcid, int result));

/**
 * @brief Send Configuration Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @param cfg config parameter
 * @param result config result
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API L2CIF_ConfigRsp(
    uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, uint16_t result, void (*cb)(uint16_t lcid, int result));

/**
 * @brief Send Disconnection Request packet
 *
 * @param lcid local channel id
 */
void BTSTACK_API L2CIF_DisconnectionReq(uint16_t lcid, void (*cb)(uint16_t lcid, int result));

/**
 * @brief Send Disconnection Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 */
void BTSTACK_API L2CIF_DisconnectionRsp(uint16_t lcid, uint8_t id, void (*cb)(uint16_t lcid, int result));

/**
 * @brief In Enhanced Retransmission mode, send RNR to information remote to stop sending data
 *
 * @param lcid local channel id
 * @param isBusy flag to indicate busy state, 0 -- non busy, 1 -- busy
 */
void BTSTACK_API L2CIF_LocalBusy(uint16_t lcid, uint8_t isBusy, void (*cb)(uint16_t lcid, int result));

/**
 * @brief Send l2cap data packet
 *
 * @param lcid local channel id
 * @param pkt packet of data
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API L2CIF_SendData(uint16_t lcid, const Packet *pkt, void (*cb)(uint16_t lcid, int result));

/**
 * @brief Register Echo callback
 *
 * @param echoCallback callback of echo
 * @param context context of caller
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API L2CIF_RegisterEcho(const L2capEcho *echoCallback, void *context);

/**
 * @brief Deregister Echo callback
 *
 */
void BTSTACK_API L2CIF_DeregisterEcho();

/**
 * @brief Send Echo Request packet
 *
 * @param aclHandle ACL Handle
 * @param data data of echo
 * @param dataLen length of data
 */
void BTSTACK_API L2CIF_EchoReq(
    uint16_t aclHandle, const uint8_t *data, uint16_t dataLen, void (*cb)(uint16_t aclHandle, int result));

/**
 * @brief Send Echo Response packet received
 *
 * @param aclHandle ACL Handle
 * @param id identifier of l2cap command
 * @param data data of echo
 * @param dataLen length of data
 */
void BTSTACK_API L2CIF_EchoRsp(
    uint16_t aclHandle, uint8_t id, const uint8_t *data, uint16_t dataLen, void (*cb)(uint16_t aclHandle, int result));

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_IF_H