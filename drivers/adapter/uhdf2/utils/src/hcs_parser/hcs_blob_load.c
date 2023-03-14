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

#include "hcs_blob_load.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hcs_blob_if.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hcs_blob_load
#define HBC_FILE_EXT ".hcb"

static bool IsHcbFile(const char *fname)
{
    char *hcbExt = strstr(fname, HBC_FILE_EXT);
    if (hcbExt == NULL) {
        return false;
    }
    return true;
}

uint32_t OpenHcsBlobFile(const char *hcsBlobPath, char **hcsBlob)
{
    // the 0 is error.
    int32_t length = 0;
    if ((hcsBlobPath == NULL) || (hcsBlob == NULL) || !IsHcbFile(hcsBlobPath)) {
        HDF_LOGE("%{public}s failed, pls check the param", __func__);
        return length;
    }

    char path[PATH_MAX] = { 0 };
    if (realpath(hcsBlobPath, path) == NULL) {
        HDF_LOGE("file %{public}s is invalid", hcsBlobPath);
        return length;
    }
    FILE *fp = fopen(path, "rb");
    do {
        if (fp == NULL) {
            HDF_LOGE("%{public}s failed, pls check the path of %{public}s", __func__, hcsBlobPath);
            break;
        }
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        if ((length <= 0) || (length >= HBC_BLOB_MAX_LENGTH)) {
            length = 0;
            HDF_LOGE("%{public}s failed, the HcsBlob file length is %{public}d",  __func__, length);
            break;
        }
        *hcsBlob = (char *)OsalMemCalloc(length + 1);
        if ((*hcsBlob) == NULL) {
            length = 0;
            HDF_LOGE("%{public}s failed, OsalMemCalloc hcsBlob memory failed", __func__);
            break;
        }
        fseek(fp, 0, SEEK_SET);
        (void)fread((void *)(*hcsBlob), length, 1, fp);
    } while (0);
    if (fp != NULL) {
        fclose(fp);
    }
    return (length > 0) ? length : 0;
}
