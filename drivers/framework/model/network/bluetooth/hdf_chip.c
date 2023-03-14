/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_chip.h"
#include "hdf_chip_config.h"
#include "hdf_device_desc.h"
#include "hdf_power.h"
#include "hdf_reset.h"
#include "osal/osal_mem.h"

struct HdfVirtualDevice *CreateVirtualDevice(struct HdfChipConfig *config) {
    struct HdfVirtualDevice *device = NULL;
    int32_t ret = HDF_SUCCESS;
    if (config == NULL) {
        return NULL;
    }
    device = (struct HdfVirtualDevice *)OsalMemCalloc(sizeof(struct HdfVirtualDevice));
    if (device == NULL) {
        return NULL;
    }
    do {
        device->name = config->name;

        device->power = CreateVirtualPower(config->powers);
        if (device->power == NULL) {
            ret = HDF_FAILURE;
            break;
        }

        device->reset = CreateVirtualReset(&config->reset);
        if (device->reset == NULL) {
            ret = HDF_FAILURE;
            break;
        }
    } while (false);

    if (ret != HDF_SUCCESS) {
        ReleaseVirtualDevice(device);
        device = NULL;
    }
    return device;
}
void ReleaseVirtualDevice(struct HdfVirtualDevice *device) {
    if (device == NULL) {
        return;
    }
    if (device->power != NULL && device->power->ops != NULL && device->power->ops->Release != NULL) {
        device->power->ops->Release(device->power);
        device->power = NULL;
    }

    if (device->reset != NULL && device->reset->ops != NULL && device->reset->ops->Release != NULL) {
        device->reset->ops->Release(device->reset);
        device->reset = NULL;
    }
    OsalMemFree(device);
}
