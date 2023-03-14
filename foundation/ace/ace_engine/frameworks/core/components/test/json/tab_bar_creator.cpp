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

#include "core/components/test/json/tab_bar_creator.h"

#include "base/log/log.h"
#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_controller.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/decoration_creator.h"
#include "core/components/test/json/edge_creator.h"
#include "core/components/test/json/image_creator.h"
#include "core/components/test/json/text_style_creator.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const static std::map<std::string, TabBarMode> MODE_VALUE_MAP = {
    { "fixed", TabBarMode::FIXED },
    { "scrollable", TabBarMode::SCROLLABEL },
};

const static std::map<std::string, TabBarIndicatorType> INDICATOR_SIZE_VALUE_MAP = {
    { "tab", TabBarIndicatorType::TAB },
    { "label", TabBarIndicatorType::LABEL },
};

} // namespace

RefPtr<Component> TabBarCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Tabbar");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != TAB_BAR_NAME) {
        LOGE("Create Tabbar err: not a tabbar json.");
        return nullptr;
    }

    if (!componentJson.Contains(TAB_TABS) || !componentJson.GetValue(TAB_TABS)->IsArray() ||
        componentJson.GetValue(TAB_TABS)->GetArraySize() <= 0) {
        LOGE("Create Tabbar error: missing %{public}s", TAB_TABS);
        return nullptr;
    }

    if (!componentJson.Contains(TAB_CONTROLLER_ID) || !componentJson.GetValue(TAB_CONTROLLER_ID)->IsNumber()) {
        LOGE("Create Tabbar error: missing %{public}s", TAB_CONTROLLER_ID);
        return nullptr;
    }

    int32_t controllerId = componentJson.GetValue(TAB_CONTROLLER_ID)->GetInt();
    RefPtr<TabController> controller = TabController::GetController(controllerId);
    RefPtr<BoxComponent> indicator = CreateIndicatorFromJson(componentJson);
    std::list<RefPtr<Component>> tabs;
    auto tabBar = AceType::MakeRefPtr<TabBarComponent>(tabs, controller, indicator);
    auto tabsJson = componentJson.GetValue(TAB_TABS);
    int32_t size = tabsJson->GetArraySize();
    for (int32_t i = 0; i < size; ++i) {
        auto item = tabsJson->GetArrayItem(i);
        RefPtr<Component> tab = CreateTabFromJson(*item, factory);
        if (tab) {
            tabBar->AppendChild(tab);
        }
    }

    if (componentJson.Contains(TAB_IS_VERTICAL) && componentJson.GetValue(TAB_IS_VERTICAL)->IsBool()) {
        tabBar->SetVertical(componentJson.GetValue(TAB_IS_VERTICAL)->GetBool());
    }

    if (componentJson.Contains(TAB_INDEX) && componentJson.GetValue(TAB_INDEX)->IsNumber()) {
        int32_t index = componentJson.GetValue(TAB_INDEX)->GetInt();
        tabBar->SetIndex(index);
        LOGD("Create Tabbar index: %{public}d", index);
    }

    UpdateTabBarComponentProperties(componentJson, tabBar);

    return tabBar;
}

void TabBarCreator::UpdateTabBarComponentProperties(
    const JsonValue& componentJson, const RefPtr<TabBarComponent>& tabBar)
{
    if (componentJson.Contains(TAB_MODE) && componentJson.GetValue(TAB_MODE)->IsString()) {
        std::string mode = componentJson.GetValue(TAB_MODE)->GetString();
        auto iter = MODE_VALUE_MAP.find(mode);
        if (iter != MODE_VALUE_MAP.end()) {
            tabBar->SetMode(iter->second);
            LOGD("Create Tabbar mod: %{public}s", mode.c_str());
        }
    }

    if (componentJson.Contains(TAB_INDICATOR_SIZE) && componentJson.GetValue(TAB_INDICATOR_SIZE)->IsString()) {
        std::string indicatorSize = componentJson.GetValue(TAB_INDICATOR_SIZE)->GetString();
        auto iter = INDICATOR_SIZE_VALUE_MAP.find(indicatorSize);
        if (iter != INDICATOR_SIZE_VALUE_MAP.end()) {
            tabBar->SetIndicatorSize(iter->second);
            LOGD("Create Tabbar indicatorSize: %{public}s", indicatorSize.c_str());
        }
    }

    if (componentJson.Contains(TAB_LABEL_PADDING) && componentJson.GetValue(TAB_LABEL_PADDING)->IsObject()) {
        auto padding = EdgeCreator::CreateFromJson(*(componentJson.GetValue(TAB_LABEL_PADDING)));
        tabBar->SetLabelPadding(padding);
    }
}

RefPtr<BoxComponent> TabBarCreator::CreateIndicatorFromJson(const JsonValue& componentJson)
{
    if (componentJson.Contains(TAB_INDICATOR) && componentJson.GetValue(TAB_INDICATOR)->IsObject()) {
        auto indicatorStyle = DecorationCreator::CreateFromJson(*componentJson.GetValue(TAB_INDICATOR));
        return AceType::MakeRefPtr<TabBarIndicatorComponent>(indicatorStyle);
    } else {
        Dimension indicatorWidth = DEFAULT_INDICATOR_WIDTH;
        if (componentJson.Contains(TAB_INDICATOR_WIDTH) && componentJson.GetValue(TAB_INDICATOR_WIDTH)->IsNumber()) {
            indicatorWidth = Dimension(componentJson.GetValue(TAB_INDICATOR_WIDTH)->GetDouble());
            LOGD("Create Tabbar indicatorWidth: %{public}f", indicatorWidth.Value());
        }

        Color indicatorColor = DEFAULT_INDICATOR_COLOR;
        if (componentJson.Contains(TAB_INDICATOR_COLOR) && componentJson.GetValue(TAB_INDICATOR_COLOR)->IsObject()) {
            indicatorColor = ColorCreator::CreateFromJson(*(componentJson.GetValue(TAB_INDICATOR_COLOR)));
        }

        Edge indicatorPadding = DEFAULT_INDICATOR_PADDING;
        if (componentJson.Contains(TAB_INDICATOR_PADDING) &&
            componentJson.GetValue(TAB_INDICATOR_PADDING)->IsObject()) {
            indicatorPadding = EdgeCreator::CreateFromJson(*(componentJson.GetValue(TAB_INDICATOR_PADDING)));
        }
        return AceType::MakeRefPtr<TabBarIndicatorComponent>(indicatorPadding, indicatorColor, indicatorWidth);
    }
}

RefPtr<Component> TabBarCreator::CreateTabFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != TAB_NAME) {
        LOGE("Create Tab err: not a tab json.");
        return nullptr;
    }
    if (componentJson.Contains(CHILD) && componentJson.GetValue(CHILD)->IsObject()) {
        return factory.CreateComponent(*(componentJson.GetValue(CHILD)));
    } else {
        RefPtr<Component> icon = nullptr;
        if (componentJson.Contains(TAB_ITEM_ICON) && componentJson.GetValue(TAB_ITEM_ICON)->IsObject()) {
            icon = ImageCreator::CreateFromJson(*(componentJson.GetValue(TAB_ITEM_ICON)), factory);
        }
        std::string text;
        if (componentJson.Contains(TAB_ITEM_TEXT) && componentJson.GetValue(TAB_ITEM_TEXT)->IsString()) {
            text = componentJson.GetValue(TAB_ITEM_TEXT)->GetString();
        }

        if (!icon && text.empty()) {
            LOGE("Create TabBar error: child == null && icon == null && text == null");
            return nullptr;
        }
        return AceType::MakeRefPtr<TabBarItemComponent>(text, icon);
    }
}

} // namespace OHOS::Ace
