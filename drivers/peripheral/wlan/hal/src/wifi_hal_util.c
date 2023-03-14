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

#include "wifi_hal_util.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

OSAL_DECLARE_MUTEX(g_mutex);
static bool g_isInitMutex = false;

int32_t HalMutexInit()
{
    int32_t ret = HDF_SUCCESS;
    if (!g_isInitMutex) {
        ret = OsalMutexInit(&g_mutex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: OsalMutexInit failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
            return ret;
        }
        g_isInitMutex = true;
    }
    return ret;
}

int32_t HalMutexDestroy()
{
    int32_t ret = HDF_SUCCESS;
    if (g_isInitMutex) {
        ret = OsalMutexDestroy(&g_mutex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: OsalMutexDestroy failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
            return ret;
        }
        g_isInitMutex = false;
    }
    return ret;
}

void HalMutexLock()
{
    if (OsalMutexLock(&g_mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock failed, line: %d\n", __FUNCTION__, __LINE__);
    }
}

void HalMutexUnlock()
{
    if (OsalMutexUnlock(&g_mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexUnlock failed, line: %d\n", __FUNCTION__, __LINE__);
    }
}