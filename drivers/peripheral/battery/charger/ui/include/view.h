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

#ifndef CHARGER_UI_VIEW_H
#define CHARGER_UI_VIEW_H

#include <mutex>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class View {
public:
    struct BRGA888Pixel {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    struct RGB888Pixel {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    enum class PixelFormat {
        BGRA888,
    };
    View() = default;;
    virtual ~View() = default;;
    void* CreateBuffer(int32_t w, int32_t h, View::PixelFormat pixelFormat);
    virtual void SetBackgroundColor(BRGA888Pixel* color);
    virtual void DrawSubView(int32_t x, int32_t y, int32_t w, int32_t h, char* buf);
    virtual void OnDraw();
    virtual void Hide();
    virtual void Show();
    virtual void OnFocus(bool foucsed);
    char* GetBuffer() const;
    void* GetRawBuffer() const;
    int32_t GetBufferSize() const
    {
        return bufferSize_;
    }
    void SyncBuffer();
    void SetViewId(int32_t id);
    int32_t GetViewId() const;
    void FreeBuffer();
    bool IsVisiable() const;
    bool IsSelected() const;
    bool IsFocusAble() const;
    void SetFocusAble(bool focusable);
    int32_t startX_ = 0;
    int32_t startY_ = 0;
    int32_t viewWidth_ = 0;
    int32_t viewHeight_ = 0;
    std::mutex mutex_;
private:
    char* viewBuffer_ = nullptr;
    char* shadowBuffer_ = nullptr;
    int32_t bufferSize_ = 0;
    bool isVisiable_ = true;
    int32_t viewId_ = 0;
    bool focusable_ = false;
    bool isFocused_ = false;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_VIEW_H
