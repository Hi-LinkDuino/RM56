/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_WIFI_CORE_H
#define HDF_WIFI_CORE_H

#include "hdf_base.h"
#include "hdf_wifi_product.h"

#ifdef __cplusplus
extern "C" {
#endif
int32_t HdfWifiGetBusIdx(void);

int32_t HdfWifiDeinitDevice(struct HdfWlanDevice *device);

int32_t HdfWifiInitDevice(struct HdfWlanDevice *device);

#ifdef __cplusplus
}
#endif

#endif // HDF_WIFI_PRODUCT_H
