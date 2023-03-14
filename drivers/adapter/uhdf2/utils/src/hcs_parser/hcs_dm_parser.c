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

#include "hcs_dm_parser.h"
#include <pthread.h>
#include "hcs_blob_if.h"
#include "hcs_blob_load.h"
#include "hcs_parser.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hcs_dm_parser
static char *g_hcsBlob = NULL;
static struct DeviceResourceNode *g_hcsTreeRoot = NULL;
static const char *g_hcsBlobPath = NULL;
static pthread_mutex_t g_getDmRootMutex = PTHREAD_MUTEX_INITIALIZER;

void ReleaseHcsTree(void)
{
    OsalMemFree(g_hcsTreeRoot);
    g_hcsTreeRoot = NULL;
    OsalMemFree(g_hcsBlob);
    g_hcsBlob = NULL;
}

void SetHcsBlobPath(const char *path)
{
    if (g_hcsBlobPath != NULL) {
        OsalMemFree((void *)g_hcsBlobPath);
    }
    g_hcsBlobPath = strdup(path);
}

static bool CreateHcsToTree(void)
{
    bool ret = true;
    do {
        uint32_t length = OpenHcsBlobFile(g_hcsBlobPath, &g_hcsBlob);
        if (length == 0) {
            ret = false;
            break;
        }
        if (!HcsCheckBlobFormat(g_hcsBlob, length)) {
            ret = false;
            break;
        }
        if (!HcsDecompile((const char *)g_hcsBlob, HBC_HEADER_LENGTH, &g_hcsTreeRoot)) {
            ret = false;
            break;
        }
    } while (0);

    if (!ret) {
        ReleaseHcsTree();
    }
    return ret;
}

const struct DeviceResourceNode *HcsGetRootNode(void)
{
    if (g_hcsTreeRoot == NULL) {
        pthread_mutex_lock(&g_getDmRootMutex);
        if ((g_hcsTreeRoot == NULL) && !CreateHcsToTree()) {
            pthread_mutex_unlock(&g_getDmRootMutex);
            HDF_LOGE("%{public}s is failed", __func__);
            return NULL;
        }
        pthread_mutex_unlock(&g_getDmRootMutex);
    }
    return g_hcsTreeRoot;
}