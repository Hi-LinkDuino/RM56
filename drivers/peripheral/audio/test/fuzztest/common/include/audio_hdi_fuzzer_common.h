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
 * @brief Defines audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter, and rendering and capturing audios.
 *
 * @since 1.0
 * @version 1.0
 */

/*
 * @file audio_adapter.h
 *
 * @brief Declares APIs for operations related to the audio adapter.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_HDI_FUZZER_COMMON_H
#define AUDIO_HDI_FUZZER_COMMON_H
#include <iostream>
#include <cstring>
#include "audio_adapter.h"
#include "audio_internal.h"
#include "audio_types.h"
#include "audio_proxy_manager.h"

namespace HMOS {
namespace Audio {
using namespace std;

#ifdef AUDIO_ADM_SO
    const std::string FUNCTION_NAME = "GetAudioManagerFuncs";
    const std::string RESOLVED_PATH = HDF_LIBRARY_FULL_PATH("libhdi_audio");
    using TestAudioManager = struct AudioManager;
#endif
#ifdef AUDIO_ADM_SERVICE
    const std::string FUNCTION_NAME = "GetAudioProxyManagerFuncs";
    const std::string RESOLVED_PATH = HDF_LIBRARY_FULL_PATH("libhdi_audio_client");
    using TestAudioManager = struct AudioProxyManager;
#endif
const uint32_t INT_32_MAX = 0x7fffffff;
const uint32_t MOVE_RIGHT_NUM = 3;
const int MOVE_LEFT_NUM = 8;
const int CHANNELCOUNT = 2;
const int FILE_CAPTURE_SIZE = 1024 * 1024 * 1;
const int SAMPLERATE = 48000;
const int DEEP_BUFFER_RENDER_PERIOD_SIZE = 4096;
const int BUFFER_LENTH = 1024 * 16;
const uint64_t MEGABYTE = 1024;
const int FRAME_SIZE = 1024;
const int FRAME_COUNT = 4;
const int ADAPTER_COUNT = 32;
const int TRY_NUM_FRAME = 20;
const int AUDIO_ADAPTER_MAX_NUM = 3;
const string AUDIO_LOW_LATENCY_CAPTURE_FILE = "//bin/lowlatencycapturetest.wav";
const string AUDIO_LOW_LATENCY_RENDER_FILE = "//bin/lowlatencyrendertest.wav";


enum AudioPCMBit {
    PCM_8_BIT  = 8,
    PCM_16_BIT = 16,
    PCM_24_BIT = 24,
    PCM_32_BIT = 32,
};
int32_t GetManager(TestAudioManager *&manager);

int32_t InitAttrs(struct AudioSampleAttributes& attrs);

int32_t InitDevDesc(struct AudioDeviceDescriptor& devDesc, const uint32_t portId, enum AudioPortPin pins);

int32_t GetAdapters(TestAudioManager *manager, struct AudioAdapterDescriptor **descs, int &size);

int32_t GetLoadAdapter(TestAudioManager *manager, struct AudioAdapter **adapter, struct AudioPort *&audioPort);

int32_t AudioCreateRender(TestAudioManager *manager, struct AudioAdapter **adapter, struct AudioRender **render);

int32_t AudioGetManagerCreateRender(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioRender **render);

int32_t AudioGetManagerCreateStartRender(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioRender **render);

int32_t AudioCreateCapture(TestAudioManager *manager, struct AudioAdapter **adapter, struct AudioCapture **capture);

int32_t AudioGetManagerCreateCapture(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioCapture **capture);

int32_t AudioGetManagerCreateStartCapture(TestAudioManager *&manager, struct AudioAdapter **adapter,
    struct AudioCapture **capture);

int32_t InitMmapDesc(FILE *fp, struct AudioMmapBufferDescripter &desc, int32_t &reqSize, bool flag);
}
}
#endif // AUDIO_HDI_FUZZER_COMMON_H

