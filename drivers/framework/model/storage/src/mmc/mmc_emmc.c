/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mmc_emmc.h"
#include "securec.h"

#define HDF_LOG_TAG mmc_emmc_c

static int32_t EmmcDeviceDefaultGetCid(struct EmmcDevice *dev, uint8_t *cid, uint32_t len)
{
    struct MmcDevice *mmc = (struct MmcDevice *)dev;

    if (memcpy_s(cid, sizeof(uint8_t) * len, (uint8_t *)(mmc->reg.rawCid),
        sizeof(mmc->reg.rawCid)) != EOK) {
        HDF_LOGE("EmmcDeviceDefaultGetCid: memcpy_s fail, size = %d!", len);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct EmmcDeviceOps g_emmcOps = {
    .getCid = EmmcDeviceDefaultGetCid,
};

int32_t EmmcDeviceGetCid(struct EmmcDevice *dev, uint8_t *cid, uint32_t len)
{
    if (dev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cid == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (dev->emmcOps == NULL || dev->emmcOps->getCid == NULL) {
        HDF_LOGE("EmmcDeviceGetCid: ops or getCid is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return dev->emmcOps->getCid(dev, cid, len);
}

void EmmcDeviceAddOps(struct EmmcDevice *dev, struct EmmcDeviceOps *ops)
{
    if (dev == NULL) {
        HDF_LOGE("EmmcDeviceAddOps: dev is NULL.");
        return;
    }
    if (ops == NULL) {
        dev->emmcOps = &g_emmcOps;
        HDF_LOGD("EmmcDeviceAddOps: use default ops.");
    } else {
        dev->emmcOps = ops;
    }
}
