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

#include "wifi_hal_ap_feature.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "securec.h"
#include "wifi_hal_cmd.h"
#include "wifi_hal_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static int32_t GetAsscociatedStasInner(const struct IWiFiAp *apFeature,
    struct StaInfo *staInfo, uint32_t size, uint32_t *num)
{
    if (apFeature == NULL || staInfo == NULL || size == 0 || num == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetAsscociatedStas(apFeature->baseFeature.ifName, staInfo, size, num);
}

static int32_t SetCountryCodeInner(const struct IWiFiAp *apFeature, const char *code, uint32_t len)
{
    if (apFeature == NULL || code == NULL || len != strlen(code)) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdSetCountryCode(apFeature->baseFeature.ifName, code, len);
}

static int32_t GetAsscociatedStas(const struct IWiFiAp *apFeature,
    struct StaInfo *staInfo, uint32_t count, uint32_t *num)
{
    HalMutexLock();
    int32_t ret = GetAsscociatedStasInner(apFeature, staInfo, count, num);
    HalMutexUnlock();
    return ret;
}

static int32_t HalSetCountryCode(const struct IWiFiAp *apFeature, const char *code, uint32_t len)
{
    HalMutexLock();
    int32_t ret = SetCountryCodeInner(apFeature, code, len);
    HalMutexUnlock();
    return ret;
}

int32_t InitApFeature(struct IWiFiAp **fe)
{
    if (fe == NULL || *fe == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (InitBaseFeature((struct IWiFiBaseFeature **)fe) != HDF_SUCCESS) {
        HDF_LOGE("%s: init base feature, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    (*fe)->getAsscociatedStas = GetAsscociatedStas;
    (*fe)->setCountryCode = HalSetCountryCode;
    return HDF_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif