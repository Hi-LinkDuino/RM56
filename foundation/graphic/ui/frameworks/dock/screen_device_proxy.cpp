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

#include "dock/screen_device_proxy.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
ScreenDeviceProxy* ScreenDeviceProxy::GetInstance()
{
    static ScreenDeviceProxy instance;
    return &instance;
}

void ScreenDeviceProxy::Flush() {}

void ScreenDeviceProxy::OnFlushReady()
{
    flush_.Notify();
}

void ScreenDeviceProxy::OnRenderFinish(const Rect& mask)
{
    if (device_ != nullptr) {
        device_->RenderFinish(mask);
    }
}

void ScreenDeviceProxy::DrawAnimatorBuffer(const Rect& invalidatedArea)
{
}

void ScreenDeviceProxy::SetAnimatorRect(const Rect& rect)
{
    curViewRect_ = rect;
    uint16_t bufferWidth = (width_ > curViewRect_.GetWidth()) ? curViewRect_.GetWidth() : width_;
    uint16_t bufferHeight = (height_ > curViewRect_.GetHeight()) ? curViewRect_.GetHeight() : height_;

    animatorImageInfo_.header.colorMode = animatorBufferMode_;
    animatorImageInfo_.dataSize = bufferWidth * bufferHeight * DrawUtils::GetByteSizeByColorMode(animatorBufferMode_);
    animatorImageInfo_.header.width = bufferWidth;
    animatorImageInfo_.header.height = bufferHeight;
    animatorImageInfo_.header.reserved = 0;
    animatorImageInfo_.data = reinterpret_cast<uint8_t*>(GetBuffer());
    if (animatorImageInfo_.data == nullptr) {
        return;
    }

    SetAnimatorbufferWidth(bufferWidth);
    if (memset_s(reinterpret_cast<void*>(const_cast<uint8_t*>(animatorImageInfo_.data)), animatorImageInfo_.dataSize, 0,
        animatorImageInfo_.dataSize) != EOK) {
        GRAPHIC_LOGE("animator buffer memset failed.");
    }
}

void ScreenDeviceProxy::SetScreenSize(uint16_t width, uint16_t height)
{
    if ((width == 0) || (height == 0)) {
        GRAPHIC_LOGE("screen size can not be zero.");
        return;
    }
    width_ = width;
    height_ = height;
}

uint8_t* ScreenDeviceProxy::GetBuffer()
{
    if (enableBitmapBuffer_) {
        return viewBitmapBuffer_;
    }
    flush_.Wait();
    if (useAnimatorBuff_) {
        if (animatorBufferAddr_ == nullptr) {
            GRAPHIC_LOGE("Invalid param animatorBufferAddr_.");
            return nullptr;
        }
        return animatorBufferAddr_;
    }
    if (frameBufferAddr_ == nullptr) {
        GRAPHIC_LOGE("Invalid param frameBufferAddr_.");
        return nullptr;
    }
    return frameBufferAddr_;
}

ColorMode ScreenDeviceProxy::GetBufferMode()
{
    if (useAnimatorBuff_) {
        return animatorBufferMode_;
    }
    return frameBufferMode_;
}

void ScreenDeviceProxy::EnableBitmapBuffer(uint8_t* viewBitmapBuffer)
{
    if (viewBitmapBuffer == nullptr) {
        return;
    }
    viewBitmapBuffer_ = viewBitmapBuffer;
    enableBitmapBuffer_ = true;
}

bool ScreenDeviceProxy::GetScreenBitmapBuffer(uint8_t* dest, uint32_t size)
{
    if ((dest == nullptr) || (size == 0)) {
        return false;
    }
    uint8_t byteSize = DrawUtils::GetByteSizeByColorMode(frameBufferMode_);
    uint32_t bufSize = width_ * height_ * byteSize;
    if (size < bufSize) {
        return false;
    }
    uint8_t* buf = GetBuffer();
    if (buf == nullptr) {
        return false;
    }
    if (memcpy_s(dest, size, buf, bufSize) != EOK) {
        return false;
    }
    return true;
}
} // namespace OHOS
