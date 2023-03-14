/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hdf_device_node_ext.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_io_service.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"

#define HDF_LOG_TAG device_node_ext

static int DeviceNodeExtPublishService(struct HdfDeviceNode *inst)
{
    struct DeviceNodeExt *devNodeExt = (struct DeviceNodeExt *)inst;
    if (devNodeExt == NULL) {
        return HDF_FAILURE;
    }

    int ret = HdfDeviceNodePublishPublicService(inst);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Device publish service failed, ret is: %d", ret);
    }

    return ret;
}

static void DeviceNodeExtConstruct(struct DeviceNodeExt *inst)
{
    struct IDeviceNode *nodeIf = (struct IDeviceNode *)inst;
    if (nodeIf != NULL) {
        HdfDeviceNodeConstruct(&inst->super);
        nodeIf->PublishService = DeviceNodeExtPublishService;
    }
}

struct HdfObject *DeviceNodeExtCreate(void)
{
    struct DeviceNodeExt *instance =
        (struct DeviceNodeExt *)OsalMemCalloc(sizeof(struct DeviceNodeExt));
    if (instance != NULL) {
        DeviceNodeExtConstruct(instance);
        instance->ioService = NULL;
    }
    return (struct HdfObject *)instance;
}

void DeviceNodeExtRelease(struct HdfObject *object)
{
    struct DeviceNodeExt *instance = (struct DeviceNodeExt *)object;
    if (instance != NULL) {
        HdfDeviceNodeDestruct(&instance->super);
        OsalMemFree(instance);
    }
}

