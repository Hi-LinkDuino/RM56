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

#include "frameworks/bridge/declarative_frontend/jsview/js_tabs.h"

#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_content_component.h"
#include "core/components_v2/tabs/tabs_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_tabs_controller.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension DEFAULT_TAB_BAR_HEIGHT = 56.0_vp;
const std::vector<BarPosition> BAR_POSITIONS = { BarPosition::START, BarPosition::END };

JSRef<JSVal> TabContentChangeEventToJSValue(const TabContentChangeEvent& eventInfo)
{
    return JSRef<JSVal>::Make(ToJSValue(eventInfo.GetIndex()));
}

} // namespace

void JSTabs::SetOnChange(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto changeHandler = AceType::MakeRefPtr<JsEventFunction<TabContentChangeEvent, 1>>(
            JSRef<JSFunc>::Cast(args[0]), TabContentChangeEventToJSValue);
        auto onChange = EventMarker([executionContext = args.GetExecutionContext(), func = std::move(changeHandler)](
                                        const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(executionContext);
            auto TabsInfo = TypeInfoHelper::DynamicCast<TabContentChangeEvent>(info);
            if (!TabsInfo) {
                LOGE("HandleChangeEvent TabsInfo == nullptr");
                return;
            }
            ACE_SCORING_EVENT("Tabs.onChange");
            func->Execute(*TabsInfo);
        });
        auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (component) {
            auto tabContent = component->GetTabContentChild();
            if (tabContent) {
                tabContent->SetChangeEventId(onChange);
            }
        }
    }
    args.ReturnSelf();
}

void JSTabs::Create(const JSCallbackInfo& info)
{
    BarPosition barVal = BarPosition::START;
    RefPtr<TabController> tabController;
    if (info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> val = obj->GetProperty("barPosition");
        if (val->IsNumber()) {
            auto barPositionVal = val->ToNumber<int32_t>();
            if (barPositionVal >= 0 && barPositionVal < static_cast<int32_t>(BAR_POSITIONS.size())) {
                barVal = BAR_POSITIONS[barPositionVal];
            }
        }
        JSRef<JSVal> controller = obj->GetProperty("controller");
        if (controller->IsObject()) {
            auto jsTabsController = JSRef<JSObject>::Cast(controller)->Unwrap<JSTabsController>();
            if (jsTabsController) {
                tabController = jsTabsController->GetController();
            }
        }
        JSRef<JSVal> index = obj->GetProperty("index");
        if (index->IsNumber()) {
            if (!tabController) {
                tabController = JSTabsController::CreateController();
            }
            tabController->SetInitialIndex(index->ToNumber<int32_t>());
        }
    }
    std::list<RefPtr<Component>> children;
    auto tabsComponent = AceType::MakeRefPtr<V2::TabsComponent>(children, barVal, tabController);
    auto tabBar = tabsComponent->GetTabBarChild();
    if (tabBar) {
        auto theme = GetTheme<TabTheme>();
        tabBar->InitStyle(theme);
        auto box = AceType::DynamicCast<BoxComponent>(tabBar->GetParent().Upgrade());
        if (box) {
            box->SetHeight(DEFAULT_TAB_BAR_HEIGHT);
        }
    }
    ViewStackProcessor::GetInstance()->PushTabs(tabsComponent);
    ViewStackProcessor::GetInstance()->Push(tabsComponent);
}

void JSTabs::Pop()
{
    ViewStackProcessor::GetInstance()->PopTabs();
    JSContainerBase::Pop();
}

void JSTabs::SetVertical(const std::string& value)
{
    auto tabsComponent = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!tabsComponent) {
        return;
    }
    bool isVertical = StringToBool(value);
    if (isVertical) {
        tabsComponent->SetDirection(FlexDirection::ROW);
    } else {
        tabsComponent->SetDirection(FlexDirection::COLUMN);
    }
    auto tabBar = tabsComponent->GetTabBarChild();
    if (tabBar) {
        tabBar->SetVertical(isVertical);
    }
    auto tabContent = tabsComponent->GetTabContentChild();
    if (tabContent) {
        tabContent->SetVertical(isVertical);
    }
}

void JSTabs::SetScrollable(const std::string& value)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabContent = component->GetTabContentChild();
    if (tabContent) {
        tabContent->SetScrollable(StringToBool(value));
    }
}

void JSTabs::SetBarMode(const std::string& value)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (tabBar) {
        tabBar->SetMode(ConvertStrToTabBarMode(value));
    }
}

void JSTabs::SetBarWidth(const JSCallbackInfo& info)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Dimension width;
    if (!ParseJsDimensionVp(info[0], width)) {
        LOGE("The arg is wrong, fail to parse dimension");
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (!tabBar) {
        LOGE("can not find tab bar component");
        return;
    }
    auto box = AceType::DynamicCast<BoxComponent>(tabBar->GetParent().Upgrade());
    if (box) {
        box->SetWidth(width);
    } else {
        LOGE("can not find box component");
    }
}

void JSTabs::SetBarHeight(const JSCallbackInfo& info)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Dimension height;
    if (!ParseJsDimensionVp(info[0], height)) {
        LOGE("The arg is wrong, fail to parse dimension");
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (!tabBar) {
        LOGE("can not find tab bar component");
        return;
    }
    auto box = AceType::DynamicCast<BoxComponent>(tabBar->GetParent().Upgrade());
    if (box) {
        box->SetHeight(height);
    } else {
        LOGE("can not find box component");
    }
}

void JSTabs::SetIndex(int32_t index)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGE("can not find tabs component");
        return;
    }
    auto controller = component->GetTabsController();
    if (controller) {
        controller->SetPendingIndex(index);
    } else {
        LOGE("can not find controller");
    }
}

void JSTabs::SetAnimationDuration(float value)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabContent = component->GetTabContentChild();
    if (!tabContent) {
        return;
    }
    tabContent->SetScrollDuration(value);
}

void JSTabs::JSBind(BindingTarget globalObj)
{
    JSClass<JSTabs>::Declare("Tabs");
    JSClass<JSTabs>::StaticMethod("create", &JSTabs::Create);
    JSClass<JSTabs>::StaticMethod("pop", &JSTabs::Pop);
    JSClass<JSTabs>::StaticMethod("vertical", &JSTabs::SetVertical);
    JSClass<JSTabs>::StaticMethod("scrollable", &JSTabs::SetScrollable);
    JSClass<JSTabs>::StaticMethod("barMode", &JSTabs::SetBarMode);
    JSClass<JSTabs>::StaticMethod("barWidth", &JSTabs::SetBarWidth);
    JSClass<JSTabs>::StaticMethod("barHeight", &JSTabs::SetBarHeight);
    JSClass<JSTabs>::StaticMethod("index", &JSTabs::SetIndex);
    JSClass<JSTabs>::StaticMethod("animationDuration", &JSTabs::SetAnimationDuration);
    JSClass<JSTabs>::StaticMethod("onChange", &JSTabs::SetOnChange);
    JSClass<JSTabs>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTabs>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTabs>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTabs>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSTabs>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTabs>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTabs>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTabs>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSTabs>::Inherit<JSContainerBase>();
    JSClass<JSTabs>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
