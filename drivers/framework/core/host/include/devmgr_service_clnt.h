/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVMGR_SERVICE_CLIENT_H
#define DEVMGR_SERVICE_CLIENT_H

#include "devhost_service_if.h"
#include "devmgr_service_if.h"
#include "hdf_device_token.h"

struct DevmgrServiceClnt {
    struct IDevmgrService *devMgrSvcIf;
};

struct DevmgrServiceClnt *DevmgrServiceClntGetInstance(void);
void DevmgrServiceClntFreeInstance(struct DevmgrServiceClnt *inst);
int DevmgrServiceClntAttachDevice(struct IHdfDeviceToken *deviceToken);
int DevmgrServiceClntDetachDevice(devid_t devid);
int DevmgrServiceClntAttachDeviceHost(uint16_t hostId, struct IDevHostService *hostService);

#endif /* DEVMGR_SERVICE_CLIENT_H */
