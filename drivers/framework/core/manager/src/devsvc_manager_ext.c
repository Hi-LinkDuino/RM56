/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devsvc_listener_holder.h"
#include "devsvc_manager.h"
#include "hdf_base.h"
#include "hdf_device_node.h"
#include "hdf_io_service.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "osal_mem.h"
#include "svcmgr_ioservice.h"
#include "osal_mutex.h"

#define HDF_LOG_TAG devsvc_mger
#define SVC_MGR_NODE_PERM 0660

struct DevSvcManagerExt {
    struct DevSvcManager super;
    struct IDeviceIoService ioService;
    struct HdfIoService *serv;
    struct OsalMutex mutex;
    bool started;
};

int32_t DeviceSvcMgrOpen(struct HdfDeviceIoClient *client)
{
    (void)client;
    return HDF_SUCCESS;
}

static int32_t DevSvcManagerExtRegisterListener(struct HdfDeviceIoClient *client, struct HdfSBuf *data)
{
    struct ServStatListenerHolder *holder = NULL;
    uint16_t devClass = DEVICE_CLASS_MAX;
    struct DevSvcManagerExt *svcmgrInst = NULL;
    if (client->device == NULL || client->device->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    svcmgrInst = (struct DevSvcManagerExt *)DevSvcManagerGetInstance();

    if (!HdfSbufReadUint16(data, &devClass)) {
        return HDF_ERR_INVALID_PARAM;
    }

    holder = ServStatListenerHolderGet((uintptr_t)client);
    if (holder != NULL) {
        HDF_LOGE("%s:register listener exist, update and return", __func__);
        holder->listenClass = devClass;
        return HDF_SUCCESS;
    }

    holder = ServStatListenerHolderCreate((uintptr_t)client, devClass);
    if (holder == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    return svcmgrInst->super.super.RegsterServListener(&svcmgrInst->super.super, holder);
}

static int32_t DevSvcManagerExtUnRegisterListener(struct HdfDeviceIoClient *client)
{
    struct ServStatListenerHolder *holder = NULL;
    struct DevSvcManagerExt *svcmgrInst = NULL;

    if (client->device == NULL || client->device->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    svcmgrInst = (struct DevSvcManagerExt *)client->device->priv;
    OsalMutexLock(&svcmgrInst->mutex);
    holder = ServStatListenerHolderGet((uintptr_t)client);
    if (holder == NULL) {
        OsalMutexUnlock(&svcmgrInst->mutex);
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }
    if (holder->node.next != NULL) {
        svcmgrInst->super.super.UnregsterServListener(&svcmgrInst->super.super, holder);
    }
    ServStatListenerHolderRelease(holder);
    OsalMutexUnlock(&svcmgrInst->mutex);

    return HDF_SUCCESS;
}

int32_t DeviceSvcMgrDispatch(struct HdfObject *service, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint64_t ioClientPtr = 0;
    struct HdfDeviceIoClient *client = NULL;
    if (service == NULL || data == NULL || reply == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint64(reply, &ioClientPtr) || ioClientPtr == 0) {
        HDF_LOGE("device svc dispatch: ioClient is null");
        return HDF_FAILURE;
    }
    HdfSbufFlush(reply);
    client = (struct HdfDeviceIoClient *)(uintptr_t)ioClientPtr;
    switch (cmdId) {
        case SVCMGR_REGISTER_LISTENER:
            return DevSvcManagerExtRegisterListener(client, data);
        case SVCMGR_UNREGISTER_LISTENER:
            return DevSvcManagerExtUnRegisterListener(client);
        default:
            break;
    }

    return HDF_ERR_NOT_SUPPORT;
}

void DeviceSvcMgrRelease(struct HdfDeviceIoClient *client)
{
    if (client == NULL) {
        return;
    }
    DevSvcManagerExtUnRegisterListener(client);
}

int DevSvcManagerExtStart(struct IDevSvcManager *svcmgr)
{
    struct DevSvcManagerExt *inst = (struct DevSvcManagerExt *)svcmgr;
    static struct HdfDeviceObject svcmgrDevObj = { 0 };
    static struct IDeviceIoService svcmgrIoService = {
        .Open = DeviceSvcMgrOpen,
        .Dispatch = NULL,
        .Release = DeviceSvcMgrRelease,
    };
    if (inst == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (inst->started) {
        return HDF_SUCCESS;
    }

    svcmgrDevObj.service = &svcmgrIoService;
    svcmgrDevObj.property = NULL;
    svcmgrDevObj.deviceClass = DEVICE_CLASS_DEFAULT;
    svcmgrDevObj.priv = (void *)svcmgr;

    inst->serv = HdfIoServicePublish(DEV_SVCMGR_NODE, SVC_MGR_NODE_PERM);
    if (inst->serv == NULL) {
        HDF_LOGE("failed to pushlish svcmgr ioservice");
    } else {
        static struct HdfIoDispatcher dispatcher = {
            .Dispatch = DeviceSvcMgrDispatch,
        };
        inst->serv->dispatcher = &dispatcher;
        inst->serv->target = (struct HdfObject *)&svcmgrDevObj;
    }

    ServStatListenerHolderinit();
    inst->started = true;
    return HDF_SUCCESS;
}

static bool DevSvcManagerExtConstruct(struct DevSvcManagerExt *inst)
{
    if (inst == NULL) {
        return false;
    }
    if (!DevSvcManagerConstruct(&inst->super)) {
        HDF_LOGE("failed to construct device service manager");
        return false;
    }
    if (OsalMutexInit(&inst->mutex) != HDF_SUCCESS) {
        HDF_LOGE("failed to create devsvcmgrext mutex");
        return false;
    }
    inst->super.super.StartService = DevSvcManagerExtStart;

    return true;
}

struct HdfObject *DevSvcManagerExtCreate(void)
{
    static struct DevSvcManagerExt *instance;
    if (instance != NULL) {
        return (struct HdfObject *)instance;
    }

    instance = OsalMemCalloc(sizeof(struct DevSvcManagerExt));
    if (!DevSvcManagerExtConstruct(instance)) {
        OsalMemFree(instance);
        instance = NULL;
    }

    return (struct HdfObject *)instance;
}

void DevSvcManagerExtRelease(struct IDevSvcManager *inst)
{
    struct DevSvcManagerExt *instance = (struct DevSvcManagerExt *)inst;
    if (instance == NULL) {
        return;
    }
    if (instance->serv != NULL) {
        HdfIoServiceRemove(instance->serv);
        instance->serv = NULL;
    }
    DevSvcManagerRelease(inst);
    instance->started = false;
}
