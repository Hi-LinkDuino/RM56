/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_PLATFORM_CORE_ENTRY_TEST_H
#define HDF_PLATFORM_CORE_ENTRY_TEST_H

#include "hdf_main_test.h"

int32_t HdfPlatformEventTestEntry(HdfTestMsg *msg);

int32_t HdfPlatformQueueTestEntry(HdfTestMsg *msg);

int32_t HdfPlatformDeviceTestEntry(HdfTestMsg *msg);

int32_t HdfPlatformManagerTestEntry(HdfTestMsg *msg);

#endif // HDF_PLATFORM_CORE_ENTRY_TEST_H
