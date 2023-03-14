/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "disc_client_stub.h"

#include "client_disc_manager.h"
#include "liteipc_adapter.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

int32_t ClientOnDiscoverySuccess(IpcIo *reply, const IpcContext *ctx, void *ipcMsg)
{
    if (reply == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "invalid param.");
        FreeBuffer(ctx, ipcMsg);
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t subscribeId = IpcIoPopInt32(reply);
    DiscClientOnDiscoverySuccess(subscribeId);
    FreeBuffer(ctx, ipcMsg);
    return SOFTBUS_OK;
}

int32_t ClientOnDiscoverFailed(IpcIo *reply, const IpcContext *ctx, void *ipcMsg)
{
    if (reply == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "invalid param.");
        FreeBuffer(ctx, ipcMsg);
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t subscribeId = IpcIoPopInt32(reply);
    int32_t failReason = IpcIoPopInt32(reply);
    DiscClientOnDiscoverFailed(subscribeId, failReason);
    FreeBuffer(ctx, ipcMsg);
    return SOFTBUS_OK;
}

int32_t ClientOnDeviceFound(IpcIo *reply, const IpcContext *ctx, void *ipcMsg)
{
    if (reply == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "invalid param.");
        FreeBuffer(ctx, ipcMsg);
        return SOFTBUS_INVALID_PARAM;
    }

    uint32_t size;
    const DeviceInfo *deviceInfo = (const DeviceInfo*)IpcIoPopFlatObj(reply, &size);
    if (deviceInfo == NULL) {
        FreeBuffer(ctx, ipcMsg);
        return SOFTBUS_ERR;
    }
    DiscClientOnDeviceFound(deviceInfo);
    FreeBuffer(ctx, ipcMsg);
    return SOFTBUS_OK;
}

int32_t ClientOnPublishSuccess(IpcIo *reply, const IpcContext *ctx, void *ipcMsg)
{
    if (reply == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "invalid param.");
        FreeBuffer(ctx, ipcMsg);
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t publishId = IpcIoPopInt32(reply);
    DiscClientOnPublishSuccess(publishId);
    FreeBuffer(ctx, ipcMsg);
    return SOFTBUS_OK;
}

int32_t ClientOnPublishFail(IpcIo *reply, const IpcContext *ctx, void *ipcMsg)
{
    if (reply == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "invalid param.");
        FreeBuffer(ctx, ipcMsg);
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t publishId = IpcIoPopInt32(reply);
    int32_t failReason = IpcIoPopInt32(reply);
    DiscClientOnPublishFail(publishId, failReason);
    FreeBuffer(ctx, ipcMsg);
    return SOFTBUS_OK;
}
