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

#include "components/ui_box_scroll_bar.h"

#include "draw/draw_rect.h"
#include "engines/gfx/gfx_engine_manager.h"

namespace {
constexpr uint16_t SCROLL_BAR_MIN_LEN = 10;
} // namespace

namespace OHOS {
void UIBoxScrollBar::SetPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    if ((width > 0) && (height > 0)) {
        backgroundRect_.SetRect(x, y, x + width - 1, y + height - 1);
    }
}

void UIBoxScrollBar::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    Rect rect;
    if (!rect.Intersect(invalidatedArea, backgroundRect_)) {
        return;
    }
    /* Draw background */
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, rect, *backgroundStyle_, opacity_);

    /* Draw foreground */
    if (backgroundRect_.GetWidth() < backgroundRect_.GetHeight()) {
        int16_t forgroundHeight = static_cast<int16_t>(foregroundProportion_ * backgroundRect_.GetHeight());
        if (forgroundHeight < SCROLL_BAR_MIN_LEN) {
            forgroundHeight = SCROLL_BAR_MIN_LEN;
        }
        int16_t forgroundTop = backgroundRect_.GetTop() +
                               static_cast<int16_t>(scrollProgress_ * (backgroundRect_.GetHeight() - forgroundHeight));
        rect.SetRect(backgroundRect_.GetLeft(), forgroundTop, backgroundRect_.GetRight(),
                     forgroundTop + forgroundHeight - 1);
    } else {
        int16_t forgroundWidth = static_cast<int16_t>(foregroundProportion_ * backgroundRect_.GetWidth());
        if (forgroundWidth < SCROLL_BAR_MIN_LEN) {
            forgroundWidth = SCROLL_BAR_MIN_LEN;
        }
        int16_t forgroundLeft = backgroundRect_.GetLeft() +
                                static_cast<int16_t>(scrollProgress_ * (backgroundRect_.GetWidth() - forgroundWidth));
        rect.SetRect(forgroundLeft, backgroundRect_.GetTop(), forgroundLeft + forgroundWidth - 1,
                     backgroundRect_.GetBottom());
    }

    rect.Intersect(invalidatedArea, rect);
    // 8: Shift right 8 bits
    backgroundOpa = (backgroundOpa == OPA_OPAQUE) ? opacity_ : (static_cast<uint16_t>(backgroundOpa) * opacity_) >> 8;
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, rect, *foregroundStyle_, backgroundOpa);
}
} // namespace OHOS
