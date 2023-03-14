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

#ifndef SMP_H
#define SMP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "btstack.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

// true:  Pairing using hardware AES-128 encryption algorithm
// false: Pairing using software AES-128 encryption algorithm
#define SMP_USING_HW_AES128_PAIR (false)

// true:  Generating Signature using hardware AES-128 encryption algorithm
// false: Generating Signature using software AES-128 encryption algorithm
#define SMP_USING_HW_AES128_SIGN (false)

// true:  Generating/Resolving RPA using hardware AES-128 encryption algorithm
// false: Generating/Resolving RPA using software AES-128 encryption algorithm
#define SMP_USING_HW_AES128_RPA (false)

#define SMP_PAIR_STATUS_SUCCESS 0x00
#define SMP_PAIR_STATUS_FAILED 0x01

#define SMP_ENCRYPT_STATUS_SUCCESS 0x00
#define SMP_ENCRYPT_STATUS_FAILED 0x01

#define SMP_GENERATE_SIGN_STATUS_SUCCESS 0x00
#define SMP_GENERATE_SIGN_STATUS_FAILED 0x01

#define SMP_GENERATE_RPA_STATUS_SUCCESS 0x00
#define SMP_GENERATE_RPA_STATUS_FAILED 0x01

#define SMP_RESOLVE_RPA_STATUS_SUCCESS 0x00
#define SMP_RESOLVE_RPA_STATUS_FAILED 0x01

#define SMP_RESOLVE_RPA_RESULT_YES 0x01
#define SMP_RESOLVE_RPA_RESULT_NO 0x00

#define SMP_GENERATE_SC_OOB_DATA_SUCCESS 0x00
#define SMP_GENERATE_SC_OOB_DATA_FAILED 0x01

#define SMP_SUCCESS BT_NO_ERROR                    /// < SMP error code success
#define SMP_ERR_NOT_ENABLE BT_BAD_STATUS           /// < SMP error code module not initialize
#define SMP_ERR_INVAL_PARAM BT_BAD_PARAM           /// < SMP error code invalid parameter
#define SMP_ERR_INVAL_STATE BT_BAD_STATUS          /// < SMP error code invalid state
#define SMP_ERR_REPEATED BT_ALREADY                /// < SMP error code repeated action
#define SMP_ERR_OUT_OF_RES BT_NO_MEMORY            /// < SMP error code out of resource
#define SMP_ERR_REMOTE_ACTION BT_OPERATION_FAILED  /// < SMP error code communication failed

#define SMP_PAIR_METHOD_JUST_WORK 0x00
// Data Type: uint32_t
#define SMP_PAIR_METHOD_PASSKEY_DISPLAY 0x02
// Data Type: uint32_t
#define SMP_PAIR_METHOD_PASSKEY_ENTRY 0x03
// Data Type: uint32_t
#define SMP_PAIR_METHOD_NUMERIC_COMPARISON 0x04
#define SMP_PAIR_METHOD_OOB_LEGACY 0x05                   // Data Type: uint8_t array (16 bytes)
#define SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV 0x06   // Data Type: uint8_t array (38 bytes)
#define SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV 0x07  // Data Type: uint8_t array (38 bytes)
#define SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND 0x08  // Data Type: uint8_t array (38 bytes)

#define SMP_BONDED_FLAG_NO 0x00
#define SMP_BONDED_FLAG_YES 0x01

#define SMP_AUTH_FLAG_NO 0x00
#define SMP_AUTH_FLAG_YES 0x01

#define SMP_PAIR_TYPE_LEGACY 0x00
#define SMP_PAIR_TYPE_SECURE_CONNECTION 0x01

#define SMP_PAIR_FAILED_NO_FAILED 0x00
#define SMP_PAIR_FAILED_PASSKEY_ENTRY 0x01
#define SMP_PAIR_FAILED_OOB_NOT_AVAILABLE 0x02
#define SMP_PAIR_FAILED_AUTH_REQ 0x03
#define SMP_PAIR_FAILED_CONFIRM_VALUE 0x04
#define SMP_PAIR_FAILED_PAIRING_NOT_SUPPORTED 0x05
#define SMP_PAIR_FAILED_ENC_KEY_SIZE 0x06
#define SMP_PAIR_FAILED_ENC_CMD_NOT_SUPPORTED 0x07
#define SMP_PAIR_FAILED_UNSPECIFIED_REASION 0x08
#define SMP_PAIR_FAILED_REPAETED_ATTEMPTS 0x09
#define SMP_PAIR_FAILED_INVALID_PARAM 0x0A
#define SMP_PAIR_FAILED_DHKEY_CHECK 0x0B
#define SMP_PAIR_FAILED_NUMERIC_COMPARISON 0x0C
#define SMP_PAIR_FAILED_BREDR_PAIRING_IN_PROGRESS 0x0D
#define SMP_PAIR_FAILED_KEY_GENERATION_NOT_ALLOWED 0x0E

#define SMP_IO_DISPLAY_ONLY 0x00
#define SMP_IO_DISPLAY_YES_NO 0x01
#define SMP_IO_KEYBOARD_ONLY 0x02
#define SMP_IO_NO_INPUT_NO_OUTPUT 0x03
#define SMP_IO_KEYBOARD_DISPLAY 0x04

#define SMP_AUTH_REQ_NO_BONDING 0x00
#define SMP_AUTH_REQ_BONDING 0x01
#define SMP_AUTH_REQ_BIT_MITM 0x04
#define SMP_AUTH_REQ_BIT_SC 0x08
#define SMP_AUTH_REQ_BIT_KEYPRESS 0x10
#define SMP_AUTH_REQ_BIT_CT2 0x20

#define SMP_KEY_DIST_BIT_ENC_KEY 0x01
#define SMP_KEY_DIST_BIT_ID_KEY 0x02
#define SMP_KEY_DIST_BIT_SIGN_KEY 0x04

#define SMP_IRK_LEN 0x10
#define SMP_CSRK_LEN 0x10
#define SMP_RAND_NUM_LEN 0x08
#define SMP_LTK_LEN 0x10

/**
 * @brief Pair param structure.
 */
typedef struct {
    uint8_t ioCapability;
    uint8_t oobDataFlag;
    uint8_t authReq;
    uint8_t maxEncKeySize;
    uint8_t initKeyDist;
    uint8_t respKeyDist;
} SMP_PairParam;

/**
 * @brief Pair result structure.
 */
typedef struct {
    uint8_t pairType;
    uint8_t bondedFlag;
    uint8_t authFlag;
    uint8_t encKeySize;
    uint8_t localKeyDist;
    uint8_t peerKeyDist;
    uint8_t localLTK[SMP_LTK_LEN];
    uint16_t localEdiv;
    uint8_t localRandom[SMP_RAND_NUM_LEN];
    uint8_t peerLTK[SMP_LTK_LEN];
    uint16_t peerEdiv;
    uint8_t peerRandom[SMP_RAND_NUM_LEN];
    uint8_t localIRK[SMP_IRK_LEN];
    BtAddr localIdentAddr;
    uint8_t peerIRK[SMP_IRK_LEN];
    BtAddr peerIdentAddr;
    uint8_t localCSRK[SMP_CSRK_LEN];
    uint8_t peerCSRK[SMP_CSRK_LEN];
} SMP_PairResult;

/**
 * @brief Security manager callback structure.
 */
typedef struct {
    void (*SMP_CallbackAuthenticationRequest)(uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue);
    void (*SMP_CallbackPairResult)(uint16_t handle, uint8_t status, const SMP_PairResult *result);
    void (*SMP_CallbackRemotePairRequest)(uint16_t handle, const SMP_PairParam *param);
    void (*SMP_CallbackRemotePairResponse)(uint16_t handle, const SMP_PairParam *param);
    void (*SMP_CallbackRemoteSecurityRequest)(uint16_t handle, uint8_t authReq);
    void (*SMP_CallbackLongTermKeyRequest)(uint16_t handle, const uint8_t *random, uint16_t ediv);
    void (*SMP_CallbackEncryptionComplete)(uint16_t handle, uint8_t status);
    void (*SMP_CallbackGenerateSignatureResult)(uint8_t status, const uint8_t *sign);
    void (*SMP_CallbackGenerateRPAResult)(uint8_t status, const uint8_t *addr);
    void (*SMP_CallbackResolveRPAResult)(uint8_t status, bool result, const uint8_t *addr, const uint8_t *irk);
    void (*SMP_CallbackGenerateScOobDataResult)(uint8_t status, const uint8_t *random, const uint8_t *confirm);
} SMP_Callback_t;

/**
 * @brief Set local IRk.
 *
 * @param irk Local IRK.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_SetIRK(const uint8_t *irk);

/**
 * @brief Set Local Identity Address.
 *
 * @param addr Local Identity Address.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_SetIdentAddr(const BtAddr *addr);

/**
 * @brief Resolve resolvable private address.
 *
 * @param addr Resolvable private address.
 * @param irk Saved irk.
 * @return Returns <b>SMP_RESOLVE_RPA_RESULT_YES</b> if the operation is successful;
 *                                           returns others if the operation fails.
 */
int SMP_ResolveRPA(const uint8_t *addr, const uint8_t *irk);

/**
 * @brief Resolve resolvable private address.
 *
 * @param addr Resolvable private address.
 * @param irk Saved irk.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_AsyncResolveRPA(const uint8_t *addr, const uint8_t *irk);

/**
 * @brief Generate resolvable private address.
 *
 * @param irk Local irk.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_GenerateRPA(const uint8_t *irk);

/**
 * @brief Set Secure Connection Only mode.
 *
 * @param mode Whether it is Secure Connection Only mode.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_SetSecureConnOnlyMode(bool mode);

/**
 * @brief Send a Security Request to the remote device.
 *
 * @param handle ACL Connection Handle.
 * @param authReq Authentication Requirements Flags.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_SendSecurityRequestToRemote(uint16_t handle, uint8_t authReq);

/**
 * @brief Generate signature.
 *
 * @param csrk Connection Signature Resolving Key.
 * @param counter Signature counter .
 * @param data Data that needs to be signed.
 * @param dataLen Data length.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_GenerateSignature(const uint8_t *csrk, uint32_t counter, const uint8_t *data, uint16_t dataLen);

/**
 * @brief Start Encryption.
 *
 * @param handle ACL Connection Handle.
 * @param random Saved peer random.
 * @param ediv Saved peer ediv.
 * @param key Saved peer long term key.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_StartEncryption(uint16_t handle, const uint8_t *random, uint16_t ediv, const uint8_t *key);

/**
 * @brief Proactively start pairing.
 *
 * @param handle ACL Connection Handle.
 * @param localAddr Local addr.
 * @param peerAddr Peer addr.
 * @param param Paired param.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_StartPair(uint16_t handle, const BtAddr *localAddr, const BtAddr *peerAddr, const SMP_PairParam *param);

/**
 * @brief Reply authentication request.
 *
 * @param handle ACL Connection Handle.
 * @param accept Accept or reject.
 * @param rejectReason Reject Reason.
 * @param pairMethod Paired method.
 * @param entryValue Authentication value.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_AuthenticationRequestReply(
    uint16_t handle, bool accept, uint8_t rejectReason, uint8_t pairMethod, const uint8_t *entryValue);

/**
 * @brief Reply to peer pairing request.
 *
 * @param handle ACL Connection Handle.
 * @param rejectReason Reject Reason.
 * @param localAddr Local addr.
 * @param peerAddr Peer addr.
 * @param param Paired param.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_RemotePairRequestReply(
    uint16_t handle, uint8_t rejectReason, const BtAddr *localAddr, const BtAddr *peerAddr, const SMP_PairParam *param);

/**
 * @brief Reply to peer pairing response.
 *
 * @param handle ACL Connection Handle.
 * @param accept Accept or reject.
 * @param rejectReason Reject Reason.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_RemotePairResponseReply(uint16_t handle, bool accept, uint8_t rejectReason);

/**
 * @brief Reply peer security request.
 *
 * @param handle ACL Connection Handle.
 * @param accept Accept or reject.
 * @param rejectReason Reject reason.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_RemoteSecurityRequestReply(uint16_t handle, bool accept, uint8_t rejectReason);

/**
 * @brief Reply to remote encryption request.
 *
 * @param handle ACL Connection Handle.
 * @param accept Accept or reject.
 * @param key Local long term key.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_LongTermKeyRequestReply(uint16_t handle, bool accept, const uint8_t *key);

/**
 * @brief Proactively cancel pairing.
 *
 * @param handle ACL Connection Handle.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_CancelPair(uint16_t handle);

/**
 * @brief Register callback functions.
 *
 * @param cb Point to <b>SMP_Callback_t</b> struct, the struct must be available before calling to
 *                  <b>SMP_UnregisterCallback</b>.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_RegisterCallback(const SMP_Callback_t *cb);

/**
 * @brief Unregister callback functions.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_UnregisterCallback();

/**
 * @brief Generate out of band data.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int SMP_GenerateScOobData();

#ifdef __cplusplus
}
#endif

#endif