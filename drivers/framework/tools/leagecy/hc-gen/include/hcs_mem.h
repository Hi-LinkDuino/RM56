/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_MEM_H
#define HCS_COMPILER_MEM_H

#include <stdint.h>

void *HcsMemAlloc(uint32_t size);

void *HcsMemZalloc(uint32_t size);

void HcsMemFree(void *ptr);

#endif // HCS_COMPILER_MEM_H
