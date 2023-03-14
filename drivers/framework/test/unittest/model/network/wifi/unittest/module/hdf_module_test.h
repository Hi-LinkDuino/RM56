/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_LITE_WIFI_MODULE_TEST_H
#define HDF_LITE_WIFI_MODULE_TEST_H

#include "hdf_base.h"

int32_t WiFiModuleTestCreateModule(void);
int32_t WiFiModuleTestDeleteModule(void);
int32_t WiFiModuleTestUpdateModule(void);

int32_t WiFiModuleTestAddFeature(void);
int32_t WiFiModuleTestDelFeature(void);
int32_t WiFiModuleTestGetFeature(void);

#endif // HDF_LITE_WIFI_MODULE_TEST_H
