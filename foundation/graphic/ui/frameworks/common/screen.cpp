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

#include "common/screen.h"
#include "core/render_manager.h"
#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/mem_api.h"
#include "securec.h"

namespace OHOS {
Screen& Screen::GetInstance()
{
    static Screen instance;
    return instance;
}

uint16_t Screen::GetWidth()
{
    return BaseGfxEngine::GetInstance()->GetScreenWidth();
}

uint16_t Screen::GetHeight()
{
    return BaseGfxEngine::GetInstance()->GetScreenHeight();
}

bool Screen::GetCurrentScreenBitmap(ImageInfo& info)
{
    BufferInfo* bufferInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
    if (bufferInfo == nullptr) {
        return false;
    }
    uint16_t screenWidth = BaseGfxEngine::GetInstance()->GetScreenWidth();
    uint16_t screenHeight = BaseGfxEngine::GetInstance()->GetScreenHeight();
    info.header.colorMode = ARGB8888;
    info.dataSize = screenWidth * screenHeight * DrawUtils::GetByteSizeByColorMode(info.header.colorMode);
    info.data = reinterpret_cast<uint8_t*>(ImageCacheMalloc(info));
    if (info.data == nullptr) {
        return false;
    }
    info.header.width = screenWidth;
    info.header.height = screenHeight;
    info.header.reserved = 0;
    info.header.compressMode = 0;

    Rect screenRect = {0, 0, static_cast<int16_t>(screenWidth - 1), static_cast<int16_t>(screenHeight - 1)};
    Point dstPos = {0, 0};
    BlendOption blendOption;
    blendOption.opacity = OPA_OPAQUE;

    BufferInfo dstBufferInfo;
    dstBufferInfo.rect = screenRect;
    dstBufferInfo.mode = ARGB8888;
    dstBufferInfo.color = 0x44;
    dstBufferInfo.phyAddr = dstBufferInfo.virAddr = static_cast<void*>(const_cast<uint8_t*>(info.data));
    dstBufferInfo.stride = screenWidth * 4; // 4: bpp
    dstBufferInfo.width = screenWidth;
    dstBufferInfo.height = screenHeight;

    BaseGfxEngine::GetInstance()->Blit(dstBufferInfo, dstPos, *bufferInfo, screenRect, blendOption);
    return true;
}

ScreenShape Screen::GetScreenShape()
{
    return BaseGfxEngine::GetInstance()->GetScreenShape();
}
} // namespace OHOS
