/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_H
#define HCS_COMPILER_H

#include <stdint.h>
#include "hcs_types.h"
#include "hcs_log.h"
#include "hcs_parser.h"
#include "hcs_ast.h"
#include "hcs_mem.h"

#define HBC_MAGIC_NUM 0xA00AA00A
#define HCS_COMPILER_VERSION_MAJOR 00
#define HCS_COMPILER_VERSION_MINOR 66

typedef struct HbcHeader {
    uint32_t magicNumber;
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t checkSum;
    int32_t totalSize;
} HbcHeader;

/* Parse compiler options */
int32_t DoOption(int32_t argc, char *argv[]);

int32_t HcsDoCompile(void);

int32_t HcsDoOptimize(void);

#endif // HCS_COMPILER_H
