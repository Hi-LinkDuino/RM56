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

#include "components/ui_arc_scroll_bar.h"

#include "draw/draw_arc.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_types.h"

namespace {
constexpr uint16_t RIGHT_SIDE_START_ANGLE_IN_DEGREE = 60;
constexpr uint16_t RIGHT_SIDE_END_ANGLE_IN_DEGREE = 120;
constexpr uint16_t LEFT_SIDE_START_ANGLE_IN_DEGREE = 240;
constexpr uint16_t LEFT_SIDE_END_ANGLE_IN_DEGREE = 300;
constexpr uint16_t SCROLL_BAR_MIN_ARC = 10;
} // namespace

namespace OHOS {
UIArcScrollBar::UIArcScrollBar()
    : radius_(0),
      width_(0),
      startAngle_(RIGHT_SIDE_START_ANGLE_IN_DEGREE),
      endAngle_(RIGHT_SIDE_END_ANGLE_IN_DEGREE),
      center_({0, 0}),
      side_(SCROLL_BAR_RIGHT_SIDE)
{
}

void UIArcScrollBar::SetPosition(int16_t x, int16_t y, int16_t width, int16_t radius)
{
    if ((width > 0) && (radius > 0)) {
        center_.x = x;
        center_.y = y;
        width_ = width;
        radius_ = radius;
    }
}

void UIArcScrollBar::SetScrollBarSide(uint8_t side)
{
    if (side == SCROLL_BAR_RIGHT_SIDE) {
        startAngle_ = RIGHT_SIDE_START_ANGLE_IN_DEGREE;
        endAngle_ = RIGHT_SIDE_END_ANGLE_IN_DEGREE;
    } else {
        startAngle_ = LEFT_SIDE_START_ANGLE_IN_DEGREE;
        endAngle_ = LEFT_SIDE_END_ANGLE_IN_DEGREE;
    }
    side_ = side;
}

void UIArcScrollBar::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    // 8: Shift right 8 bits
    backgroundOpa = (backgroundOpa == OPA_OPAQUE) ? opacity_ : (static_cast<uint16_t>(backgroundOpa) * opacity_) >> 8;
    DrawBackground(gfxDstBuffer, invalidatedArea, backgroundOpa);
    DrawForeground(gfxDstBuffer, invalidatedArea, backgroundOpa);
}

void UIArcScrollBar::DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    uint16_t foregoundAngleRange = static_cast<uint16_t>(foregroundProportion_ * (endAngle_ - startAngle_));
    if (foregoundAngleRange < SCROLL_BAR_MIN_ARC) {
        foregoundAngleRange = SCROLL_BAR_MIN_ARC;
    }
    int16_t startAngle;
    int16_t endAngle;
    int16_t minAngle;
    int16_t maxAngle;
    if (side_ == SCROLL_BAR_RIGHT_SIDE) {
        minAngle = startAngle_;
        maxAngle = endAngle_ - foregoundAngleRange;
        startAngle = minAngle + static_cast<int16_t>(scrollProgress_ * (maxAngle - minAngle));
        endAngle = startAngle + foregoundAngleRange;
    } else {
        maxAngle = endAngle_;
        minAngle = startAngle_ + foregoundAngleRange;
        endAngle = maxAngle - static_cast<int16_t>(scrollProgress_ * (maxAngle - minAngle));
        startAngle = endAngle - foregoundAngleRange;
    }
    if ((startAngle > endAngle_) || (endAngle < startAngle_)) {
        return;
    }
    ArcInfo arcInfo = {{0, 0}, {0, 0}, 0, 0, 0, nullptr};
    arcInfo.radius = (radius_ > 0) ? (radius_ - 1) : 0;
    arcInfo.center = center_;
    arcInfo.startAngle = MATH_MAX(startAngle, startAngle_);
    arcInfo.endAngle = MATH_MIN(endAngle, endAngle_);
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, *foregroundStyle_, backgroundOpa,
                                          foregroundStyle_->lineCap_);
}

void UIArcScrollBar::DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    ArcInfo arcInfo = {{0, 0}, {0, 0}, 0, 0, 0, nullptr};
    arcInfo.radius = radius_;
    arcInfo.center = center_;
    arcInfo.startAngle = startAngle_;
    arcInfo.endAngle = endAngle_;
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, *backgroundStyle_, backgroundOpa,
                                          backgroundStyle_->lineCap_);
}
} // namespace OHOS
