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

#ifndef GRAPHIC_LITE_UI_ABSTRACT_SCROLL_BAR_H
#define GRAPHIC_LITE_UI_ABSTRACT_SCROLL_BAR_H

#include "gfx_utils/rect.h"
#include "gfx_utils/style.h"
#include "gfx_utils/graphic_buffer.h"

namespace OHOS {
class UIAbstractScrollBar : public HeapBase {
public:
    UIAbstractScrollBar();

    virtual ~UIAbstractScrollBar() {}

    virtual void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height) {}

    virtual void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa) {}

    virtual void SetScrollBarSide(uint8_t side) {}

    void SetScrollProgress(float scrollProgress)
    {
        scrollProgress_ = scrollProgress;
    }

    void SetForegroundProportion(float foregroundPropotion)
    {
        if ((foregroundPropotion < 0) || (foregroundPropotion > 1.0f)) {
            return;
        }
        foregroundProportion_ = foregroundPropotion;
    }

    void SetOpacity(uint8_t opacity)
    {
        opacity_ = opacity;
    }

    uint8_t GetOpacity()
    {
        return opacity_;
    }

protected:
    uint8_t opacity_ = OPA_TRANSPARENT;
    float scrollProgress_ = 0;
    float foregroundProportion_ = 0;
    Style* backgroundStyle_ = nullptr;
    Style* foregroundStyle_ = nullptr;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_ABSTRACT_SCROLL_BAR_H
