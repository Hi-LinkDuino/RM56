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

#include "frameworks/bridge/common/dom/dom_slider.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/slider/slider_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSlider::DOMSlider(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    // init component and default theme
    sliderChild_ = AceType::MakeRefPtr<SliderComponent>(val_, step_, min_, max_);
    watchSliderChild_ = AceType::MakeRefPtr<WatchSliderComponent>();
    trackChild_ = sliderChild_->GetTrack();
    blockChild_ = sliderChild_->GetBlock();
    paddingChild_ = AceType::MakeRefPtr<PaddingComponent>();
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        paddingChild_->SetChild(watchSliderChild_);
    } else {
        paddingChild_->SetChild(sliderChild_);
    }
}

void DOMSlider::InitializeStyle()
{
    RefPtr<SliderTheme> theme = GetTheme<SliderTheme>();
    if (!theme) {
        return;
    }
    blockColor_ = theme->GetBlockColor();
    color_ = theme->GetTrackBgColor();
    selectColor_ = theme->GetTrackSelectedColor();
    sliderChild_->InitStyle(theme);
}

void DOMSlider::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_ROTATION) {
        LOGD("Rotation focus slider");
        auto controller = (SystemProperties::GetDeviceType() == DeviceType::WATCH)
                              ? watchSliderChild_->GetRotationController()
                              : sliderChild_->GetRotationController();
        if (controller) {
            LOGD("Rotation focus request");
            controller->RequestRotation(true);
        }
    }
}

void DOMSlider::ResetInitializedStyle()
{
    sliderChild_->InitStyle(GetTheme<SliderTheme>());
}

bool DOMSlider::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, DOMSlider&)> sliderAttrsOperators[] = {
        { DOM_MAX, [](const std::string& val, DOMSlider& slider) { slider.max_ = StringToDouble(val); } },
        { DOM_MAX_ICON, [](const std::string& val, DOMSlider& slider) { slider.maxIconUrl_ = val; } },
        { DOM_MIN, [](const std::string& val, DOMSlider& slider) { slider.min_ = StringToDouble(val); } },
        { DOM_MIN_ICON, [](const std::string& val, DOMSlider& slider) { slider.minIconUrl_ = val; } },
        { DOM_SLIDER_MODE, [](const std::string& val, DOMSlider& slider) {
                if (val == DOM_INSET) {
                    slider.mode_ = SliderMode::INSET;
                } else {
                    slider.mode_ = SliderMode::OUTSET;
                }
            } },
        { DOM_SHOW_STEPS, [](const std::string& val, DOMSlider& slider) { slider.showSteps_ = StringToBool(val); } },
        { DOM_SHOW_TIPS, [](const std::string& val, DOMSlider& slider) { slider.showTips_ = StringToBool(val); } },
        { DOM_STEP,
            [](const std::string& val, DOMSlider& slider) {
                slider.step_ = StringToDouble(val) > 0 ? StringToDouble(val) : slider.step_;
            } },
        { DOM_TYPE,
            [](const std::string& val, DOMSlider& slider) {
                if (val == DOM_CONTINUOUS) {
                    slider.isContinuous_ = true;
                } else if (val == DOM_INTERMITTENT) {
                    slider.isContinuous_ = false;
                } else {
                    slider.isContinuous_ = true;
                }
            } },
        { DOM_VALUE, [](const std::string& val, DOMSlider& slider) {
                slider.val_ = StringToDouble(val);
                slider.sliderChild_->SetCurrentValue(StringToDouble(val));
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(sliderAttrsOperators, ArraySize(sliderAttrsOperators), attr.first.c_str());
    if (operatorIter != -1) {
        sliderAttrsOperators[operatorIter].value(attr.second, *this);
        return true;
    } else {
        return false;
    }
}

bool DOMSlider::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMSlider&)> sliderStylesOperators[] = {
        { DOM_BLOCK_COLOR,
            [](const std::string& val, DOMSlider& slider) { slider.blockColor_ = slider.ParseColor(val); } },
        { DOM_COLOR, [](const std::string& val, DOMSlider& slider) { slider.color_ = slider.ParseColor(val); } },
        { DOM_PADDING_LEFT,
            [](const std::string& val, DOMSlider& slider) { slider.paddingLeft_ = slider.ParseDimension(val); } },
        { DOM_PADDING_RIGHT,
            [](const std::string& val, DOMSlider& slider) { slider.paddingRight_ = slider.ParseDimension(val); } },
        { DOM_SELECTED_COLOR,
            [](const std::string& val, DOMSlider& slider) { slider.selectColor_ = slider.ParseColor(val); } },
    };
    auto operatorIter =
        BinarySearchFindIndex(sliderStylesOperators, ArraySize(sliderStylesOperators), style.first.c_str());
    if (operatorIter != -1) {
        sliderStylesOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

bool DOMSlider::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    LOGD("DOMSlider AddEvent");
    if (event == DOM_CHANGE) {
        onMoveEndEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        sliderChild_->SetOnMovingEventId(onMoveEndEventId_);
        watchSliderChild_->SetOnMoveEndEventId(onMoveEndEventId_);
        return true;
    }
    return false;
}

void DOMSlider::PrepareSpecializedComponent()
{
    auto min = min_;
    min_ = (max_ < min) ? max_ : min;
    max_ = (max_ < min) ? min : max_;
    sliderChild_->SetMaxValue(max_);
    sliderChild_->SetMinValue(min_);
    if (declaration_) {
        sliderChild_->SetDisable(declaration_->IsDisabled());
    }
    if (val_ < min_ || val_ > max_) {
        val_ = (val_ < min_) ? min_ : max_;
    }
    if (LessOrEqual(step_, 0.0) || step_ > max_ - min_) {
        step_ = max_ - min_;
    }
    watchSliderChild_->SetMinValue(min_);
    watchSliderChild_->SetMaxValue(max_);
    watchSliderChild_->SetValue(val_);
    watchSliderChild_->SetMaxIconUrl(maxIconUrl_);
    watchSliderChild_->SetMinIconUrl(minIconUrl_);
    watchSliderChild_->SetBackgroundColor(color_);
    watchSliderChild_->SetSelectColor(selectColor_);
    watchSliderChild_->SetContinuous(isContinuous_);

    sliderChild_->SetShowSteps(showSteps_);
    sliderChild_->SetShowTips(showTips_);
    sliderChild_->SetSliderMode(mode_);
    sliderChild_->SetStepValue(step_);
    sliderChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    paddingChild_->SetPadding(Edge(paddingLeft_, 0.0_vp, paddingRight_, 0.0_vp));
    trackChild_->SetBackgroundColor(color_);
    trackChild_->SetSelectColor(selectColor_);
    blockChild_->SetBlockColor(blockColor_);
    sliderChild_->InitStyle(GetTheme<SliderTheme>());
}

} // namespace OHOS::Ace::Framework
