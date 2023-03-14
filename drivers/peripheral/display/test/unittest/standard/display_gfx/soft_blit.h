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

#ifndef BLIT_CHECK_H
#define BLIT_CHECK_H
#include "display_type.h"

namespace {
struct ColorRGBAf {
    ColorRGBAf(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
    inline void Clear()
    {
        mR = 0;
        mB = 0;
        mG = 0;
        mA = 0;
    }
    float mR = 0;
    float mB = 0;
    float mG = 0;
    float mA = 0;
};

struct ColorRGBA32 {
    explicit ColorRGBA32(ColorRGBAf colorFrom);
    explicit ColorRGBA32(uint32_t pixel);
    bool Compare(ColorRGBA32 &other);
    uint32_t mC = 0;
    uint8_t mR = 0;
    uint8_t mB = 0;
    uint8_t mG = 0;
    uint8_t mA = 0;
};
}
class SoftBlit {
public:
    SoftBlit(const BufferHandle &srcBuffer, const IRect &srcRect, const BufferHandle &dstBuffer, const IRect &dstRect,
        const BlendType type);
    bool RunAndCheck(const BufferHandle &exBuffer);
    ~SoftBlit() {}

private:
    IRect mSrcRect = { 0 };
    IRect mDstRect = { 0 };
    BufferHandle mSrcBuffer = { 0 };
    BufferHandle mDstBuffer = { 0 };
    BlendType mType = BLEND_BUTT;
};

#endif // BLIT_CHECK_H
