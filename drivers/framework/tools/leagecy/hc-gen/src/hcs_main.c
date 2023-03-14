/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_compiler.h"
#include "hcs_decompiler.h"
#include "hcs_option.h"

int32_t main(int32_t argc, char *argv[])
{
    int32_t status = DoOption(argc, argv);
    if (status != NOERR) {
        return status;
    }

    if (HcsOptDecompile()) {
        status = HcsDoDecompile();
    } else {
        status = HcsDoCompile();
    }

    return status ? EFAIL : NOERR;
}
