/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "wifi_module.h"
#include "wifi_base.h"
#include "ap.h"
#include "sta.h"
#include "p2p.h"
#include "hdf_wlan_config.h"
#include "securec.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

#ifdef __cplusplus
extern "C" {
#endif

int32_t DelFeature(struct WifiModule *module, uint16_t featureType);
int32_t AddFeature(struct WifiModule *module, uint16_t featureType, struct WifiFeature *featureData);

static int32_t InitFeatures(struct WifiModule *module)
{
    int32_t ret;
    uint32_t i;

    if (module == NULL) {
        HDF_LOGE("%s: input module is NULL", __func__);
        return HDF_FAILURE;
    }
    ret = BaseInit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:BaseInit failed!ret=%d", __func__, ret);
        return ret;
    }

    module->feList.fe[HDF_WIFI_FEATURE_AP] = GetWifiApFeature();
    module->feList.fe[HDF_WIFI_FEATURE_STA] = GetWifiStaFeature();
    module->feList.fe[HDF_WIFI_FEATURE_P2P] = GetWifiP2pFeature();

    for (i = 0; i < HDF_WIFI_FEATURE_NUM; i++) {
        if ((module->moduleConfig.hslConfig->featureMap & (1 << i)) && module->feList.fe[i] != NULL) {
            module->feList.fe[i]->init(module->feList.fe[i]);
        }
    }
    return HDF_SUCCESS;
}

static int32_t DeInitFeatures(struct WifiModule *module)
{
    int32_t ret;
    uint32_t i;
    if (module == NULL) {
        HDF_LOGE("%s: no  module", __func__);
        return HDF_FAILURE;
    }
    // make sure the features should be free firstly
    for (i = 0; i < HDF_WIFI_FEATURE_NUM; i++) {
        if ((module->feList.fe[i] != NULL) && (module->feList.fe[i]->deInit != NULL)) {
            module->feList.fe[i]->deInit(module->feList.fe[i]);
            module->feList.fe[i] = NULL;
        }
    }

    ret = BaseDeinit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:BaseDeinit failed!ret=%d", __func__, ret);
    }
    return ret;
}

int16_t InitWifiModule(struct WifiModule *module, const struct HdfConfigWlanModuleConfig *config)
{
    int32_t ret;
    if (module == NULL || config == NULL) {
        HDF_LOGE("%s:Input is NULL!", __func__);
        return HDF_FAILURE;
    }
    module->iface.deInit = DeInitFeatures;
    module->iface.addFeature = AddFeature;
    module->iface.delFeature = DelFeature;
    module->modulePrivate = NULL;
    module->moduleConfig.hslConfig = config;

    ret = InitFeatures(module);
    if (ret != 0) {
        HDF_LOGE("%s: module int error ret=%d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s:hdf wifi module init succ", __func__);
    return HDF_SUCCESS;
}

#ifdef __cplusplus
}
#endif
