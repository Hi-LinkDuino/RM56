/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdf_audio_server_common.h"

namespace OHOS::HDI::Audio_Bluetooth {
const int  MANAGER_ADAPTER_NAME_LEN = 32;

int32_t g_serverAdapterNum = 0;
struct AudioInfoInAdapter *g_renderAndCaptureManage = NULL;

static int32_t AdapterManageInit(struct AudioInfoInAdapter *adapterManage,
    const char *adapterName)
{
    int32_t ret;

    if (adapterManage == NULL || adapterName == NULL) {
        HDF_LOGE("%{public}s: Parameter error! ", __func__);

        return HDF_FAILURE;
    }

    adapterManage->adapterName = (char *)calloc(1, MANAGER_ADAPTER_NAME_LEN);
    if (adapterManage->adapterName == NULL) {
        HDF_LOGE("%{public}s: calloc adapter name failed!", __func__);

        return HDF_FAILURE;
    }

    ret = memcpy_s((void *)adapterManage->adapterName, MANAGER_ADAPTER_NAME_LEN,
        adapterName, MANAGER_ADAPTER_NAME_LEN);
    if (ret != EOK) {
        HDF_LOGE("%{public}s: memcpy adapter name fail!", __func__);
        AudioMemFree((void **)&adapterManage->adapterName);

        return HDF_FAILURE;
    }

    adapterManage->adapter = NULL;
    adapterManage->adapterUserNum = 0;
    adapterManage->renderStatus = 0;
    adapterManage->renderPriority = -1;
    adapterManage->render = NULL;
    adapterManage->renderBusy = false;
    adapterManage->renderDestory = false;
    adapterManage->renderPid = 0;

    return HDF_SUCCESS;
}

int32_t ServerManageGetAdapterNum(void)
{
    return ((g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum);
}

struct AudioInfoInAdapter *ServerManageGetAdapters(void)
{
    return g_renderAndCaptureManage;
}


void AdaptersServerManageRelease(
    const struct AudioInfoInAdapter *adaptersManage, int32_t num)
{
    int32_t i;

    if (adaptersManage == NULL || num <= 0) {
        HDF_LOGE("%{public}s: Parameter error! ", __func__);

        return;
    }

    num = (num > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : num;
    for (i = 0; i < num; i++) {
        if (adaptersManage[i].adapterName != NULL) {
            AudioMemFree((void **)&adaptersManage[i].adapterName);
        }
    }
    AudioMemFree((void **)&adaptersManage);
}

void AdaptersServerManageInfomationRecycle(void)
{
    AdaptersServerManageRelease(g_renderAndCaptureManage, g_serverAdapterNum);
    g_renderAndCaptureManage = NULL;
    g_serverAdapterNum = 0;
}

int32_t AdaptersServerManageInit(const struct AudioAdapterDescriptor *descs, int32_t num)
{
    int32_t i, ret;
    struct AudioInfoInAdapter *adaptersManage = NULL;

    if (descs == NULL || num <= 0) {
        HDF_LOGE("%{public}s: Parameter error! ", __func__);

        return HDF_FAILURE;
    }

    num = (num > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : num;
    adaptersManage = (struct AudioInfoInAdapter *)calloc(1,
        num * sizeof(struct AudioInfoInAdapter));
    if (adaptersManage == NULL) {
        HDF_LOGE("%{public}s: calloc adaptersManage failed! ", __func__);

        return HDF_FAILURE;
    }
    for (i = 0; i < num; i++) {
        ret = AdapterManageInit(&adaptersManage[i], descs[i].adapterName);
        if (ret != HDF_SUCCESS) {
            AdaptersServerManageRelease(adaptersManage, num);

            return ret;
        }
    }
    g_serverAdapterNum = num;
    g_renderAndCaptureManage = adaptersManage;

    return HDF_SUCCESS;
}

int32_t HdiServiceRenderCaptureReadData(struct HdfSBuf *data, const char **adapterName, uint32_t *pid)
{
    if (adapterName == NULL || data == NULL || pid == NULL) {
        return HDF_FAILURE;
    }
    if ((*adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterName Is NULL ", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, pid)) {
        HDF_LOGE("%{public}s: read buf fail ", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WriteAudioSampleAttributes(struct HdfSBuf *reply, const struct AudioSampleAttributes *attrs)
{
    if (reply == NULL || attrs == NULL) {
        return HDF_FAILURE;
    }
    uint32_t tempAttrParam = (uint32_t)attrs->type;
    if (!HdfSbufWriteUint32(reply, tempAttrParam)) {
        return HDF_FAILURE;
    }
    tempAttrParam = (uint32_t)attrs->interleaved;
    if (!HdfSbufWriteUint32(reply, tempAttrParam)) {
        return HDF_FAILURE;
    }
    tempAttrParam = (uint32_t)attrs->format;
    if (!HdfSbufWriteUint32(reply, tempAttrParam)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->sampleRate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->channelCount)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->period)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->frameSize)) {
        return HDF_FAILURE;
    }
    tempAttrParam = (uint32_t)(attrs->isBigEndian);
    if (!HdfSbufWriteUint32(reply, tempAttrParam)) {
        return HDF_FAILURE;
    }
    tempAttrParam = (uint32_t)(attrs->isSignedData);
    if (!HdfSbufWriteUint32(reply, tempAttrParam)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->startThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->stopThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, attrs->silenceThreshold)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t ReadAudioSapmleAttrbutes(struct HdfSBuf *data, struct AudioSampleAttributes *attrs)
{
    if (data == NULL || attrs == NULL) {
        return HDF_FAILURE;
    }
    uint32_t tempAttrParam = 0;
    if (!HdfSbufReadUint32(data, &tempAttrParam)) {
        return HDF_FAILURE;
    }
    attrs->type = (AudioCategory)tempAttrParam;
    if (!HdfSbufReadUint32(data, &tempAttrParam)) {
        return HDF_FAILURE;
    }
    attrs->interleaved = (bool)tempAttrParam;
    if (!HdfSbufReadUint32(data, &tempAttrParam)) {
        return HDF_FAILURE;
    }
    attrs->format = (AudioFormat)tempAttrParam;
    if (!HdfSbufReadUint32(data, &(attrs->sampleRate))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &(attrs->channelCount))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &(attrs->period))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &(attrs->frameSize))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &tempAttrParam)) {
        return HDF_FAILURE;
    }
    attrs->isBigEndian = (bool)tempAttrParam;
    if (!HdfSbufReadUint32(data, &tempAttrParam)) {
        return HDF_FAILURE;
    }
    attrs->isSignedData = (bool)tempAttrParam;
    if (!HdfSbufReadUint32(data, &(attrs->startThreshold))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &(attrs->stopThreshold))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &(attrs->silenceThreshold))) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioAdapterListGetAdapter(const char *adapterName, struct AudioAdapter **adapter)
{
    int32_t i, num;

    LOG_FUN_INFO();
    if (g_renderAndCaptureManage == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (adapterName == NULL || adapter == NULL) {
        HDF_LOGE("%{public}s: The pointer is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ?
        MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            *adapter = g_renderAndCaptureManage[i].adapter;
            return HDF_SUCCESS;
        }
    }
    return HDF_ERR_INVALID_PARAM;
}

int32_t AudioDestroyFormerRender(struct AudioInfoInAdapter *renderManage)
{
    LOG_FUN_INFO();
    if (renderManage == NULL || renderManage->adapter == NULL || renderManage->render == NULL) {
        HDF_LOGE("%{public}s: input para is NULL. ", __func__);
        return HDF_FAILURE;
    }
    int count = 0;
    renderManage->renderDestory = true;
    while (renderManage->renderBusy) {
        if (count > 1000) { // Less than 1000
            HDF_LOGE("%{public}s: , count = %{public}d", __func__, count);
            renderManage->renderDestory = false;
            return AUDIO_HAL_ERR_AO_BUSY; // render is busy now
        }
        usleep(500); // sleep 500us
        count++;
    }
    renderManage->renderPid = 0;
    if (renderManage->adapter->DestroyRender(renderManage->adapter, renderManage->render)) {
        renderManage->renderDestory = false;
        return HDF_FAILURE;
    }
    renderManage->render = NULL;
    renderManage->renderStatus = 0;
    renderManage->renderBusy = false;
    renderManage->renderDestory = false;
    renderManage->renderPriority = -1;
    return HDF_SUCCESS;
}

int32_t AudioJudgeRenderPriority(const int32_t priority, int which)
{
    int32_t num;

    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    if (which < 0 || which >= num) {
        HDF_LOGE("%{public}s: invalid value! ", __func__);
        return HDF_FAILURE;
    }
    if (g_renderAndCaptureManage == NULL) {
        return HDF_FAILURE;
    }
    if (g_renderAndCaptureManage[which].renderPriority <= priority) {
        if (AudioDestroyFormerRender(&g_renderAndCaptureManage[which])) {
            HDF_LOGE("%{public}s: AudioDestroyFormerRender: Fail. ", __func__);
            return HDF_FAILURE;
        }
        return HDF_SUCCESS;
    } else {
        return AUDIO_HAL_ERR_AO_BUSY; // render is busy now
    }
    return HDF_FAILURE;
}

int32_t AudioCreatRenderCheck(const char *adapterName, const int32_t priority)
{
    int32_t i;

    LOG_FUN_INFO();
    if (adapterName == NULL || g_renderAndCaptureManage == NULL) {
        return HDF_FAILURE;
    }

    int32_t num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ?
        MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            if (!(g_renderAndCaptureManage[i].renderStatus)) {
                return HDF_SUCCESS;
            } else {
                return AudioJudgeRenderPriority(priority, i);
            }
        }
    }
    HDF_LOGE("%{public}s: Can not find Adapter! ", __func__);
    return HDF_FAILURE;
}

int32_t AudioAddRenderInfoInAdapter(const char *adapterName,
    struct AudioRender *render,
    const struct AudioAdapter *adapter,
    const int32_t priority,
    uint32_t renderPid)
{
    int32_t i, num;

    if (adapterName == NULL || adapter == NULL || render == NULL) {
        HDF_LOGE("%{public}s: input para is NULL. ", __func__);
        return HDF_FAILURE;
    }
    if (g_renderAndCaptureManage == NULL) {
        return HDF_FAILURE;
    }
    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            g_renderAndCaptureManage[i].renderStatus = 1;
            g_renderAndCaptureManage[i].renderPriority = priority;
            g_renderAndCaptureManage[i].render = render;
            g_renderAndCaptureManage[i].renderPid = renderPid;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%{public}s: Can not find Adapter! ", __func__);
    return HDF_FAILURE;
}

void AudioSetRenderStatus(const char *adapterName, bool renderStatus)
{
    int32_t i, num;
    if (g_renderAndCaptureManage == NULL) {
        return;
    }
    LOG_FUN_INFO();
    if (adapterName == NULL) {
        HDF_LOGE("%{public}s: adapterName is null ", __func__);
        return;
    }

    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            g_renderAndCaptureManage[i].renderBusy = renderStatus;
            return;
        }
    }
    HDF_LOGE("%{public}s: AudioDestroyRenderInfoInAdapter: Can not find Adapter! ", __func__);
    return;
}

int32_t AudioGetRenderStatus(const char *adapterName)
{
    int32_t i;
    int32_t num;
    LOG_FUN_INFO();
    if (adapterName == NULL || g_renderAndCaptureManage == NULL) {
        return HDF_FAILURE;
    }

    num = (g_serverAdapterNum >
        MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            if (!g_renderAndCaptureManage[i].renderDestory) {
                return HDF_SUCCESS;
            } else {
                g_renderAndCaptureManage[i].renderBusy = false;
                return HDF_FAILURE;
            }
        }
    }
    HDF_LOGE("%{public}s: AudioDestroyRenderInfoInAdapter: Can not find Adapter! ", __func__);
    return HDF_FAILURE;
}

int32_t AudioDestroyRenderInfoInAdapter(const char *adapterName)
{
    int32_t i, num;

    LOG_FUN_INFO();
    if (adapterName == NULL) {
        HDF_LOGE("%{public}s: adapterName is null ", __func__);
        return HDF_FAILURE;
    }

    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    if (g_renderAndCaptureManage == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            g_renderAndCaptureManage[i].renderStatus = 0;
            g_renderAndCaptureManage[i].renderPriority = -1;
            g_renderAndCaptureManage[i].render = NULL;
            g_renderAndCaptureManage[i].renderPid = 0;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%{public}s: Can not find Adapter! ", __func__);
    return HDF_FAILURE;
}

int32_t AudioAdapterListGetPid(const char *adapterName, uint32_t *pid)
{
    LOG_FUN_INFO();
    int32_t i, num;
    if (g_renderAndCaptureManage == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (adapterName == NULL || pid == NULL) {
        HDF_LOGE("%{public}s: The pointer is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            HDF_LOGE("%{public}s: i = %{public}d", __func__, i);
            *pid = g_renderAndCaptureManage[i].renderPid;
            HDF_LOGE("%{public}s: pid = %{public}u", __func__, *pid);
            return HDF_SUCCESS;
        }
    }
    return HDF_ERR_INVALID_PARAM;
}

int32_t AudioAdapterListGetAdapterRender(const char *adapterName,
    struct AudioAdapter **adapter, struct AudioRender **render)
{
    int32_t i, num;

    LOG_FUN_INFO();
    if (g_renderAndCaptureManage == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (adapterName == NULL || adapter == NULL || render == NULL) {
        HDF_LOGE("%{public}s: The pointer is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            *adapter = g_renderAndCaptureManage[i].adapter;
            *render = g_renderAndCaptureManage[i].render;
            return HDF_SUCCESS;
        }
    }
    return HDF_ERR_INVALID_PARAM;
}

int32_t AudioAdapterListGetRender(const char *adapterName, struct AudioRender **render, uint32_t pid)
{
    int32_t num;

    HDF_LOGE("%{public}s: enter ", __func__);
    if (g_renderAndCaptureManage == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (adapterName == NULL || render == NULL) {
        HDF_LOGE("%{public}s: pointer is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (int32_t i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            HDF_LOGE("%{public}s: renderPid = %{public}u, pid = %{public}u", __func__,
                g_renderAndCaptureManage[i].renderPid, pid);
            if (g_renderAndCaptureManage[i].renderPid != pid) {
                HDF_LOGE("%{public}s: renderPid != pid ", __func__);
                return AUDIO_HAL_ERR_INVALID_OBJECT;
            }
            *render = g_renderAndCaptureManage[i].render;
            return HDF_SUCCESS;
        }
    }
    return HDF_ERR_INVALID_PARAM;
}

int32_t AudioAdapterListCheckAndGetRender(struct AudioRender **render, struct HdfSBuf *data)
{
    if (render == NULL || data == NULL) {
        return HDF_FAILURE;
    }
    struct AudioRender *renderTemp = NULL;
    const char *adapterName = NULL;
    uint32_t pid = 0;
    if (HdiServiceRenderCaptureReadData(data, &adapterName, &pid) < 0) {
        HDF_LOGE("%{public}s: HdiServiceRenderStart: HdiServiceRenderCaptureReadData fail ", __func__);
        return HDF_FAILURE;
    }
    int ret = AudioAdapterListGetRender(adapterName, &renderTemp, pid);
    if (ret < 0) {
        return ret;
    }
    if (renderTemp == NULL) {
        return HDF_FAILURE;
    }
    *render = renderTemp;
    return HDF_SUCCESS;
}

int32_t AudioAdapterCheckListExist(const char *adapterName)
{
    int32_t i, num;

    LOG_FUN_INFO();
    if (g_renderAndCaptureManage == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (adapterName == NULL) {
        HDF_LOGE("%{public}s: The pointer is null. ", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return AUDIO_HAL_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            if (g_renderAndCaptureManage[i].adapterUserNum == 0) {
                return AUDIO_HAL_ERR_INTERNAL;
            } else if (g_renderAndCaptureManage[i].adapterUserNum > 0) {
                g_renderAndCaptureManage[i].adapterUserNum++;
                return AUDIO_HAL_SUCCESS;
            }
        }
    }
    return AUDIO_HAL_ERR_INVALID_PARAM;
}

int32_t AudioAdapterListDestory(const char *adapterName, struct AudioAdapter **adapter)
{
    int32_t i, num;
    if (adapter == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    LOG_FUN_INFO();
    if (adapterName == NULL) {
        HDF_LOGE("%{public}s: adapterName is NULL. ", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (g_renderAndCaptureManage == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ? MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return AUDIO_HAL_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            if (g_renderAndCaptureManage[i].adapterUserNum == 1) {
                g_renderAndCaptureManage[i].adapterUserNum--;
                *adapter = g_renderAndCaptureManage[i].adapter;
                g_renderAndCaptureManage[i].adapter = NULL;
                return AUDIO_HAL_SUCCESS;
            } else if (g_renderAndCaptureManage[i].adapterUserNum > 1) {
                g_renderAndCaptureManage[i].adapterUserNum--;
                return AUDIO_HAL_ERR_INTERNAL;
            }
        }
    }
    return AUDIO_HAL_ERR_INVALID_PARAM;
}

int32_t AudioAdapterListAdd(const char *adapterName, struct AudioAdapter *adapter)
{
    int32_t i, num;

    LOG_FUN_INFO();
    if (adapterName == NULL || adapter == NULL) {
        HDF_LOGE("%{public}s: adapterName is NULL. ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    num = (g_serverAdapterNum > MAX_AUDIO_ADAPTER_NUM_SERVER) ?
        MAX_AUDIO_ADAPTER_NUM_SERVER : g_serverAdapterNum;
    if (g_renderAndCaptureManage == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    for (i = 0; i < num; i++) {
        if (g_renderAndCaptureManage[i].adapterName == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
        if (!strcmp(g_renderAndCaptureManage[i].adapterName, adapterName)) {
            g_renderAndCaptureManage[i].adapter = adapter;
            g_renderAndCaptureManage[i].adapterUserNum = 1;
            return HDF_SUCCESS;
        }
    }
    return HDF_ERR_INVALID_PARAM;
}

int32_t HdiServicePositionWrite(struct HdfSBuf *reply,
    uint64_t frames, struct AudioTimeStamp time)
{
    if (reply == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, frames)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt64(reply, time.tvSec)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt64(reply, time.tvNSec)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t HdiServiceReqMmapBuffer(struct AudioMmapBufferDescripter *desc, struct HdfSBuf *data)
{
    int32_t ret;
    if (desc == NULL || data == NULL) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint64_t memAddr = 0;
    if (!HdfSbufReadUint64(data, &memAddr)) {
        HDF_LOGE("%{public}s: memAddr Is NULL", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    desc->memoryAddress = (void *)(uintptr_t)memAddr;
    ret = HdfSbufReadFileDescriptor(data);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    desc->memoryFd = ret;
    if (!HdfSbufReadInt32(data, &desc->totalBufferFrames)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufReadInt32(data, &desc->transferFrameSize)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufReadInt32(data, &desc->isShareable)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufReadUint32(data, &desc->offset)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}
}