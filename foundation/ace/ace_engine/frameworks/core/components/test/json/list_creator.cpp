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

#include "core/components/test/json/list_creator.h"

#include "base/log/log.h"
#include "core/components/list/list_component.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ListCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    FlexDirection direction = FlexDirection::COLUMN;
    int32_t totalCount = 0;
    int32_t cachedCount = 0;
    bool scrollPage = false;
    std::string requestItemEvent;

    auto children = factory.CreateChildComponent(componentJson);
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>(children);

    if (componentJson.Contains(LIST_DIRECTION) && componentJson.GetValue(LIST_DIRECTION)->IsNumber()) {
        direction = static_cast<FlexDirection>(componentJson.GetValue(LIST_DIRECTION)->GetInt());
        list->SetDirection(direction);
    }

    if (componentJson.Contains(LIST_COUNT) && componentJson.GetValue(LIST_COUNT)->IsNumber()) {
        totalCount = componentJson.GetValue(LIST_COUNT)->GetInt();
        list->SetTotalCount(totalCount);
    }

    if (componentJson.Contains(LIST_EVENT_REQUEST_ITEM) &&
        componentJson.GetValue(LIST_EVENT_REQUEST_ITEM)->IsString()) {
        requestItemEvent = componentJson.GetValue(LIST_EVENT_REQUEST_ITEM)->GetString();
        list->SetOnRequestItem(EventMarker { requestItemEvent });
    }

    if (componentJson.Contains(LIST_CACHED_COUNT) && componentJson.GetValue(LIST_CACHED_COUNT)->IsNumber()) {
        cachedCount = componentJson.GetValue(LIST_CACHED_COUNT)->GetInt();
        list->SetCachedCount(cachedCount);
    }

    if (componentJson.Contains(LIST_SCROLL_PAGE) && componentJson.GetValue(LIST_SCROLL_PAGE)->IsBool()) {
        scrollPage = componentJson.GetBool(LIST_SCROLL_PAGE, false);
        list->SetScrollPage(scrollPage);
    }

    list->SetScrollEffect(CreateEdgeEffect(componentJson));
    list->SetScrollBar(CreateScrollBar(componentJson));

    RefPtr<ScrollPositionController> controller = AceType::MakeRefPtr<ScrollPositionController>();
    if (componentJson.Contains(LIST_POSITION) && componentJson.GetValue(LIST_POSITION)->IsNumber()) {
        double initPosition = componentJson.GetValue(LIST_POSITION)->GetDouble();
        controller->SetInitialOffset(initPosition);
    }
    list->SetPositionController(controller);

    LOGD("Create list component: totalCount = %{public}d, requestItemEvent = \"%{private}s\", direction = %{public}d, "
         "cachedCount_ = %{public}d, scrollPage = %{public}d",
        totalCount, requestItemEvent.c_str(), direction, cachedCount, scrollPage);

    return list;
}

RefPtr<ScrollEdgeEffect> ListCreator::CreateEdgeEffect(const JsonValue& componentJson)
{
    Color fadeColor = Color::GRAY;
    EdgeEffect effect = EdgeEffect::SPRING;

    RefPtr<ScrollEdgeEffect> scrollEffect;
    if (componentJson.Contains(EDGE_EFFECT) && componentJson.GetValue(EDGE_EFFECT)->IsNumber()) {
        effect = static_cast<EdgeEffect>(componentJson.GetValue(EDGE_EFFECT)->GetInt());
    }
    if (effect == EdgeEffect::FADE) {
        if (componentJson.Contains(FADE_COLOR) && componentJson.GetValue(FADE_COLOR)->IsObject()) {
            auto colorJson = componentJson.GetValue(FADE_COLOR);
            fadeColor = ColorCreator::CreateFromJson(*colorJson);
        }
        scrollEffect = AceType::MakeRefPtr<ScrollFadeEffect>(fadeColor);
    } else if (effect == EdgeEffect::NONE) {
        scrollEffect = AceType::MakeRefPtr<ScrollEdgeEffect>(EdgeEffect::NONE);
    } else {
        scrollEffect = AceType::MakeRefPtr<ScrollSpringEffect>();
    }

    return scrollEffect;
}

RefPtr<ScrollBar> ListCreator::CreateScrollBar(const JsonValue& componentJson)
{
    DisplayMode displayMode = DisplayMode::OFF;
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(displayMode);
    if (componentJson.Contains(SCROLL_BAR_DISPLAY_MODE) &&
        componentJson.GetValue(SCROLL_BAR_DISPLAY_MODE)->IsNumber()) {
        displayMode = static_cast<DisplayMode>(componentJson.GetValue(SCROLL_BAR_DISPLAY_MODE)->GetInt());
    }

    if (displayMode != DisplayMode::OFF) {
        scrollBar = AceType::MakeRefPtr<ScrollBar>(displayMode);
        if (componentJson.Contains(SCROLL_BAR_SHAPE_MODE) &&
            componentJson.GetValue(SCROLL_BAR_SHAPE_MODE)->IsNumber()) {
            ShapeMode shapeMode = static_cast<ShapeMode>(componentJson.GetValue(SCROLL_BAR_SHAPE_MODE)->GetInt());
            scrollBar->SetShapeMode(shapeMode);
        }
    }

    return scrollBar;
}

} // namespace OHOS::Ace
