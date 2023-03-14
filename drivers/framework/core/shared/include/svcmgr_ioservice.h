/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVSVC_SVCMGR_IOSERVICE_H
#define DEVSVC_SVCMGR_IOSERVICE_H

#include "hdf_base.h"
#include "hdf_service_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEV_SVCMGR_NODE "devsvc_mgr"

enum SvcMgrIoCmd {
    SVCMGR_REGISTER_LISTENER,
    SVCMGR_UNREGISTER_LISTENER,
};

struct ISvcMgrIoservice {
    int32_t (*RegisterServiceStatusListener)(struct ISvcMgrIoservice *self,
        struct ServiceStatusListener *listener, uint16_t deviceClass);
    int32_t (*UnregisterServiceStatusListener)(struct ISvcMgrIoservice *self,
        struct ServiceStatusListener *listener);
};

struct ISvcMgrIoservice *SvcMgrIoserviceGet(void);
void SvcMgrIoserviceRelease(struct ISvcMgrIoservice *svcmgr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // DEVSVC_SVCMGR_IOSERVICE_H