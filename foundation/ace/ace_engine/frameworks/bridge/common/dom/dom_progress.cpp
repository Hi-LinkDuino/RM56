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

#include "frameworks/bridge/common/dom/dom_progress.h"

#include "base/utils/utils.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {


bool IsGradient(const std::string& value)
{
    auto gradientJson = JsonUtil::ParseJsonString(value);
    if (!gradientJson->IsObject()) {
        return false;
    }
    return true;
}

Gradient ParseGradient(const DOMProgress& progress, const std::string& value)
{
    // only support linear gradient
    auto gradientJson = JsonUtil::ParseJsonString(value);
    Gradient gradient = Gradient();
    if (!gradientJson->IsObject()) {
        LOGW("gradientJson json param error");
        return gradient;
    }
    auto gradientValue = gradientJson->GetValue(DOM_VALUES);
    if ((gradientValue == nullptr) || (!gradientValue->IsArray()) || (gradientValue->GetArraySize() <= 0)) {
        LOGW("gradientValue json param error");
        return gradient;
    }
    auto values = gradientValue->GetArrayItem(0);
    gradient.SetDirection(GradientDirection::START_TO_END);
    auto colors = values->GetValue(DOM_GRADIENT_VALUES);
    if (colors != nullptr && colors->IsArray()) {
        for (int32_t index = 0; index < colors->GetArraySize(); index++) {
            // remove the " at front and end. check the color string longer than ""
            // "#FFFFFF" -> #FFFFFF
            if (colors->GetArrayItem(index)->ToString().length() > 2) {
                gradient.AddColor(GradientColor(progress.ParseColor(colors->GetArrayItem(index)->ToString().substr(
                    1, colors->GetArrayItem(index)->ToString().length() - 2))));
            }
        }
    }
    return gradient;
}

RefPtr<ProgressComponent> DOMProgress::CreateProgressComponent(
    double min, double percent, double cachedValue, double max, ProgressType type)
{
    if (type == ProgressType::CIRCLE) {
        return AceType::MakeRefPtr<ProgressComponent>(min, percent, cachedValue, max, ProgressType::LINEAR);
    }
    return AceType::MakeRefPtr<ProgressComponent>(min, percent, cachedValue, max, type);
}

DOMProgress::DOMProgress(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}

bool DOMProgress::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, DOMProgress&)> progressAttrsOperators[] = {
        { DOM_RING_CLOCKWISH_DIRECTION,
            [](const std::string& val, DOMProgress& progress) { progress.clockwiseDirection_ = StringToBool(val); } },
        { DOM_EFFECTS_ON,
            [](const std::string& val, DOMProgress& progress) { progress.showAnimationEffect_ = StringToBool(val); } },
        { DOM_PROGRESS_PERCENT,
            [](const std::string& val, DOMProgress& progress) {
                progress.percent_ = StringToDouble(val);
                if (progress.percent_ > progress.max_) {
                    progress.percent_ = progress.max_;
                }
                if (progress.percent_ < progress.min_) {
                    progress.percent_ = progress.min_;
                }
            } },
        { DOM_PROGRESS_SECONDARY_PERCENT,
            [](const std::string& val, DOMProgress& progress) {
                progress.cachedValue_ = StringToDouble(val);
                if (progress.cachedValue_ > progress.max_) {
                    progress.cachedValue_ = progress.max_;
                }
                if (progress.cachedValue_ < progress.min_) {
                    progress.cachedValue_ = progress.min_;
                }
            } },
        { DOM_PROGRESS_TYPE,
            [](const std::string& val, DOMProgress& progress) {
                if (val == DOM_PROGRESS_TYPE_CIRCULAR) {
                    progress.type_ = ProgressType::CIRCLE;
                } else if (val == DOM_PROGRESS_TYPE_RING) {
                    progress.type_ = ProgressType::RING;
                } else if (val == DOM_PROGRESS_TYPE_HORIZONTAL) {
                    progress.type_ = ProgressType::LINEAR;
                } else if (val == DOM_PROGRESS_TYPE_SCALE) {
                    progress.type_ = ProgressType::SCALE;
                } else if (val == DOM_PROGRESS_TYPE_MOON) {
                    progress.type_ = ProgressType::MOON;
                } else if ((val == DOM_PROGRESS_TYPE_ARC)) {
                    progress.type_ = ProgressType::ARC;
                } else if ((val == DOM_PROGRESS_TYPE_BUBBLE)) {
                    progress.type_ = ProgressType::BUBBLE;
                } else {
                    progress.type_ = ProgressType::LINEAR;
                }
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(progressAttrsOperators, ArraySize(progressAttrsOperators), attr.first.c_str());
    if (operatorIter != -1) {
        progressAttrsOperators[operatorIter].value(attr.second, *this);
        return true;
    }
    return false;
}

bool DOMProgress::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, DOMProgress&)> progressStylesOperators[] = {
        { DOM_PROGRESS_BACKGROUND_COLOR,
            [](const std::string& val, DOMProgress& progress) {
                progress.backgroundColor_.first = progress.ParseColor(val);
                progress.backgroundColor_.second = true;
            } },
        { DOM_PROGRESS_BUBBLE_RADIUS,
            [](const std::string& val, DOMProgress& progress) {
                progress.diameter_ = progress.ParseDimension(val);
            } },
        { DOM_CENTER_X,
            [](const std::string& val, DOMProgress& progress) {
                progress.centerX_.first = StringToDouble(val);
                progress.centerX_.second = true;
            } },
        { DOM_CENTER_Y,
            [](const std::string& val, DOMProgress& progress) {
                progress.centerY_.first = StringToDouble(val);
                progress.centerY_.second = true;
            } },
        { DOM_PROGRESS_COLOR,
            [](const std::string& val, DOMProgress& progress) {
                if (IsGradient(val)) {
                    progress.gradient_ = ParseGradient(progress, val);
                } else {
                    progress.color_.first = progress.ParseColor(val);
                    progress.color_.second = true;
                }
            } },
        { DOM_PROGRESS_DIAMETER,
            [](const std::string& val, DOMProgress& progress) {
                progress.bubbleRadius_ = progress.ParseDimension(val);
            } },
        { DOM_DIRECTION,
            [](const std::string& val, DOMProgress& progress) { progress.isStartToEnd_ = val == DOM_START_TO_END; } },
        { DOM_PROGRESS_RADIUS,
            [](const std::string& val, DOMProgress& progress) {
                progress.radius_.first = StringToDouble(val);
                progress.radius_.second = true;
            } },
        { DOM_SCALE_NUMBER,
            [](const std::string& val, DOMProgress& progress) {
                progress.scaleNumber_.first = StringUtils::StringToInt(val);
                progress.scaleNumber_.second = true;
            } },
        { DOM_SCALE_WIDTH,
            [](const std::string& val, DOMProgress& progress) {
                progress.scaleWidth_.first = progress.ParseDimension(val);
                progress.scaleWidth_.second = true;
            } },
        { DOM_PROGRESS_SECONDARY_COLOR,
            [](const std::string& val, DOMProgress& progress) {
                progress.cachedColor_.first = progress.ParseColor(val);
                progress.cachedColor_.second = true;
            } },
        { DOM_START_DEGREE,
            [](const std::string& val, DOMProgress& progress) { progress.startDegree_ = StringToDouble(val); } },
        { DOM_PROGRESS_STROKE_WIDTH,
            [](const std::string& val, DOMProgress& progress) {
                progress.strokeWidth_.first = progress.ParseDimension(val);
                progress.strokeWidth_.second = true;
            } },
        { DOM_SWEEP_DEGREE,
            [](const std::string& val, DOMProgress& progress) { progress.sweepDegree_ = StringToDouble(val); } },
    };
    auto operatorIter =
        BinarySearchFindIndex(progressStylesOperators, ArraySize(progressStylesOperators), style.first.c_str());
    if (operatorIter != -1) {
        progressStylesOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

void DOMProgress::PrepareSpecializedComponent()
{
    InitProgressIfNeed();
    if (type_ == ProgressType::CIRCLE) {
        loadingProgressChild_->SetProgressColor(color_.first);
        return;
    }
    if (type_ == ProgressType::BUBBLE) {
        bubbleProgressChild_->SetBubbleRadius(bubbleRadius_);
        bubbleProgressChild_->SetDiameter(diameter_);
        return;
    }
    progressChild_->SetMaxValue(max_);
    progressChild_->SetMinValue(min_);
    progressChild_->SetValue(percent_);
    progressChild_->SetCachedValue(cachedValue_);
    progressChild_->GetTrack()->SetSelectColor(color_.first);
    progressChild_->GetTrack()->SetCachedColor(cachedColor_.first);
    progressChild_->GetTrack()->SetBackgroundColor(backgroundColor_.first);
    progressChild_->GetTrack()->SetTrackThickness(strokeWidth_.first);
    progressChild_->GetTrack()->SetShowAnimation(showAnimationEffect_);
    progressChild_->SetAnimationPlay(showAnimationEffect_);
    progressChild_->GetTrack()->SetTextDirection(
        IsRightToLeft() && isStartToEnd_ ? TextDirection::RTL : TextDirection::LTR);
    if (gradient_.IsValid()) {
        progressChild_->GetTrack()->SetSelectGradient(gradient_);
    }
    if (type_ == ProgressType::RING) {
        auto info = progressChild_->GetTrack()->GetTrackInfo();
        info->SetClockwise(clockwiseDirection_);
    } else if (type_ == ProgressType::SCALE) {
        auto info = progressChild_->GetTrack()->GetTrackInfo();
        info->SetScaleWidth(scaleWidth_.first);
        info->SetScaleNumber(scaleNumber_.first);
        info->SetClockwise(clockwiseDirection_);
    } else if (type_ == ProgressType::ARC) {
        // draw arc progress
        progressChild_->GetTrack()->GetTrackInfo()->SetStartDegree(startDegree_);
        progressChild_->GetTrack()->GetTrackInfo()->SetSweepDegree(sweepDegree_);
        if (radius_.second) {
            progressChild_->GetTrack()->SetRadius(radius_.first);
        }
        if (centerX_.second) {
            progressChild_->GetTrack()->SetCenterX(centerX_.first);
        }
        if (centerY_.second) {
            progressChild_->GetTrack()->SetCenterY(centerY_.first);
        }
    }
}

void DOMProgress::OnSetStyleFinished()
{
    // the range is from -360 to 360 degree
    static constexpr double defaultStartDegree = -120;
    static constexpr double defaultSweepDegree = 240;
    if (startDegree_ > 360.0 || startDegree_ < -360.0) {
        startDegree_ = defaultStartDegree;
        sweepDegree_ = defaultSweepDegree;
        return;
    }
    if (sweepDegree_ > 360.0 || sweepDegree_ < -360.0) {
        startDegree_ = defaultStartDegree;
        sweepDegree_ = defaultSweepDegree;
        return;
    }
}

RefPtr<Component> DOMProgress::GetSpecializedComponent()
{
    if (type_ == ProgressType::CIRCLE) {
        return loadingProgressChild_;
    } else if (type_ == ProgressType::BUBBLE) {
        return bubbleProgressChild_;
    } else {
        return progressChild_;
    }
}

void DOMProgress::InitProgressIfNeed()
{
    auto theme = GetTheme<ProgressTheme>();
    if (type_ == ProgressType::CIRCLE) {
        // Width_ and height_ in circular progress are usually the same with diameter in loading progress component.
        // If width_ and height_ are different, choose smaller one as diameter.
        if (!loadingProgressChild_) {
            loadingProgressChild_ = AceType::MakeRefPtr<LoadingProgressComponent>();
        }
        if (theme) {
            if (!color_.second) {
                color_.first = theme->GetProgressColor();
            }
            loadingProgressChild_->SetMoveRatio(theme->GetMoveRatio());
            loadingProgressChild_->SetRingRadius(theme->GetRingRadius());
            loadingProgressChild_->SetOrbitRadius(theme->GetOrbitRadius());
            loadingProgressChild_->SetCometTailLen(theme->GetCometTailLen());
        }
        return;
    }

    if (type_ == ProgressType::BUBBLE) {
        if (!bubbleProgressChild_) {
            bubbleProgressChild_ = AceType::MakeRefPtr<BubbleProgressComponent>();
        }
        return;
    }

    if (!progressChild_ || progressChild_->GetType() != type_) {
        progressChild_ = CreateProgressComponent(min_, percent_, cachedValue_, max_, type_);
    }
    if (!theme) {
        return;
    }
    if (!color_.second) {
        color_.first = type_ == ProgressType::MOON ? theme->GetMoonFrontColor() : theme->GetTrackSelectedColor();
    }
    if (!backgroundColor_.second) {
        backgroundColor_.first =
            type_ == ProgressType::MOON ? theme->GetMoonBackgroundColor() : theme->GetTrackBgColor();
    }
    if (!cachedColor_.second) {
        cachedColor_.first = theme->GetTrackCachedColor();
    }
    if (!strokeWidth_.second) {
        if (type_ == ProgressType::SCALE) {
            strokeWidth_.first = theme->GetScaleLength();
        } else if (type_ == ProgressType::RING) {
            strokeWidth_.first = theme->GetRingThickness();
        } else {
            strokeWidth_.first = theme->GetTrackThickness();
        }
    }
    if (!scaleWidth_.second) {
        scaleWidth_.first = theme->GetScaleWidth();
    }
    if (!scaleNumber_.second) {
        scaleNumber_.first = theme->GetScaleNumber();
    }
}

} // namespace OHOS::Ace::Framework
