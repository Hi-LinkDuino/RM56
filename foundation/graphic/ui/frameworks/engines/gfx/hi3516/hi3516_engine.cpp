/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "engines/gfx/hi3516/hi3516_engine.h"
#include "draw/draw_utils.h"
#include "hals/gfx_engines.h"
#include "lite_wm_type.h"

namespace OHOS {
const int16_t HARDWARE_ACC_SIZE_LIMIT = 50 * 50;

__attribute__((constructor)) void RegisterHi3516GfxEngine()
{
    BaseGfxEngine::InitGfxEngine(new Hi3516GfxEngine());
}

void Hi3516GfxEngine::Fill(BufferInfo& dst,
                           const Rect& fillArea,
                           const ColorType color,
                           const OpacityType opacity)
{
#if ENABLE_GFX_ENGINES
    if ((opacity != OPA_OPAQUE) && (fillArea.GetSize() >= HARDWARE_ACC_SIZE_LIMIT)) {
        LiteSurfaceData data;
        data.phyAddr = static_cast<uint8_t *>(dst.phyAddr);
        data.width = dst.width;
        data.height = dst.height;
        data.stride = dst.stride;
        data.pixelFormat = IMAGE_PIXEL_FORMAT_ARGB8888;
        if (GfxEngines::GetInstance()->GfxFillArea(data, fillArea, color, opacity)) {
            return;
        }
    }
#endif
    BaseGfxEngine::Fill(dst, fillArea, color, opacity);
}
}
