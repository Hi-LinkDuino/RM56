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

#ifndef GRAPHIC_LITE_SCREEN_DEVICE_PROXY_H
#define GRAPHIC_LITE_SCREEN_DEVICE_PROXY_H

#include "gfx_utils/color.h"
#include "dock/screen_device.h"
#include "gfx_utils/graphic_assert.h"
#include "graphic_semaphore.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/transform.h"
#if ENABLE_WINDOW
#include "surface.h"
#endif

namespace OHOS {
/** @brief A display device proxy */
class ScreenDeviceProxy : public HeapBase {
public:
    static ScreenDeviceProxy* GetInstance();

    void SetDevice(ScreenDevice* device)
    {
        device_ = device;
    }

    void Flush();

    void OnFlushReady();

    void OnRenderFinish(const Rect& mask);

    bool HardwareFill(const Rect& fillArea,
                      uint32_t color,
                      OpacityType opa,
                      uint8_t* dst,
                      uint32_t dstStride,
                      ColorMode dstColorMode)
    {
        if (device_ != nullptr) {
            return device_->HardwareFill(fillArea, color, opa, dst, dstStride, dstColorMode);
        }
        return false;
    }

    bool HardwareBlend(const uint8_t* src,
                       const Rect& srcRect,
                       uint32_t srcStride,
                       uint32_t srcLineNumber,
                       ColorMode srcColorMode,
                       uint32_t color,
                       OpacityType opa,
                       uint8_t* dst,
                       uint32_t dstStride,
                       ColorMode dstColorMode,
                       uint32_t x,
                       uint32_t y)
    {
        if (device_ != nullptr) {
            return device_->HardwareBlend(src, srcRect, srcStride, srcLineNumber, srcColorMode,
                                          color, opa, dst, dstStride, dstColorMode, x, y);
        }
        return false;
    }

    bool HardwareTransform(const uint8_t* src,
                           ColorMode srcColorMode,
                           const Rect& srcRect,
                           const Matrix3<float>& transformMatrix,
                           OpacityType opa,
                           uint32_t color,
                           const Rect& mask,
                           uint8_t* dst,
                           uint32_t dstStride,
                           ColorMode dstColorMode,
                           const TransformOption& option)
    {
        if (device_ != nullptr) {
            return device_->HardwareTransform(src, srcColorMode, srcRect, transformMatrix, opa, color, mask, dst,
                                              dstStride, dstColorMode, option);
        }
        return false;
    }

    void SnapShot(uint32_t len, bool justCopy, uint8_t* dest, const Rect& rect, bool justRender)
    {
        if (device_ != nullptr) {
            device_->SnapShot(len, justCopy, dest, rect, justRender);
        }
    }

    void SetFramebuffer(uint8_t* addr, ColorMode mode, uint16_t width)
    {
        frameBufferAddr_ = addr;
        frameBufferWidth_ = width;
        frameBufferMode_ = mode;
    }

    void SetAnimatorbuffer(uint8_t* addr, ColorMode mode, uint16_t width)
    {
        animatorBufferAddr_ = addr;
        animatorBufferWidth_ = width;
        animatorBufferMode_ = mode;
    }

    void SetAnimatorbufferWidth(uint16_t width)
    {
        animatorBufferWidth_ = width;
    }

    void EnableAnimatorBuffer(bool status)
    {
        useAnimatorBuff_ = status;
    }

    void SetAnimatorRect(const Rect& rect);

    void SetAnimatorTransMap(TransformMap& transMap)
    {
        transMap_ = transMap;
    }

    void DrawAnimatorBuffer(const Rect& invalidatedArea);

    bool GetAnimatorBufferStatus()
    {
        return useAnimatorBuff_;
    }

#if ENABLE_WINDOW
    AllocationInfo& GetAllocationInfo()
    {
        return gfxAlloc_;
    }
#endif

    uint16_t GetBufferWidth() const
    {
        if (enableBitmapBuffer_) {
            return bitmapBufferWidth_;
        }
        if (useAnimatorBuff_) {
            return animatorBufferWidth_;
        }
        return frameBufferWidth_;
    }

    void SetScreenSize(uint16_t width, uint16_t height);

    uint16_t GetScreenWidth() const
    {
        return width_;
    }

    uint16_t GetScreenHeight() const
    {
        return height_;
    }

    uint32_t GetScreenArea() const
    {
        return width_ * height_;
    }

    uint8_t* GetBuffer();

    ColorMode GetBufferMode();

    void SetViewBitmapBufferWidth(uint16_t width)
    {
        bitmapBufferWidth_ = width;
    }

    void EnableBitmapBuffer(uint8_t* viewBitmapBuffer);

    void DisableBitmapBuffer()
    {
        enableBitmapBuffer_ = false;
    }

    bool GetScreenBitmapBuffer(uint8_t* dest, uint32_t size);
;

private:
    ScreenDeviceProxy() {}
    virtual ~ScreenDeviceProxy() {}

    ScreenDeviceProxy(const ScreenDeviceProxy&) = delete;
    ScreenDeviceProxy& operator=(const ScreenDeviceProxy&) = delete;
    ScreenDeviceProxy(ScreenDeviceProxy&&) = delete;
    ScreenDeviceProxy& operator=(ScreenDeviceProxy&&) = delete;

    ScreenDevice* device_ = nullptr;
    FlushSem flush_ = FlushSem(false);
    uint16_t width_ = HORIZONTAL_RESOLUTION;
    uint16_t height_ = VERTICAL_RESOLUTION;

    uint8_t* frameBufferAddr_ = nullptr;
    uint16_t frameBufferWidth_ = 0;
    ColorMode frameBufferMode_ = ARGB8888;

    uint8_t* animatorBufferAddr_ = nullptr;
    uint16_t animatorBufferWidth_ = 0;
    ColorMode animatorBufferMode_ = ARGB8888;
    Rect curViewRect_;
    TransformMap transMap_;
    bool useAnimatorBuff_ = false;
    ImageInfo animatorImageInfo_ = {{0}};
    // snapshot related
    uint8_t* viewBitmapBuffer_ = nullptr;
    uint16_t bitmapBufferWidth_ = 0;
    bool enableBitmapBuffer_ = false;
    // snapshot related
#if ENABLE_WINDOW
    AllocationInfo gfxAlloc_ = {0};
#endif
};
} // namespace OHOS
#endif // GRAPHIC_LITE_SCREEN_DEVICE_PROXY_H
