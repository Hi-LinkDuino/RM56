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
    struct AudioRender *render = nullptr;
    int32_t ret = AudioGetManagerCreateStartRender(manager, &adapter, &render);
    if (ret < 0 || adapter == nullptr || render == nullptr || manager == nullptr) {
        return ret;
    }
    char keyValueList[] = "attr-route=1;attr-format=32;attr-channels=2;attr-frame-count=82;attr-sampling-rate=48000";
    char keyValueListValue[256] = {};
    int32_t listLenth = 256;
    ret = render->attr.SetExtraParams(render, keyValueList);
    if (ret < 0) {
        adapter->DestroyRender(adapter, render);
        manager->UnloadAdapter(manager, adapter);
        return ret;
    }
    uint8_t *dataFuzz = const_cast<uint8_t *>(data);
    struct AudioRender *renderFuzz = reinterpret_cast<struct AudioRender *>(dataFuzz);
    render->attr.GetExtraParams(renderFuzz, keyValueListValue, listLenth);
    render->control.Stop((AudioHandle)render);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
    render = nullptr;
    return 0;
}
