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

#include <dlfcn.h>
#include <unistd.h>
#include "hdf_base.h"
#include "hdf_load_hdi.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG dev_load_hdi

#ifdef __ARM64__
#define HDI_SO_PATH HDF_LIBRARY_DIR"64"
#else
#define HDI_SO_PATH HDF_LIBRARY_DIR
#endif
struct HdiObject *LoadHdi(const char *name, uint32_t version)
{
    char path[PATH_MAX + 1] = {0};
    char resolvedPath[PATH_MAX + 1] = {0};

    if (name == NULL) {
        HDF_LOGE("name is NULL");
        return NULL;
    }

    if (snprintf_s(path, sizeof(path), sizeof(path) - 1, "%s/%s", HDI_SO_PATH, name) < 0) {
        HDF_LOGE("%{public}s snprintf_s failed", __func__);
        return NULL;
    }
    if (realpath(path, resolvedPath) == NULL) {
        HDF_LOGE("%{public}s file name invalid", __func__);
        return NULL;
    }

    struct HdiObject *hdi = (struct HdiObject *)OsalMemCalloc(sizeof(*hdi));
    if (hdi == NULL) {
        HDF_LOGE("%{public}s malloc failed", __func__);
        return NULL;
    }
    void *handler = dlopen(resolvedPath, RTLD_LAZY);
    if (handler == NULL) {
        HDF_LOGE("%{public}s dlopen failed %{public}s", __func__, dlerror());
        OsalMemFree(hdi);
        return NULL;
    }
    struct HdiBase *base = *(struct HdiBase **)dlsym(handler, "hdfHdiDesc");
    if (base == NULL) {
        HDF_LOGE("%{public}s dlsym failed %{public}s", __func__, dlerror());
        dlclose(handler);
        OsalMemFree(hdi);
        return NULL;
    }
    if (version != base->moduleVersion) {
        dlclose(handler);
        OsalMemFree(hdi);
        return NULL;
    }
    if (base->OpenHdi) {
        base->OpenHdi();
    }
    hdi->dlHandler = (uintptr_t)handler;
    hdi->hdiBase = base;
    return hdi;
}

void CloseHdi(struct HdiObject *hdi)
{
    if (hdi == NULL || hdi->dlHandler == 0 || hdi->hdiBase == NULL) {
        HDF_LOGE("%{public}s para invalid", __func__);
        return;
    }

    struct HdiBase *base = hdi->hdiBase;

    if (base->CloseHdi) {
        base->CloseHdi();
    }

    dlclose((void *)hdi->dlHandler);
    hdi->dlHandler = 0;
    hdi->hdiBase = NULL;
    OsalMemFree(hdi);
}

