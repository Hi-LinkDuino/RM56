/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_POWER_H
#define HDF_POWER_H
#include "hdf_chip.h"

struct HdfPower *CreateVirtualPower(const struct HdfPowersConfig *powers);

#endif