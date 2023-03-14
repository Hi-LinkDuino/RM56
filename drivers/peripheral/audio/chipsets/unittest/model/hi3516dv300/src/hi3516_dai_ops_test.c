/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_dai_ops_test.h"
#include "audio_host.h"
#include "audio_dai_if.h"
#include "audio_platform_if.h"
#include "hi3516_dai_ops.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hi3516_common_func.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER


int32_t TestDaiDeviceInit(void)
{
    struct AudioCard *audioCard = NULL;
    struct DaiDevice *dai = NULL;
    int ret;

    ret = DaiDeviceInit(audioCard, dai);

    HDF_LOGI("TestDaiHwParams: success");
    return HDF_SUCCESS;
}

int32_t TestDaiStartup(void)
{
    struct AudioCard *card = NULL;
    struct DaiDevice *device = NULL;
    int ret;

    ret = DaiDeviceInit(card, device);
    HDF_LOGI("TestDaiHwParams: success");
    return HDF_SUCCESS;
}

int32_t TestDaiHwParams(void)
{
    // CodecDaiHwParams: channels = 2, rate = 48000,
    // PERIODSIZE = 960,         PERIODCOUNT = 8, FORMAT = 2,
    // cardServiceName = audio_service_0
    int ret;
    struct AudioCard *card = NULL;
    struct AudioPcmHwParams *param = NULL;

    ret = DaiHwParams(card, param);

    HDF_LOGI("TestDaiHwParams: success");
    return HDF_SUCCESS;
}

int32_t TestDaiTrigger(void)
{
    int ret;
    struct AudioCard *card = NULL;
    int cmd;
    struct DaiDevice *cpuDai = NULL;

    ret = GetAudioCard(&card);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestDaiTrigger: get card instance failed.");
        return HDF_FAILURE;
    }

    if (card->rtd == NULL || card->rtd->cpuDai == NULL) {
        HDF_LOGE("TestDaiTrigger: card rtd is NULL.");
        return HDF_FAILURE;
    }
    cpuDai = card->rtd->cpuDai;
    if (cpuDai->devData == NULL || cpuDai->devData->ops == NULL) {
        HDF_LOGE("TestDaiTrigger: cpuDai param is NULL.");
        return HDF_FAILURE;
    }
    cmd = 1;

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestDaiTrigger: Trigger fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestDaiTrigger: success");
    return HDF_SUCCESS;
}
