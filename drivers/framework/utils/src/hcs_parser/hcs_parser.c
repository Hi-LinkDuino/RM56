/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_parser.h"
#include "hcs_blob_if.h"
#include "hcs_generate_tree.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hcs_parser

static int32_t GetHcsTreeSize(const char *blob, int32_t nodeLength)
{
    return GenerateCfgTree(blob, nodeLength, NULL, NULL);
}

bool HcsDecompile(const char *hcsBlob, uint32_t offset, struct DeviceResourceNode **root)
{
    int32_t nodeLength = HcsGetNodeLength(hcsBlob + offset);
    int32_t treeMemLength;
    char *treeMem = NULL;
    int32_t treeLayer;
    if (nodeLength < 0) {
        HDF_LOGE("%s failed, HcsGetNodeLength error", __func__);
        return false;
    }

    treeMemLength = GetHcsTreeSize(hcsBlob + offset, nodeLength);
    if (treeMemLength <= 0) {
        HDF_LOGE("%s failed, GetHcsTreeSize error, treeMemLength = %d", __func__, treeMemLength);
        return false;
    }

    treeMem = (char *)OsalMemCalloc(treeMemLength);
    if (treeMem == NULL) {
        HDF_LOGE("%s failed, OsalMemCalloc error", __func__);
        return false;
    }
    treeLayer = GenerateCfgTree(hcsBlob + offset, nodeLength, treeMem, root);
    if (treeLayer <= 0) {
        HDF_LOGE("%s failed, the treeLayer is %d", __func__, treeLayer);
        OsalMemFree(treeMem);
        *root = NULL;
        return false;
    }
    return true;
}