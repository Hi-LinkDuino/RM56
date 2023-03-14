/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DSOFTBUS_DRIVER_H
#define HDF_DSOFTBUS_DRIVER_H

#include "hdf_base.h"
#include "hdf_sbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HdfSoftbusBroadcastEvent(uint32_t moudleId, const struct HdfSBuf *data);

#ifdef __cplusplus
}
#endif

#endif // HDF_DSOFTBUS_DRIVER_H