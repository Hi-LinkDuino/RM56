/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SDIO_INTF_H
#define HDF_SDIO_INTF_H

#include "hdf_base.h"
#include "hdf_ibus_intf.h"
#include "hdf_log.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int32_t HdfSdioBusInit(struct BusDev *dev, const struct HdfConfigWlanBus *busConfig);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

