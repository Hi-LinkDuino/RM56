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

#ifndef IPC_ADAPT_H
#define IPC_ADAPT_H

#include <stdbool.h>
#include <stdint.h>
#include "device_auth.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_REQUEST_PARAMS_NUM 8
#define CB_TYPE_DEV_AUTH 1
#define CB_TYPE_TMP_DEV_AUTH 2
#define CB_TYPE_LISTENER 3

typedef struct {
    int32_t type;
    int32_t valSz;
    uint8_t *val;
    int32_t idx;
} IpcDataInfo;

enum {
    CB_ID_ON_TRANS = 1,
    CB_ID_SESS_KEY_DONE,
    CB_ID_ON_FINISH,
    CB_ID_ON_ERROR,
    CB_ID_ON_REQUEST,
    CB_ID_ON_GROUP_CREATED,
    CB_ID_ON_GROUP_DELETED,
    CB_ID_ON_DEV_BOUND,
    CB_ID_ON_DEV_UNBOUND,
    CB_ID_ON_DEV_UNTRUSTED,
    CB_ID_ON_LAST_GROUP_DELETED,
    CB_ID_ON_TRUST_DEV_NUM_CHANGED,
};

typedef int32_t (*IpcServiceCall)(const IpcDataInfo *, int32_t, uintptr_t);

int32_t IpcEncodeCallReplay(uintptr_t replayCache, int32_t type, const uint8_t *result, int32_t resultSz);
uint32_t SetIpcCallMap(uintptr_t ipcInstance, IpcServiceCall method, int32_t methodId);

void SetCbCtxToDataCtx(uintptr_t callCtx, int32_t cbIdx);
int32_t CreateCallCtx(uintptr_t *callCtx, uintptr_t *cbCtx);
void DestroyCallCtx(uintptr_t *callCtx, uintptr_t *cbCtx);
int32_t DoBinderCall(uintptr_t callCtx, int32_t methodId, bool withSync);
int32_t SetCallRequestParamInfo(uintptr_t callCtx, int32_t type, const uint8_t *param, int32_t paramSz);

int32_t CreateServiceInstance(uintptr_t *ipcInstance);
void DestroyServiceInstance(uintptr_t *ipcInstance);

int32_t AddDevAuthServiceToManager(uintptr_t *serviceCtx);
void DecodeCallReply(uintptr_t callCtx, IpcDataInfo *replyCache, int32_t cacheNum);

int32_t InitIpcCallBackList(void);
void DeInitIpcCallBackList(void);
void InitDeviceAuthCbCtx(DeviceAuthCallback *ctx, int32_t type);
void InitDevAuthListenerCbCtx(DataChangeListener *ctx);
int32_t AddIpcCallBackByAppId(const char *appId, const uint8_t *cbPtr, int32_t cbSz, int32_t type);
void DelIpcCallBackByAppId(const char *appId, int32_t type);
int32_t AddIpcCallBackByReqId(int64_t reqId, const uint8_t *cbPtr, int32_t cbSz, int32_t type);
void DelIpcCallBackByReqId(int64_t reqId, int32_t type, bool withLock);

int32_t DecodeIpcData(uintptr_t data, int32_t *type, uint8_t **val, int32_t *valSz);
void ProcCbHook(int32_t callbackId, uintptr_t cbHook,
    const IpcDataInfo *cbDataCache, int32_t cacheNum, uintptr_t replyCtx);

int32_t GetIpcRequestParamByType(const IpcDataInfo *ipcParams, int32_t paramNum,
    int32_t type, uint8_t *paramCache, int32_t *cacheLen);
int32_t AddReqIdByAppId(const char *appId, int64_t reqId);
void AddIpcCbObjByAppId(const char *appId, int32_t objIdx, int32_t type);
void AddIpcCbObjByReqId(int64_t reqId, int32_t objIdx, int32_t type);

bool IsCallbackMethod(int32_t methodId);
bool IsServiceRunning(void);
void ResetIpcCallBackNodeByNodeId(int32_t nodeIdx);
int32_t InitProxyAdapt(void);
void UnInitProxyAdapt(void);

#ifdef __cplusplus
}
#endif
#endif
