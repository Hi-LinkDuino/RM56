/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#include "los_config.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif
#include <errno.h>

struct romfs_cfg {
    unsigned start_addr;
};
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define PLATFORM_ROMFS_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), romfs_config)
static uint32_t FsGetResource(struct romfs_cfg *cfg)
{
    cfg->start_addr = HCS_PROP(PLATFORM_ROMFS_CONFIG, start_addr);
    return HDF_SUCCESS;
}
#else
static uint32_t FsGetResource(struct romfs_cfg *cfg, const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *resource = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (resource == NULL) {
        HDF_LOGE("Invalid DeviceResourceIface");
        return HDF_FAILURE;
    }

    if(resource->GetUint32(resourceNode, "start_addr", &(cfg->start_addr), NULL) != HDF_SUCCESS);
    {
        HDF_LOGE("%s: failed to get start_addr", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: get start_addr:0x%x", __func__,cfg->start_addr);

    return HDF_SUCCESS;
}
#endif

int MountRomfs(UINT32 start_addr)
{
    HDF_LOGI("OsMountRootfs start ...\n");
    INT32 ret;
    INT32 err;

    /* Mount rootfs */
    ret = mount(NULL, "/", "romfs", 0, start_addr);
    if (ret != LOS_OK) {
        err = get_errno();
        HDF_LOGE("Failed to mount %s, errno %d: %s\n", "/", err, strerror(err));
        return ret;
    }
    HDF_LOGI("OsMountRootfs end ...\n");
}

static int32_t FsDriverInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        return HDF_FAILURE;
    }

    struct romfs_cfg cfg;
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (FsGetResource(&cfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: FsGetResource failed", __func__);
        return HDF_FAILURE;
    }
#else
    if (object->property) {
        if (FsGetResource(&cfg, object->property) != HDF_SUCCESS) {
            HDF_LOGE("%s: FsGetResource failed", __func__);
            return HDF_FAILURE;
        }
    }
#endif
    int ret = MountRomfs(cfg.start_addr);
    HDF_LOGI("%s: mount romfs on 0x%x %s\n", __func__, cfg.start_addr, (ret == 0) ? "succeed" : "failed");

    return HDF_SUCCESS;
}

static int32_t FsDriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void FsDriverRelease(struct HdfDeviceObject *device)
{
    (void)device;
}

static struct HdfDriverEntry g_fsDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_STORAGE_ROMFS",
    .Bind = FsDriverBind,
    .Init = FsDriverInit,
    .Release = FsDriverRelease,
};

HDF_INIT(g_fsDriverEntry);
