/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devmgr_service_clnt.h"
#include "hdf_base.h"
#include "hdf_device.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"

#define HDF_LOG_TAG devmgr_service_clnt

int DevmgrServiceClntAttachDeviceHost(uint16_t hostId, struct IDevHostService *hostService)
{
    struct IDevmgrService *devMgrSvcIf = NULL;
    struct DevmgrServiceClnt *inst = DevmgrServiceClntGetInstance();
    if (inst == NULL || inst->devMgrSvcIf == NULL) {
        HDF_LOGE("failed to attach device host, get device manager service client is null");
        return HDF_FAILURE;
    }

    devMgrSvcIf = inst->devMgrSvcIf;
    if (devMgrSvcIf->AttachDeviceHost == NULL) {
        HDF_LOGE("failed to attach device host, attach device host function is null");
        return HDF_FAILURE;
    }
    return devMgrSvcIf->AttachDeviceHost(devMgrSvcIf, hostId, hostService);
}

int DevmgrServiceClntAttachDevice(struct IHdfDeviceToken *deviceToken)
{
    struct IDevmgrService *devMgrSvcIf = NULL;
    struct DevmgrServiceClnt *inst = DevmgrServiceClntGetInstance();
    if ((inst == NULL) || (inst->devMgrSvcIf == NULL)) {
        HDF_LOGE("devmgr client failed to attach device, inst is null");
        return HDF_FAILURE;
    }

    devMgrSvcIf = inst->devMgrSvcIf;
    if (devMgrSvcIf->AttachDevice == NULL) {
        HDF_LOGE("devmgr client failed to attach device, dmsOps->AttachDevice is nul");
        return HDF_FAILURE;
    }
    return devMgrSvcIf->AttachDevice(devMgrSvcIf, deviceToken);
}

int DevmgrServiceClntDetachDevice(devid_t devid)
{
    struct IDevmgrService *devMgrSvcIf = NULL;
    struct DevmgrServiceClnt *inst = DevmgrServiceClntGetInstance();
    if (inst == NULL || inst->devMgrSvcIf == NULL) {
        HDF_LOGE("devmgr client failed to deatch device, inst is null");
        return HDF_FAILURE;
    }

    devMgrSvcIf = inst->devMgrSvcIf;
    if (devMgrSvcIf->DetachDevice == NULL) {
        return HDF_FAILURE;
    }
    return devMgrSvcIf->DetachDevice(devMgrSvcIf, devid);
}

struct DevmgrServiceClnt *DevmgrServiceClntGetInstance()
{
    static struct DevmgrServiceClnt instance = {0};
    if (instance.devMgrSvcIf == NULL) {
        instance.devMgrSvcIf = (struct IDevmgrService *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVMGR_SERVICE);
    }
    return &instance;
}

void DevmgrServiceClntFreeInstance(struct DevmgrServiceClnt *inst)
{
    if ((inst != NULL) && (inst->devMgrSvcIf != NULL)) {
        HdfObjectManagerFreeObject((struct HdfObject *)inst->devMgrSvcIf);
        inst->devMgrSvcIf = NULL;
    }
}
