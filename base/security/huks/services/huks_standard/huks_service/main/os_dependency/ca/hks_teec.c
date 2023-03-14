/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "hks_teec.h"

#include <unistd.h>

#include "hks_log.h"
#include "hks_mem.h"
#include "tee_client_api.h"
#include "tee_client_id.h"
#include "tee_client_type.h"

#ifdef HKS_ROUTER
#define HISI_HKS_TA_PATH "/lib/sec/86310d18-5659-47c9-b212-841a3ca4f814.sec"
#else
#define HISI_HKS_TA_PATH "/vendor/bin/86310d18-5659-47c9-b212-841a3ca4f814.sec"
#endif

#define MAX_TEE_PARAMS_NUMS 4
#define PROVISION_PARAM_COUNT 4

#define TEE_SERVICE_HKS_TA \
{ \
    0x86310d18, \
    0x5659, \
    0x47c9, \
    { \
        0xb2, 0x12, 0x84, 0x1a, 0x3c, 0xa4, 0xf8, 0x14 \
    } \
}

typedef struct {
    enum TEEC_ParamType paramType;
    uint32_t index;
    union {
        TEEC_TempMemoryReference tmpRef;
        TEEC_Value value;
    };
} TeecOpParam;

typedef struct {
    TeecOpParam *params;
    uint32_t length;
} TeecOpParamSet;

typedef struct {
    uint32_t paramTypes;
    TeecOpParamSet paramSet;
} TeecOperation;

static TEEC_Context *g_context = NULL;
static TEEC_Session *g_sessionSelfStart = NULL;

static inline void InitializeBlob(struct HksBlob *blob, uint32_t size, uint8_t *data)
{
    if (blob != NULL) {
        blob->data = data;
        blob->size = size;
    }
}

static inline uint64_t Uint32To64(uint32_t high, uint32_t low)
{
    return ((uint64_t)(high) << 32) + (uint64_t)(low); /* 32 is higher bits */
}

static TEEC_Result TeecUuidInit(TEEC_Context *context, const char *taPath,
    TEEC_UUID *uuid, const char *taName, uint32_t srcLen)
{
    if (!access(taPath, F_OK)) {
        context->ta_path = (uint8_t *)taPath;
        if (memcpy_s(uuid, sizeof(TEEC_UUID), taName, srcLen) != EOK) {
            HKS_LOG_E("memcpy failed while copy ta name");
            return TEEC_ERROR_GENERIC;
        }

        HKS_LOG_I("run on ta path:%s ", taPath);
        return TEEC_SUCCESS;
    }

    return TEEC_ERROR_GENERIC;
}

static TEEC_Result OpenSession(TEEC_Context *context, TEEC_Session **session)
{
    TEEC_UUID uuid = { 0, 0, 0, { 0 } };
    TEEC_UUID uuidHisi = TEE_SERVICE_HKS_TA;
    TEEC_Result ret = TeecUuidInit(context, HISI_HKS_TA_PATH, &uuid, (const char*)&uuidHisi, sizeof(TEEC_UUID));
    if (ret != TEEC_SUCCESS) {
        HKS_LOG_E("ta uuid init failed!");
        return TEEC_ERROR_GENERIC;
    }

    TEEC_Session *localSession = (TEEC_Session *)HksMalloc(sizeof(TEEC_Session));
    if (localSession == NULL) {
        HKS_LOG_E("out of memory!");
        return (TEEC_Result)TEEC_ERROR_OUT_OF_MEMORY;
    }

    TEEC_Operation operation;

    (void)memset_s(&operation, sizeof(operation), 0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE,
        TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);

    uint32_t origin = 0;
    ret = TEEC_OpenSession(context, localSession, &uuid, TEEC_LOGIN_IDENTIFY, NULL, &operation, &origin);
    if (ret != TEEC_SUCCESS) {
        HksFree(localSession);
        HKS_LOG_E("open ta session failed, ret=0x%x, origin=%u", ret, origin);
        return ret;
    }

    *session = localSession;
    HKS_LOG_I("Open Session success");
    return ret;
}

static TEEC_Result TeecOpen(void)
{
    if (g_context != NULL) {
        return TEEC_SUCCESS;
    }

    g_context = HksMalloc(sizeof(TEEC_Context));
    if (g_context == NULL) {
        HKS_LOG_E("memory allocate failed!");
        return TEEC_ERROR_OUT_OF_MEMORY;
    }

    TEEC_Result result = TEEC_InitializeContext(NULL, g_context);
    if (result != TEEC_SUCCESS) {
        HKS_LOG_E("Initialize TEE context failed, ret=0x%x", result);
        HKS_FREE_PTR(g_context);
        return result;
    }

    result = OpenSession(g_context, &g_sessionSelfStart);
    if (result != TEEC_SUCCESS) {
        TEEC_FinalizeContext(g_context);
        HKS_FREE_PTR(g_context);
        HKS_LOG_E("Open Session failed!");
    }

    return result;
}

static void FillUpCommand(const TeecOpParam *src, TEEC_Parameter *des)
{
    switch (src->paramType) {
        case TEEC_MEMREF_TEMP_INPUT:
        case TEEC_MEMREF_TEMP_OUTPUT:
        case TEEC_MEMREF_TEMP_INOUT:
            des->tmpref.buffer = src->tmpRef.buffer;
            des->tmpref.size = src->tmpRef.size;
            break;
        case TEEC_VALUE_INPUT:
        case TEEC_VALUE_OUTPUT:
        case TEEC_VALUE_INOUT:
            des->value.a = src->value.a;
            des->value.b = src->value.b;
            break;
        case TEEC_NONE:
            break;
        default:
            break;
    }
}

static TEEC_Result TeecRequestCmdInner(enum HksCmdId pkiCmdId, TEEC_Operation *operation,
    TeecOperation *teecOperation)
{
    TEEC_Result ret;
    if (g_sessionSelfStart == NULL) {
        ret = TeecOpen();
        if (ret != TEEC_SUCCESS) {
            HKS_LOG_E("teec open failed!");
            return ret;
        }
    }

    if (memset_s(operation, sizeof(TEEC_Operation), 0, sizeof(TEEC_Operation)) != EOK) {
        HKS_LOG_E("memset for operation failed!");
        return TEEC_ERROR_GENERIC;
    }

    operation->started = 1;
    operation->paramTypes = teecOperation->paramTypes;

    TeecOpParamSet paramSet = teecOperation->paramSet;
    TeecOpParam *params = paramSet.params;
    for (uint32_t i = 0; i < paramSet.length; i++) {
        FillUpCommand(&params[i], &operation->params[params[i].index]);
    }

    uint32_t retOrigin = 0;
    ret = TEEC_InvokeCommand(g_sessionSelfStart, pkiCmdId, operation, &retOrigin);
    if (ret != TEEC_SUCCESS) {
        HKS_LOG_E("invoke km command failed, cmd = %u, ret = 0x%x, retOrigin = %u", pkiCmdId, ret, retOrigin);
    }

    return ret;
}

static void FillUpArgs(const struct HksParam *src, TeecOpParam *dest)
{
    switch (dest->paramType) {
        case TEEC_MEMREF_TEMP_INPUT:
        case TEEC_MEMREF_TEMP_OUTPUT:
        case TEEC_MEMREF_TEMP_INOUT:
            dest->tmpRef.buffer = src->blob.data;
            dest->tmpRef.size = src->blob.size;
            break;
        case TEEC_VALUE_INPUT:
        case TEEC_VALUE_OUTPUT:
        case TEEC_VALUE_INOUT:
            dest->value.a = (uint32_t)((src->uint64Param) >> 32); /* 32 is higher bits */
            dest->value.b = (uint32_t)(src->uint64Param);
            break;
        case TEEC_NONE:
            break;
        default:
            break;
    }
}

static int32_t HksTeeCommand(uint32_t paramTypes, const struct HksParam params[MAX_TEE_PARAMS_NUMS],
    uint32_t msgType, TEEC_Operation *operation)
{
    TeecOpParam opParams[MAX_TEE_PARAMS_NUMS];
    for (uint32_t i = 0; i < MAX_TEE_PARAMS_NUMS; ++i) {
        opParams[i].index = i;
        opParams[i].paramType = TEEC_PARAM_TYPE_GET(paramTypes, i);
        FillUpArgs(&params[i], &opParams[i]);
    }

    TeecOpParamSet teeParams = {
        .params = opParams,
        .length = MAX_TEE_PARAMS_NUMS
    };
    TeecOperation teecOperation = {
        .paramTypes = paramTypes,
        .paramSet = teeParams
    };

    return TeecRequestCmdInner(msgType, operation, &teecOperation);
}

int32_t HksTeeOpen(void)
{
    return TeecOpen();
}

#ifdef HKS_SUPPORT_API_INJECT_KEY
int32_t HksTeeProvision(const struct HksBlob *keybox, struct HksBlob *challenge,
    const struct HksBlob *challengeIn, struct HksBlob *signature, struct HksBlob *certData)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT, TEEC_MEMREF_TEMP_INOUT,
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, keybox->size, keybox->data);
    InitializeBlob(&params[1].blob, challenge->size, challenge->data);
    InitializeBlob(&params[2].blob, signature->size, signature->data); /* 2 is array index */
    InitializeBlob(&params[3].blob, certData->size, certData->data); /* 3 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_INJECT_KEY, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("command key provision failed");
        return ret;
    }

    ret = ProvisionVerify(&operation, challengeIn, (PROVISION_PARAM_COUNT >> 1), NULL, false);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("command HksTeeVerifyKeybox failed\n");
    }

    return ret;
}
#endif

#ifdef HKS_SUPPORT_API_INJECT_KEY_VERIFY
int32_t HksTeeProvisionVerify(const struct HksBlob *verify, struct HksBlob *challenge,
    const struct HksBlob *challengeIn, struct HksBlob *signature)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_INOUT,
        TEEC_NONE, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, signature->size, signature->data);
    InitializeBlob(&params[1].blob, challenge->size, challenge->data);

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_INJECT_KEY_VERIFY, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("command key provision verify failed");
        return ret;
    }

    /* here we only have one key signature to verify, so the count is half of the provision param count */
    ret = ProvisionVerify(&operation, challengeIn, (PROVISION_PARAM_COUNT >> 1), verify, true);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("verify provision failed");
    }

    return ret;
}
#endif

int32_t HksTeeGenerateKey(const struct HksBlob *keyBlob, const struct HksParamSet *paramSetIn, struct HksBlob *keyOut)
{
    uint8_t keyInBuffer[MAX_KEY_SIZE] = {0};
    struct HksBlob keyInBlob = { MAX_KEY_SIZE, keyInBuffer };
    const struct HksBlob *keyIn = &keyInBlob;
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, keyBlob->size, keyBlob->data);
    InitializeBlob(&params[1].blob, paramSetIn->paramSetSize, (uint8_t *)paramSetIn);
    InitializeBlob(&params[2].blob, keyIn->size, keyIn->data); /* 2 is array index */
    InitializeBlob(&params[3].blob, keyOut->size, keyOut->data); /* 3 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_GENERATE_KEY, &operation);
    if (ret == HKS_SUCCESS) {
        keyOut->size = operation.params[3].tmpref.size; /* 3 is array index */
    }

    return ret;
}

int32_t HksTeeCheckKeyLegality(const struct HksParamSet *paramSet, const struct HksBlob *key)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[1].blob, key->size, key->data);

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_CHECK_KEY_LEGALITY, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("command HksTeeCheckKeyLegality failed");
    }

    return ret;
}

int32_t HksTeeGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[1].blob, random->size, random->data);

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_GENERATE_RANDOM, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("command HksTeeGenerateRandom failed");
    }

    return ret;
}

int32_t HksTeeImportKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, key->size, key->data);
    InitializeBlob(&params[1].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[2].blob, keyOut->size, keyOut->data); /* 2 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_IMPORT_KEY, &operation);
    if (ret == HKS_SUCCESS) {
        keyOut->size = operation.params[2].tmpref.size; /* 2 is array index */
    }

    return ret;
}

int32_t HksTeeExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *keyOut)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, key->size, key->data);
    InitializeBlob(&params[1].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[2].blob, keyOut->size, keyOut->data); /* 2 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_EXPORT_KEY, &operation);
    if (ret == HKS_SUCCESS) {
        keyOut->size = operation.params[2].tmpref.size; /* 2 is array index */
    }

    return ret;
}

int32_t HksTeeAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[1].blob, privateKey->size, privateKey->data);
    InitializeBlob(&params[2].blob, peerPublicKey->size, peerPublicKey->data); /* 2 is array index */
    InitializeBlob(&params[3].blob, agreedKey->size, agreedKey->data); /* 3 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_AGREE_KEY, &operation);
    if (ret == HKS_SUCCESS) {
        agreedKey->size = operation.params[3].tmpref.size; /* 3 is array index */
    }

    return ret;
}

int32_t HksTeeDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey, struct HksBlob *derivedKey)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[1].blob, kdfKey->size, kdfKey->data);
    InitializeBlob(&params[2].blob, derivedKey->size, derivedKey->data); /* 2 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_DERIVE_KEY, &operation);
    if (ret == HKS_SUCCESS) {
        derivedKey->size = operation.params[2].tmpref.size; /* 2 is array index */
    }

    return ret;
}

int32_t HksTeeWrapKey(const struct HksBlob *wrapKey, const struct HksBlob *wrappedKey,
    const struct HksParamSet *paramSet, struct HksBlob *wrappedData)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, wrapKey->size, wrapKey->data);
    InitializeBlob(&params[1].blob, wrappedKey->size, wrappedKey->data);
    InitializeBlob(&params[2].blob, paramSet->paramSetSize, (uint8_t *)paramSet); /* 2 is array index */
    InitializeBlob(&params[3].blob, wrappedData->size, wrappedData->data); /* 3 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_WRAP, &operation);
    if (ret == HKS_SUCCESS) {
        wrappedData->size = operation.params[3].tmpref.size; /* 3 is array index */
    }

    return ret;
}

int32_t HksTeeUnwrapKey(const struct HksBlob *wrapKey, const struct HksBlob *wrappedData,
    const struct HksParamSet *paramSet, struct HksBlob *output)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, wrapKey->size, wrapKey->data);
    InitializeBlob(&params[1].blob, wrappedData->size, wrappedData->data);
    InitializeBlob(&params[2].blob, paramSet->paramSetSize, (uint8_t *)paramSet); /* 2 is array index */
    InitializeBlob(&params[3].blob, output->size, output->data); /* 3 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_UNWRAP, &operation);
    if (ret == HKS_SUCCESS) {
        output->size = operation.params[3].tmpref.size; /* 3 is array index */
    }

    return ret;
}

int32_t HksTeeAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *certChain)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, key->size, key->data);
    InitializeBlob(&params[1].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[2].blob, certChain->size, certChain->data); /* 2 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_ATTEST_KEY, &operation);
    if (ret == HKS_SUCCESS) {
        certChain->size = operation.params[2].tmpref.size; /* 2 is array index */
    }

    return ret;
}

static bool IsVerify(uint32_t cmdId)
{
    return (cmdId == HKS_CMD_ID_VERIFY || cmdId == HKS_CMD_ID_VERIFY_FINAL);
}

int32_t ProcessInit(uint32_t cmdId, const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    uint64_t *operationHandle)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_VALUE_OUTPUT, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, keyBlob->size, keyBlob->data);
    InitializeBlob(&params[1].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    params[2].uint64Param = 0; /* 2 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, cmdId, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("ProcessInit[%u] failed", cmdId);
        return ret;
    }

    *operationHandle = Uint32To64(operation.params[2].value.a, operation.params[2].value.b); /* 2 is array index */
    return ret;
}

int32_t ProcessUpdate(uint32_t cmdId, uint64_t operationHandle,
    const struct HksBlob *inData, const struct HksBlob *outData, bool isOutput)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
        (isOutput ? TEEC_MEMREF_TEMP_OUTPUT : TEEC_NONE), TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    params[0].uint64Param = operationHandle;
    InitializeBlob(&params[1].blob, inData->size, inData->data);

    if (isOutput) {
        InitializeBlob(&params[2].blob, outData->size, outData->data); /* 2 is array index */
    } else {
        InitializeBlob(&params[2].blob, 0, NULL); /* 2 is array index */
    }

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, cmdId, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("ProcessUpdate[%u] failed", cmdId);
    }

    return ret;
}

int32_t ProcessFinal(uint32_t cmdId, uint64_t operationHandle,
    const struct HksBlob *srcData, struct HksBlob *inOut, bool isInput)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
        isInput ? TEEC_MEMREF_TEMP_INPUT : TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    params[0].uint64Param = operationHandle;
    InitializeBlob(&params[1].blob, srcData->size, srcData->data);
    InitializeBlob(&params[2].blob, inOut->size, inOut->data); /* 2 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, cmdId, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("ProcessFinal[%u] failed", cmdId);
        return ret;
    }

    if (!isInput) {
        inOut->size = operation.params[2].tmpref.size; /* 2 is array index */
    }
    return ret;
}

int32_t ProcessOnce(uint32_t cmdId, const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *inOut)
{
    bool isInput = IsVerify(cmdId);
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
        isInput ? TEEC_MEMREF_TEMP_INPUT : TEEC_MEMREF_TEMP_OUTPUT);

    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, keyBlob->size, keyBlob->data);
    InitializeBlob(&params[1].blob, paramSet->paramSetSize, (uint8_t *)paramSet);
    InitializeBlob(&params[2].blob, srcData->size, srcData->data); /* 2 is array index */
    InitializeBlob(&params[3].blob, inOut->size, inOut->data); /* 3 is array index */

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, cmdId, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("ProcessOnce[%u] failed", cmdId);
        return ret;
    }

    if (!isInput) {
        inOut->size = operation.params[3].tmpref.size; /* 3 is array index */
    }

    return ret;
}

int32_t HksTeeProcessInit(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    uint64_t *operationHandle)
{
    return ProcessInit(cmdId, key, paramSet, operationHandle);
}

int32_t HksTeeProcessMultiUpdate(uint32_t cmdId, uint64_t operationHandle, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    bool isOutput = (cmdId == HKS_CMD_ID_ENCRYPT_UPDATE) || (cmdId == HKS_CMD_ID_DECRYPT_UPDATE);
    return ProcessUpdate(cmdId, operationHandle, inData, outData, isOutput);
}

int32_t HksTeeSign(const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    return ProcessOnce(HKS_CMD_ID_SIGN, keyBlob, paramSet, srcData, signature);
}

int32_t HksTeeVerify(const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    return ProcessOnce(HKS_CMD_ID_VERIFY, keyBlob, paramSet, srcData, (struct HksBlob *)signature);
}

int32_t HksTeeMac(const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    return ProcessOnce(HKS_CMD_ID_MAC, keyBlob, paramSet, srcData, mac);
}

int32_t HksTeeEncrypt(const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    return ProcessOnce(HKS_CMD_ID_ENCRYPT, keyBlob, paramSet, plainText, cipherText);
}

int32_t HksTeeDecrypt(const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    return ProcessOnce(HKS_CMD_ID_DECRYPT, keyBlob, paramSet, cipherText, plainText);
}

int32_t HksTeeExportTrustCerts(struct HksBlob *certChain)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, certChain->size, certChain->data);

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_EXPORT_TRUST_CERT, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Invoke HKS_CMD_ID_EXPORT_TRUST_CERT failed");
        return ret;
    }

    certChain->size = operation.params[0].tmpref.size;
    return ret;
}

int32_t HksTeeImportTrustCerts(const struct HksBlob *certChain)
{
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS];
    InitializeBlob(&params[0].blob, certChain->size, certChain->data);

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HKS_CMD_ID_IMPORT_TRUST_CERT, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Invoke HKS_CMD_ID_IMPORT_TRUST_CERT failed");
        return ret;
    }

    return ret;
}

int32_t HcmTeeIsDeviceKeyExist(void)
{
    HKS_LOG_D("enter");
    uint32_t paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    struct HksParam params[MAX_TEE_PARAMS_NUMS] = {0};

    TEEC_Operation operation;
    int32_t ret = HksTeeCommand(paramTypes, params, HCM_CMD_ID_IS_DEVICE_KEY_EXIST, &operation);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Invoke HCM_CMD_ID_IS_DEVICE_KEY_EXIST failed");
    }
    return ret;
}