/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_GENER_H
#define HCS_COMPILER_GENER_H

#include <stdint.h>

int32_t HcsBytecodeOutput(void);
int32_t HcsTextCodeOutput(void);
int32_t HcsBinaryToHexdump(const char *inputFileName);

#endif // HCS_COMPILER_GENER_H
