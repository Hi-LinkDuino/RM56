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

#include "audio_hdi_fuzzer_common.h"

using namespace HMOS::Audio;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    TestAudioManager *manager = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    int32_t ret = AudioGetManagerCreateCapture(manager, &adapter, &capture);
    if (ret < 0 || adapter == nullptr || capture == nullptr || manager == nullptr) {
        return ret;
    }
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    if (fp == nullptr) {
        adapter->DestroyCapture(adapter, capture);
        manager->UnloadAdapter(manager, adapter);
        return 0;
    }
    ret = capture->control.Start((AudioHandle)capture);
    if (ret < 0 || manager == nullptr) {
        adapter->DestroyCapture(adapter, capture);
        manager->UnloadAdapter(manager, adapter);
        fclose(fp);
        return ret;
    }
    bool isRender = true;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    if (ret < 0 || manager == nullptr) {
        adapter->DestroyCapture(adapter, capture);
        manager->UnloadAdapter(manager, adapter);
        fclose(fp);
        return 0;
    }
    uint8_t *dataFuzz = const_cast<uint8_t *>(data);
    struct AudioCapture *captureFuzz = reinterpret_cast<struct AudioCapture *>(dataFuzz);
    capture->attr.ReqMmapBuffer((AudioHandle)captureFuzz, reqSize, &desc);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
    fclose(fp);
    return 0;
}