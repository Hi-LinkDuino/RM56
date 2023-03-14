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

#ifndef SMP_DEF_H
#define SMP_DEF_H

#include "btstack.h"
#include "hci/hci.h"
#include "packet.h"
#include "platform/include/alarm.h"
#include "platform/include/event.h"
#include "smp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SMP_CODE_PAIRING_REQ 0x01
#define SMP_CODE_PAIRING_RSP 0x02
#define SMP_CODE_PAIRING_CFM 0x03
#define SMP_CODE_PAIRING_RAND 0x04
#define SMP_CODE_PAIRING_FAIL 0x05
#define SMP_CODE_ENCRYPTION_INFO 0x06
#define SMP_CODE_MASTER_IDENTITY 0x07
#define SMP_CODE_IDENTITY_INFO 0x08
#define SMP_CODE_IDENTITY_ADDR_INFO 0x09
#define SMP_CODE_SIGNING_INFO 0x0A
#define SMP_CODE_SECURITY_REQ 0x0B
#define SMP_CODE_PAIRING_PUBLIC_KEY 0x0C
#define SMP_CODE_PAIRING_DHKEY_CHECK 0x0D
#define SMP_CODE_PAIRING_KEYPRESS_NTY 0x0E

#define SMP_CMD_PAIR_REQ_DATA_LEN 7
#define SMP_CMD_PAIR_RSP_DATA_LEN 7
#define SMP_CMD_PAIR_CFM_DATA_LEN 17
#define SMP_CMD_PAIR_RAND_DATA_LEN 17
#define SMP_CMD_PAIR_FAIL_DATA_LEN 2
#define SMP_CMD_PAIR_PUBLIC_KEY_DATA_LEN 65
#define SMP_CMD_PAIR_DHK_CHECK_DATA_LEN 17
#define SMP_CMD_KEYPRESS_DATA_LEN 2
#define SMP_CMD_ENCRYPTION_INFO_DATA_LEN 17
#define SMP_CMD_MASTER_IDENTITY_DATA_LEN 11
#define SMP_CMD_IDENTITY_INFO_DATA_LEN 17
#define SMP_CMD_IDENTITY_ADDR_INFO_DATA_LEN 8
#define SMP_CMD_SIGNING_INFO_DATA_LEN 17
#define SMP_CMD_SECURITY_REQ_DATA_LEN 2

#define SMP_PUBLICKEY_LEN 0x40
#define SMP_PUBLICKEY_X_LEN 0x20
#define SMP_PUBLICKEY_Y_LEN 0x20
#define SMP_DHKEY_LEN 0x20
#define SMP_TK_LEN 0x10
#define SMP_CONFIRM_DATA_LEN 0x10
#define SMP_RANDOM_DATA_LEN 0x10
#define SMP_STK_LEN 0x10
#define SMP_MACKEY_LEN 0x10
#define SMP_DHKEY_CHECK_LEN 0x10
#define SMP_SC_OOB_LEN 0x26
#define SMP_LEGACY_OOB_LEN 0x10
#define SMP_MASTER_RAND_LEN 0x08
#define SMP_SIGNCOUNTER_LEN 0x04
#define SMP_SIGNATURE_LEN 0x0C
#define SMP_SIGN_DATA_MAX_LEN 0x0200
#define SMP_AES_CMAC_OUTPUT_LEN 0x10
#define SMP_ENCRYPT_KEY_LEN 0x10
#define SMP_ENCRYPT_PLAINTEXTDATA_LEN 0x10

#define SMP_ROLE_MASTER 0x00
#define SMP_ROLE_SLAVE 0x01

#define SMP_STATE_IDLE 0x00
#define SMP_STATE_PAIRING 0x01
#define SMP_STATE_SC_OOB_DATA_GENERATING 0x02
#define SMP_STATE_SC_OOB_DATA_GENERATED 0x03

#define SMP_GENERATE_SIGNATURE_STEP_1 0x0141
#define SMP_GENERATE_SIGNATURE_STEP_2 0x0142
#define SMP_GENERATE_SIGNATURE_STEP_3 0x0143

#define SMP_GENERATE_RPA_STEP_1 0x0151
#define SMP_RPA_HIGH_BIT_LEN 0x03

#define SMP_RESOLVE_RPA_STEP_1 0x0161

#define SMP_GENERATE_SC_OOB_DATA_STEP_1 0x0171
#define SMP_GENERATE_SC_OOB_DATA_STEP_2 0x0172
#define SMP_GENERATE_SC_OOB_DATA_STEP_3 0x0173
#define SMP_GENERATE_SC_OOB_DATA_STEP_4 0x0174

#define SMP_PAIR_WAIT_TIME 30000
#define SMP_RESO_RPA_WAIT_TIME 30000
#define SMP_GEN_SC_OOB_DATA_WAIT_TIME 30000

#define SMP_KEY_DIST_CMD_FLAG_BIT_ENC_INFO 0x01
#define SMP_KEY_DIST_CMD_FLAG_BIT_MASTER_IDENT 0x02
#define SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_INFO 0x04
#define SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_ADDR 0x08
#define SMP_KEY_DIST_CMD_FLAG_BIT_SIGN_INFO 0x10

#define SMP_ENC_KEY_SIZE_MIN 0x07
#define SMP_ENC_KEY_SIZE_MAX 0x10

#define SMP_SLAVE_CONFIRM_RECV_FLAG_NO 0x00
#define SMP_SLAVE_CONFIRM_RECV_FLAG_YES 0x01

#define SMP_SLAVE_PUBLICKEY_RECV_FLAG_NO 0x00
#define SMP_SLAVE_PUBLICKEY_RECV_FLAG_YES 0x01

#define SMP_SLAVE_DHKEYCHECK_RECV_FLAG_NO 0x00
#define SMP_SLAVE_DHKEYCHECK_RECV_FLAG_YES 0x01

#define SMP_MASTER_ENCRYPTED_FLAG_NO 0x00
#define SMP_MASTER_ENCRYPTED_FLAG_YES 0x01

#define SMP_MAX_PASSKEY_VALUE 0x000F423F  // 999999
#define SMP_MAX_NUMERIC_VALUE 0x000F423F  // 999999
#define SMP_PASSKEY_LEN 0x04

#define SMP_ENTRY_VALUE_LEN 0x26

#define SMP_INIT_WAIT_TIME (-1)

typedef struct {
    BtAddr addr;
    SMP_PairParam pairParam;
    uint8_t pairMethod;
    uint8_t confirm[SMP_CONFIRM_DATA_LEN];
    uint8_t random[SMP_RANDOM_DATA_LEN];
    uint8_t keyDist;
    uint8_t keyDistCmdFlag;
    uint8_t LTK[SMP_LTK_LEN];
    uint16_t masterIdEdiv;
    uint8_t masterIdRand[SMP_MASTER_RAND_LEN];
    uint8_t IRK[SMP_IRK_LEN];
    BtAddr identityAddr;
    uint8_t CSRK[SMP_CSRK_LEN];
    uint8_t publicKey[SMP_PUBLICKEY_LEN];
    uint8_t DHKeyCheck[SMP_DHKEY_CHECK_LEN];
    uint8_t oobRand[SMP_RANDOM_DATA_LEN];
} SMP_PairProcessData;

typedef struct {
    uint16_t handle;
    uint8_t role;
    uint8_t state;
    uint16_t step;
    Alarm *alarm;
    uint8_t encKeySize;
    uint8_t TK[SMP_TK_LEN];
    uint8_t STK[SMP_STK_LEN];
    uint8_t pairType;
    uint8_t slaveConfirmRecvFlag;
    uint8_t slavePubKeyRecvFlag;
    uint8_t slaveDHKeyCheckRecvFlag;
    uint8_t masterEncryptedFlag;
    uint8_t DHKey[SMP_DHKEY_LEN];
    uint8_t macKey[SMP_MACKEY_LEN];
    uint8_t scConfirmCheckCounter;
    SMP_PairProcessData local;
    SMP_PairProcessData peer;
} SMP_PairMng;

typedef struct {
    uint16_t step;
    Alarm *timeoutTimer;
    uint8_t key[16];
    uint8_t X[16];
    uint8_t Y[16];
    uint8_t *M;
    uint16_t length;
    uint8_t M_last[16];
    int n;
    int i;
    uint16_t signDataLen;
    uint32_t signCounter;
    uint8_t address[6];
} SMP_EncCmd;

typedef struct {
    SMP_EncCmd *encCmd;
} SMP_EncTimeoutParam;

typedef struct {
    void *data;
} SMP_StepParam;

typedef struct {
    const HciLeEncryptReturnParam *encRetParam;
    SMP_EncCmd *encCmd;
} SMP_EncData;

typedef struct {
    uint8_t state;
    uint16_t step;
    Alarm *alarm;
    uint8_t confirm[SMP_CONFIRM_DATA_LEN];
    uint8_t random[SMP_RANDOM_DATA_LEN];
    uint8_t publicKey[SMP_PUBLICKEY_LEN];
} SMP_ScOobMng;

typedef struct {
    uint8_t irk[SMP_IRK_LEN];
} SMP_SetIrkTask_t;

typedef struct {
    BtAddr addr;
} SMP_SetIdentAddrTask_t;

typedef struct {
    uint8_t addr[BT_ADDRESS_SIZE];
    uint8_t irk[SMP_IRK_LEN];
} SMP_ResoRpaTask_t;

typedef struct {
    uint8_t addr[BT_ADDRESS_SIZE];
    uint8_t irk[SMP_IRK_LEN];
} SMP_AsyncResoRpaTask_t;

typedef struct {
    uint8_t irk[SMP_IRK_LEN];
} SMP_GenRpaTask_t;

typedef struct {
    bool mode;
} SMP_SetSecConnOnlyModeTask_t;

typedef struct {
    uint16_t handle;
    uint8_t authReq;
} SMP_SendSecReqToRemoteTask_t;

typedef struct {
    uint8_t csrk[SMP_CSRK_LEN];
    uint32_t counter;
    uint8_t *data;
    uint16_t dataLen;
} SMP_GenSignTask_t;

typedef struct {
    uint16_t handle;
    uint8_t random[SMP_MASTER_RAND_LEN];
    uint16_t ediv;
    uint8_t key[SMP_LTK_LEN];
} SMP_StartEncTask_t;

typedef struct {
    uint16_t handle;
    BtAddr localAddr;
    BtAddr peerAddr;
    SMP_PairParam param;
} SMP_StartPairTask_t;

typedef struct {
    uint16_t handle;
    bool accept;
    uint8_t rejectReason;
    uint8_t pairMethod;
    uint8_t entryValue[SMP_ENTRY_VALUE_LEN];
} SMP_AuthReqReplyTask_t;

typedef struct {
    uint16_t handle;
    bool accept;
    uint8_t rejectReason;
    BtAddr localAddr;
    BtAddr peerAddr;
    SMP_PairParam param;
} SMP_RemotePairReqReplyTask_t;

typedef struct {
    uint16_t handle;
    bool accept;
    uint8_t rejectReason;
} SMP_RemotePairRspReplyTask_t;

typedef struct {
    uint16_t handle;
    bool accept;
    uint8_t rejectReason;
} SMP_RemoteSecReqReplyTask_t;

typedef struct {
    uint16_t handle;
    bool accept;
    uint8_t key[SMP_LTK_LEN];
} SMP_LongTermKeyReqReplyTask_t;

typedef struct {
    uint16_t handle;
} SMP_CancelPairTask_t;

typedef struct {
    SMP_Callback_t cb;
} SMP_RegCbTask_t;

typedef struct {
    uint16_t handle;
    Packet *pkt;
} SMP_RecvDataTask_t;

typedef struct {
    uint16_t handle;
} SMP_DisconnectedTask_t;

typedef struct {
    uint16_t aclHandle;
    int result;
} SMP_SendL2capDataCbTask_t;

typedef void (*SMP_StepFunc)(const SMP_StepParam *param);

#ifdef __cplusplus
}
#endif

#endif