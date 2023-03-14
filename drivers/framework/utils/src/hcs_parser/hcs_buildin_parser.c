/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_blob_if.h"
#include "hcs_parser.h"
#include "hcs_tree_if.h"
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hcs_parser

static struct DeviceResourceNode *g_hcsTreeRoot = NULL;

void __attribute__((weak)) HdfGetBuildInConfigData(const unsigned char **data, unsigned int *size);

static bool BuildHcsTree(void)
{
    uint32_t length;
    const unsigned char *hcsBlob = NULL;
    if (HdfGetBuildInConfigData == NULL) {
        HDF_LOGE("no build-in hdf config");
        return false;
    }
    HdfGetBuildInConfigData(&hcsBlob, &length);
    if (!HcsCheckBlobFormat((const char *)hcsBlob, length)) {
        return false;
    }
    if (!HcsDecompile((const char *)hcsBlob, HBC_HEADER_LENGTH, &g_hcsTreeRoot)) {
        return false;
    }
    return true;
}

const struct DeviceResourceNode *HcsGetRootNode(void)
{
    if (g_hcsTreeRoot == NULL && !BuildHcsTree()) {
        HDF_LOGE("failed to rebuild config tree");
        return NULL;
    }

    if (g_hcsTreeRoot == NULL) {
        HDF_LOGE("failed to get build-in hcs root node");
    }
    return g_hcsTreeRoot;
}