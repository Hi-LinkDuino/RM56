/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_module_test.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_wifi_product.h"
#include "ap.h"

struct HdfConfigWlanModuleConfig *g_hdfWlanModuleCfg = NULL;
struct WifiModule *g_hdfWifiModule = NULL;

struct HdfConfigWlanRoot g_moduleData1 = {
    .wlanConfig.moduleConfig = {
        .featureMap = 0xffff,
        .station = {
            .name = "sta",
            .mode = 0x1,
        },
        .hostAp = {
            .name = "ap",
            .mode = 0x0,
            .vapResNum = 0x1,
            .userResNum = 0x8,
        },
        .p2p = {
            .name = "p2p",
            .mode = 0x0,
        },
    }
};

static int32_t P2pInit(struct WifiFeature *feature)
{
    (void)feature;
    return HDF_SUCCESS;
}

static int32_t P2pDeinit(struct WifiFeature *feature)
{
    (void)feature;
    return HDF_SUCCESS;
}
struct WifiFeature g_p2pFeature = {
    .name = "p2p",
    .init = P2pInit,
    .deInit = P2pDeinit,
};

static int32_t WiFiModuleTestSetEnv(void)
{
    g_hdfWlanModuleCfg = &g_moduleData1.wlanConfig.moduleConfig;
    if (g_hdfWifiModule == NULL) {
        int ret;
        g_hdfWifiModule = OsalMemCalloc(sizeof(struct WifiModule));
        if (g_hdfWifiModule == NULL) {
            HDF_LOGE("%s:oom", __func__);
            return HDF_FAILURE;
        }
        ret = InitWifiModule(g_hdfWifiModule, g_hdfWlanModuleCfg);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Init WifiModule failed!ret=%d", __func__, ret);
            OsalMemFree(g_hdfWifiModule);
            g_hdfWifiModule = NULL;
            return ret;
        }
    }
    return HDF_SUCCESS;
}

int32_t WiFiModuleTestCreateModule(void)
{
    int32_t ret;
    struct WifiModule *module = NULL;
    ret = WiFiModuleTestSetEnv();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:WiFiModuleTestSetEnv failed.ret=%d", __func__, ret);
        return ret;
    }
    module = OsalMemCalloc(sizeof(struct WifiModule));
    if (module == NULL) {
        HDF_LOGE("%s fail WifiModuleCreate FAIL ", __func__);
        return HDF_FAILURE;
    }

    ret = InitWifiModule(module, g_hdfWlanModuleCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:Init WifiModule failed!ret=%d", __func__, ret);
        OsalMemFree(module);
        return ret;
    }
    OsalMemFree(module);
    module = NULL;

    return HDF_SUCCESS;
}

int32_t WiFiModuleTestAddFeature(void)
{
    struct WifiModule *module = HdfWlanGetModule();
    if (module == NULL) {
        HDF_LOGE("%s:Cannot get module!", __func__);
        return HDF_FAILURE;
    }
    if (module->iface.addFeature == NULL) {
        HDF_LOGE("%s:bad module found!", __func__);
        return HDF_FAILURE;
    }
    return module->iface.addFeature(module, HDF_WIFI_FEATURE_P2P, &g_p2pFeature);
}

int32_t WiFiModuleTestDelFeature(void)
{
    struct WifiModule *module = HdfWlanGetModule();
    if (module == NULL) {
        HDF_LOGE("%s:Cannot get module!", __func__);
        return HDF_FAILURE;
    }
    if (module->iface.delFeature == NULL) {
        HDF_LOGE("%s:bad module found!", __func__);
        return HDF_FAILURE;
    }
    return module->iface.delFeature(module, HDF_WIFI_FEATURE_P2P);
}

int32_t WiFiModuleTestDeleteModule(void)
{
    int ret = WiFiModuleTestSetEnv();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:Set env failed!", __func__);
        return ret;
    }
    OsalMemFree(g_hdfWifiModule);
    g_hdfWifiModule = NULL;
    return HDF_SUCCESS;
}