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

#include "btm_sco.h"

#include <securec.h>

#include "btstack.h"
#include "hci/hci.h"
#include "hci/hci_error.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "btm.h"
#include "btm_acl.h"
#include "btm_controller.h"
#include "btm_sco_def.h"

#define SCO_CONNECTION_TYPE_SCO 1
#define SCO_CONNECTION_TYPE_ESCO 2

#define SCO_CONNECTION_STATUS_CONNECTING 1
#define SCO_CONNECTION_STATUS_CONNECTED 2
#define SCO_CONNECTION_STATUS_DISCONNECTING 3
#define SCO_CONNECTION_STATUS_DISCONNECTED 0

#define FOREACH_CALLBACKS_START(x)                         \
    MutexLock(g_scoCallbackListLock);                      \
    ListNode *node_ = ListGetFirstNode(g_scoCallbackList); \
    while (node_ != NULL) {                                \
        x = ListGetNodeData(node_);                        \
        if ((x) != NULL) {

#define FOREACH_CALLBACKS_END       \
    }                               \
    node_ = ListGetNextNode(node_); \
    }                               \
    MutexUnlock(g_scoCallbackListLock);

#define STATUS_NONE 0
#define STATUS_INITIALIZED 1

#define IS_INITIALIZED() (g_status == STATUS_INITIALIZED)

#define INVALID_HANDLE 0xFFFF

#define LINEAR_PCM_BIT_POS_BIT_INDEX 2

#define INPUT_SAMPLE_SIZE_16_BIT 16

typedef struct {
    BtAddr addr;
    uint16_t scoHandle;
    uint16_t aclHandle;
    uint8_t type;
    uint8_t status;
} BtmScoConnection;

typedef struct {
    const BtmScoCallbacks *callbacks;
    void *context;
} BtmScoCallbacksBlock;

static List *g_scoList = NULL;
static Mutex *g_scoListLock = NULL;

static List *g_scoCallbackList = NULL;
static Mutex *g_scoCallbackListLock = NULL;

static HciEventCallbacks g_hciEventCallbacks;

static uint8_t g_status = STATUS_NONE;

static BtmScoConnection *BtmAllocScoConnection(const BtAddr *addr, uint16_t aclHandle)
{
    BtmScoConnection *connection = MEM_MALLOC.alloc(sizeof(BtmScoConnection));
    if (connection != NULL) {
        connection->addr = *addr;
        connection->aclHandle = aclHandle;
        connection->scoHandle = INVALID_HANDLE;
    }
    return connection;
}

static void BtmFreeScoConnection(void *connection)
{
    MEM_CALLOC.free(connection);
}

static BtmScoCallbacksBlock *BtmAllocScoCallbacksBlock(const BtmScoCallbacks *callbacks, void *context)
{
    BtmScoCallbacksBlock *block = MEM_MALLOC.alloc(sizeof(BtmScoCallbacksBlock));
    if (block != NULL) {
        block->callbacks = callbacks;
        block->context = context;
    }
    return block;
}

static void BtmFreeScoCallbacksBlock(void *block)
{
    MEM_MALLOC.free(block);
}

void BtmInitSco()
{
    g_scoList = ListCreate(BtmFreeScoConnection);
    g_scoListLock = MutexCreate();

    g_scoCallbackList = ListCreate(BtmFreeScoCallbacksBlock);
    g_scoCallbackListLock = MutexCreate();

    g_status = STATUS_INITIALIZED;
}

void BtmCloseSco()
{
    g_status = STATUS_NONE;

    if (g_scoCallbackList != NULL) {
        ListDelete(g_scoCallbackList);
        g_scoCallbackList = NULL;
    }
    if (g_scoCallbackListLock != NULL) {
        MutexDelete(g_scoCallbackListLock);
        g_scoCallbackListLock = NULL;
    }

    if (g_scoList != NULL) {
        ListDelete(g_scoList);
        g_scoList = NULL;
    }
    if (g_scoListLock != NULL) {
        MutexDelete(g_scoListLock);
        g_scoListLock = NULL;
    }
}

void BtmStartSco()
{
    HCI_RegisterEventCallbacks(&g_hciEventCallbacks);
}

void BtmStopSco()
{
    HCI_DeregisterEventCallbacks(&g_hciEventCallbacks);

    MutexLock(g_scoListLock);
    ListClear(g_scoList);
    MutexUnlock(g_scoListLock);

    MutexLock(g_scoCallbackListLock);
    ListClear(g_scoCallbackList);
    MutexUnlock(g_scoCallbackListLock);
}

int BTM_RegisterScoCallbacks(const BtmScoCallbacks *callbacks, void *context)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmScoCallbacksBlock *block = BtmAllocScoCallbacksBlock(callbacks, context);
    if (block == NULL) {
        return BT_NO_MEMORY;
    }

    MutexLock(g_scoCallbackListLock);
    ListAddLast(g_scoCallbackList, block);
    MutexUnlock(g_scoCallbackListLock);
    return BT_NO_ERROR;
}

int BTM_DeregisterScoCallbacks(const BtmScoCallbacks *callbacks)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmScoCallbacksBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks == callbacks) {
        ListRemoveNode(g_scoCallbackList, block);
        break;
    }
    FOREACH_CALLBACKS_END;
    return BT_NO_ERROR;
}

int BTM_WriteVoiceSetting(uint16_t voiceSetting)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    HciWriteVoiceSettingParam param = {
        .voiceSetting = voiceSetting,
    };
    return HCI_WriteVoiceSetting(&param);
}

static bool IsSameAddress(const BtAddr *addr1, const BtAddr *addr2)
{
    bool isSame = true;
    for (int i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr1->addr[i] != addr2->addr[i]) {
            isSame = false;
        }
    }
    return isSame;
}

static BtmScoConnection *BtmScoFindScoConnectionByAddress(const BtAddr *addr, uint8_t status)
{
    BtmScoConnection *scoConnection = NULL;

    BtmScoConnection *item = NULL;

    ListNode *node = ListGetFirstNode(g_scoList);
    while (node != NULL) {
        item = ListGetNodeData(node);
        if (item != NULL) {
            if (item->status == status && IsSameAddress(&item->addr, addr)) {
                scoConnection = item;
                break;
            }
            item = NULL;
        }
        node = ListGetNextNode(node);
    }

    return scoConnection;
}

static BtmScoConnection *BtmScoFindScoConnectionByScoHandle(uint16_t scoHandle, uint8_t status)
{
    BtmScoConnection *scoConnection = NULL;

    BtmScoConnection *item = NULL;

    ListNode *node = ListGetFirstNode(g_scoList);
    while (node != NULL) {
        item = ListGetNodeData(node);
        if (item != NULL) {
            if (item->status == status && scoHandle == item->scoHandle) {
                scoConnection = item;
                break;
            }
            item = NULL;
        }
        node = ListGetNextNode(node);
    }

    return scoConnection;
}

int BTM_CreateScoConnection(const BtmCreateScoConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t aclHandle = 0xffff;
    int result = BtmGetAclHandleByAddress(&param->addr, &aclHandle);
    if (result != BT_NO_ERROR) {
        return result;
    }

    MutexLock(g_scoListLock);
    if (ListGetSize(g_scoList) >= BTM_MAX_SCO) {
        MutexUnlock(g_scoListLock);
        return BT_CONNECT_NUM_MAX;
    } else {
        BtmScoConnection *scoConnection = BtmAllocScoConnection(&param->addr, aclHandle);
        if (scoConnection != NULL) {
            scoConnection->status = SCO_CONNECTION_STATUS_CONNECTING;

            ListAddLast(g_scoList, scoConnection);
            MutexUnlock(g_scoListLock);
        } else {
            MutexUnlock(g_scoListLock);
            return BT_NO_MEMORY;
        }

        BTM_ExitSniffMode(&param->addr);

        HciSetupSynchronousConnectionParam setupSCOParam = {
            .connectionHandle = aclHandle,
            .transmitBandwidth = param->transmitBandwidth,
            .receiveBandwidth = param->receiveBandwidth,
            .maxLatency = param->maxLatency,
            .voiceSetting = param->voiceSetting,
            .retransmissionEffort = param->retransmissionEffort,
            .packetType = param->packetType,
        };
        result = HCI_SetupSynchronousConnection(&setupSCOParam);
        if (result != BT_NO_ERROR) {
            MutexLock(g_scoListLock);
            ListRemoveNode(g_scoList, scoConnection);
            MutexUnlock(g_scoListLock);
        }
    }

    return result;
}

int BTM_AcceptScoConnectionRequest(const BtmAcceptScoConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t aclHandle = 0xffff;
    int result = BtmGetAclHandleByAddress(&param->addr, &aclHandle);
    if (result != BT_NO_ERROR) {
        return result;
    }

    MutexLock(g_scoListLock);
    if (ListGetSize(g_scoList) >= BTM_MAX_SCO) {
        MutexUnlock(g_scoListLock);
        result = BT_CONNECT_NUM_MAX;
    } else {
        BtmScoConnection *scoConnection = BtmAllocScoConnection(&param->addr, aclHandle);
        if (scoConnection != NULL) {
            scoConnection->status = SCO_CONNECTION_STATUS_CONNECTING;

            ListAddLast(g_scoList, scoConnection);
            MutexUnlock(g_scoListLock);
        } else {
            MutexUnlock(g_scoListLock);
            return BT_NO_MEMORY;
        }

        HciAcceptSynchronousConnectionRequestParam acceptParam = {
            .addr =
                {
                    .raw = {0},
                },
            .transmitBandwidth = param->transmitBandwidth,
            .receiveBandwidth = param->receiveBandwidth,
            .maxLatency = param->maxLatency,
            .voiceSetting = param->voiceSetting,
            .retransmissionEffort = param->retransmissionEffort,
            .packetType = param->packetType,
        };
        (void)memcpy_s(acceptParam.addr.raw, BT_ADDRESS_SIZE, param->addr.addr, BT_ADDRESS_SIZE);
        result = HCI_AcceptSynchronousConnectionRequest(&acceptParam);
    }

    return result;
}

int BTM_ModifyScoConnection(const BtmModifyScoConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result;

    MutexLock(g_scoListLock);
    BtmScoConnection *scoConnection =
        BtmScoFindScoConnectionByScoHandle(param->connectionHandle, SCO_CONNECTION_STATUS_CONNECTED);
    if (scoConnection != NULL) {
        MutexUnlock(g_scoListLock);

        HciSetupSynchronousConnectionParam setupParam = {
            .connectionHandle = param->connectionHandle,
            .transmitBandwidth = param->transmitBandwidth,
            .receiveBandwidth = param->receiveBandwidth,
            .maxLatency = param->maxLatency,
            .voiceSetting = param->voiceSetting,
            .retransmissionEffort = param->retransmissionEffort,
            .packetType = param->packetType,
        };
        result = HCI_SetupSynchronousConnection(&setupParam);
    } else {
        MutexUnlock(g_scoListLock);
        result = BT_BAD_STATUS;
    }

    return result;
}

static uint16_t BtmEscoParamToVoiceSetting(const BtmEscoParameters *escoParam)
{
    uint16_t voiceSetting = 0;

    if (escoParam->inputCodingFormat.codingFormat == HCI_CODING_FORMAT_U_LAW_LOG) {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_CODING_ULAW;
    } else if (escoParam->inputCodingFormat.codingFormat == HCI_CODING_FORMAT_A_LAW_LOG) {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_CODING_ALAW;
    } else {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_CODING_LINEAR;
    }

    if (escoParam->inputPCMDataFormat == HCI_PCM_DATA_FORMAT_1_COMPLEMENT) {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_DATA_FORMAT_1S_COMPLEMENT;
    } else if (escoParam->inputPCMDataFormat == HCI_PCM_DATA_FORMAT_SIGN_MAGNITUDE) {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_DATA_FORMAT_SIGN_MAGNITUDE;
    } else if (escoParam->inputPCMDataFormat == HCI_PCM_DATA_FORMAT_UNSIGNED) {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_DATA_FORMAT_UNSIGNED;
    } else {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_DATA_FORMAT_2S_COMPLEMENT;
    }

    if (escoParam->transmitCodingFormat.codingFormat == HCI_CODING_FORMAT_U_LAW_LOG) {
        voiceSetting |= HCI_VOICE_SETTING_AIR_CODING_FORMAT_ULAW;
    } else if (escoParam->transmitCodingFormat.codingFormat == HCI_CODING_FORMAT_A_LAW_LOG) {
        voiceSetting |= HCI_VOICE_SETTING_AIR_CODING_FORMAT_ALAW;
    } else if (escoParam->transmitCodingFormat.codingFormat == HCI_CODING_FORMAT_MSBC) {
        voiceSetting |= HCI_VOICE_SETTING_AIR_CODING_FORMAT_TRANSPARENT_DATA;
    } else {
        voiceSetting |= HCI_VOICE_SETTING_AIR_CODING_FORMAT_CVSD;
    }

    voiceSetting |= ((escoParam->inputPCMSamplePayloadMSBPosition & 0x7) << LINEAR_PCM_BIT_POS_BIT_INDEX);

    if (escoParam->inputCodedDataSize == INPUT_SAMPLE_SIZE_16_BIT) {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_SAMPLE_SIZE_16_BIT;
    } else {
        voiceSetting |= HCI_VOICE_SETTING_INPUT_SAMPLE_SIZE_8_BIT;
    }

    return voiceSetting;
}

static int BtmEnhancedCreateEscoConnection(
    uint16_t connectionHandle, const BtmCreateEscoConnectionParam *param, const BtmEscoParameters *escoParam)
{
    HciEnhancedSetupSynchronousConnectionParam setupParam = {
        .connectionHandle = connectionHandle,
        .transmitBandwidth = param->transmitBandwidth,
        .receiveBandwidth = param->receiveBandwidth,
        .transmitCodingFormat =
            {
                .codingFormat = escoParam->transmitCodingFormat.codingFormat,
                .companyID = escoParam->transmitCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->transmitCodingFormat.vendorSpecificCodecID,
            },
        .receiveCodingFormat =
            {
                .codingFormat = escoParam->receiveCodingFormat.codingFormat,
                .companyID = escoParam->receiveCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->receiveCodingFormat.vendorSpecificCodecID,
            },
        .transmitCodecFrameSize = escoParam->transmitCodecFrameSize,
        .receiveCodecFrameSize = escoParam->receiveCodecFrameSize,
        .inputBandwidth = escoParam->inputBandwidth,
        .outputBandwidth = escoParam->outputBandwidth,
        .inputCodingFormat =
            {
                .codingFormat = escoParam->inputCodingFormat.codingFormat,
                .companyID = escoParam->inputCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->inputCodingFormat.vendorSpecificCodecID,
            },
        .outputCodingFormat =
            {
                .codingFormat = escoParam->outputCodingFormat.codingFormat,
                .companyID = escoParam->outputCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->outputCodingFormat.vendorSpecificCodecID,
            },
        .inputCodedDataSize = escoParam->inputCodedDataSize,
        .outputCodedDataSize = escoParam->outputCodedDataSize,
        .inputPCMDataFormat = escoParam->inputPCMDataFormat,
        .outputPCMDataFormat = escoParam->outputPCMDataFormat,
        .inputPCMSamplePayloadMSBPosition = escoParam->inputPCMSamplePayloadMSBPosition,
        .outputPCMSamplePaylandMSBPosition = escoParam->outputPCMSamplePayloadMSBPosition,
        .inputDataPath = escoParam->inputDataPath,
        .outputDataPath = escoParam->outputDataPath,
        .inputTransportUnitSize = escoParam->inputTransportUnitSize,
        .outputTransportUnitSize = escoParam->outputTransportUnitSize,
        .maxLatency = param->maxLatency,
        .packetType = param->packetType,
        .retransmissionEffort = param->retransmissionEffort,
    };
    return HCI_EnhancedSetupSynchronousConnection(&setupParam);
}

int BTM_CreateEscoConnection(const BtmCreateEscoConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t aclHandle = 0xffff;
    int result = BtmGetAclHandleByAddress(&param->addr, &aclHandle);
    if (result != BT_NO_ERROR) {
        return result;
    }

    const BtmEscoParameters *escoParam = BtmGetEscoParameters(param->codec);
    if (escoParam == NULL) {
        return BT_BAD_PARAM;
    }

    MutexLock(g_scoListLock);
    if (ListGetSize(g_scoList) >= BTM_MAX_SCO) {
        MutexUnlock(g_scoListLock);
        return BT_CONNECT_NUM_MAX;
    }

    BtmScoConnection *scoConnection = BtmAllocScoConnection(&param->addr, aclHandle);
    if (scoConnection != NULL) {
        scoConnection->status = SCO_CONNECTION_STATUS_CONNECTING;

        ListAddLast(g_scoList, scoConnection);
        MutexUnlock(g_scoListLock);
    } else {
        MutexUnlock(g_scoListLock);
        return BT_NO_MEMORY;
    }

    BTM_ExitSniffMode(&param->addr);

    if (BtmIsControllerSupportedEnhancedSetupSynchronousConnection()) {
        result = BtmEnhancedCreateEscoConnection(aclHandle, param, escoParam);
    } else {
        uint16_t voiceSetting = BtmEscoParamToVoiceSetting(escoParam);
        HciSetupSynchronousConnectionParam setupParam = {
            .connectionHandle = aclHandle,
            .transmitBandwidth = param->transmitBandwidth,
            .receiveBandwidth = param->receiveBandwidth,
            .maxLatency = param->maxLatency,
            .voiceSetting = voiceSetting,
            .retransmissionEffort = param->retransmissionEffort,
            .packetType = param->packetType,
        };
        result = HCI_SetupSynchronousConnection(&setupParam);
    }

    return result;
}

static int BtmEnhancedModifyEscoConnection(
    const BtmModifyEscoConnectionParam *param, const BtmEscoParameters *escoParam)
{
    HciEnhancedSetupSynchronousConnectionParam setupParam = {
        .connectionHandle = param->connectionHandle,
        .transmitBandwidth = param->transmitBandwidth,
        .receiveBandwidth = param->receiveBandwidth,
        .transmitCodingFormat =
            {
                .codingFormat = escoParam->transmitCodingFormat.codingFormat,
                .companyID = escoParam->transmitCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->transmitCodingFormat.vendorSpecificCodecID,
            },
        .receiveCodingFormat =
            {
                .codingFormat = escoParam->receiveCodingFormat.codingFormat,
                .companyID = escoParam->receiveCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->receiveCodingFormat.vendorSpecificCodecID,
            },
        .transmitCodecFrameSize = escoParam->transmitCodecFrameSize,
        .receiveCodecFrameSize = escoParam->receiveCodecFrameSize,
        .inputBandwidth = escoParam->inputBandwidth,
        .outputBandwidth = escoParam->outputBandwidth,
        .inputCodingFormat =
            {
                .codingFormat = escoParam->inputCodingFormat.codingFormat,
                .companyID = escoParam->inputCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->inputCodingFormat.vendorSpecificCodecID,
            },
        .outputCodingFormat =
            {
                .codingFormat = escoParam->outputCodingFormat.codingFormat,
                .companyID = escoParam->outputCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->outputCodingFormat.vendorSpecificCodecID,
            },
        .inputCodedDataSize = escoParam->inputCodedDataSize,
        .outputCodedDataSize = escoParam->outputCodedDataSize,
        .inputPCMDataFormat = escoParam->inputPCMDataFormat,
        .outputPCMDataFormat = escoParam->outputPCMDataFormat,
        .inputPCMSamplePayloadMSBPosition = escoParam->inputPCMSamplePayloadMSBPosition,
        .outputPCMSamplePaylandMSBPosition = escoParam->outputPCMSamplePayloadMSBPosition,
        .inputDataPath = escoParam->inputDataPath,
        .outputDataPath = escoParam->outputDataPath,
        .inputTransportUnitSize = escoParam->inputTransportUnitSize,
        .outputTransportUnitSize = escoParam->outputTransportUnitSize,
        .maxLatency = param->maxLatency,
        .packetType = param->packetType,
        .retransmissionEffort = param->retransmissionEffort,
    };
    return HCI_EnhancedSetupSynchronousConnection(&setupParam);
}

int BTM_ModifyEscoConnection(const BtmModifyEscoConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    const BtmEscoParameters *escoParam = BtmGetEscoParameters(param->codec);
    if (escoParam == NULL) {
        return BT_BAD_PARAM;
    }

    int result;

    MutexLock(g_scoListLock);
    BtmScoConnection *scoConnection =
        BtmScoFindScoConnectionByScoHandle(param->connectionHandle, SCO_CONNECTION_STATUS_CONNECTED);
    if (scoConnection != NULL) {
        MutexUnlock(g_scoListLock);

        if (BtmIsControllerSupportedEnhancedSetupSynchronousConnection()) {
            result = BtmEnhancedModifyEscoConnection(param, escoParam);
        } else {
            uint16_t voiceSetting = BtmEscoParamToVoiceSetting(escoParam);
            HciSetupSynchronousConnectionParam setupParam = {
                .connectionHandle = param->connectionHandle,
                .transmitBandwidth = param->transmitBandwidth,
                .receiveBandwidth = param->receiveBandwidth,
                .maxLatency = param->maxLatency,
                .voiceSetting = voiceSetting,
                .retransmissionEffort = param->retransmissionEffort,
                .packetType = param->packetType,
            };
            result = HCI_SetupSynchronousConnection(&setupParam);
        }
    } else {
        MutexUnlock(g_scoListLock);
        result = BT_BAD_STATUS;
    }

    return result;
}

static int BtmEnhancedAcceptEscoConnection(
    const BtmAcceptEscoConnectionRequestParam *param, const BtmEscoParameters *escoParam)
{
    HciEnhancedAcceptSynchronousConnectionRequestParam acceptParam = {
        .bdAddr = {.raw = {0}},
        .transmitBandwidth = param->transmitBandwidth,
        .receiveBandwidth = param->receiveBandwidth,
        .transmitCodingFormat =
            {
                .codingFormat = escoParam->transmitCodingFormat.codingFormat,
                .companyID = escoParam->transmitCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->transmitCodingFormat.vendorSpecificCodecID,
            },
        .receiveCodingFormat =
            {
                .codingFormat = escoParam->receiveCodingFormat.codingFormat,
                .companyID = escoParam->receiveCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->receiveCodingFormat.vendorSpecificCodecID,
            },
        .transmitCodecFrameSize = escoParam->transmitCodecFrameSize,
        .receiveCodecFrameSize = escoParam->receiveCodecFrameSize,
        .inputBandwidth = escoParam->inputBandwidth,
        .outputBandwidth = escoParam->outputBandwidth,
        .inputCodingFormat =
            {
                .codingFormat = escoParam->inputCodingFormat.codingFormat,
                .companyID = escoParam->inputCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->inputCodingFormat.vendorSpecificCodecID,
            },
        .outputCodingFormat =
            {
                .codingFormat = escoParam->outputCodingFormat.codingFormat,
                .companyID = escoParam->outputCodingFormat.companyID,
                .vendorSpecificCodecID = escoParam->outputCodingFormat.vendorSpecificCodecID,
            },
        .inputCodedDataSize = escoParam->inputCodedDataSize,
        .outputCodedDataSize = escoParam->outputCodedDataSize,
        .inputPCMDataFormat = escoParam->inputPCMDataFormat,
        .outputPCMDataFormat = escoParam->outputPCMDataFormat,
        .inputPCMSamplePayloadMSBPosition = escoParam->inputPCMSamplePayloadMSBPosition,
        .outputPCMSamplePaylandMSBPosition = escoParam->outputPCMSamplePayloadMSBPosition,
        .inputDataPath = escoParam->inputDataPath,
        .outputDataPath = escoParam->outputDataPath,
        .inputTransportUnitSize = escoParam->inputTransportUnitSize,
        .outputTransportUnitSize = escoParam->outputTransportUnitSize,
        .maxLatency = param->maxLatency,
        .packetType = param->packetType,
        .retransmissionEffort = param->retransmissionEffort,
    };
    (void)memcpy_s(acceptParam.bdAddr.raw, BT_ADDRESS_SIZE, param->addr.addr, BT_ADDRESS_SIZE);
    return HCI_EnhancedAcceptSynchronousConnectionRequest(&acceptParam);
}

static int BtmAcceptEscoConnection(const BtmAcceptEscoConnectionRequestParam *param, const BtmEscoParameters *escoParam)
{
    uint16_t voiceSetting = BtmEscoParamToVoiceSetting(escoParam);
    HciAcceptSynchronousConnectionRequestParam acceptParam = {
        .addr =
            {
                .raw = {0},
            },
        .transmitBandwidth = param->transmitBandwidth,
        .receiveBandwidth = param->receiveBandwidth,
        .maxLatency = param->maxLatency,
        .voiceSetting = voiceSetting,
        .retransmissionEffort = param->retransmissionEffort,
        .packetType = param->packetType,
    };
    (void)memcpy_s(acceptParam.addr.raw, BT_ADDRESS_SIZE, param->addr.addr, BT_ADDRESS_SIZE);
    return HCI_AcceptSynchronousConnectionRequest(&acceptParam);
}

int BTM_AcceptEscoConnectionRequest(const BtmAcceptEscoConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    const BtmEscoParameters *escoParam = BtmGetEscoParameters(param->codec);
    if (escoParam == NULL) {
        return BT_BAD_PARAM;
    }

    uint16_t aclHandle = 0xffff;
    int result = BtmGetAclHandleByAddress(&param->addr, &aclHandle);
    if (result != BT_NO_ERROR) {
        return result;
    }

    MutexLock(g_scoListLock);
    if (ListGetSize(g_scoList) >= BTM_MAX_SCO) {
        MutexUnlock(g_scoListLock);
        result = BT_CONNECT_NUM_MAX;
    } else {
        BtmScoConnection *scoConnection = BtmAllocScoConnection(&param->addr, aclHandle);
        if (scoConnection != NULL) {
            scoConnection->status = SCO_CONNECTION_STATUS_CONNECTING;

            ListAddLast(g_scoList, scoConnection);
            MutexUnlock(g_scoListLock);
        } else {
            MutexUnlock(g_scoListLock);
            return BT_NO_MEMORY;
        }

        if (BtmIsControllerSupportedEnhancedAcceptSynchronousConnection()) {
            result = BtmEnhancedAcceptEscoConnection(param, escoParam);
        } else {
            result = BtmAcceptEscoConnection(param, escoParam);
        }
    }

    return result;
}

static void BtmScoOnConnectionComplete(const HciConnectionCompleteEventParam *eventParam)
{
    if (eventParam->linkType != HCI_LINK_TYPE_SCO) {
        return;
    }

    BtAddr addr = {
        .addr = {0},
        .type = BT_PUBLIC_DEVICE_ADDRESS,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->bdAddr.raw, BT_ADDRESS_SIZE);

    BtmScoConnection *scoConnection = NULL;
    MutexLock(g_scoListLock);

    scoConnection = BtmScoFindScoConnectionByAddress(&addr, SCO_CONNECTION_STATUS_CONNECTING);
    if (scoConnection != NULL) {
        if (eventParam->status == HCI_SUCCESS) {
            scoConnection->scoHandle = eventParam->connectionHandle;
            scoConnection->type = SCO_CONNECTION_TYPE_SCO;
            scoConnection->status = SCO_CONNECTION_STATUS_CONNECTED;
        } else {
            ListRemoveNode(g_scoList, scoConnection);
        }
    }
    MutexUnlock(g_scoListLock);

    BtmScoConnectionCompleteParam param = {
        .status = eventParam->status,
        .connectionHandle = (eventParam->status == HCI_SUCCESS) ? eventParam->connectionHandle : INVALID_HANDLE,
        .addr = &addr,
    };

    BtmScoCallbacksBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks->scoConnectionComplete != NULL) {
        block->callbacks->scoConnectionComplete(&param, block->context);
    }
    FOREACH_CALLBACKS_END;
}

static void BtmScoOnSynchronousConnectionComplete(const HciSynchronousConnectionCompleteEventParam *eventParam)
{
    BtAddr addr = {
        .addr = {0},
        .type = BT_PUBLIC_DEVICE_ADDRESS,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->bdAddr.raw, BT_ADDRESS_SIZE);

    BtmScoConnection *scoConnection = NULL;
    uint8_t type = (eventParam->linkType == HCI_LINK_TYPE_ESCO) ? SCO_CONNECTION_TYPE_ESCO : SCO_CONNECTION_TYPE_SCO;
    MutexLock(g_scoListLock);
    scoConnection = BtmScoFindScoConnectionByAddress(&addr, SCO_CONNECTION_STATUS_CONNECTING);
    if (scoConnection != NULL) {
        if (eventParam->status == HCI_SUCCESS) {
            scoConnection->scoHandle = eventParam->connectionHandle;
            scoConnection->type = type;
            scoConnection->status = SCO_CONNECTION_STATUS_CONNECTED;
        } else {
            ListRemoveNode(g_scoList, scoConnection);
        }
    }
    MutexUnlock(g_scoListLock);

    BtmScoConnectionCompleteParam param = {
        .status = eventParam->status,
        .connectionHandle = (eventParam->status == HCI_SUCCESS) ? eventParam->connectionHandle : INVALID_HANDLE,
        .addr = &addr,
    };

    BtmScoCallbacksBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks->scoConnectionComplete != NULL) {
        block->callbacks->scoConnectionComplete(&param, block->context);
    }
    FOREACH_CALLBACKS_END;
}

int BTM_RejectScoConnectionRequest(const BtmRejectScoConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    HciRejectSynchronousConnectionRequestParam rejectParam = {
        .bdAddr =
            {
                .raw = {0},
            },
        .reason = param->reason,
    };
    (void)memcpy_s(rejectParam.bdAddr.raw, BT_ADDRESS_SIZE, param->addr.addr, BT_ADDRESS_SIZE);

    int result = HCI_RejectSynchronousConnectionRequest(&rejectParam);
    return result;
}

int BTM_DisconnectScoConnection(uint16_t connectionHandle, uint8_t reason)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result;

    MutexLock(g_scoListLock);
    BtmScoConnection *scoConnection =
        BtmScoFindScoConnectionByScoHandle(connectionHandle, SCO_CONNECTION_STATUS_CONNECTED);
    if (scoConnection != NULL) {
        scoConnection->status = SCO_CONNECTION_STATUS_DISCONNECTING;
        MutexUnlock(g_scoListLock);

        HciDisconnectParam param = {
            .connectionHandle = connectionHandle,
            .reason = reason,
        };
        result = HCI_Disconnect(&param);
    } else {
        MutexUnlock(g_scoListLock);
        result = BT_BAD_STATUS;
    }

    return result;
}

static void BtmScoOnSynchronousConnectionChanged(const HciSynchronousConnectionChangedEventParam *eventParam)
{
    if (eventParam->status != HCI_SUCCESS) {
        return;
    }

    BtmScoConnectionChangedParam param = {
        .connectionHandle = eventParam->connectionHandle,
        .transmissionInterval = eventParam->transmissionInterval,
        .retransmissionWindow = eventParam->retransmissionWindow,
        .rxPacketLength = eventParam->rxPacketLength,
        .txPacketLength = eventParam->txPacketLength,
    };

    BtmScoCallbacksBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks->scoConnectionChanged != NULL) {
        block->callbacks->scoConnectionChanged(&param, block->context);
    }
    FOREACH_CALLBACKS_END;
}

static void BtmScoOnWriteVoiceSettingComplete(const HciWriteVoiceSettingParamReturnParam *returnParam)
{
    MutexLock(g_scoCallbackListLock);

    BtmScoCallbacksBlock *block = NULL;
    ListNode *node = ListGetFirstNode(g_scoCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->callbacks->writeVoiceSettingComplete != NULL) {
            block->callbacks->writeVoiceSettingComplete(returnParam->status, block->context);
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_scoCallbackListLock);
}

static void BtmScoOnConnectionRequest(const HciConnectionRequestEventParam *eventParam)
{
    if (eventParam->linkType != HCI_LINK_TYPE_SCO && eventParam->linkType != HCI_LINK_TYPE_ESCO) {
        return;
    }

    MutexLock(g_scoListLock);
    if (ListGetSize(g_scoList) >= BTM_MAX_SCO) {
        HciRejectSynchronousConnectionRequestParam rejectParam = {
            .bdAddr = eventParam->bdAddr,
            .reason = HCI_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES,
        };
        HCI_RejectSynchronousConnectionRequest(&rejectParam);

        MutexUnlock(g_scoListLock);
    } else {
        MutexUnlock(g_scoListLock);

        BtAddr addr = {
            .addr = {0},
            .type = BT_PUBLIC_DEVICE_ADDRESS,
        };
        (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->bdAddr.raw, BT_ADDRESS_SIZE);

        BtmScoConnectionRequestParam param = {
            .addr = &addr,
            .linkType = eventParam->linkType,
        };

        BtmScoCallbacksBlock *block = NULL;
        FOREACH_CALLBACKS_START(block);
        if (block->callbacks->scoConnectionRequest != NULL) {
            block->callbacks->scoConnectionRequest(&param, block->context);
        }
        FOREACH_CALLBACKS_END;
    }
}

NO_SANITIZE("cfi") static void BtmScoOnDisconnectComplete(const HciDisconnectCompleteEventParam *param)
{
    MutexLock(g_scoListLock);
    BtmScoConnection *scoConnection =
        BtmScoFindScoConnectionByScoHandle(param->connectionHandle, SCO_CONNECTION_STATUS_DISCONNECTING);
    if (scoConnection == NULL) {
        scoConnection = BtmScoFindScoConnectionByScoHandle(param->connectionHandle, SCO_CONNECTION_STATUS_CONNECTED);
    }

    if (scoConnection != NULL) {
        if (param->status == HCI_SUCCESS) {
            ListRemoveNode(g_scoList, scoConnection);
        }
    }
    MutexUnlock(g_scoListLock);

    BtmScoDisconnectionCompleteParam callbackParam = {
        .status = param->status,
        .connectionHandle = param->connectionHandle,
        .reason = param->reason,
    };

    BtmScoCallbacksBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    block->callbacks->scoDisconnectionComplete(&callbackParam, block->context);
    FOREACH_CALLBACKS_END;
}

static HciEventCallbacks g_hciEventCallbacks = {
    .connectionComplete = BtmScoOnConnectionComplete,
    .synchronousConnectionComplete = BtmScoOnSynchronousConnectionComplete,
    .synchronousConnectionChanged = BtmScoOnSynchronousConnectionChanged,

    .connectionRequest = BtmScoOnConnectionRequest,
    .disconnectComplete = BtmScoOnDisconnectComplete,

    .writeVoiceSettingComplete = BtmScoOnWriteVoiceSettingComplete,
};
