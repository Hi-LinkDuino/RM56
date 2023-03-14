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

#include "devhost_service_proxy.h"
#include "dev_attribute_serialize.h"
#include "devmgr_service_full.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"
#include "osal_message.h"

#define HDF_LOG_TAG devhost_service_proxy

static int32_t DevHostServiceProxyOpsDevice(
    struct IDevHostService *inst, const struct HdfDeviceInfo *attribute, int opsCode)
{
    int status = HDF_FAILURE;
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct DevHostServiceProxy *hostClnt = (struct DevHostServiceProxy *)inst;
    if (hostClnt->remote == NULL || data == NULL) {
        HDF_LOGE("Adding device failed, hostClnt->remote or data or reply is null");
        goto FINISHED;
    }

    DeviceAttributeSerialize(attribute, data);
    status = hostClnt->remote->dispatcher->Dispatch(hostClnt->remote, DEVHOST_SERVICE_ADD_DEVICE, data, NULL);
FINISHED:
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    return status;
}

static int32_t DevHostServiceProxyAddDevice(
    struct IDevHostService *inst, const struct HdfDeviceInfo *attribute)
{
    return DevHostServiceProxyOpsDevice(inst, attribute, DEVHOST_SERVICE_ADD_DEVICE);
}

static int32_t DevHostServiceProxyDelDevice(
    struct IDevHostService *inst, devid_t devid)
{
    int status = HDF_FAILURE;
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct DevHostServiceProxy *hostClnt = (struct DevHostServiceProxy *)inst;
    if (hostClnt->remote == NULL || data == NULL) {
        HDF_LOGE("Del device failed, hostClnt->remote or data is null");
        goto FINISHED;
    }

    HdfSbufWriteUint32(data, devid);
    status = hostClnt->remote->dispatcher->Dispatch(hostClnt->remote, DEVHOST_SERVICE_DEL_DEVICE, data, NULL);

FINISHED:
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    return status;
}

void DevHostServiceProxyOnRemoteDied(struct HdfDeathRecipient *recipient, struct HdfRemoteService *service)
{
    if (recipient == NULL || service == NULL) {
        return;
    }

    struct HdfMessageTask *task = DevmgrServiceFullGetMessageTask();
    if (task == NULL) {
        return;
    }
    struct HdfMessage *message = HdfMessageObtain(0);
    if (message == NULL) {
        return;
    }
    struct DevHostServiceProxy *proxy = HDF_SLIST_CONTAINER_OF(
        struct HdfDeathRecipient, recipient, struct DevHostServiceProxy, recipient);
    message->messageId = DEVMGR_MESSAGE_DEVHOST_DIED;
    message->data[0] = (void *)(uintptr_t)proxy->hostId;
    HDF_LOGD("%{public}s: host %{public}u dead, respawn it", __FUNCTION__, proxy->hostId);
    task->SendMessage(task, message, false);
}

void DevHostServiceProxyConstruct(
    struct DevHostServiceProxy *inst, struct HdfRemoteService *remote)
{
    inst->remote = remote;
    inst->super.AddDevice = DevHostServiceProxyAddDevice;
    inst->super.DelDevice = DevHostServiceProxyDelDevice;
    inst->recipient.OnRemoteDied = DevHostServiceProxyOnRemoteDied;
    HdfRemoteServiceAddDeathRecipient(remote, &inst->recipient);
}

struct IDevHostService *DevHostServiceProxyObtain(uint32_t hostId, struct HdfRemoteService *remote)
{
    struct DevHostServiceProxy *instance =
        (struct DevHostServiceProxy *)OsalMemCalloc(sizeof(struct DevHostServiceProxy));
    if (instance != NULL) {
        instance->hostId = hostId;
        DevHostServiceProxyConstruct(instance, remote);
    }
    return (struct IDevHostService *)instance;
}

void DevHostServiceProxyRecycle(struct DevHostServiceProxy *inst)
{
    if (inst != NULL) {
        if (inst->remote != NULL) {
            HdfRemoteServiceRecycle(inst->remote);
            inst->remote = NULL;
        }
        OsalMemFree(inst);
    }
}

