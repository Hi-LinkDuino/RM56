/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_tree_if.h"

const struct DeviceResourceNode *HdfGetHcsRootNode(void)
{
    return HcsGetRootNode();
}