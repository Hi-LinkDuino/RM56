/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "module_manager.h"

#include "hdf_log.h"
#ifdef ENABLE_LWIP_MONITOR
#include "softbus_lwip_monitor.h"
#endif
#ifdef ENABLE_WLAN_PARAM_MONITOR
#include "wlan_param_monitor.h"
#endif

#define HDF_LOG_TAG "hdf_dsoftbus"

static SoftbusDriverModule g_modules[] = {
#ifdef ENABLE_LWIP_MONITOR
    {
        .init = SoftbusLwipMonitorInit,
        .deinit = NULL,
        .process = NULL,
    },
#endif
#ifdef ENABLE_WLAN_PARAM_MONITOR
    {
        .init = SoftbusWlanParamMonitorInit,
        .deinit = NULL,
        .process = SoftbusWlanParamMonitorProcess,
    },
#endif
};

void SoftbusDispatchModuleCommand(int32_t moduleId, const struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t i;

    for (i = 0; i < HDF_ARRAY_SIZE(g_modules); ++i) {
        if (g_modules[i].moduleId != moduleId) {
            continue;
        }
        if (g_modules[i].process == NULL) {
            HDF_LOGE("module(%d) no process function", moduleId);
            break;
        }
        g_modules[i].process(reqData, rspData);
        return;
    }
    HDF_LOGE("no moduleId: %d process command", moduleId);
}

int32_t SoftbusModuleManagerInit(struct HdfDeviceObject *device)
{
    int32_t i;

    for (i = 0; i < HDF_ARRAY_SIZE(g_modules); ++i) {
        if (g_modules[i].init == NULL) {
            HDF_LOGE("module(%d) is no init function", g_modules[i].moduleId);
            break;
        }
        if (g_modules[i].init(device) != HDF_SUCCESS) {
            HDF_LOGE("init module(%d) fail", g_modules[i].moduleId);
            break;
        }
    }
    if (i < HDF_ARRAY_SIZE(g_modules)) {
        for (i = i - 1; i >= 0; --i) {
            if (g_modules[i].deinit == NULL) {
                continue;
            }
            g_modules[i].deinit();
        }
        return HDF_FAILURE;
    }
    HDF_LOGE("init softbus module manager success");
    return HDF_SUCCESS;
}

void SoftbusModuleManagerDeinit(void)
{
    int32_t i;

    for (i = 0; i < HDF_ARRAY_SIZE(g_modules); ++i) {
        if (g_modules[i].deinit == NULL) {
            continue;
        }
        g_modules[i].deinit();
    }
    HDF_LOGE("deinit softbus module manager");
}