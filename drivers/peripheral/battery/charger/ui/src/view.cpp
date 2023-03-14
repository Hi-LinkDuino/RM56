/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "view.h"

#include "securec.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
constexpr int32_t RGBA_PIXEL_SIZE = 4;
void* View::CreateBuffer(int32_t w, int32_t h, View::PixelFormat pixelFormat)
{
    int32_t pixelSize = -1;
    switch (static_cast<int32_t>(pixelFormat)) {
        case int32_t(View::PixelFormat::BGRA888):
            pixelSize = RGBA_PIXEL_SIZE;
            break;
        default:
            BATTERY_HILOGD(FEATURE_CHARGING, "Unsupported pixel_format=%{public}d, Use default BGRA888",
                           static_cast<int32_t>(pixelFormat));
            pixelSize = RGBA_PIXEL_SIZE;
            break;
    }
    bufferSize_ = w * h * pixelSize;
    viewBuffer_ = static_cast<char*>(malloc(bufferSize_));
    if (viewBuffer_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "Allocate memory for view failed: %{public}d", errno);
        return nullptr;
    }
    shadowBuffer_ = static_cast<char*>(malloc(bufferSize_));
    if (shadowBuffer_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "Allocate memory for shadow failed: %{public}d", errno);
        free(viewBuffer_);
        viewBuffer_ = nullptr;
        return nullptr;
    }
    if (memset_s(viewBuffer_, bufferSize_, 0, bufferSize_) != EOK) {
        BATTERY_HILOGE(FEATURE_CHARGING, "Clean view buffer failed.");
        free(viewBuffer_);
        viewBuffer_ = nullptr;
        return nullptr;
    }
    viewWidth_ = w;
    viewHeight_ = h;
    return viewBuffer_;
}

void View::SetBackgroundColor(BRGA888Pixel* color)
{
    BRGA888Pixel pixelBuffer[viewWidth_];
    for (int32_t w = 0; w < viewWidth_; w++) {
        pixelBuffer[w].r = color->r;
        pixelBuffer[w].g = color->g;
        pixelBuffer[w].b = color->b;
        pixelBuffer[w].a = color->a;
    }
    for (int32_t h = 0; h < viewHeight_; h++) {
        if (memcpy_s(viewBuffer_ + h * viewWidth_ * sizeof(BRGA888Pixel), viewWidth_ * sizeof(BRGA888Pixel) + 1,
            reinterpret_cast<char*>(pixelBuffer), viewWidth_ * sizeof(BRGA888Pixel)) != EOK) {
            return;
        }
    }
    if (isVisiable_) {
        OnDraw();
        BATTERY_HILOGD(FEATURE_CHARGING, "view---visible");
    }
}

void View::DrawSubView(int32_t x, int32_t y, int32_t w, int32_t h, char* buf)
{
    int32_t minWidth = ((x + w) <= viewWidth_) ? w : (viewWidth_ - x);
    int32_t minHeight = ((y + h) <= viewHeight_) ? h : (viewHeight_ - y);
    BATTERY_HILOGD(FEATURE_CHARGING, "x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", x, y, w, h);
    BATTERY_HILOGD(FEATURE_CHARGING, "minWidth = %{public}d, minHeight = %{public}d", minWidth, minHeight);
    for (int32_t i = 0; i < minHeight; i++) {
        char* src = buf + i * w * static_cast<int32_t>(sizeof(BRGA888Pixel));
        char* dst = shadowBuffer_ + (i + y) * viewWidth_ * static_cast<int32_t>(sizeof(BRGA888Pixel)) +
            x * static_cast<int32_t>(sizeof(BRGA888Pixel));
        if (memcpy_s(dst, minWidth * static_cast<int32_t>(sizeof(BRGA888Pixel)) + 1, src,
            minWidth * static_cast<int32_t>(sizeof(BRGA888Pixel))) != EOK) {
            return;
        }
    }
}

void View::OnDraw()
{
    std::unique_lock<std::mutex> locker(mutex_);
    SyncBuffer();
}

void View::Hide()
{
    if (isVisiable_) {
        isVisiable_ = false;
        OnDraw();
    }
}

void View::Show()
{
    if (!isVisiable_) {
        isVisiable_ = true;
        OnDraw();
    }
}

void View::SyncBuffer()
{
    if (memcpy_s(shadowBuffer_, bufferSize_, viewBuffer_, bufferSize_) != EOK) {
        BATTERY_HILOGD(FEATURE_CHARGING, "Sync buffer failed.");
    }
}

char* View::GetBuffer() const
{
    return shadowBuffer_;
}

void* View::GetRawBuffer() const
{
    return viewBuffer_;
}

void View::OnFocus(bool foucsed)
{
    isFocused_ = foucsed;
    OnDraw();
}

void View::SetViewId(int32_t id)
{
    viewId_ = id;
}

int32_t View::GetViewId() const
{
    return viewId_;
}

void View::FreeBuffer()
{
    free(viewBuffer_);
    free(shadowBuffer_);
    viewBuffer_ = nullptr;
    shadowBuffer_ = nullptr;
}

bool View::IsVisiable() const
{
    return isVisiable_;
}

bool View::IsSelected() const
{
    return isFocused_;
}

bool View::IsFocusAble() const
{
    return focusable_;
}

void View::SetFocusAble(bool focusable)
{
    focusable_ = focusable;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
