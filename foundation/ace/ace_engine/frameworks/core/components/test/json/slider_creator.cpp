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

#include "core/components/test/json/slider_creator.h"

#include "base/log/log.h"
#include "core/components/slider/block_component.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/track/track_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> SliderCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson start");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != SLIDER_NAME) {
        LOGE("Create Slider error: not a slider json");
        return nullptr;
    }
    if (!componentJson.Contains(VALUE) || !componentJson.GetValue(VALUE)->IsNumber()) {
        LOGE("Create Slider error: missing initial value");
        return nullptr;
    }
    if (!componentJson.Contains(SLIDER_MIN) || !componentJson.GetValue(SLIDER_MIN)->IsNumber()) {
        LOGE("Create Slider error: missing min");
        return nullptr;
    }
    if (!componentJson.Contains(SLIDER_MAX) || !componentJson.GetValue(SLIDER_MAX)->IsNumber()) {
        LOGE("Create Slider error: missing max");
        return nullptr;
    }
    if (!componentJson.Contains(SLIDER_STEP) || !componentJson.GetValue(SLIDER_STEP)->IsNumber()) {
        LOGE("Create Slider error: missing step");
        return nullptr;
    }
    double value = componentJson.GetValue(VALUE)->GetDouble();
    double min = componentJson.GetValue(SLIDER_MIN)->GetDouble();
    double max = componentJson.GetValue(SLIDER_MAX)->GetDouble();
    double step = componentJson.GetValue(SLIDER_STEP)->GetDouble();

    // Check minValue and maxValue
    if (max <= min || min > value || max < value || step < 0 || step > (max - min)) {
        LOGE("Create Slider error: max is smaller or equals than min");
        return nullptr;
    }
    RefPtr<SliderComponent> slider = AceType::MakeRefPtr<SliderComponent>(value, step, min, max);
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto theme = themeManager->GetTheme<SliderTheme>();
    if (theme) {
        slider->SetThemeStyle(theme);
    }
    // Create slider bar
    CreateSliderBar(slider, componentJson);

    // Create slider block
    CreateSliderBlock(slider, componentJson);

    // Read method event
    auto onMovedEndEventIdJson = componentJson.GetValue(ON_MOVE_END_ID);
    if (onMovedEndEventIdJson && onMovedEndEventIdJson->IsString()) {
        slider->SetOnMoveEndEventId(EventMarker(onMovedEndEventIdJson->GetString()));
    }
    return slider;
}

void SliderCreator::CreateSliderBlock(RefPtr<SliderComponent>& slider, const JsonValue& componentJson)
{
    auto sliderBlock = slider->GetBlock();
    auto blockColorJson = componentJson.GetValue(SLIDER_BLOCK_COLOR);
    if (blockColorJson && blockColorJson->IsObject()) {
        auto blockColor = ColorCreator::CreateFromJson(*blockColorJson);
        sliderBlock->SetBlockColor(blockColor);
    }
}

void SliderCreator::CreateSliderBar(RefPtr<SliderComponent>& slider, const JsonValue& componentJson)
{
    auto sliderBar = slider->GetTrack();
    auto sliderRailColorJson = componentJson.GetValue(SLIDER_BAR_BACKGROUND_COLOR);
    if (sliderRailColorJson && sliderRailColorJson->IsObject()) {
        auto sliderRailColor = ColorCreator::CreateFromJson(*sliderRailColorJson);
        sliderBar->SetBackgroundColor(sliderRailColor);
    }
    auto selectColorJson = componentJson.GetValue(SLIDER_SELECTED_COLOR);
    if (selectColorJson && selectColorJson->IsObject()) {
        auto selectColor = ColorCreator::CreateFromJson(*selectColorJson);
        sliderBar->SetSelectColor(selectColor);
    }
}

} // namespace OHOS::Ace