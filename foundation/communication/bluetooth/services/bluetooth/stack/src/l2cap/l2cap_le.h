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
 * @file l2cap_le.h
 *
 * @brief Implement LE part of bluetooth l2cap protocol
 *
 */

#ifndef L2CAP_LE_H
#define L2CAP_LE_H

#include "l2cap_def.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Initialize l2cap for LE
 *
 * @param traceLevel debug log level.
 */
void L2CAP_LeInitialize(int traceLevel);

/**
 * @brief Finalize l2cap for LE
 *
 */
void L2CAP_LeFinalize();

/**
 * @brief Register l2cap le_psm for LE Credit Based Connection
 *
 * @param lpsm protocol psm
 * @param svc callback for protocol psm
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeRegisterService(uint16_t lpsm, const L2capLeService *svc, void *context);

/**
 * @brief Deregister l2cap le_psm for LE Credit Based Connection
 *
 * @param lpsm protocol psm
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeDeregisterService(uint16_t lpsm);

/**
 * @brief Send LE Credit Based Connection Request packet
 *
 * @param addr remote bluetooth address
 * @param cfg config parameter
 * @param lcid OUT parameter, local channel id
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeCreditBasedConnectionReq(
    const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, const L2capLeConfigInfo *cfg, uint16_t *lcid);

/**
 * @briefSend LE Credit Based Connection Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @param cfg config parameter
 * @param result result of connection
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeCreditBasedConnectionRsp(uint16_t lcid, uint8_t id, const L2capLeConfigInfo *cfg, uint16_t result);

/**
 * @brief Send Disconnection Request packet
 *
 * @param lcid local channel id
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeDisconnectionReq(uint16_t lcid);

/**
 * @brief Send Disconnection Response packet
 *
 * @param lcid local channel id
 * @param id identifier of l2cap command
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeDisconnectionRsp(uint16_t lcid, uint8_t id);

/**
 * @brief Send Le data packet for LE Credit Based Connection
 *
 * @param lcid local channel id
 * @param pkt packet of data
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeSendData(uint16_t lcid, Packet *pkt);

/**
 * @brief Register LE Fix Channel data callback
 *
 * @param cid fix channel id
 * @param chan callback of fix channel
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeRegisterFixChannel(uint16_t cid, const L2capLeFixChannel *chan);

/**
 * @brief Deregister LE Fix Channel data callback
 *
 * @param cid fix channel id
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeDeregisterFixChannel(uint16_t cid);

/**
 * @brief Create Le ACL connection
 *
 * @param addr remote bluetooth address
 * @param param connection parameter
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeConnect(const BtAddr *addr, const L2capLeConnectionParameter *param);

/**
 * @brief Cancel Le ACL connection
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeConnectCancel(const BtAddr *addr);

/**
 * @brief Destroy Le ACL connection
 *
 * @param aclHandle ACL handle
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeDisconnect(uint16_t aclHandle);

/**
 * @brief Send LE Fix Channel data
 *
 * @param aclHandle ACL handle
 * @param cid fix channel id
 * @param pkt packet of data
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeSendFixChannelData(uint16_t aclHandle, uint16_t cid, const Packet *pkt);

/**
 * @brief Register LE connection parameter update
 *
 * @param cb callback for connection parameter update
 * @param context context of caller
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeRegisterConnectionParameterUpdate(const L2capLeConnectionParameterUpdate *cb, void *context);

/**
 * @brief Deregister LE connection parameter update
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeDeregisterConnectionParameterUpdate();

/**
 * @brief Send Connection Parameter Update Request packet,
 *        This command shall only be sent from the LE peripheral device to the LE center.
 *
 * @param aclHandle ACL handle
 * @param param connection parameter
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeConnectionParameterUpdateReq(uint16_t aclHandle, const L2capLeConnectionParameter *param);

/**
 * @brief Send Connection Parameter Update Response packet
 *
 * @param aclHandle ACL handle
 * @param id cidentifier of l2cap command
 * @param result result of the request
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int L2CAP_LeConnectionParameterUpdateRsp(uint16_t aclHandle, uint8_t id, uint16_t result);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_LE_H
