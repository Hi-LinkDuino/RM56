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

#include "audio_adapter_info_common.h"
#include "audio_internal.h"
#include "audio_bluetooth_manager.h"
namespace OHOS::HDI::Audio_Bluetooth {
int32_t AudioManagerGetAllAdapters(struct AudioManager *manager,
                                   struct AudioAdapterDescriptor **descs,
                                   int *size)
{
    int32_t ret;
    LOG_FUN_INFO();
    if (manager == NULL || descs == NULL || size == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioAdaptersForUser(descs, size);
    if (ret < 0) {
        LOG_FUN_ERR("AudioAdaptersForUser FAIL!");
        return AUDIO_HAL_ERR_NOTREADY; // Failed to read sound card configuration file
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioManagerLoadAdapter(struct AudioManager *manager, const struct AudioAdapterDescriptor *desc,
                                struct AudioAdapter **adapter)
{
    LOG_FUN_INFO();
    if (manager == NULL || desc == NULL || desc->adapterName == NULL || desc->ports == NULL || adapter == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    LOGV("%s: adapter name %s", __func__, desc->adapterName);
    if (AudioAdapterExist(desc->adapterName)) {
        LOGE("%s: not supported this adapter %s", __func__, desc->adapterName);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)calloc(1, sizeof(*hwAdapter));
    if (hwAdapter == NULL) {
        LOGE("%s: calloc AudioHwAdapter failed", __func__);
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwAdapter->common.InitAllPorts = AudioAdapterInitAllPorts;
    hwAdapter->common.CreateRender = AudioAdapterCreateRender;
    hwAdapter->common.DestroyRender = AudioAdapterDestroyRender;
    hwAdapter->common.GetPortCapability = AudioAdapterGetPortCapability;
    hwAdapter->common.SetPassthroughMode = AudioAdapterSetPassthroughMode;
    hwAdapter->common.GetPassthroughMode = AudioAdapterGetPassthroughMode;
    *adapter = &hwAdapter->common;
    hwAdapter->adapterDescriptor = *desc;
    hwAdapter->adapterMgrRenderFlag = 0; // The adapterMgrRenderFlag init is zero

    LOGV("%s call bluetooth RegisterObserver interface", __func__);
    OHOS::Bluetooth::GetProxy();
    OHOS::Bluetooth::RegisterObserver();
    
    return AUDIO_HAL_SUCCESS;
}

void AudioManagerUnloadAdapter(struct AudioManager *manager, struct AudioAdapter *adapter)
{
    struct AudioHwAdapter *hwAdapter = reinterpret_cast<struct AudioHwAdapter *>(adapter);
    if (manager == NULL || hwAdapter == NULL) {
        return;
    }
    if (hwAdapter->portCapabilitys != NULL) {
        int32_t portNum = hwAdapter->adapterDescriptor.portNum;
        int32_t i = 0;
        while (i < portNum) {
            if (&hwAdapter->portCapabilitys[i] != NULL) {
                AudioMemFree((void **)&hwAdapter->portCapabilitys[i].capability.subPorts);
            }
            i++;
        }
        AudioMemFree((void **)&hwAdapter->portCapabilitys);
    }
    AudioMemFree((void **)&adapter);
}

static struct AudioManager g_audioManagerFuncs = {
    .GetAllAdapters = AudioManagerGetAllAdapters,
    .LoadAdapter = AudioManagerLoadAdapter,
    .UnloadAdapter = AudioManagerUnloadAdapter,
};

struct AudioManager *GetAudioManagerFuncs(void)
{
    return &g_audioManagerFuncs;
}
}