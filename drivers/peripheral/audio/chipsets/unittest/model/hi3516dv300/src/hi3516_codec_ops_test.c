/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_codec_ops_test.h"
#include "audio_codec_base.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hi3516_codec_ops.h"
#include "hi3516_common_func.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

int32_t TestCodecDeviceInit(void)
{
    int32_t ret;
    struct AudioCard *audioCard = NULL;
    struct CodecDevice *codec = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = CodecDeviceInit(audioCard, codec);

    HDF_LOGI("TestCodecDeviceInit:%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecDaiDeviceInit(void)
{
    int ret;
    struct AudioCard *card = NULL;
    struct DaiDevice *device = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = CodecDaiDeviceInit(card, device);

    HDF_LOGI("TestCodecDaiDeviceInit: success");
    OsalMemFree(device);
    return HDF_SUCCESS;
}

int32_t TestCodecDaiStartup(void)
{
    int ret;
    struct AudioCard *card = NULL;
    struct DaiDevice *device = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = CodecDaiStartup(card, device);

    HDF_LOGI("TestCodecDaiStartup: success");
    return HDF_SUCCESS;
}

int32_t TestCodecDaiHwParams(void)
{
    struct AudioCard *card = NULL;
    struct AudioPcmHwParams *param = NULL;
    int ret;
    HDF_LOGI("TestCodecDaiHwParams: enter");

    ret = CodecDaiHwParams(card, param);

    HDF_LOGI("TestCodecDaiHwParams: success");
    return HDF_SUCCESS;
}
