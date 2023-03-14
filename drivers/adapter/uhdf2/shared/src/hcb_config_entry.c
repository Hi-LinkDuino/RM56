/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <securec.h>
#include "hcs_dm_parser.h"
#include "hcs_tree_if.h"
#include "hdf_attribute_manager.h"
#include "hdf_log.h"

#define HDF_LOG_TAG attribute_manager

#ifdef __OHOS_STANDARD_SYS__
#define HOST_CONFIG_PATH HDF_CONFIG_DIR
#else
#define HOST_CONFIG_PATH "/system/etc/hdfconfig"
#endif
#define PRODUCT_PROPERTY "ro.build.product"
#define PRODUCT_NAME_MAX 128

static int GetProductName(char *name, int maxLen)
{
    return  strcpy_s(name, maxLen, "default");
}

const struct DeviceResourceNode *HdfGetHcsRootNode(void)
{
    char productName[PRODUCT_NAME_MAX] = { 0 };
    char configPath[PATH_MAX] = { 0 };

    int ret = GetProductName(productName, PRODUCT_NAME_MAX);
    if (ret != HDF_SUCCESS) {
        return NULL;
    }

    ret = sprintf_s(configPath, PATH_MAX - 1, "%s/hdf_%s.hcb", HOST_CONFIG_PATH, productName);
    if (ret < 0) {
        HDF_LOGE("config path error");
        return NULL;
    }

    const char *configFileName = configPath;
    SetHcsBlobPath(configFileName);
    const struct DeviceResourceNode *mgrRoot = HcsGetRootNode();
    return mgrRoot;
}
