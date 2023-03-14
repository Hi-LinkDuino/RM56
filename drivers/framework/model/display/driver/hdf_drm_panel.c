/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_drm_panel.h"
#include <drm/drmP.h>
#include <drm/drm_atomic_helper.h>
#include <linux/backlight.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <video/mipi_display.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>
#include "osal_mem.h"

static inline struct HdfDrmPanel *ToHdfDrmPanel(const struct drm_panel *panel)
{
    return container_of(panel, struct HdfDrmPanel, panel);
}

static int HdfDrmPanelUnprepare(struct drm_panel *panel)
{
    return 0;
}

static int HdfDrmPanelPrepare(struct drm_panel *panel)
{
    return 0;
}

static int HdfDrmPanelDisable(struct drm_panel *panel)
{
    struct HdfDrmPanel *hdfDrmPanel = ToHdfDrmPanel(panel);
    struct PanelData *panelData;

    HDF_LOGD("%s line = %d\n", __func__, __LINE__);
    OsalMutexLock(&hdfDrmPanel->manager->dispMutex);
    panelData = hdfDrmPanel->manager->panelManager->panel[hdfDrmPanel->index];
    panelData->off(panelData);
    OsalMutexUnlock(&hdfDrmPanel->manager->dispMutex);
    return HDF_SUCCESS;
}

static int HdfDrmPanelEnable(struct drm_panel *panel)
{
    struct HdfDrmPanel *hdfDrmPanel = ToHdfDrmPanel(panel);
    struct PanelData *panelData;

    HDF_LOGD("%s line = %d\n", __func__, __LINE__);
    panelData = hdfDrmPanel->manager->panelManager->panel[hdfDrmPanel->index];
    OsalMutexLock(&hdfDrmPanel->manager->dispMutex);
    panelData->on(panelData);
    OsalMutexUnlock(&hdfDrmPanel->manager->dispMutex);
    return HDF_SUCCESS;
}

static int HdfDrmPanelGetModes(struct drm_panel *panel)
{
    struct drm_connector *connector = panel->connector;
    struct HdfDrmPanel *hdfDrmPanel = ToHdfDrmPanel(panel);
    struct drm_display_mode *mode = NULL;
    struct PanelInfo *panelInfo = NULL;

    HDF_LOGD("%s line = %d\n", __func__, __LINE__);
    panelInfo = hdfDrmPanel->manager->panelManager->panel[hdfDrmPanel->index]->info;
    mode = drm_mode_duplicate(panel->drm, &hdfDrmPanel->mode);
    if (!mode) {
        HDF_LOGE("failed to add mode %ux%ux@%u",
            hdfDrmPanel->mode.hdisplay, hdfDrmPanel->mode.vdisplay,
            hdfDrmPanel->mode.vrefresh);
        return -ENOMEM;
    }
    drm_mode_set_name(mode);
    mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
    drm_mode_probed_add(connector, mode);
    connector->display_info.width_mm = panelInfo->pWidth;
    connector->display_info.height_mm = panelInfo->pHeight;
    return 1;
}

static struct drm_panel_funcs g_hdfDrmPanelFuncs = {
    .get_modes = HdfDrmPanelGetModes,
    .enable = HdfDrmPanelEnable,
    .disable = HdfDrmPanelDisable,
    .prepare = HdfDrmPanelPrepare,
    .unprepare = HdfDrmPanelUnprepare,
};

static ssize_t SuspendStore(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int32_t ret;
    struct HdfDrmPanel *hdfDrmPanel = dev_get_drvdata(dev);

    ret = HdfDrmPanelDisable(&hdfDrmPanel->panel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s HdfDrmPanelDisable fail", __func__);
        return count;
    }
    ret = HdfDrmPanelUnprepare(&hdfDrmPanel->panel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s HdfDrmPanelUnprepare fail", __func__);
        return count;
    }
    return count;
}
static DEVICE_ATTR(suspend, S_IWUSR, NULL, SuspendStore);

static ssize_t ResumeStore(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int32_t ret;
    struct HdfDrmPanel *hdfDrmPanel = dev_get_drvdata(dev);

    ret = HdfDrmPanelPrepare(&hdfDrmPanel->panel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s HdfDrmPanelPrepare fail", __func__);
        return count;
    }
    ret = HdfDrmPanelEnable(&hdfDrmPanel->panel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s HdfDrmPanelEnable fail", __func__);
        return count;
    }
    return count;
}
static DEVICE_ATTR(resume, S_IWUSR, NULL, ResumeStore);

static ssize_t BacklightStore(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int32_t ret;
    unsigned long level;
    struct PanelData *panel = NULL;
    struct HdfDrmPanel *hdfDrmPanel = dev_get_drvdata(dev);

    ret = kstrtoul(buf, 0, &level);
    if (ret != 0) {
        return ret;
    }
    HDF_LOGI("%s enter", __func__);
    OsalMutexLock(&hdfDrmPanel->manager->dispMutex);
    panel = hdfDrmPanel->manager->panelManager->panel[hdfDrmPanel->index];
    OsalMutexUnlock(&hdfDrmPanel->manager->dispMutex);
    ret = UpdateBrightness(panel->blDev, level);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s UpdateBrightness fail", __func__);
    }

    return count;
}
static DEVICE_ATTR(backlight, S_IWUSR, NULL, BacklightStore);

#define ATTR_NUM  3
static struct device_attribute *g_panelAttrs[] = {
    &dev_attr_suspend,
    &dev_attr_resume,
    &dev_attr_backlight,
    NULL,
};

static void CreateDrmMode(struct HdfDrmPanel *hdfDrmPanel)
{
    uint32_t panelIndex;
    struct PanelInfo *panelInfo = NULL;

    panelIndex = hdfDrmPanel->index;
    panelInfo = hdfDrmPanel->manager->panelManager->panel[panelIndex]->info;
    hdfDrmPanel->mode.clock = panelInfo->clockFreq / 1000;
    hdfDrmPanel->mode.hdisplay = panelInfo->width;
    hdfDrmPanel->mode.hsync_start = panelInfo->width + panelInfo->hfp;
    hdfDrmPanel->mode.hsync_end = panelInfo->width + panelInfo->hfp + panelInfo->hsw;
    hdfDrmPanel->mode.htotal = panelInfo->width + panelInfo->hfp + panelInfo->hsw + panelInfo->hbp;
    hdfDrmPanel->mode.vdisplay = panelInfo->height;
    hdfDrmPanel->mode.vsync_start = panelInfo->height + panelInfo->vfp;
    hdfDrmPanel->mode.vsync_end = panelInfo->height + panelInfo->vfp + panelInfo->vsw;
    hdfDrmPanel->mode.vtotal = panelInfo->height + panelInfo->vfp + panelInfo->vsw + panelInfo->vbp;
    hdfDrmPanel->mode.flags = 0;
    hdfDrmPanel->mode.vrefresh = drm_mode_vrefresh(&hdfDrmPanel->mode);
}

static struct HdfDrmPanel *InstanceHdfDrmPanel(struct DispManager *manager, int32_t index)
{
    struct HdfDrmPanel *hdfDrmPanel = NULL;

    hdfDrmPanel = (struct HdfDrmPanel *)OsalMemCalloc(sizeof(struct HdfDrmPanel));
    if (hdfDrmPanel == NULL) {
        HDF_LOGE("%s hdfDrmPanel malloc fail", __func__);
        return NULL;
    }
    hdfDrmPanel->index = index;
    hdfDrmPanel->manager = manager;
    CreateDrmMode(hdfDrmPanel);
    hdfDrmPanel->panel.funcs = &g_hdfDrmPanelFuncs;
    return hdfDrmPanel;
}

int32_t HdfDrmPanelEntryInit(struct HdfDeviceObject *object)
{
    uint32_t ret;
    uint32_t panelNum;
    struct HdfDrmPanel *hdfDrmPanel = NULL;
    struct mipi_dsi_device *dsiDev = NULL;
    struct DispManager *manager = NULL;

    manager = GetDispManager();
    if (manager == NULL) {
        HDF_LOGE("%s manager is null", __func__);
        return HDF_FAILURE;
    }
    panelNum = manager->panelManager->panelNum;
    for (uint32_t i = 0; i < panelNum; i++) {
        hdfDrmPanel = InstanceHdfDrmPanel(manager, i);
        if (hdfDrmPanel == NULL) {
            return HDF_FAILURE;
        }
        dsiDev = (struct mipi_dsi_device *)manager->panelManager->panel[i]->priv;
        hdfDrmPanel->panel.dev = &dsiDev->dev;
        drm_panel_init(&hdfDrmPanel->panel);
        ret = drm_panel_add(&hdfDrmPanel->panel);
        if (ret) {
            HDF_LOGE("%s drm_panel_add() failed", __func__);
            OsalMemFree(hdfDrmPanel);
            return ret;
        }
        ret = mipi_dsi_attach(dsiDev);
        if (ret) {
            HDF_LOGE("%s mipi_dsi_attach failed", __func__);
            drm_panel_remove(&hdfDrmPanel->panel);
            return ret;
        }
        mipi_dsi_set_drvdata(dsiDev, hdfDrmPanel);
        for (uint32_t j = 0; j < ATTR_NUM; j++) {
            if (device_create_file(&dsiDev->dev, g_panelAttrs[j]) != 0) {
                HDF_LOGE("%s line = %d device_create_file fail", __func__, __LINE__);
            }
        }
        HDF_LOGI("%s panel[%d] registered success", i, __func__);
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_hdfDrmPanelEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_DRMPANEL",
    .Init = HdfDrmPanelEntryInit,
};

HDF_INIT(g_hdfDrmPanelEntry);
