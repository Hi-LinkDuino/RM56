/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "das_task_main.h"
#include "alg_loader.h"
#include "base_sub_task.h"
#include "das_lite_token_manager.h"
#include "das_standard_token_manager.h"
#include "das_task_common.h"
#include "hc_log.h"
#include "iso_task_main.h"
#include "pake_v2_task_main.h"
#include "pake_protocol_dl_common.h"
#include "pake_protocol_ec_common.h"
#include "pake_v1_task_main.h"
#include "protocol_common.h"

typedef struct DasProtocolEntityT {
    ProtocolType type;
    uint32_t algInProtocol;
    const TokenManager *tokenManagerInstance;
    SubTaskBase *(*createSubTask)(const CJson *);
} DasProtocolEntity;

IMPLEMENT_HC_VECTOR(SubTaskVec, void *, 1)
DECLARE_HC_VECTOR(DasProtocolEntityVec, void *);
IMPLEMENT_HC_VECTOR(DasProtocolEntityVec, void *, 1)

DasProtocolEntityVec g_protocolEntityVec;
ProtocolType g_taskTypeToProtocolType[] = {
    ISO, // TASK_TYPE_ISO_PROTOCOL = 0,
    PAKE_V1, // TASK_TYPE_PAKE_V1_PROTOCOL = 1,
    PAKE_V2 // TASK_TYPE_PAKE_V2_PROTOCOL = 2,
};

static void GetMinVersion(VersionStruct *version)
{
    version->first = 1;
    version->second = 0;
    version->third = 0;
}

static void GetMaxVersion(VersionStruct *version)
{
    version->first = MAJOR_VERSION_NO;
    version->second = 0;
    version->third = 0;

    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && *ptr != NULL) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            version->third = (version->third) | temp->algInProtocol;
        }
    }
}

static void InitVersionInfo(VersionInfo *versionInfo)
{
    GetMinVersion(&(versionInfo->minVersion));
    GetMaxVersion(&(versionInfo->curVersion));
    versionInfo->versionStatus = INITIAL;
}

static int AddVersionToOut(const VersionInfo *versionInfo, CJson *out)
{
    CJson *payload = GetObjFromJson(out, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGD("Not find payload.");
        return HC_SUCCESS;
    }
    return AddVersionToJson(payload, &(versionInfo->minVersion), &(versionInfo->curVersion));
}

static int CombineJson(CJson *desObj, const CJson *srcObj)
{
    CHECK_PTR_RETURN_ERROR_CODE(desObj, "desObj");
    CHECK_PTR_RETURN_ERROR_CODE(srcObj, "srcObj");
    int res;
    int len = GetItemNum(srcObj);
    for (int i = 0; i < len; i++) {
        CJson *item = GetItemFromArray(srcObj, i);
        const char *key = GetItemKey(item);
        CJson *payload = GetObjFromJson(desObj, FIELD_PAYLOAD);
        if (strcmp(key, FIELD_PAYLOAD) == 0 && payload != NULL) {
            res = CombineJson(payload, item);
            if (res != HC_SUCCESS) {
                LOGE("Combine payload failed, res: %x.", res);
                return res;
            }
        } else {
            if (AddObjToJson(desObj, key, item) != HC_SUCCESS) {
                LOGE("AddObjToJson failed.");
                return HC_ERR_JSON_ADD;
            }
        }
    }
    return HC_SUCCESS;
}

static void DestroyTaskT(Task *task)
{
    if (task == NULL) {
        return;
    }
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(task->vec, index, ptr) {
        if (ptr != NULL && *ptr != NULL) {
            ((SubTaskBase *)(*ptr))->destroyTask((SubTaskBase *)(*ptr));
        }
    }
    DESTROY_HC_VECTOR(SubTaskVec, &(task->vec));
    HcFree(task);
}

static int ProcessMultiTask(Task *task, const CJson *in, CJson *out, int32_t *status)
{
    int res;
    uint32_t index;
    void **ptr = NULL;
    CJson *tmpOut = NULL;
    CJson *combinedSendToPeer = CreateJson();
    if (combinedSendToPeer == NULL) {
        LOGE("Create combinedSendToPeer failed.");
        return HC_ERR_JSON_CREATE;
    }
    FOR_EACH_HC_VECTOR(task->vec, index, ptr) {
        if ((ptr == NULL) || (*ptr == NULL)) {
            LOGD("Null ptr in subTask vector.");
            continue;
        }
        tmpOut = CreateJson();
        if (tmpOut == NULL) {
            LOGE("Create tmpOut failed.");
            res = HC_ERR_JSON_CREATE;
            goto ERR;
        }
        res = ((SubTaskBase *)(*ptr))->process((*ptr), in, tmpOut, status);
        if (res != HC_SUCCESS) {
            LOGE("Process subTask failed, index: %u, res: %x.", index, res);
            goto ERR;
        }

        CJson *tmpSendToPeer = GetObjFromJson(tmpOut, FIELD_SEND_TO_PEER);
        res = CombineJson(combinedSendToPeer, tmpSendToPeer);
        if (res != HC_SUCCESS) {
            LOGE("CombineJson failed, res: %x.", res);
            goto ERR;
        }
        FreeJson(tmpOut);
        tmpOut = NULL;
    }
    if (AddObjToJson(out, FIELD_SEND_TO_PEER, combinedSendToPeer) != HC_SUCCESS) {
        LOGE("Add combinedSendToPeer to json object failed.");
        res = HC_ERR_JSON_ADD;
        goto ERR;
    }
ERR:
    FreeJson(combinedSendToPeer);
    FreeJson(tmpOut);
    return res;
}

static int NegotiateAndProcessTask(Task *task, const CJson *in, CJson *out, int32_t *status)
{
    VersionStruct curVersionPeer = { 0, 0, 0 };
    VersionStruct minVersionPeer = { 0, 0, 0 };
    int res = GetVersionFromJson(in, &minVersionPeer, &curVersionPeer);
    if (res != HC_SUCCESS) {
        LOGE("Get peer version info failed, res: %x.", res);
        return res;
    }
    res = NegotiateVersion(&minVersionPeer, &curVersionPeer, &(task->versionInfo.curVersion));
    if (res != HC_SUCCESS) {
        LOGE("NegotiateVersion failed, res: %x.", res);
        return res;
    }
    if (!IsVersionEqual(&(task->versionInfo.curVersion), &curVersionPeer)) {
        LOGE("Negotiated version is not matched with peer.");
        return HC_ERR_UNSUPPORTED_VERSION;
    }
    ProtocolType protocolType = GetPrototolType(&(task->versionInfo.curVersion), task->versionInfo.opCode);
    LOGI("Client select protocolType: %d", protocolType);

    SubTaskBase *subTask = NULL;
    uint32_t index = 0;
    void **ptr = task->vec.getp(&(task->vec), 0);
    while (index < task->vec.size(&(task->vec)) && ptr != NULL) {
        SubTaskBase *temp = (SubTaskBase *)(*ptr);
        if (g_taskTypeToProtocolType[temp->getTaskType(temp)] == protocolType) {
            subTask = temp;
            index++;
        } else {
            temp->destroyTask(temp);
            task->vec.eraseElement(&(task->vec), ptr, index);
        }
        ptr = task->vec.getp(&(task->vec), index);
    }
    if (subTask == NULL) {
        LOGE("Can't find matched subTask.");
        return HC_ERR_NOT_SUPPORT;
    }
    subTask->curVersion = task->versionInfo.curVersion;
    res = subTask->process(subTask, in, out, status);
    if (res != HC_SUCCESS) {
        LOGE("Process subTask failed, res: %x.", res);
    }
    return res;
}

static bool IsPeerErrMessage(const CJson *in, int32_t *res)
{
    int32_t message = 0;
    if (GetIntFromJson(in, FIELD_MESSAGE, &message) != HC_SUCCESS) {
        LOGD("There is no message code.");
        return false;
    }
    if (message != ERR_MESSAGE) {
        return false;
    }

    if (GetIntFromJson(in, FIELD_ERROR_CODE, res) != HC_SUCCESS) {
        LOGE("Get peer error code failed.");
    }
    return true;
}

static int ProcessTaskT(Task *task, const CJson *in, CJson *out, int32_t *status)
{
    int32_t res;
    if (IsPeerErrMessage(in, &res)) {
        LOGE("Receive error message from peer, errCode: %x.", res);
        DasSendErrMsgToSelf(out, HC_ERR_PEER_ERROR);
        return HC_ERR_PEER_ERROR;
    }
    if (task->vec.size(&(task->vec)) == 0) {
        LOGE("Task hasn't subTask.");
        res = HC_ERR_TASK_IS_NULL;
        goto ERR;
    }

    if (task->versionInfo.versionStatus == INITIAL) {
        res = ProcessMultiTask(task, in, out, status);
        if (res != HC_SUCCESS) {
            LOGE("ProcessMultiTask failed, res: %x.", res);
            goto ERR;
        }
        task->versionInfo.versionStatus = VERSION_CONFIRM;
    } else if (task->versionInfo.versionStatus == VERSION_CONFIRM) {
        res = NegotiateAndProcessTask(task, in, out, status);
        if (res != HC_SUCCESS) {
            LOGE("NegotiateAndProcessTask failed, res: %x.", res);
            goto ERR;
        }
        task->versionInfo.versionStatus = VERSION_DECIDED;
    } else {
        SubTaskBase *subTask = HC_VECTOR_GET(&(task->vec), 0);
        res = subTask->process(subTask, in, out, status);
        if (res != HC_SUCCESS) {
            LOGE("Process subTask failed, res: %x.", res);
            goto ERR;
        }
    }

    res = AddVersionToOut(&(task->versionInfo), out);
    if (res != HC_SUCCESS) {
        LOGE("AddVersionToOut failed, res: %x.", res);
        goto ERR;
    }
    return res;
ERR:
    if (task->versionInfo.versionStatus == INITIAL) {
        DasSendErrMsgToSelf(out, res);
    } else {
        DasSendErrorToOut(out, res);
    }
    return res;
}

static int CreateMultiSubTask(Task *task, const CJson *in)
{
    InitVersionInfo(&(task->versionInfo));
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && (*ptr) != NULL) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            SubTaskBase *subTask = temp->createSubTask(in);
            if (subTask == NULL) {
                LOGE("Create subTask failed, protocolType: %d.", temp->type);
                return HC_ERR_ALLOC_MEMORY;
            }
            subTask->curVersion = task->versionInfo.curVersion;
            task->vec.pushBackT(&(task->vec), (void *)subTask);
        }
    }
    return HC_SUCCESS;
}

static int CreateSingleSubTask(Task *task, const CJson *in)
{
    VersionStruct curVersionPeer = { 0, 0, 0 };
    VersionStruct minVersionPeer = { 0, 0, 0 };
    int res = GetVersionFromJson(in, &minVersionPeer, &curVersionPeer);
    if (res != HC_SUCCESS) {
        LOGE("Get peer version info failed, res: %x.", res);
        return res;
    }
    InitVersionInfo(&(task->versionInfo));
    res = NegotiateVersion(&minVersionPeer, &curVersionPeer, &(task->versionInfo.curVersion));
    if (res != HC_SUCCESS) {
        LOGE("NegotiateVersion failed, res: %x.", res);
        return res;
    }
    task->versionInfo.versionStatus = VERSION_DECIDED;

    ProtocolType protocolType = GetPrototolType(&(task->versionInfo.curVersion), task->versionInfo.opCode);
    LOGI("Server select protocolType: %d", protocolType);

    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && (*ptr) != NULL && (((DasProtocolEntity *)(*ptr))->type == protocolType)) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            SubTaskBase *subTask = temp->createSubTask(in);
            if (subTask == NULL) {
                LOGE("Create subTask failed.");
                return HC_ERR_ALLOC_MEMORY;
            }
            subTask->curVersion = task->versionInfo.curVersion;
            task->vec.pushBackT(&(task->vec), (void *)subTask);
            return HC_SUCCESS;
        }
    }

    LOGE("Can't find protocolType.");
    return HC_ERR_NOT_SUPPORT;
}

Task *CreateTaskT(int32_t *taskId, const CJson *in, CJson *out)
{
    int res;
    Task *task = NULL;
    bool isClient = true;
    if (GetBoolFromJson(in, FIELD_IS_CLIENT, &isClient) != HC_SUCCESS) {
        LOGE("Get isClient failed.");
        res = HC_ERR_JSON_GET;
        goto ERR;
    }
    task = (Task *)HcMalloc(sizeof(Task), 0);
    if (task == NULL) {
        LOGE("Malloc for das task failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    task->vec = CREATE_HC_VECTOR(SubTaskVec);
    task->destroyTask = DestroyTaskT;
    task->processTask = ProcessTaskT;

    Uint8Buff taskIdBuf = { (uint8_t *)taskId, sizeof(int) };
    res = GetLoaderInstance()->generateRandom(&taskIdBuf);
    if (res != 0) {
        LOGE("Generate taskId failed.");
        goto ERR;
    }
    task->taskId = *taskId;
    if (GetIntFromJson(in, FIELD_OPERATION_CODE, &(task->versionInfo.opCode)) != HC_SUCCESS) {
        LOGE("Get opcode failed.");
        res = HC_ERR_JSON_GET;
        goto ERR;
    }
    if (isClient) {
        res = CreateMultiSubTask(task, in);
    } else {
        res = CreateSingleSubTask(task, in);
    }
    if (res != HC_SUCCESS) {
        LOGE("Create sub task failed, res: %x.", res);
        goto ERR;
    }
    return task;
ERR:
    if (isClient) {
        DasSendErrMsgToSelf(out, res);
    } else {
        DasSendErrorToOut(out, res);
    }
    DestroyTaskT(task);
    return NULL;
}

int32_t RegisterLocalIdentityInTask(const char *pkgName, const char *serviceType, Uint8Buff *authId, int userType)
{
    int32_t res = HC_SUCCESS;
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && (*ptr) != NULL) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            if ((temp->tokenManagerInstance == NULL) || (temp->tokenManagerInstance->registerLocalIdentity == NULL)) {
                LOGD("Protocol type: %d, unsupported method!", temp->type);
                continue;
            }
            res = temp->tokenManagerInstance->registerLocalIdentity(pkgName, serviceType, authId, userType);
            if (res != HC_SUCCESS) {
                LOGE("Protocol type: %d, registerLocalIdentity failed, res: %d!", temp->type, res);
                return HC_ERR_GENERATE_KEY_FAILED;
            }
        }
    }
    return res;
}

int32_t UnregisterLocalIdentityInTask(const char *pkgName, const char *serviceType, Uint8Buff *authId, int userType)
{
    int32_t res = HC_SUCCESS;
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && (*ptr) != NULL) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            if ((temp->tokenManagerInstance == NULL) || (temp->tokenManagerInstance->unregisterLocalIdentity == NULL)) {
                LOGD("Protocol type: %d, unsupported method!", temp->type);
                continue;
            }
            res = temp->tokenManagerInstance->unregisterLocalIdentity(pkgName, serviceType, authId, userType);
            if (res != HC_SUCCESS) {
                LOGE("Protocol type: %d, unregisterLocalIdentity failed, res: %d!", temp->type, res);
                return res;
            }
        }
    }
    return res;
}

int32_t DeletePeerAuthInfoInTask(const char *pkgName, const char *serviceType, Uint8Buff *authIdPeer, int userTypePeer)
{
    int32_t res = HC_SUCCESS;
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && (*ptr) != NULL) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            if ((temp->tokenManagerInstance == NULL) || (temp->tokenManagerInstance->deletePeerAuthInfo == NULL)) {
                LOGD("Protocol type: %d, unsupported method!", temp->type);
                continue;
            }
            res = temp->tokenManagerInstance->deletePeerAuthInfo(pkgName, serviceType, authIdPeer, userTypePeer);
            if (res != HC_SUCCESS) {
                LOGE("Protocol type: %d, deletePeerAuthInfo failed, res: %d!", temp->type, res);
                return res;
            }
        }
    }
    return res;
}

int32_t GetPublicKeyInTask(const char *pkgName, const char *serviceType, Uint8Buff *authIdPeer, int userTypePeer,
                           Uint8Buff *returnPk)
{
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && (*ptr) != NULL) {
            DasProtocolEntity *temp = (DasProtocolEntity *)(*ptr);
            if ((temp->tokenManagerInstance == NULL) || (temp->tokenManagerInstance->getPublicKey == NULL)) {
                LOGD("Protocol type: %d, unsupported method!", temp->type);
                continue;
            }
            return temp->tokenManagerInstance->getPublicKey(pkgName, serviceType, authIdPeer, userTypePeer, returnPk);
        }
    }
    LOGE("Failed to find valid protocol!");
    return HC_ERR_NOT_SUPPORT;
}

static uint32_t GetPakeAlgInProtocol(int offset)
{
    uint32_t algInProtocol = PSK_SPEKE;
#ifdef P2P_PAKE_DL_TYPE
    algInProtocol |= (GetPakeDlAlg() << offset);
#endif
#ifdef P2P_PAKE_EC_TYPE
    algInProtocol |= (GetPakeEcAlg() << offset);
#endif
    (void)offset;
    return algInProtocol;
}

int32_t InitDasProtocolEntities(void)
{
    g_protocolEntityVec = CREATE_HC_VECTOR(DasProtocolEntityVec);
    DasProtocolEntity *protocol = NULL;
    if (IsIsoSupported()) {
        protocol = (DasProtocolEntity *)HcMalloc(sizeof(DasProtocolEntity), 0);
        if (protocol == NULL) {
            LOGE("Malloc for Iso dasProtocolEntity failed.");
            return HC_ERR_ALLOC_MEMORY;
        }
        protocol->type = ISO;
        protocol->algInProtocol = ISO_ALG;
        protocol->createSubTask = CreateIsoSubTask;
        protocol->tokenManagerInstance = GetLiteTokenManagerInstance();
        g_protocolEntityVec.pushBackT(&g_protocolEntityVec, (void *)protocol);
    }

    if (IsSupportPakeV1()) {
        protocol = (DasProtocolEntity *)HcMalloc(sizeof(DasProtocolEntity), 0);
        if (protocol == NULL) {
            LOGE("Malloc for pake v1 dasProtocolEntity failed.");
            return HC_ERR_ALLOC_MEMORY;
        }
        protocol->type = PAKE_V1;
        protocol->algInProtocol = GetPakeAlgInProtocol(ALG_OFFSET_FOR_PAKE_V1);
        protocol->createSubTask = CreatePakeV1SubTask;
        protocol->tokenManagerInstance = GetStandardTokenManagerInstance();
        g_protocolEntityVec.pushBackT(&g_protocolEntityVec, (void *)protocol);
    }

    if (IsSupportPakeV2()) {
        protocol = (DasProtocolEntity *)HcMalloc(sizeof(DasProtocolEntity), 0);
        if (protocol == NULL) {
            LOGE("Malloc for pake v2 dasProtocolEntity failed.");
            return HC_ERR_ALLOC_MEMORY;
        }
        protocol->type = PAKE_V2;
        protocol->algInProtocol = GetPakeAlgInProtocol(ALG_OFFSET_FOR_PAKE_V2);
        protocol->createSubTask = CreatePakeV2SubTask;
        protocol->tokenManagerInstance = GetStandardTokenManagerInstance();
        g_protocolEntityVec.pushBackT(&g_protocolEntityVec, (void *)protocol);
    }

    return HC_SUCCESS;
}

void DestroyDasProtocolEntities(void)
{
    uint32_t index;
    void **ptr = NULL;
    FOR_EACH_HC_VECTOR(g_protocolEntityVec, index, ptr) {
        if (ptr != NULL && *ptr != NULL) {
            HcFree((DasProtocolEntity *)(*ptr));
            *ptr = NULL;
        }
    }
    DESTROY_HC_VECTOR(DasProtocolEntityVec, &g_protocolEntityVec);
}
