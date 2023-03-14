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

#ifndef HDF_AUDIO_SERVER_COMMON_H
#define HDF_AUDIO_SERVER_COMMON_H

#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "audio_internal.h"
namespace OHOS::HDI::Audio_Bluetooth {
#define MAX_AUDIO_ADAPTER_NUM_SERVER    8   // Limit the number of sound cards supported to a maximum of 8
#define STR_MAX 512

/* RenderManage Info */
struct AudioInfoInAdapter {
    const char *adapterName;
    struct AudioAdapter *adapter;
    int adapterUserNum;
    int renderStatus;
    int renderPriority;
    struct AudioRender *render;
    bool renderBusy;
    bool renderDestory;
    uint32_t renderPid;
};

int32_t AudioAdapterListGetAdapter(const char *adapterName, struct AudioAdapter **adapter);
int32_t AudioCreatRenderCheck(const char *adapterName, const int32_t priority);
int32_t AudioAddRenderInfoInAdapter(const char *adapterName,
    struct AudioRender *render,
    const struct AudioAdapter *adapter,
    const int32_t priority,
    uint32_t renderPid);
int32_t AudioDestroyRenderInfoInAdapter(const char *adapterName);
int32_t AudioAdapterListGetAdapterRender(const char *adapterName,
    struct AudioAdapter **adapter, struct AudioRender **render);
int32_t AudioAdapterCheckListExist(const char *adapterName);
int32_t AudioAdapterListDestory(const char *adapterName, struct AudioAdapter **adapter);
int32_t AudioAdapterListAdd(const char *adapterName, struct AudioAdapter *adapter);
int32_t HdiServiceRenderCaptureReadData(struct HdfSBuf *data,
    const char **adapterName, uint32_t *pid);
int32_t AudioAdapterListGetRender(const char *adapterName,
    struct AudioRender **render, uint32_t pid);
int32_t AudioAdapterListGetPid(const char *adapterName, uint32_t *pid);
void AudioSetRenderStatus(const char *adapterName, bool renderStatus);
int32_t AudioGetRenderStatus(const char *adapterName);
int32_t AudioAdapterListCheckAndGetRender(struct AudioRender **render, struct HdfSBuf *data);
int32_t ReadAudioSapmleAttrbutes(struct HdfSBuf *data, struct AudioSampleAttributes *attrs);
int32_t WriteAudioSampleAttributes(struct HdfSBuf *reply, const struct AudioSampleAttributes *attrs);
int32_t ServerManageGetAdapterNum(void);
struct AudioInfoInAdapter *ServerManageGetAdapters(void);
void AdaptersServerManageRelease(const struct AudioInfoInAdapter *adaptersManage, int32_t num);
void AdaptersServerManageInfomationRecycle(void);
int32_t AdaptersServerManageInit(const struct AudioAdapterDescriptor *descs, int32_t num);
int32_t HdiServicePositionWrite(struct HdfSBuf *reply,
    uint64_t frames, struct AudioTimeStamp time);
int32_t HdiServiceReqMmapBuffer(struct AudioMmapBufferDescripter *desc, struct HdfSBuf *data);
}
#endif
