/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "audio_sapm_test.h"
#include "audio_sapm.h"

#define HDF_LOG_TAG audio_host_test

int32_t AudioSapmNewComponentsTest(void)
{
    struct AudioCard *audioCard = NULL;
    struct AudioSapmComponent *component = NULL;
    int32_t maxNum = 0;
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioSapmNewComponents(audioCard, component, maxNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: audioCart or component is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioSapmAddRoutesTest(void)
{
    struct AudioCard *audioCard = NULL;
    struct AudioSapmRoute *route = NULL;
    int32_t routeMaxNum = 0;
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioSapmAddRoutes(audioCard, route, routeMaxNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: audioCard or route is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioSapmNewControlsTest(void)
{
    struct AudioCard *audioCard = NULL;
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioSapmNewControls(audioCard);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: audioCard is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
int32_t AudioSapmSleepTest(void)
{
    struct AudioCard *audioCard = NULL;
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioSapmSleep(audioCard);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioSapmSleep is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioSampPowerUpTest(void)
{
    struct AudioCard *audioCard = NULL;
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioSampPowerUp(audioCard);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioSampPowerUp is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioSampSetPowerMonitorTest(void)
{
    struct AudioCard *audioCard = NULL;
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioSampSetPowerMonitor(audioCard, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioSampSetPowerMonitor is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCodecSapmSetCtrlOpsTest(void)
{
    int32_t ret;
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioCodecSapmSetCtrlOps(kcontrol, elemValue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioCodecSapmSetCtrlOps is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCodecSapmGetCtrlOpsTest(void)
{
    int32_t ret;
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioCodecSapmGetCtrlOps(kcontrol, elemValue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioCodecSapmGetCtrlOps is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAccessorySapmSetCtrlOpsTest(void)
{
    int32_t ret;
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioAccessorySapmSetCtrlOps(kcontrol, elemValue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioAccessorySapmSetCtrlOps is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAccessorySapmGetCtrlOpsTest(void)
{
    int32_t ret;
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioAccessorySapmGetCtrlOps(kcontrol, elemValue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AudioAccessorySapmGetCtrlOps is NULL", __func__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
