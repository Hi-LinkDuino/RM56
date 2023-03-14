/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_host_test.h"
#include "audio_host.h"
#include "devsvc_manager_clnt.h"

#define HDF_LOG_TAG audio_host_test

const static char *g_audioServiceName[] = {
    "codec_service_0",
    "codec_service_1",
    "dma_service_0",
    "dai_service",
    "audio_service_0",
    "audio_service_1",
    "render_service",
    "capture_service",
    "control_service",
};

int32_t AudioHostCreateAndBindTest(void)
{
    int i;
    struct HdfDeviceObject *device = NULL;
    HDF_LOGI("%s: enter", __func__);

    if (AudioHostCreateAndBind(NULL) != NULL) {
        HDF_LOGE("%s_[%d] codecDevice is not NULL", __func__, __LINE__);
        return HDF_FAILURE;
    }

    for (i = 0; i < sizeof(g_audioServiceName) / sizeof(g_audioServiceName[0]); ++i) {
        device = DevSvcManagerClntGetDeviceObject(g_audioServiceName[i]);
        if (AudioHostCreateAndBind(device) == NULL) {
            HDF_LOGE("audiohost:%s create and bind fail!", g_audioServiceName[i]);
        }
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t GetCardInstanceTest(void)
{
    int i;
    HDF_LOGI("%s: enter", __func__);

    for (i = 0; i < sizeof(g_audioServiceName) / sizeof(g_audioServiceName[0]); ++i) {
        if (GetCardInstance(g_audioServiceName[i]) == NULL) {
            HDF_LOGE("%s: get %s fail!", __func__, g_audioServiceName[i]);
        }
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
