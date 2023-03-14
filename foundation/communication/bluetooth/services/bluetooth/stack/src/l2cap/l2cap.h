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
 * @file l2cap.h
 *
 * @brief Implement BDR part of bluetooth l2cap protocol
 *
 */

#ifndef L2CAP_H
#define L2CAP_H

#include "l2cap_def.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Initialize l2cap for BR/EDR
 *
 * @param traceLevel debug log level.
 */
void L2CAP_Initialize(int traceLevel);

/**
 * @brief Finalize l2cap for BR/EDR
 *
 */
void L2CAP_Finalize();

/**
 * @brief Register l2cap psm
 *
 * @param psm protocol psm
 * @param svc callback for protocol psm
 * @param context context for protocol psm
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_RegisterService(uint16_t lpsm, const L2capService *svc, void *context);

/**
 * @brief Deregister l2cap psm
 *
 * @param psm protocol psm
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_DeregisterService(uint16_t lpsm);

/**
 * @brief Send Connection Request packets
 *
 * @param addr remote bluetooth address
 * @param lpsm local protocol psm
 * @param rpsm remote protocol psm
 * @param lcid OUT parameter, local channel id
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_ConnectReq(const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, uint16_t *lcid);

/**
 * @brief Send Connection Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @param result indicates the outcome of the connection request
 * @param status indicates the status of the connection
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_ConnectRsp(uint16_t lcid, uint8_t id, uint16_t result, uint16_t status);

/**
 * @brief Send Configuration Request packet
 *
 * @param lcid local channel id
 * @param cfg config parameter
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_ConfigReq(uint16_t lcid, const L2capConfigInfo *cfg);

/**
 * @brief Send Configuration Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @param cfg config parameter
 * @param result config result
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_ConfigRsp(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, uint16_t result);

/**
 * @brief Send Disconnection Request packet
 *
 * @param lcid local channel id
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_DisconnectionReq(uint16_t lcid);

/**
 * @brief Send Disconnection Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_DisconnectionRsp(uint16_t lcid, uint8_t id);

/**
 * @brief In Enhanced Retransmission mode, send RNR to information remote to stop sending data
 *
 * @param lcid local channel id
 * @param isBusy flag to indicate busy state, 0 -- non busy, 1 -- busy
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LocalBusy(uint16_t lcid, uint8_t isBusy);

/**
 * @brief Send l2cap data packet
 *
 * @param lcid local channel id
 * @param pkt packet of data
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_SendData(uint16_t lcid, Packet *pkt);

/**
 * @brief Register Echo callback
 *
 * @param echoCallback callback of echo
 * @param context context of caller
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_RegisterEcho(const L2capEcho *echoCallback, void *context);

/**
 * @brief Deregister Echo callback
 *
 */
int L2CAP_DeregisterEcho();

/**
 * @brief Send Echo Request packet
 *
 * @param aclHandle ACL Handle
 * @param data data of echo
 * @param dataLen length of data
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_EchoReq(uint16_t aclHandle, const uint8_t *data, uint16_t dataLen);

/**
 * @brief Send Echo Response packet received
 *
 * @param aclHandle ACL Handle
 * @param id identifier of l2cap command
 * @param data data of echo
 * @param dataLen length of data
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_EchoRsp(uint16_t aclHandle, uint8_t id, const uint8_t *data, uint16_t dataLen);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_H