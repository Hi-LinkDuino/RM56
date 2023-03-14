/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "lite_lcdkit.h"
#include "hdf_log.h"

#define PARSE_PANEL_SYMBOL(node, ops, symbol, out) do { \
    if ((ops)->GetUint32((node), (symbol), (out), 0)) { \
        HDF_LOGE("%s: get symbol:%s failed", __func__, (symbol)); \
        return HDF_FAILURE; \
    } \
} while (0)

static int32_t GetDsiCmdCount(uint8_t *array, int32_t len, uint32_t *count)
{
    int32_t cnt = 0;
    uint8_t dlen;

    while (len > 0) {
        dlen = array[DATA_LEN];
        array = array + DSI_CMD_HEAD + dlen;
        len = len - (dlen + DSI_CMD_HEAD);
        cnt++;
    }
    if (len != 0) {
        HDF_LOGE("%s: dsi cmd count error", __func__);
        return HDF_FAILURE;
    }
    *count = cnt;
    return HDF_SUCCESS;
}

static int32_t ParseDsiCmd(struct PanelCmd *cmd, int32_t count, uint8_t *array, int32_t len)
{
    struct DsiCmdDesc *dsiCmd = (struct DsiCmdDesc *)OsalMemCalloc(count * sizeof(struct DsiCmdDesc));
    if (dsiCmd == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    int32_t ret;
    int32_t i;
    int32_t num = 0;
    cmd->count = count;
    cmd->dsiCmd = dsiCmd;
    uint8_t *tmpArray = array;
    struct DsiCmdDesc *tmpCmd = dsiCmd;
    while (count > 0 && len > 0) {
        tmpCmd->dataType = tmpArray[DATA_TYPE];
        tmpCmd->delay = tmpArray[CMD_DELAY];
        tmpCmd->dataLen = tmpArray[DATA_LEN];
        tmpCmd->payload = (uint8_t *)OsalMemCalloc(tmpCmd->dataLen * sizeof(uint8_t));
        if (tmpCmd->payload == NULL) {
            HDF_LOGE("%s: OsalMemCalloc failed", __func__);
            for (i = 0; i < num; i++) {
                OsalMemFree(dsiCmd[i]->payload);
            }
            OsalMemFree(array);
            OsalMemFree(dsiCmd);
            cmd->dsiCmd = NULL;
            cmd->count = 0;
            return HDF_FAILURE;
        }

        ret = memcpy_s(tmpCmd->payload, tmpCmd->dataLen, &tmpArray[DSI_CMD_HEAD], DSI_CMD_HEAD);
        if (ret != EOK) {
            HDF_LOGE("%s: memcpy_s failed, ret %d", __func__, ret);
            for (i = 0; i < num; i++) {
                OsalMemFree(dsiCmd[i]->payload);
            }
            OsalMemFree(array);
            OsalMemFree(dsiCmd);
            cmd->dsiCmd = NULL;
            cmd->count = 0;
            return ret;
        }

        tmpArray += DSI_CMD_HEAD + tmpCmd->dataLen;
        tmpCmd++;
        count--;
        num++;
        len -= DSI_CMD_HEAD + tmpCmd->dataLen;
    }
    OsalMemFree(array);
    return HDF_SUCCESS;
}

static int32_t ParseCmdConfig(const struct DeviceResourceNode *node, struct DeviceResourceIface *drsOps,
    const char *name, struct PanelCmd *cmd)
{
    int32_t len = drsOps->GetElemNum(node, name);
    uint8_t *array = (uint8_t *)OsalMemCalloc(len * sizeof(uint8_t));
    if (array == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8Array(node, name, array, len, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetUint8Array failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    uint32_t count = 0;
    if (GetDsiCmdCount(array, len, &count) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetDsiCmdCount failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    return ParseDsiCmd(cmd, count, array, len);
}

static int32_t ParsePanelInfo(const struct DeviceResourceNode *node, struct DeviceResourceIface *drsOps,
    struct PanelInfo *info)
{
    /* panel setting */
    PARSE_PANEL_SYMBOL(node, drsOps, "width", &info->width);
    PARSE_PANEL_SYMBOL(node, drsOps, "height", &info->height);
    PARSE_PANEL_SYMBOL(node, drsOps, "hbp", &info->hbp);
    PARSE_PANEL_SYMBOL(node, drsOps, "hfp", &info->hfp);
    PARSE_PANEL_SYMBOL(node, drsOps, "hsw", &info->hsw);
    PARSE_PANEL_SYMBOL(node, drsOps, "vbp", &info->vbp);
    PARSE_PANEL_SYMBOL(node, drsOps, "vfp", &info->vfp);
    PARSE_PANEL_SYMBOL(node, drsOps, "vsw", &info->vsw);
    PARSE_PANEL_SYMBOL(node, drsOps, "frameRate", &info->frameRate);
    PARSE_PANEL_SYMBOL(node, drsOps, "intfType", &info->intfType);
    PARSE_PANEL_SYMBOL(node, drsOps, "intfSync", &info->intfSync);
    /* mipi setting */
    PARSE_PANEL_SYMBOL(node, drsOps, "dsiLane", &info->mipi.lane);
    PARSE_PANEL_SYMBOL(node, drsOps, "mode", &info->mipi.mode);
    PARSE_PANEL_SYMBOL(node, drsOps, "burstMode", &info->mipi.burstMode);
    PARSE_PANEL_SYMBOL(node, drsOps, "pixelFmt", &info->mipi.format);
    /* backlight setting */
    PARSE_PANEL_SYMBOL(node, drsOps, "blkType", &info->blk.type);
    PARSE_PANEL_SYMBOL(node, drsOps, "minLevel", &info->blk.minLevel);
    PARSE_PANEL_SYMBOL(node, drsOps, "maxLevel", &info->blk.maxLevel);
    PARSE_PANEL_SYMBOL(node, drsOps, "defLevel", &info->blk.defLevel);
    /* pwm setting */
    if (info->blk.type == BLK_PWM) {
        PARSE_PANEL_SYMBOL(node, drsOps, "pwmDev", &info->pwm.dev);
        PARSE_PANEL_SYMBOL(node, drsOps, "pwmPeriod", &info->pwm.period);
    }
    return HDF_SUCCESS;
}

static int32_t ParsePowerSetting(const struct DeviceResourceNode *node, struct DeviceResourceIface *drsOps,
    struct PowerSetting *setting)
{
    int32_t count = drsOps->GetElemNum(node, "powerSetting");
    if ((count % POWER_SETTING_SIZE) != 0) {
        HDF_LOGE("%s: count invalid", __func__);
        return HDF_FAILURE;
    }
    uint32_t *array = (uint32_t *)OsalMemCalloc(count * sizeof(uint32_t));
    if (array == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32Array(node, "powerSetting", array, count, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetUint32Array failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    uint32_t *tmp = array;
    setting->power = (struct PowerDesc *)OsalMemCalloc((count / POWER_SETTING_SIZE) * sizeof(struct PowerDesc));
    if (setting->power == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    int32_t i;
    for (i = 0; i < (count / POWER_SETTING_SIZE); i++) {
        setting->power[i].type = tmp[i];        // get power type
        setting->power[i].num = tmp[i + 1];     // 1-get power num
        setting->power[i].vol = tmp[i + 2];     // 2-get power vol
        tmp += POWER_SETTING_SIZE;              // next power setting
    }
    setting->count = count / POWER_SETTING_SIZE;
    OsalMemFree(array);
    return HDF_SUCCESS;
}

static int32_t ParsePowerSequeue(const struct DeviceResourceNode *node, struct DeviceResourceIface *drsOps,
    const char *name, struct PowerSequeue *seq)
{
    int32_t count = drsOps->GetElemNum(node, name);
    if ((count % POWER_SEQUEUE_SIZE) != 0) {
        HDF_LOGE("%s: count invalid", __func__);
        return HDF_FAILURE;
    }
    uint32_t *array = (uint32_t *)OsalMemCalloc(count * sizeof(uint32_t));
    if (array == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32Array(node, name, array, count, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetUint32Array failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    uint32_t *tmp = array;
    seq->pwCtrl = (struct PowerCtrl *)OsalMemCalloc((count / POWER_SEQUEUE_SIZE) * sizeof(struct PowerCtrl));
    if (seq->pwCtrl == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        OsalMemFree(array);
        return HDF_FAILURE;
    }
    int32_t i;
    for (i = 0; i < (count / POWER_SEQUEUE_SIZE); i++) {
        seq->pwCtrl[i].num = tmp[i];          // get power num
        seq->pwCtrl[i].opt = tmp[i + 1];      // 1-get power operate
        seq->pwCtrl[i].delay = tmp[i + 2];    // 2-get power delay
        tmp += POWER_SEQUEUE_SIZE;                    // next power setting
    }
    seq->count = count / POWER_SEQUEUE_SIZE;
    OsalMemFree(array);
    return HDF_SUCCESS;
}

static int32_t ParsePanelConfig(const struct DeviceResourceNode *node, struct PanelConfig *cfg)
{
    struct DeviceResourceIface *drsOps = NULL;
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL || drsOps->GetUint32Array == NULL ||
        drsOps->GetElemNum == NULL || drsOps->GetUint8Array == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_FAILURE;
    }
    if (ParsePanelInfo(node, drsOps, &cfg->info)) {
        HDF_LOGE("%s: ParsePanelInfo failed", __func__);
        return HDF_FAILURE;
    }
    PARSE_PANEL_SYMBOL(node, drsOps, "dsiDev", &cfg->dsiDev);
    if (ParsePowerSetting(node, drsOps, &cfg->setting)) {
        HDF_LOGE("%s: Parse power setting failed", __func__);
        return HDF_FAILURE;
    }
    if (ParseCmdConfig(node, drsOps, "panelOnCmd", &cfg->onCmd)) {
        HDF_LOGE("%s: Parse panel on command failed", __func__);
        return HDF_FAILURE;
    }
    if (ParseCmdConfig(node, drsOps, "panelOffCmd", &cfg->offCmd)) {
        HDF_LOGE("%s: Parse panel off command failed", __func__);
        return HDF_FAILURE;
    }
    if (ParsePowerSequeue(node, drsOps, "powerOnSeq", &cfg->onSeq)) {
        HDF_LOGE("%s: Parse power on seq failed", __func__);
        return HDF_FAILURE;
    }
    if (ParsePowerSequeue(node, drsOps, "powerOffSeq", &cfg->offSeq)) {
        HDF_LOGE("%s: Parse power off seq failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t ParseLcdConfig(const struct DeviceResourceNode *node, struct PanelConfig *cfg)
{
    if (node == NULL || cfg == NULL) {
        HDF_LOGE("%s: node or cfg is null", __func__);
        return HDF_FAILURE;
    }
    struct DeviceResourceIface *drsOps = NULL;
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "active_panel", &cfg->actPanel, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: get active panel failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: actPanel = %s", __func__, cfg->actPanel);
    const struct DeviceResourceNode *panelConfigNode = drsOps->GetChildNode(node, cfg->actPanel);
    if (panelConfigNode == NULL) {
        HDF_LOGE("%s: panelConfigNode is null", __func__);
        return HDF_FAILURE;
    }
    return ParsePanelConfig(panelConfigNode, cfg);
}

