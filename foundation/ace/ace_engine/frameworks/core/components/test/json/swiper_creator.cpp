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

#include "core/components/test/json/swiper_creator.h"

#include "base/log/log.h"
#include "core/components/swiper/swiper_component.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/edge_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

void ParseIndicatorProperty(const JsonValue& componentJson, const RefPtr<SwiperComponent>& swiperComponent)
{
    auto indicator = swiperComponent->GetIndicator();
    if (componentJson.Contains(SWIPER_INDICATOR_COLOR) && componentJson.GetValue(SWIPER_INDICATOR_COLOR)->IsObject()) {
        auto colorJson = componentJson.GetValue(SWIPER_INDICATOR_COLOR);
        auto color = ColorCreator::CreateFromJson(*colorJson);
        indicator->SetColor(color);
    }
    if (componentJson.Contains(SWIPER_INDICATOR_SELECTED_COLOR) &&
        componentJson.GetValue(SWIPER_INDICATOR_SELECTED_COLOR)->IsObject()) {
        auto colorJson = componentJson.GetValue(SWIPER_INDICATOR_SELECTED_COLOR);
        auto color = ColorCreator::CreateFromJson(*colorJson);
        indicator->SetSelectedColor(color);
    }
    if (componentJson.Contains(SWIPER_INDICATOR_SIZE) && componentJson.GetValue(SWIPER_INDICATOR_SIZE)->IsNumber()) {
        double size = componentJson.GetValue(SWIPER_INDICATOR_SIZE)->GetDouble();
        indicator->SetSize(Dimension(size, DimensionUnit::VP));
    }
    if (componentJson.Contains(SWIPER_INDICATOR_SELECTED_SIZE) &&
        componentJson.GetValue(SWIPER_INDICATOR_SELECTED_SIZE)->IsNumber()) {
        double size = componentJson.GetValue(SWIPER_INDICATOR_SELECTED_SIZE)->GetDouble();
        indicator->SetSelectedSize(Dimension(size, DimensionUnit::VP));
    }
    if (componentJson.Contains(SWIPER_INDICATOR_TOP) && componentJson.GetValue(SWIPER_INDICATOR_TOP)->IsNumber()) {
        double value = componentJson.GetValue(SWIPER_INDICATOR_TOP)->GetDouble();
        indicator->SetTop(Dimension(value, DimensionUnit::PX));
    }
    if (componentJson.Contains(SWIPER_INDICATOR_LEFT) && componentJson.GetValue(SWIPER_INDICATOR_LEFT)->IsNumber()) {
        double value = componentJson.GetValue(SWIPER_INDICATOR_LEFT)->GetDouble();
        indicator->SetLeft(Dimension(value, DimensionUnit::PX));
    }
    if (componentJson.Contains(SWIPER_INDICATOR_BOTTOM) &&
        componentJson.GetValue(SWIPER_INDICATOR_BOTTOM)->IsNumber()) {
        double value = componentJson.GetValue(SWIPER_INDICATOR_BOTTOM)->GetDouble();
        indicator->SetBottom(Dimension(value, DimensionUnit::PX));
    }
    if (componentJson.Contains(SWIPER_INDICATOR_RIGHT) && componentJson.GetValue(SWIPER_INDICATOR_RIGHT)->IsNumber()) {
        double value = componentJson.GetValue(SWIPER_INDICATOR_RIGHT)->GetDouble();
        indicator->SetRight(Dimension(value, DimensionUnit::PX));
    }
}

} // namespace

RefPtr<Component> SwiperCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson swiper");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != SWIPER_NAME) {
        LOGE("Create Swiper error: not a swiper json.");
        return nullptr;
    }

    bool showIndicator = true;
    if (componentJson.Contains(SWIPER_SHOW_INDICATOR) && componentJson.GetValue(SWIPER_SHOW_INDICATOR)->IsBool()) {
        showIndicator = componentJson.GetValue(SWIPER_SHOW_INDICATOR)->GetBool();
    }

    auto children = factory.CreateChildComponent(componentJson);
    auto swiperComponent = AceType::MakeRefPtr<SwiperComponent>(children, showIndicator);

    if (showIndicator) {
        auto themeManager = AceType::MakeRefPtr<ThemeManager>();
        auto theme = themeManager->GetTheme<SwiperIndicatorTheme>();
        if (theme) {
            swiperComponent->GetIndicator()->InitStyle(theme);
        }
        ParseIndicatorProperty(componentJson, swiperComponent);
    }

    if (componentJson.Contains(SWIPER_ON_CHANGED) && componentJson.GetValue(SWIPER_ON_CHANGED)->IsString()) {
        auto onChanged = componentJson.GetValue(SWIPER_ON_CHANGED)->GetString();
        swiperComponent->SetChangeEventId(EventMarker(onChanged));
    }
    if (componentJson.Contains(SWIPER_INDEX) && componentJson.GetValue(SWIPER_INDEX)->IsNumber()) {
        uint32_t index = componentJson.GetValue(SWIPER_INDEX)->GetUInt();
        swiperComponent->SetIndex(index);
    }
    if (componentJson.Contains(SWIPER_DURATION) && componentJson.GetValue(SWIPER_DURATION)->IsNumber()) {
        double duration = componentJson.GetValue(SWIPER_DURATION)->GetDouble();
        swiperComponent->SetDuration(duration);
    }

    if (componentJson.Contains(SWIPER_LOOP) && componentJson.GetValue(SWIPER_LOOP)->IsBool()) {
        swiperComponent->SetLoop(componentJson.GetValue(SWIPER_LOOP)->GetBool());
    }
    if (componentJson.Contains(SWIPER_AUTO_PLAY) && componentJson.GetValue(SWIPER_AUTO_PLAY)->IsBool()) {
        swiperComponent->SetAutoPlay(componentJson.GetValue(SWIPER_AUTO_PLAY)->GetBool());
    }
    if (componentJson.Contains(SWIPER_AUTO_PLAY_INTERVAL) &&
        componentJson.GetValue(SWIPER_AUTO_PLAY_INTERVAL)->IsNumber()) {
        swiperComponent->SetAutoPlayInterval(componentJson.GetValue(SWIPER_AUTO_PLAY_INTERVAL)->GetDouble());
    }
    if (componentJson.Contains(SWIPER_IS_VERTICAL) && componentJson.GetValue(SWIPER_IS_VERTICAL)->IsBool()) {
        componentJson.GetValue(SWIPER_IS_VERTICAL)->GetBool() ? swiperComponent->SetAxis(Axis::VERTICAL)
                                                              : swiperComponent->SetAxis(Axis::HORIZONTAL);
    }
    return swiperComponent;
}

} // namespace OHOS::Ace
