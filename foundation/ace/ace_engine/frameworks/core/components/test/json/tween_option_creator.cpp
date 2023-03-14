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

#include "core/components/test/json/tween_option_creator.h"

#include <map>

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "core/animation/curve_animation.h"
#include "core/components/test/json/offset_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const std::map<std::string, RefPtr<Curve>> CURVE_MAP = {
    { "linear", Curves::LINEAR },
    { "ease", Curves::EASE },
    { "ease-in", Curves::EASE_IN },
    { "ease-out", Curves::EASE_OUT },
    { "ease-in-out", Curves::EASE_IN_OUT },
};

} // namespace

TweenOption TweenOptionCreator::CreateFromJson(const JsonValue& json)
{
    TweenOption tweenOption;
    if (json.Contains(OPTION_TRANSLATE) && json.GetValue(OPTION_TRANSLATE)->IsObject()) {
        Offset offsetComponent = OffsetCreator::CreateFromJson(*(json.GetValue(OPTION_TRANSLATE)));
        double deltaX = offsetComponent.GetX();
        double deltaY = offsetComponent.GetY();
        auto translate = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Dimension(), Dimension()),
            DimensionOffset(Dimension(deltaX, DimensionUnit::PX), Dimension(deltaY, DimensionUnit::PX)), nullptr);
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE, translate);
    }
    if (json.Contains(OPTION_TRANSLATE_X) && json.GetValue(OPTION_TRANSLATE_X)->IsNumber()) {
        double deltaX = json.GetValue(OPTION_TRANSLATE_X)->GetDouble();
        auto translateX =
            AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Dimension(), Dimension()),
                DimensionOffset(Dimension(deltaX, DimensionUnit::PX), Dimension()), nullptr);
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateX);
    }
    if (json.Contains(OPTION_TRANSLATE_Y) && json.GetValue(OPTION_TRANSLATE_Y)->IsNumber()) {
        double deltaY = json.GetValue(OPTION_TRANSLATE_Y)->GetDouble();
        auto translateY =
            AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Dimension(), Dimension()),
                DimensionOffset(Dimension(), Dimension(deltaY, DimensionUnit::PX)), nullptr);
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE_Y, translateY);
    }
    if (json.Contains(OPTION_SCALE) && json.GetValue(OPTION_SCALE)->IsNumber()) {
        auto scale =
            AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, json.GetValue(OPTION_SCALE)->GetDouble(), nullptr);
        tweenOption.SetTransformFloatAnimation(AnimationType::SCALE, scale);
    }
    if (json.Contains(OPTION_SCALE_X) && json.GetValue(OPTION_SCALE_X)->IsNumber()) {
        auto scaleX =
            AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, json.GetValue(OPTION_SCALE)->GetDouble(), nullptr);
        tweenOption.SetTransformFloatAnimation(AnimationType::SCALE_X, scaleX);
    }
    if (json.Contains(OPTION_SCALE_Y) && json.GetValue(OPTION_SCALE_Y)->IsNumber()) {
        auto scaleY =
            AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, json.GetValue(OPTION_SCALE)->GetDouble(), nullptr);
        tweenOption.SetTransformFloatAnimation(AnimationType::SCALE_Y, scaleY);
    }
    if (json.Contains(OPTION_ROTATE_Z) && json.GetValue(OPTION_ROTATE_Z)->IsNumber()) {
        auto rotateZ =
            AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, json.GetValue(OPTION_ROTATE_Z)->GetDouble(), nullptr);
        tweenOption.SetTransformFloatAnimation(AnimationType::ROTATE_Z, rotateZ);
    }
    if (json.Contains(OPTION_ROTATE_X) && json.GetValue(OPTION_ROTATE_X)->IsNumber()) {
        auto rotateX =
            AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, json.GetValue(OPTION_ROTATE_X)->GetDouble(), nullptr);
        tweenOption.SetTransformFloatAnimation(AnimationType::ROTATE_X, rotateX);
    }
    if (json.Contains(OPTION_ROTATE_Y) && json.GetValue(OPTION_ROTATE_Y)->IsNumber()) {
        auto rotateY =
            AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, json.GetValue(OPTION_ROTATE_Y)->GetDouble(), nullptr);
        tweenOption.SetTransformFloatAnimation(AnimationType::ROTATE_Y, rotateY);
    }
    if (json.Contains(DURATION) && json.GetValue(DURATION)->IsNumber()) {
        tweenOption.SetDuration(json.GetValue(DURATION)->GetInt());
    }
    if (json.Contains(DELAY) && json.GetValue(DELAY)->IsNumber()) {
        tweenOption.SetDelay(json.GetValue(DELAY)->GetInt());
    }
    if (json.Contains(ITERATION) && json.GetValue(ITERATION)->IsNumber()) {
        tweenOption.SetIteration(json.GetValue(ITERATION)->GetInt());
    }
    if (json.Contains(FILL_MODE) && json.GetValue(FILL_MODE)->IsString()) {
        if (json.GetValue(FILL_MODE)->GetString() == "forwards") {
            tweenOption.SetFillMode(FillMode::FORWARDS);
        } else {
            tweenOption.SetFillMode(FillMode::NONE);
        }
    }
    if (json.Contains(CURVE) && json.GetValue(CURVE)->IsString()) {
        auto curve = CURVE_MAP.find(json.GetValue(CURVE)->GetString());
        if (curve != CURVE_MAP.end()) {
            tweenOption.SetCurve(curve->second);
        } else {
            tweenOption.SetCurve(Curves::EASE);
        }
    }
    return tweenOption;
}

} // namespace OHOS::Ace