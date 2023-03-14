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
    int32_t ret = GetManager(manager);
    if (ret < 0 || manager == nullptr) {
        return 0;
    }
    struct AudioAdapter *adapter = nullptr;
    struct AudioPort *renderPort = nullptr;
    ret = GetLoadAdapter(manager, &adapter, renderPort);
    if (ret < 0 || adapter == nullptr || renderPort == nullptr) {
        return 0;
    }
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);
    struct AudioRender *render = nullptr;
    uint8_t *dataFuzz = const_cast<uint8_t *>(data);
    struct AudioAdapter *adapterFuzz = reinterpret_cast<struct AudioAdapter *>(dataFuzz);
    adapter->CreateRender(adapterFuzz, &devDesc, &attrs, &render);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
    return 0;
}