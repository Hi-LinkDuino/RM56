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
    FILE *fp = fopen(AUDIO_LOW_LATENCY_RENDER_FILE.c_str(), "wb+");
    if (fp == nullptr) {
        return 0;
    }
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    int32_t ret = AudioGetManagerCreateStartRender(manager, &adapter, &render);
    if (ret < 0 || adapter == nullptr || render == nullptr || manager == nullptr) {
        fclose(fp);
        return ret;
    }
    bool isRender = true;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    if (ret < 0) {
        adapter->DestroyRender(adapter, render);
        manager->UnloadAdapter(manager, adapter);
        fclose(fp);
        return 0;
    }
    uint8_t *dataFuzz = const_cast<uint8_t *>(data);
    struct AudioRender *renderFuzz = reinterpret_cast<struct AudioRender *>(dataFuzz);
    render->attr.ReqMmapBuffer((AudioHandle)renderFuzz, reqSize, &desc);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
    ret = fclose(fp);
    if (ret != 0) {
        return ret;
    }
    return 0;
}