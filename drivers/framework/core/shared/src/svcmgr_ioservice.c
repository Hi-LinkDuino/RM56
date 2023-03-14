/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "svcmgr_ioservice.h"
#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "ioservstat_listener.h"
#include "osal_mem.h"

struct SvcMgrIoservice {
    struct ISvcMgrIoservice svcmgr;
    struct HdfIoService *iosvc;
    struct DListHead listeners;
};

static int32_t SetListenClass(struct SvcMgrIoservice *svcmgrInst, uint16_t devClass)
{
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    (void)HdfSbufWriteUint16(data, devClass);
    if (svcmgrInst->iosvc == NULL || svcmgrInst->iosvc->dispatcher == NULL
        || svcmgrInst->iosvc->dispatcher->Dispatch == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return svcmgrInst->iosvc->dispatcher->Dispatch(
        (struct HdfObject *)svcmgrInst->iosvc, SVCMGR_REGISTER_LISTENER, data, NULL);
}

static int32_t UnSetListenClass(struct SvcMgrIoservice *svcmgrInst, uint16_t devClass)
{
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    (void)HdfSbufWriteUint16(data, devClass);
    if (svcmgrInst->iosvc == NULL || svcmgrInst->iosvc->dispatcher == NULL
        || svcmgrInst->iosvc->dispatcher->Dispatch == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return svcmgrInst->iosvc->dispatcher->Dispatch(
        (struct HdfObject *)svcmgrInst->iosvc, SVCMGR_UNREGISTER_LISTENER, data, NULL);
}

int32_t SvcMgrIoserviceRegSvcStatListener(
    struct ISvcMgrIoservice *self, struct ServiceStatusListener *listener, uint16_t deviceClass)
{
    if (self == NULL || listener == NULL || deviceClass >= DEVICE_CLASS_MAX) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct SvcMgrIoservice *svcmgrInst = CONTAINER_OF(self, struct SvcMgrIoservice, svcmgr);
    struct IoServiceStatusListener *listenerInst
        = CONTAINER_OF(listener, struct IoServiceStatusListener, svcstatListener);

    listenerInst->deviceClass = deviceClass;
    int ret = SetListenClass(svcmgrInst, deviceClass);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to set listen class");
        return ret;
    }

    return HdfDeviceRegisterEventListener(svcmgrInst->iosvc, &listenerInst->ioservListener);
}

int32_t SvcMgrIoserviceUnRegSvcStatListener(struct ISvcMgrIoservice *self, struct ServiceStatusListener *listener)
{
    if (self == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct SvcMgrIoservice *svcmgrInst = CONTAINER_OF(self, struct SvcMgrIoservice, svcmgr);
    struct IoServiceStatusListener *listenerInst
        = CONTAINER_OF(listener, struct IoServiceStatusListener, svcstatListener);

    int ret = HdfDeviceUnregisterEventListener(svcmgrInst->iosvc, &listenerInst->ioservListener);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (HdfIoserviceGetListenerCount(svcmgrInst->iosvc) == 0) {
        ret = UnSetListenClass(svcmgrInst, listenerInst->deviceClass);
    }

    return ret;
}

static void SvcMgrIoserviceConstruct(struct ISvcMgrIoservice *svcmgrInst)
{
    svcmgrInst->RegisterServiceStatusListener = SvcMgrIoserviceRegSvcStatListener;
    svcmgrInst->UnregisterServiceStatusListener = SvcMgrIoserviceUnRegSvcStatListener;
}

struct ISvcMgrIoservice *SvcMgrIoserviceGet(void)
{
    struct SvcMgrIoservice *svcmgrInst = OsalMemCalloc(sizeof(struct SvcMgrIoservice));
    if (svcmgrInst == NULL) {
        return NULL;
    }

    svcmgrInst->iosvc = HdfIoServiceBind(DEV_SVCMGR_NODE);
    if (svcmgrInst->iosvc == NULL) {
        HDF_LOGE("ioserivce %s not exist", DEV_SVCMGR_NODE);
        OsalMemFree(svcmgrInst);
        return NULL;
    }
    SvcMgrIoserviceConstruct(&svcmgrInst->svcmgr);
    return &svcmgrInst->svcmgr;
}

void SvcMgrIoserviceRelease(struct ISvcMgrIoservice *svcmgr)
{
    if (svcmgr == NULL) {
        return;
    }
    struct SvcMgrIoservice *svcmgrInst = CONTAINER_OF(svcmgr, struct SvcMgrIoservice, svcmgr);
    HdfIoServiceRecycle(svcmgrInst->iosvc);
    OsalMemFree(svcmgrInst);
}