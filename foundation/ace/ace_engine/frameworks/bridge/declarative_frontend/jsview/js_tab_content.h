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

#ifndef FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_TAB_CONTENT_H
#define FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_TAB_CONTENT_H

#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/core/components/tab_bar/tab_content_component.h"
#include "frameworks/core/components_v2/tabs/tab_content_item_component.h"
#include "frameworks/core/components_v2/tabs/tabs_component.h"

namespace OHOS::Ace::Framework {

class JSTabContent : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create();

protected:
    static void SetTabBar(const JSCallbackInfo& info);

    // The width and height of tabcontent depend on the size constraints of tabs and tabbar.
    static void SetTabContentWidth(const JSCallbackInfo& info)
    {
        LOGW("TabContent does not support width and height settings");
    }

    static void SetTabContentHeight(const JSCallbackInfo& info)
    {
        LOGW("TabContent does not support width and height settings");
    }

    static void SetTabContentSize(const JSCallbackInfo& info)
    {
        LOGW("TabContent does not support width and height settings");
    }

private:
    static RefPtr<Component> ProcessTabBarBuilderFunction(
        RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSObject> builderFunc);
    static RefPtr<Component> ProcessTabBarTextIconPair(
        RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSVal> text, JSRef<JSVal> icon);
    static RefPtr<TextComponent> CreateTabBarLabelComponent(
        RefPtr<V2::TabContentItemComponent>& tabContent, const std::string& labelStr);
    static RefPtr<TextComponent> ProcessTabBarLabel(
        RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSVal> labelVal);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_TAB_CONTENT_H
