/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_dai_base_test.h"
#include "audio_dai_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_dai_base_test

int32_t DaiDataFromCardTest(void)
{
    struct AudioCard card;
    if (DaiDataFromCard(NULL) != NULL) {
        return HDF_FAILURE;
    }

    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));
    if (DaiDataFromCard(&card) != NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DaiGetConfigInfoTest(void)
{
    struct HdfDeviceObject device;
    struct DaiData data;
    (void)memset_s(&device, sizeof(struct HdfDeviceObject), 0, sizeof(struct HdfDeviceObject));
    (void)memset_s(&data, sizeof(struct DaiData), 0, sizeof(struct DaiData));

    if (DaiGetConfigInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (DaiGetConfigInfo(&device, &data) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DaiCheckSampleRateTest(void)
{
    if (DaiCheckSampleRate(0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (DaiCheckSampleRate(AUDIO_SAMPLE_RATE_8000) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DaiSetConfigInfoTest(void)
{
    struct DaiData data;
    if (DaiSetConfigInfo(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    (void)memset_s(&data, sizeof(struct DaiData), 0, sizeof(struct DaiData));
    if (DaiSetConfigInfo(&data) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

