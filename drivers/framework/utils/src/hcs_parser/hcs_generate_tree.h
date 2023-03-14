/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_GENERATE_TREE_H
#define HCS_GENERATE_TREE_H

#include "device_resource_if.h"

#define TREE_STACK_MAX 64
struct TreeStack {
    uint32_t offset; // The offset of the node in the blob.
    struct DeviceResourceNode *node; // The head node of a layer tree.
};
int32_t GenerateCfgTree(const char *treeStart, int32_t length, char *treeMem, struct DeviceResourceNode **root);

#endif /* HCS_GENERATE_TREE_H */