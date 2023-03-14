/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "disp_hal.h"
#include "hdf_log.h"
#include "lcd_abs_if.h"
#include "mipi_dsi_core.h"
#include "mipi_dsi.h"

int32_t DispInit(uint32_t devId)
{
    struct PanelData *panelData = GetPanel(devId);
    if (panelData == NULL) {
        HDF_LOGE("%s: GetPanelData failed", __func__);
        return HDF_FAILURE;
    }
    if (panelData->init) {
        /* panel driver init */
        if (panelData->init(panelData) != HDF_SUCCESS) {
            HDF_LOGE("%s: panelData->init failed", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DispGetInfo(uint32_t devId, struct DispInfo *info)
{
    if (info == NULL) {
        return HDF_FAILURE;
    }
    struct PanelData *panelData = GetPanel(devId);
    if (panelData == NULL) {
        HDF_LOGE("%s: GetPanelData failed", __func__);
        return HDF_FAILURE;
    }
    struct PanelInfo *panelInfo = panelData->info;
    if (panelInfo == NULL) {
        HDF_LOGE("%s: GetPanelInfo failed", __func__);
        return HDF_FAILURE;
    }
    info->width = panelInfo->width;
    info->height = panelInfo->height;
    info->hbp = panelInfo->hbp;
    info->hfp = panelInfo->hfp;
    info->hsw = panelInfo->hsw;
    info->vbp = panelInfo->vbp;
    info->vfp = panelInfo->vfp;
    info->vsw = panelInfo->vsw;
    info->intfType = (uint32_t)panelInfo->intfType;
    info->intfSync = panelInfo->intfSync;
    info->frameRate = panelInfo->frameRate;
    return HDF_SUCCESS;
}

int32_t DispOn(uint32_t devId)
{
    struct PanelData *panelData = GetPanel(devId);
    if (panelData == NULL) {
        HDF_LOGE("%s: GetPanelData failed", __func__);
        return HDF_FAILURE;
    }
    if (panelData->on) {
        /* panel driver on */
        if (panelData->on(panelData) != HDF_SUCCESS) {
            HDF_LOGE("%s: panelData->on failed", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DispOff(uint32_t devId)
{
    struct PanelData *panelData = GetPanel(devId);
    if (panelData == NULL) {
        HDF_LOGE("%s: GetPanelData failed", __func__);
        return HDF_FAILURE;
    }
    if (panelData->off) {
        /* panel driver off */
        if (panelData->off(panelData) != HDF_SUCCESS) {
            HDF_LOGE("%s: panelData->off failed", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DispSetBacklight(uint32_t devId, uint32_t level)
{
    struct PanelData *panelData = GetPanel(devId);
    if (panelData == NULL) {
        HDF_LOGE("%s: GetPanelData failed", __func__);
        return HDF_FAILURE;
    }
    if (panelData->setBacklight) {
        /* panel driver set backlight */
        if (panelData->setBacklight(panelData, level) != HDF_SUCCESS) {
            HDF_LOGE("%s: panelData->setBacklight failed", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

#define HDF_MIPI_DSI_SERVICE "HDF_PLATFORM_MIPI_DSI"

static struct MipiDsiService *GetDispService()
{
    static struct MipiDsiService *service = NULL;
    if (service == NULL) {
        struct MipiDsiCntlr *mipiDsiCntlr = (struct MipiDsiCntlr *)DevSvcManagerClntGetService(HDF_MIPI_DSI_SERVICE);
        if (mipiDsiCntlr == NULL) {
            HDF_LOGE("failed to get service %s", HDF_MIPI_DSI_SERVICE);
            return NULL;
        }
        service = (struct MipiDsiService *)mipiDsiCntlr->priv; // @see mipi_dsi.c
    }
    return service;
}

void *DispMmap(uint32_t size)
{
    struct MipiDsiService *service = GetDispService();
    if (service == NULL || service->mmap == NULL) {
        return NULL;
    }
    return service->mmap(size);
}

void DispFlush(void)
{
    struct MipiDsiService *service = GetDispService();
    if (service == NULL || service->flush == NULL) {
        return;
    }
    service->flush();
}
