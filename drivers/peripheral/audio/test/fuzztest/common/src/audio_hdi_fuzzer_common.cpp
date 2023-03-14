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

/*
 * @addtogroup Audio
 * @{
 *
 * @brief Defines audio-related APIs for fuzztest.
 *
 * @since 1.0
 * @version 1.0
 */

/*
 * @file audio_hdi_fuzzer_common.h
 *
 * @brief Declares APIs for operations related to the audio fuzztest.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_fuzzer_common.h"

namespace HMOS {
namespace Audio {
int32_t GetManager(TestAudioManager *&manager)
{
    void *handleSo = nullptr;
    TestAudioManager *(*GetAudioManager)() = nullptr;
    handleSo = dlopen(RESOLVED_PATH.c_str(), RTLD_LAZY);
    if (handleSo == nullptr) {
        return HDF_FAILURE;
    }
    GetAudioManager = (TestAudioManager *(*)())(dlsym(handleSo, FUNCTION_NAME.c_str()));
    if (GetAudioManager == nullptr) {
        return HDF_FAILURE;
    }
    manager = GetAudioManager();
    if (manager == nullptr) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
int32_t InitAttrs(struct AudioSampleAttributes& attrs)
{
    attrs.format = AUDIO_FORMAT_PCM_16_BIT;
    attrs.channelCount = CHANNELCOUNT;
    attrs.sampleRate = SAMPLERATE;
    attrs.interleaved = 0;
    attrs.type = AUDIO_IN_MEDIA;
    attrs.period = DEEP_BUFFER_RENDER_PERIOD_SIZE;
    attrs.frameSize = PCM_16_BIT * CHANNELCOUNT / MOVE_LEFT_NUM;
    attrs.isBigEndian = false;
    attrs.isSignedData = true;
    attrs.startThreshold = DEEP_BUFFER_RENDER_PERIOD_SIZE / (PCM_16_BIT * attrs.channelCount / MOVE_LEFT_NUM);
    attrs.stopThreshold = INT_32_MAX;
    attrs.silenceThreshold = BUFFER_LENTH;
    return HDF_SUCCESS;
}

int32_t InitDevDesc(struct AudioDeviceDescriptor& devDesc, const uint32_t portId, enum AudioPortPin pins)
{
    devDesc.portId = portId;
    devDesc.pins = pins;
    devDesc.desc = nullptr;
    return HDF_SUCCESS;
}

int32_t SwitchAdapter(struct AudioAdapterDescriptor *descs, const std::string& adapterNameCase,
    enum AudioPortDirection portFlag, struct AudioPort *&audioPort, int size)
{
    if (descs == nullptr || size > ADAPTER_COUNT) {
        return HDF_FAILURE;
    }

    for (int index = 0; index < size; index++) {
        struct AudioAdapterDescriptor *desc = &descs[index];
        if (desc == nullptr || desc->adapterName == nullptr) {
            continue;
        }
        if (strcmp(desc->adapterName, adapterNameCase.c_str())) {
            continue;
        }
        for (uint32_t port = 0; port < desc->portNum; port++) {
            if (desc->ports[port].dir == portFlag) {
                audioPort = &desc->ports[port];
                return index;
            }
        }
    }
    return HDF_FAILURE;
}

int32_t GetAdapters(TestAudioManager *manager, struct AudioAdapterDescriptor **descs, int &size)
{
    int32_t ret = -1;
    if (manager == nullptr || descs == nullptr) {
        return HDF_FAILURE;
    }
    ret = manager->GetAllAdapters(manager, descs, &size);
    if (ret < 0 || *descs == nullptr || size != AUDIO_ADAPTER_MAX_NUM) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GetLoadAdapter(TestAudioManager *manager, struct AudioAdapter **adapter, struct AudioPort *&audioPort)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapterDescriptor *descs = nullptr;
    if (manager == nullptr || adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = GetAdapters(manager, &descs, size);
    if (ret < 0) {
        return ret;
    }
    if (descs == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    std::string adapterName = "usb";
    int index = SwitchAdapter(descs, adapterName, PORT_OUT_IN, audioPort, size);
    if (index < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    desc = &descs[index];
    ret = manager->LoadAdapter(manager, desc, adapter);
    if (ret < 0) {
        return ret;
    }
    if (*adapter == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioCreateRender(TestAudioManager *manager, struct AudioAdapter **adapter, struct AudioRender **render)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioPort *renderPort = nullptr;
    if (manager == nullptr || adapter == nullptr || render == nullptr) {
        return HDF_FAILURE;
    }
    ret = GetLoadAdapter(manager, adapter, renderPort);
    if (ret < 0) {
        return ret;
    }
    if (*adapter == nullptr || (*adapter)->CreateRender == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);
    ret = (*adapter)->CreateRender(*adapter, &devDesc, &attrs, render);
    if (ret < 0) {
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    if (*render == nullptr) {
        manager->UnloadAdapter(manager, *adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return HDF_SUCCESS;
}
int32_t AudioGetManagerCreateRender(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioRender **render)
{
    int32_t ret = -1;
    ret = GetManager(manager);
    if (ret < 0 || manager == nullptr) {
        return HDF_FAILURE;
    }
    ret = AudioCreateRender(manager, adapter, render);
    if (ret < 0 || adapter == nullptr || render == nullptr) {
        return HDF_FAILURE;
    }
    return ret;
}
int32_t AudioGetManagerCreateStartRender(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioRender **render)
{
    int32_t ret = -1;
    ret = AudioGetManagerCreateRender(manager, adapter, render);
    if (ret < 0 || adapter == nullptr || render == nullptr) {
        return HDF_FAILURE;
    }
    ret = (*render)->control.Start((AudioHandle)*render);
    if (ret < 0) {
        (*adapter)->DestroyRender(*adapter, *render);
        manager->UnloadAdapter(manager, *adapter);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
int32_t AudioCreateCapture(TestAudioManager *manager, struct AudioAdapter **adapter, struct AudioCapture **capture)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioPort *capturePort = nullptr;
    if (manager == nullptr || adapter == nullptr || capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = GetLoadAdapter(manager, adapter, capturePort);
    if (ret < 0) {
        return ret;
    }
    if (*adapter == nullptr || (*adapter)->CreateCapture == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InitAttrs(attrs);
    InitDevDesc(devDesc, capturePort->portId, PIN_IN_MIC);
    ret = (*adapter)->CreateCapture(*adapter, &devDesc, &attrs, capture);
    if (ret < 0) {
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    if (*capture == nullptr) {
        manager->UnloadAdapter(manager, *adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return HDF_SUCCESS;
}
int32_t AudioGetManagerCreateCapture(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioCapture **capture)
{
    int32_t ret = -1;
    ret = GetManager(manager);
    if (ret < 0 || manager == nullptr) {
        return HDF_FAILURE;
    }
    ret = AudioCreateCapture(manager, adapter, capture);
    if (ret < 0 || adapter == nullptr || capture == nullptr) {
        return HDF_FAILURE;
    }
    return ret;
}
int32_t AudioGetManagerCreateStartCapture(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioCapture **capture)
{
    int32_t ret = -1;
    ret = AudioGetManagerCreateCapture(manager, adapter, capture);
    if (ret < 0 || adapter == nullptr || capture == nullptr || manager == nullptr) {
        return HDF_FAILURE;
    }
    ret = (*capture)->control.Start((AudioHandle)*capture);
    if (ret < 0) {
        (*adapter)->DestroyCapture(*adapter, *capture);
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    return HDF_SUCCESS;
}
int32_t InitMmapDesc(FILE *fp, struct AudioMmapBufferDescripter &desc, int32_t &reqSize, bool flag)
{
    if (fp == NULL) {
        return HDF_FAILURE;
    }
    int fd = fileno(fp);
    if (fd == -1) {
        return HDF_FAILURE;
    }
    if (flag) {
        fseek(fp, 0, SEEK_END);
        reqSize = ftell(fp);
        desc.offset = 134;
    } else {
        reqSize = FILE_CAPTURE_SIZE;
        ftruncate(fd, FILE_CAPTURE_SIZE);
        desc.offset = 0;
    }
    desc.memoryFd = fd;
    desc.isShareable = 1;
    desc.transferFrameSize = DEEP_BUFFER_RENDER_PERIOD_SIZE / FRAME_COUNT;
    return HDF_SUCCESS;
}
}
}