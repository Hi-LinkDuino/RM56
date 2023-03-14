/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_OPTION_H
#define HCS_COMPILER_OPTION_H
#include <stdbool.h>

bool HcsOptShouldAlign(void);

void HcsOptSetAlign(bool align);

bool HcsOptShouldGenTextConfig(void);

bool HcsOptShouldGenByteCodeConfig(void);

bool HcsOptDecompile(void);

bool HcsOptShouldGenHexdump(void);

const char *HcsOptGetSymbolNamePrefix(void);

bool HcsVerbosePrint(void);

#endif // HCS_COMPILER_OPTION_H
