/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_codec_base.h"
#include "audio_driver_log.h"
#include "audio_parse.h"
#include "audio_sapm.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

static char *g_audioSapmCompNameList[AUDIO_SAPM_COMP_NAME_LIST_MAX] = {
    "ADCL", "ADCR", "DACL", "DACR", "LPGA", "RPGA", "SPKL", "SPKR", "MIC"
};

static char *g_audioSapmCfgNameList[AUDIO_SAPM_CFG_NAME_LIST_MAX] = {
    "LPGA MIC Switch", "RPGA MIC Switch", "Dacl enable", "Dacr enable"
};

static const char *g_audioCodecControlsList[AUDIO_CTRL_LIST_MAX] = {
    "Main Playback Volume", "Main Capture Volume",
    "Playback Mute", "Capture Mute", "Mic Left Gain",
    "Mic Right Gain", "External Codec Enable",
    "Internally Codec Enable", "Render Channel Mode", "Captrue Channel Mode"
};

int32_t CodecGetServiceName(const struct HdfDeviceObject *device, const char **drvCodecName)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input device para is nullptr.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("node instance is nullptr.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("from resource get drsOps fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "serviceName", drvCodecName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read codecServiceName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t CodecGetDaiName(const struct HdfDeviceObject *device, const char **drvDaiName)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("drs ops failed!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "codecDaiName", drvDaiName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read codecDaiName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t CodecGetConfigInfo(const struct HdfDeviceObject *device, struct CodecData *codecData)
{
    if (device == NULL || codecData == NULL) {
        AUDIO_DRIVER_LOG_ERR("param is null!");
        return HDF_FAILURE;
    }

    if (codecData->regConfig != NULL) {
        ADM_LOG_ERR("g_codecData regConfig  fail!");
        return HDF_FAILURE;
    }

    codecData->regConfig = (struct AudioRegCfgData *)OsalMemCalloc(sizeof(*(codecData->regConfig)));
    if (codecData->regConfig == NULL) {
        ADM_LOG_ERR("malloc AudioRegCfgData fail!");
        return HDF_FAILURE;
    }

    if (CodecGetRegConfig(device, codecData->regConfig) != HDF_SUCCESS) {
        ADM_LOG_ERR("CodecGetRegConfig fail!");
        OsalMemFree(codecData->regConfig);
        codecData->regConfig = NULL;
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SapmCtrlToSapmComp(struct AudioSapmComponent *sapmComponents,
    const struct AudioSapmCtrlConfig *sapmCompItem, uint16_t index)
{
    if (sapmComponents == NULL || sapmCompItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    sapmComponents[index].componentName =
        g_audioSapmCompNameList[sapmCompItem[index].compNameIndex];
    sapmComponents[index].reg      = sapmCompItem[index].reg;
    sapmComponents[index].sapmType = sapmCompItem[index].sapmType;
    sapmComponents[index].mask     = sapmCompItem[index].mask;
    sapmComponents[index].shift    = sapmCompItem[index].shift;
    sapmComponents[index].invert   = sapmCompItem[index].invert;
    sapmComponents[index].kcontrolsNum = sapmCompItem[index].kcontrolsNum;

    return HDF_SUCCESS;
}

static int32_t CodecSetSapmConfigInfo(struct CodecData *codeData, struct AudioRegCfgGroupNode **regCfgGroup)
{
    uint16_t index;
    struct AudioSapmCtrlConfig *sapmCompItem;
    struct AudioControlConfig  *sapmCtrlItem;
    struct AudioMixerControl   *ctlSapmRegCfgItem;
    struct AudioKcontrol *audioSapmControls;
    if (codeData == NULL || regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    sapmCompItem = regCfgGroup[AUDIO_SAPM_COMP_GROUP]->sapmCompItem;
    sapmCtrlItem = regCfgGroup[AUDIO_SAPM_CFG_GROUP]->ctrlCfgItem;
    ctlSapmRegCfgItem = regCfgGroup[AUDIO_CTRL_SAPM_PATAM_GROUP]->regCfgItem;

    if (sapmCompItem == NULL || sapmCtrlItem == NULL || ctlSapmRegCfgItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("sapmCompItem, sapmCtrlItem, ctlSapmRegCfgItem is NULL.");
        return HDF_FAILURE;
    }

    audioSapmControls = (struct AudioKcontrol *)OsalMemCalloc(
        regCfgGroup[AUDIO_SAPM_CFG_GROUP]->itemNum * sizeof(struct AudioKcontrol));
    if (audioSapmControls == NULL) {
        AUDIO_DRIVER_LOG_ERR("OsalMemCalloc failed.");
        return HDF_FAILURE;
    }
    for (index = 0; index < regCfgGroup[AUDIO_SAPM_CFG_GROUP]->itemNum; index++) {
        audioSapmControls[index].iface = sapmCtrlItem[index].iface;
        audioSapmControls[index].name = g_audioSapmCfgNameList[sapmCtrlItem[index].arrayIndex];
        audioSapmControls[index].privateValue = (unsigned long)(uintptr_t)(void*)(&ctlSapmRegCfgItem[index]);
        audioSapmControls[index].Info = AudioInfoCtrlOps;
        audioSapmControls[index].Get  = AudioCodecSapmGetCtrlOps;
        audioSapmControls[index].Set  = AudioCodecSapmSetCtrlOps;
    }

    codeData->numSapmComponent = regCfgGroup[AUDIO_SAPM_COMP_GROUP]->itemNum;
    codeData->sapmComponents = (struct AudioSapmComponent *)
        OsalMemCalloc(codeData->numSapmComponent * sizeof(struct AudioSapmComponent));
    if (codeData->sapmComponents == NULL) {
        OsalMemFree(audioSapmControls);
        AUDIO_DRIVER_LOG_ERR("OsalMemCalloc failed.");
        return HDF_FAILURE;
    }

    for (index = 0; index < codeData->numSapmComponent; index++) {
        if (SapmCtrlToSapmComp(codeData->sapmComponents, sapmCompItem, index)) {
            OsalMemFree(audioSapmControls);
            return HDF_FAILURE;
        }

        if (sapmCompItem[index].kcontrolsNum) {
            codeData->sapmComponents[index].kcontrolNews =
                &audioSapmControls[sapmCompItem[index].kcontrolNews - 1];
        }
    }

    return HDF_SUCCESS;
}

int32_t CodecSetConfigInfo(struct CodecData *codeData,  struct DaiData *daiData)
{
    uint16_t index;
    struct AudioIdInfo   *audioIdInfo;
    struct AudioRegCfgGroupNode **regCfgGroup;
    struct AudioControlConfig  *compItem;
    struct AudioMixerControl   *ctlRegCfgItem;
    if (codeData == NULL || daiData == NULL || codeData->regConfig == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    audioIdInfo = &(codeData->regConfig->audioIdInfo);
    regCfgGroup = codeData->regConfig->audioRegParams;
    daiData->regCfgGroup = regCfgGroup;
    codeData->regCfgGroup = regCfgGroup;
    if (audioIdInfo == NULL || regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_ERR("audioIdInfo or regCfgGroup is NULL.");
        return HDF_FAILURE;
    }

    compItem = regCfgGroup[AUDIO_CTRL_CFG_GROUP]->ctrlCfgItem;
    ctlRegCfgItem = regCfgGroup[AUDIO_CTRL_PATAM_GROUP]->regCfgItem;
    if (compItem == NULL || ctlRegCfgItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("compItem or ctlRegCfgItem is NULL.");
        return HDF_FAILURE;
    }

    codeData->numControls = regCfgGroup[AUDIO_CTRL_CFG_GROUP]->itemNum;
    codeData->controls =
        (struct AudioKcontrol *)OsalMemCalloc(codeData->numControls * sizeof(struct AudioKcontrol));
    if (codeData->controls == NULL) {
        AUDIO_DRIVER_LOG_ERR("OsalMemCalloc failed.");
        return HDF_FAILURE;
    }

    for (index = 0; index < codeData->numControls; index++) {
        codeData->controls[index].iface   = compItem[index].iface;
        codeData->controls[index].name    = g_audioCodecControlsList[compItem[index].arrayIndex];
        codeData->controls[index].Info    = AudioInfoCtrlOps;
        codeData->controls[index].privateValue = (unsigned long)(uintptr_t)(void*)(&ctlRegCfgItem[index]);
        if (compItem[index].enable) {
            codeData->controls[index].Get = AudioCodecGetCtrlOps;
            codeData->controls[index].Set = AudioCodecSetCtrlOps;
        }
    }

    codeData->virtualAddress = (uintptr_t)OsalIoRemap(audioIdInfo->chipIdRegister, audioIdInfo->chipIdSize);

    if (CodecSetSapmConfigInfo(codeData, regCfgGroup) != HDF_SUCCESS) {
        OsalMemFree(codeData->controls);
        codeData->controls = NULL;
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CodecSetCtlFunc(struct CodecData *codeData, const void *aiaoGetCtrl, const void *aiaoSetCtrl)
{
    uint32_t index;
    struct AudioRegCfgGroupNode **regCfgGroup;
    struct AudioControlConfig *compItem;
    if (codeData == NULL || codeData->regConfig == NULL ||
        aiaoGetCtrl == NULL || aiaoSetCtrl == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }
    regCfgGroup = codeData->regConfig->audioRegParams;
    if (regCfgGroup == NULL || regCfgGroup[AUDIO_CTRL_CFG_GROUP] == NULL) {
        AUDIO_DRIVER_LOG_ERR("regCfgGroup or regCfgGroup[AUDIO_CTRL_CFG_GROUP] is NULL.");
        return HDF_FAILURE;
    }

    compItem = regCfgGroup[AUDIO_CTRL_CFG_GROUP]->ctrlCfgItem;
    if (compItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("compItem is NULL.");
        return HDF_FAILURE;
    }

    for (index = 0; index < codeData->numControls; index++) {
        if (!compItem[index].enable) {
            codeData->controls[index].Get = aiaoGetCtrl;
            codeData->controls[index].Set = aiaoSetCtrl;
        }
    }

    return HDF_SUCCESS;
}
