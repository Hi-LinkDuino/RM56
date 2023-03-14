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

#include "core/components/declaration/swiper/swiper_declaration.h"

#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t METHOD_SWIPE_TO_ARGS_SIZE = 1;
constexpr double INDICATOR_POINT_SCALE = 1.33;

} // namespace

using namespace Framework;

SwiperDeclaration::SwiperDeclaration()
{
    indicator_ = AceType::MakeRefPtr<SwiperIndicator>();
    swiperController_ = AceType::MakeRefPtr<SwiperController>();
    rotationController_ = AceType::MakeRefPtr<RotationController>();
}

void SwiperDeclaration::InitializeStyle()
{
    RefPtr<SwiperIndicatorTheme> theme = GetTheme<SwiperIndicatorTheme>();
    if (theme) {
        indicator_->InitStyle(theme);
    }
}
void SwiperDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SWIPER_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_SWIPER_STYLE);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_SWIPER_EVENT);
    AddSpecializedMethod(DeclarationConstants::DEFAULT_SWIPER_METHOD);
    AddSpecializedRemoteMessageEvent(DeclarationConstants::DEFAULT_SWIPER_EVENT);
}

bool SwiperDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SwiperDeclaration&)> swiperAttrsOperators[] = {
        { DOM_SWIPER_ANIMATION_OPACITY,
            [](const std::string& val, SwiperDeclaration& swiper) { swiper.SetAnimationOpacity(StringToBool(val)); } },
        { DOM_AUTOPLAY,
            [](const std::string& val, SwiperDeclaration& swiper) { swiper.SetAutoPlay(StringToBool(val)); } },
        { DOM_CACHED_SIZE,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.SetCachedSize(StringUtils::StringToInt(val));
            } },
        { DOM_DIGITAL_INDICATOR,
            [](const std::string& val, SwiperDeclaration& swiper) { swiper.SetDigitalIndicator(StringToBool(val)); } },
        { DOM_DISPLAY_MODE,
            [](const std::string& val, SwiperDeclaration& swiper) {
                if (val == DOM_DISPLAY_MODE_AUTO_LINEAR) {
                    swiper.SetDisplayMode(SwiperDisplayMode::AUTO_LINEAR);
                } else {
                    swiper.SetDisplayMode(SwiperDisplayMode::STRETCH);
                }
            } },
        { DOM_DURATION,
            [](const std::string& val, SwiperDeclaration& swiper) { swiper.SetDuration(StringToDouble(val)); } },
        { DOM_INDEX,
            [](const std::string& val, SwiperDeclaration& swiper) { swiper.SetIndex(StringUtils::StringToInt(val)); } },
        { DOM_INDICATOR,
            [](const std::string& val, SwiperDeclaration& swiper) { swiper.showIndicator_ = StringToBool(val); } },
        { DOM_INDICATOR_DISABLED,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetIndicatorDisabled(StringToBool(val));
            } },
        { DOM_INDICATOR_MASK,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetIndicatorMask(StringToBool(val));
            } },
        { DOM_INTERVAL, [](const std::string& val,
                            SwiperDeclaration& swiper) { swiper.SetAutoPlayInterval(StringToDouble(val)); } },
        { DOM_LOOP, [](const std::string& val, SwiperDeclaration& swiper) { swiper.SetLoop(StringToBool(val)); } },
        { DOM_SCROLL_EFFECT, [](const std::string& val, SwiperDeclaration& swiper) {
            if (val == DOM_SCROLL_EFFECT_SPRING) {
                swiper.SetEdgeEffect(EdgeEffect::SPRING);
            } else if (val == DOM_SCROLL_EFFECT_FADE) {
                swiper.SetEdgeEffect(EdgeEffect::FADE);
            } else {
                swiper.SetEdgeEffect(EdgeEffect::NONE);
            }
        } },
        { DOM_VERTICAL,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.SetAxis(StringToBool(val) ? Axis::VERTICAL : Axis::HORIZONTAL);
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(swiperAttrsOperators, ArraySize(swiperAttrsOperators), attr.first.c_str());
    if (operatorIter != -1) {
        swiperAttrsOperators[operatorIter].value(attr.second, *this);
        return true;
    } else {
        return false;
    }
}

bool SwiperDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted buy key.
    static const LinearMapNode<void (*)(const std::string&, SwiperDeclaration&)> swiperStylesOperators[] = {
        { DOM_ANIMATION_CURVE,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.SetAnimationCurve(ConvertStrToAnimationCurve(val));
            } },
        { DOM_FADE_COLOR,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.SetFadeColor(swiper.ParseColor(val));
            } },
        { DOM_INDICATOR_BOTTOM,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetBottom(swiper.ParseDimension(val));
            } },
        { DOM_INDICATOR_COLOR,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetColor(swiper.ParseColor(val));
            } },
        { DOM_INDICATOR_LEFT,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetLeft(swiper.ParseDimension(val));
            } },
        { DOM_INDICATOR_RIGHT,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetRight(swiper.ParseDimension(val));
            } },
        { DOM_INDICATOR_SELECTEDCOLOR,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetSelectedColor(swiper.ParseColor(val));
            } },
        { DOM_INDICATOR_SIZE,
            [](const std::string& val, SwiperDeclaration& swiper) {
                Dimension indicatorSize = swiper.ParseDimension(val);
                swiper.indicator_->SetSize(indicatorSize);
                swiper.indicator_->SetSelectedSize(indicatorSize);
                swiper.indicator_->SetPressSize(indicatorSize * INDICATOR_POINT_SCALE);
                swiper.indicator_->SetHoverSize(indicatorSize * INDICATOR_POINT_SCALE * INDICATOR_POINT_SCALE);
            } },
        { DOM_INDICATOR_TOP,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.indicator_->SetTop(swiper.ParseDimension(val));
            } },
        { DOM_NEXT_MARGIN,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.SetNextMargin(swiper.ParseDimension(val));
            } },
        { DOM_PREVIOUS_MARGIN,
            [](const std::string& val, SwiperDeclaration& swiper) {
                swiper.SetPreviousMargin(swiper.ParseDimension(val));
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(swiperStylesOperators, ArraySize(swiperStylesOperators), style.first.c_str());
    if (operatorIter != -1) {
        swiperStylesOperators[operatorIter].value(style.second, *this);
        return true;
    } else {
        return false;
    }
}

bool SwiperDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    if (event == DOM_CHANGE) {
        SetChangeEventId(EventMarker(eventId, event, pageId));
        return true;
    } else if (event == DOM_ROTATION) {
        SetRotationEventId(EventMarker(eventId, event, pageId));
        return true;
    } else if (event == DOM_CLICK) {
        SetClickEventId(EventMarker(eventId, event, pageId));
        return true;
    } else if (event == DOM_CATCH_BUBBLE_CLICK) {
        EventMarker eventMarker(eventId, event, pageId);
        eventMarker.SetCatchMode(true);
        SetClickEventId(eventMarker);
        return true;
    } else if (event == DOM_ANIMATION_FINISH) {
        SetAnimationFinishEventId(EventMarker(eventId, event, pageId));
        return true;
    } else {
        return false;
    }
}

void SwiperDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_METHOD_SWIPE_TO) {
        auto controller = GetSwiperController();
        if (!controller) {
            LOGE("get controller failed");
            return;
        }

        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != METHOD_SWIPE_TO_ARGS_SIZE) {
            LOGE("parse args error");
            return;
        }

        std::unique_ptr<JsonValue> indexValue = argsValue->GetArrayItem(0)->GetValue("index");
        if (!indexValue || !indexValue->IsNumber()) {
            LOGE("get index failed");
            return;
        }
        int32_t index = indexValue->GetInt();
        controller->SwipeTo(index);
    } else if (method == DOM_METHOD_SHOW_PREVIOUS) {
        auto controller = GetSwiperController();
        if (!controller) {
            return;
        }
        controller->ShowPrevious();
    } else if (method == DOM_METHOD_SHOW_NEXT) {
        auto controller = GetSwiperController();
        if (!controller) {
            return;
        }
        controller->ShowNext();
    } else if (method == DOM_ROTATION) {
        auto controller = GetRotationController();
        if (controller) {
            LOGD("SwiperDeclaration rotation focus request");
            controller->RequestRotation(true);
        }
    } else {
        LOGW("unsupported method: %{public}s", method.c_str());
    }
}

} // namespace OHOS::Ace
