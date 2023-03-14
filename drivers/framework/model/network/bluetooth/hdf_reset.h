/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_RESET_H
#define HDF_RESET_H
#include "hdf_chip.h"
#include "hdf_chip_config.h"

struct HdfReset *CreateVirtualReset(const struct HdfResetConfig *powers);

#endif