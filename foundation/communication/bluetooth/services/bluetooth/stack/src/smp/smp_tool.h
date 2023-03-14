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

#ifndef SMP_TOOL_H
#define SMP_TOOL_H

#include <stdint.h>

#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IO_COUNTS 5

#define CRYPT_AESCMAC_Z_LEN 16
#define CRYPT_AESCMAC_TMP_LEN 16
#define CRYPT_AESCMAC_X_LEN 16
#define CRYPT_AESCMAC_Y_LEN 16
#define CRYPT_AESCMAC_PADDED_LEN 16
#define CRYPT_AESCMAC_KEY_LEN 16
#define CRYPT_AESCMAC_IN_LEN 16
#define CRYPT_AESCMAC_OUT_LEN 16
#define CRYPT_AESCMAC_NUM 15
#define CRYPT_AESCMAC_CONST_RB_LEN 16
typedef struct {
    uint8_t input[CRYPT_AESCMAC_IN_LEN];
    uint8_t output1[CRYPT_AESCMAC_OUT_LEN];
    uint8_t output2[CRYPT_AESCMAC_OUT_LEN];
} SMP_CryptAesCmacStep1Param;

typedef struct {
    uint8_t input1[CRYPT_AESCMAC_IN_LEN];
    uint8_t input2[CRYPT_AESCMAC_IN_LEN];
    const uint8_t *input3;
    int length;
    int n;
    uint8_t output[CRYPT_AESCMAC_OUT_LEN];
} SMP_CryptAesCmacStep2Param;

typedef struct {
    uint16_t stepA;
    uint16_t stepB;
    const uint8_t *message;
    uint8_t messageSize;
    uint8_t aesCmacOutput[CRYPT_AESCMAC_OUT_LEN];
    uint8_t key[CRYPT_AESCMAC_KEY_LEN];
    int n;
    uint32_t signCounter;
    bool isUsingHwAes128;
} SMP_CryptAesCmacStep3Param;

typedef struct {
    uint16_t stepA;
    uint16_t stepB;
    SMP_EncCmd *pEncCmdData;
    uint8_t X[CRYPT_AESCMAC_X_LEN];
    bool isUsingHwAes128;
} SMP_CryptAesCmacStep4Param;

#define CRYPT_C1_P1_LEN 16
#define CRYPT_C1_P2_LEN 16
#define CRYPT_C1_PADDING_LEN 4
#define CRYPT_C1_IA_LEN 6
#define CRYPT_C1_RA_LEN 6
#define CRYPT_C1_IN_LEN 16
#define CRYPT_C1_OUT_LEN 16
typedef struct {
    uint8_t r[SMP_RANDOM_DATA_LEN];
    uint8_t preq[SMP_CMD_PAIR_REQ_DATA_LEN];
    uint8_t pres[SMP_CMD_PAIR_RSP_DATA_LEN];
    uint8_t iat;
    uint8_t rat;
    uint8_t output[CRYPT_C1_OUT_LEN];
} SMP_CryptC1Step1Param;

typedef struct {
    uint8_t ia[CRYPT_C1_IA_LEN];
    uint8_t ra[CRYPT_C1_RA_LEN];
    uint8_t input[CRYPT_C1_IN_LEN];
    uint8_t output[CRYPT_C1_OUT_LEN];
} SMP_CryptC1Step2Param;

#define CRYPT_F4_OUT_LEN 65
typedef struct {
    uint8_t U[SMP_PUBLICKEY_X_LEN];
    uint8_t V[SMP_PUBLICKEY_X_LEN];
    uint8_t Z[1];
    uint8_t output[CRYPT_F4_OUT_LEN];
} SMP_CryptF4Param;

#define CRYPT_F5_KEYID_LEN 4
#define CRYPT_F5_LENGTH_LEN 2
#define CRYPT_F5_A1_LEN 7
#define CRYPT_F5_A2_LEN 7
#define CRYPT_F5_OUT_LEN 53
typedef struct {
    uint8_t N1[SMP_RANDOM_DATA_LEN];
    uint8_t N2[SMP_RANDOM_DATA_LEN];
    uint8_t A1[CRYPT_F5_A1_LEN];
    uint8_t A2[CRYPT_F5_A2_LEN];
    uint8_t output[CRYPT_F5_OUT_LEN];
} SMP_CryptF5Param;

#define CRYPT_F6_A1_LEN 7
#define CRYPT_F6_A2_LEN 7
#define CRYPT_F6_IO_LEN 3
#define CRYPT_F6_OUT_LEN 65
typedef struct {
    uint8_t N1[SMP_RANDOM_DATA_LEN];
    uint8_t N2[SMP_RANDOM_DATA_LEN];
    uint8_t R[SMP_RANDOM_DATA_LEN];
    uint8_t IOcap[CRYPT_F6_IO_LEN];
    uint8_t A1[CRYPT_F6_A1_LEN];
    uint8_t A2[CRYPT_F6_A2_LEN];
    uint8_t output[CRYPT_F6_OUT_LEN];
} SMP_CryptF6Param;

#define CRYPT_G2_OUT_LEN 80
typedef struct {
    uint8_t U[SMP_PUBLICKEY_X_LEN];
    uint8_t V[SMP_PUBLICKEY_X_LEN];
    uint8_t Y[SMP_RANDOM_DATA_LEN];
    uint8_t output[CRYPT_G2_OUT_LEN];
} SMP_CryptG2Param;

#define CRYPT_S1_OUT_LEN 16
typedef struct {
    uint8_t r1[SMP_RANDOM_DATA_LEN];
    uint8_t r2[SMP_RANDOM_DATA_LEN];
    uint8_t output[CRYPT_S1_OUT_LEN];
} SMP_CryptS1Param;

void SMP_ConstituteC1Step1Param(uint8_t role, const uint8_t *random, uint8_t randomLen, SMP_CryptC1Step1Param *param);
int SMP_CryptographicC1Step1(uint16_t step, SMP_CryptC1Step1Param *param);
int SMP_ConstituteC1Step2Param(const SMP_EncData *encData, uint8_t role, SMP_CryptC1Step2Param *param);
int SMP_CryptographicC1Step2(uint16_t step, SMP_CryptC1Step2Param *param);
void SMP_CryptographicS1(SMP_CryptS1Param *param);
void SMP_ConstituteF4Param(bool isPasskey, SMP_CryptF4Param *param);
void SMP_CryptographicF4(SMP_CryptF4Param *param);
void SMP_ConstituteF5Param(uint8_t role, SMP_CryptF5Param *cryptF5Param);
void SMP_CryptographicF5(SMP_CryptF5Param *param);
void SMP_ConstituteF6Param(bool isCalculatePeer, SMP_CryptF6Param *cryptF6Param);
void SMP_CryptographicF6(SMP_CryptF6Param *param);
void SMP_CryptographicG2(SMP_CryptG2Param *param);
void SMP_CryptographicAesCmacStep1(SMP_CryptAesCmacStep1Param *param);
void SMP_CryptographicAesCmacStep2(SMP_CryptAesCmacStep2Param *param);
void SMP_ConstituteAesCmacStep3Param(
    const SMP_EncData *encData, const uint8_t *data, uint8_t dataLen, SMP_CryptAesCmacStep3Param *param);
int SMP_CryptographicAesCmacStep3(const SMP_CryptAesCmacStep3Param *param);
int SMP_ConstituteAesCmacStep4Param(
    const SMP_EncData *encData, uint16_t stepA, uint16_t stepB, uint8_t role, SMP_CryptAesCmacStep4Param *param);
int SMP_CryptographicAesCmacStep4(SMP_CryptAesCmacStep4Param *param);
void SMP_MemoryReverseCopy(uint8_t *dest, const uint8_t *source, uint16_t length);
void SMP_ReverseMemoryOrder(uint8_t *buf, uint16_t length);
void SMP_ClearPairState(SMP_PairMng *mng);
void SMP_CalculatePairType(SMP_PairMng *mng);
void SMP_CalculateEncKeySize(SMP_PairMng *mng);
void SMP_CalculatePairMethod(SMP_PairMng *mng);
void SMP_CalculateKeyDistribution(SMP_PairMng *mng);
int SMP_CheckRemotePairParam(const SMP_PairParam *pairParam, uint8_t *reason);
List *SMP_ListCreate(FreeDataCb cb);
void SMP_ListDelete(List *list);
void SMP_ListAddLast(List *list, void *data);
bool SMP_ListRemoveNode(List *list, void *data);
ListNode *SMP_ListGetFirstNode(const List *list);
void *SMP_ListGetNodeData(const ListNode *node);
bool SMP_IsScOobPair(uint8_t pairMethod);
bool SMP_IsPasskeyEntryPair(uint8_t pairMethod);
bool SMP_IsJustworkOrNumericPair(uint8_t pairMethod);
void SMP_GetPublicKeyX(const uint8_t *pubKey, uint8_t *x, uint8_t length);
void SMP_LongTermKeyCopy(uint8_t *destLtk, const uint8_t *sourceLtk, uint8_t encKeySize);

#ifdef __cplusplus
}
#endif

#endif