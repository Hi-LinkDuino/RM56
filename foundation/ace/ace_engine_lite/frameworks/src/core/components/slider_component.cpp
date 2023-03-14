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

#include "slider_component.h"
#include "ace_log.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
SliderComponent::SliderComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      minValue_(0),
      maxValue_(DEFAULT_VALUE),
      value_(0),
      startPosition_({0, 0}),
      direction_(UISlider::Direction::DIR_LEFT_TO_RIGHT)
{
    SetComponentName(K_SLIDER);
#if (FEATURE_ROTATION_API == 1)
    RegisterNamedFunction(FUNC_ROTATION_NAME, HandleRotationRequest);
#endif // FEATURE_ROTATION_API
}


inline UIView *SliderComponent::GetComponentRootView() const
{
    return const_cast<UISlider *>(&sliderView_);
}

bool SliderComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_MIN:
            minValue_ = IntegerOf(attrValue);
            break;
        case K_MAX:
            maxValue_ = IntegerOf(attrValue);
            break;
        case K_STEP:
            // depend on native implementation
            break;
        case K_VALUE:
            value_ = IntegerOf(attrValue);
            break;
        default:
            return false;
    }
    return true;
}

bool SliderComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
                                                   jerry_value_t funcValue,
                                                   bool isStopPropagation)
{
    switch (eventTypeId) {
        case K_CHANGE:
            sliderView_.SetSliderEventListener(&eventListener_);
            eventListener_.SetBindChangeFuncName(funcValue);
            break;
        default:
            return false;
    }

    sliderView_.SetDraggable(true);
    sliderView_.SetTouchable(true);
    return true;
}

bool SliderComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    uint16_t stylePropNameId = GetStylePropNameId(style);
    switch (stylePropNameId) {
        case K_COLOR: {
            // set slider background style color
            if (!GetStyleColorValue(style, color, alpha)) {
                return false;
            }
            sliderView_.SetBackgroundStyle(STYLE_BACKGROUND_COLOR, GetRGBColor(color).full);
            sliderView_.SetBackgroundStyle(STYLE_BACKGROUND_OPA, alpha);
            break;
        }
        case K_SELECTED_COLOR: {
            // set slider indicator style color
            if (!GetStyleColorValue(style, color, alpha)) {
                return false;
            }
            sliderView_.SetForegroundStyle(STYLE_BACKGROUND_COLOR, GetRGBColor(color).full);
            sliderView_.SetForegroundStyle(STYLE_BACKGROUND_OPA, alpha);
            break;
        }
        case K_BLOCK_COLOR: {
            // set slider knob style color
            if (!GetStyleColorValue(style, color, alpha)) {
                return false;
            }
            break;
        }
        case K_LEFT: {
            startPosition_.x = GetStylePixelValue(style);
            break;
        }
        case K_TOP: {
            startPosition_.y = GetStylePixelValue(style);
            break;
        }
        case K_DIRECTION: {
            SetDirection(GetStyleStrValue(style));
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}

void SliderComponent::PostUpdate(uint16_t attrKeyId)
{
    if (!KeyParser::IsKeyValid(attrKeyId)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "slider component post update check args failed");
        return;
    }
    switch (attrKeyId) {
        case K_LEFT:
        case K_TOP:
            sliderView_.SetPosition(startPosition_.x, startPosition_.y);
            sliderView_.Invalidate();
            break;
        case K_HEIGHT:
        case K_WIDTH: {
            SetWidthAndHeight();
            sliderView_.Invalidate();
            break;
        }
        case K_MAX:
        case K_MIN:
        case K_VALUE:
            sliderView_.SetRange(maxValue_, minValue_);
            sliderView_.SetValue(value_);
            sliderView_.Invalidate();
            break;
        default:
            break;
    }
}

void SliderComponent::OnViewAttached()
{
    if (value_ < minValue_) {
        value_ = minValue_;
    } else if (value_ > maxValue_) {
        value_ = maxValue_;
    }
    sliderView_.SetDirection(direction_);
    sliderView_.SetPosition(startPosition_.x, startPosition_.y);
    sliderView_.SetRange(maxValue_, minValue_);
    SetWidthAndHeight();
    sliderView_.SetBackgroundStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
    sliderView_.SetForegroundStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
    sliderView_.SetBackgroundStyle(STYLE_BORDER_WIDTH, 0);
    sliderView_.SetForegroundStyle(STYLE_BORDER_WIDTH, 0);
    sliderView_.SetValue(value_);
}

void SliderComponent::SetDirection(const char * const directionValue)
{
    if (directionValue == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get js slider direction is nullptr.");
        return;
    }
    const char * const sliderDirectionColumn = "column";
    direction_ = (!strcmp(directionValue, sliderDirectionColumn)) ? UISlider::Direction::DIR_BOTTOM_TO_TOP
                                                                  : UISlider::Direction::DIR_LEFT_TO_RIGHT;
}

void SliderComponent::SetWidthAndHeight()
{
    int16_t visibleHeight = GetHeight();
    int16_t visibleWidth = GetWidth();
    sliderView_.SetHeight(visibleHeight);
    sliderView_.SetWidth(visibleWidth);
    if (direction_ == UISlider::Direction::DIR_BOTTOM_TO_TOP) {
        sliderView_.SetValidWidth(SLIDER_WIDTH);
        sliderView_.SetValidHeight(visibleHeight - DEFAULT_PADDING);
    } else {
        sliderView_.SetValidHeight(SLIDER_WIDTH);
        sliderView_.SetValidWidth(visibleWidth - DEFAULT_PADDING);
    }
}
} // namespace ACELite
} // namespace OHOS
