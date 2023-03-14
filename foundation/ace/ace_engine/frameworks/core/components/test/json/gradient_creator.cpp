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

#include "core/components/test/json/gradient_creator.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const GradientDirection GRADIENT_DIRECTIONS[] = {
    GradientDirection::LEFT,
    GradientDirection::TOP,
    GradientDirection::RIGHT,
    GradientDirection::BOTTOM,
};
const DimensionUnit DIMENSION_UNITS[] = { DimensionUnit::PX, DimensionUnit::VP, DimensionUnit::FP,
    DimensionUnit::PERCENT };

} // namespace

Gradient GradientCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("Gradient creator");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != GRADIENT_NAME) {
        LOGE("Create Gradient err: not a Gradient json.");
        return Gradient();
    }

    // set direction
    Gradient gradient = Gradient();
    if (json.Contains(GRADIENT_DIRECTION) || json.GetValue(GRADIENT_DIRECTION)->IsNumber()) {
        gradient.SetDirection(ConvertIntToEnum(
            json.GetValue(GRADIENT_DIRECTION)->GetInt(), GRADIENT_DIRECTIONS, GradientDirection::LEFT));
    }
    if (json.Contains(GRADIENT_ANGLE) && json.GetValue(GRADIENT_ANGLE)->IsNumber()) {
        gradient.GetLinearGradient().angle = AnimatableDimension(json.GetValue(GRADIENT_ANGLE)->GetDouble());
    }
    if (json.Contains(GRADIENT_REPEAT) && json.GetValue(GRADIENT_REPEAT)->IsBool()) {
        gradient.SetRepeat(json.GetValue(GRADIENT_REPEAT)->GetBool());
    }

    // set gradient colors
    if (json.Contains(GRADIENT_COLOR_LIST) && json.GetValue(GRADIENT_COLOR_LIST)->IsArray()) {
        auto colorsJson = json.GetValue(GRADIENT_COLOR_LIST);
        int32_t size = colorsJson->GetArraySize();
        for (int32_t index = 0; index < size; index++) {
            auto colorItem = CreateGradientColorFromJson(*(colorsJson->GetArrayItem(index)));
            gradient.AddColor(colorItem);
        }
    }
    return gradient;
}

GradientColor GradientCreator::CreateGradientColorFromJson(const JsonValue& componentJson)
{
    GradientColor gradientColor;
    if (componentJson.Contains(GRADIENT_COLOR) && componentJson.GetValue(GRADIENT_COLOR)->IsObject()) {
        auto color = ColorCreator::CreateFromJson(*(componentJson.GetValue(GRADIENT_COLOR)));
        gradientColor.SetColor(color);
    }
    if (componentJson.Contains(GRADIENT_HAS_VALUE) && componentJson.GetValue(GRADIENT_HAS_VALUE)->IsBool()) {
        if (!componentJson.GetValue(GRADIENT_HAS_VALUE)->GetBool()) {
            gradientColor.SetHasValue(false);
            return gradientColor;
        }
    }
    if (componentJson.Contains(GRADIENT_DIMENSION) && componentJson.GetValue(GRADIENT_DIMENSION)->IsObject()) {
        auto dimension = CreateDimensionFromJson(*(componentJson.GetValue(GRADIENT_DIMENSION)));
        gradientColor.SetDimension(dimension);
    }
    return gradientColor;
}

Dimension GradientCreator::CreateDimensionFromJson(const JsonValue& componentJson)
{
    double value = 0.0;
    DimensionUnit dimensionUnit = DimensionUnit::PERCENT;
    if (componentJson.Contains(GRADIENT_DIMENSION_VALUE) &&
        componentJson.GetValue(GRADIENT_DIMENSION_VALUE)->IsNumber()) {
        value = componentJson.GetValue(GRADIENT_DIMENSION_VALUE)->GetDouble();
    }
    if (componentJson.Contains(GRADIENT_DIMENSION_UNIT) &&
        componentJson.GetValue(GRADIENT_DIMENSION_UNIT)->IsNumber()) {
        dimensionUnit = ConvertIntToEnum(
            componentJson.GetValue(GRADIENT_DIMENSION_UNIT)->GetInt(), DIMENSION_UNITS, DimensionUnit::PERCENT);
    }
    return Dimension(value, dimensionUnit);
}

} // namespace OHOS::Ace
