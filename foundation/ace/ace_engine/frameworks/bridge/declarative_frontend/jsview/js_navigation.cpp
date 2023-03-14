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

#include "frameworks/bridge/declarative_frontend/jsview/js_navigation.h"

#include "core/components/navigation_bar/navigation_bar_component_v2.h"
#include "core/components/navigation_bar/navigation_container_component.h"
#include "core/components/option/option_component.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

JSRef<JSVal> TitleModeChangeEventToJSValue(const NavigationTitleModeChangeEvent& eventInfo)
{
    return JSRef<JSVal>::Make(ToJSValue(eventInfo.IsMiniBar() ? static_cast<int32_t>(NavigationTitleMode::MINI)
                                                              : static_cast<int32_t>(NavigationTitleMode::FULL)));
}

void ParseToolBarItems(const JSRef<JSArray>& jsArray, std::list<RefPtr<ToolBarItem>>& items)
{
    auto length = jsArray->Length();
    for (size_t i = 0; i < length; i++) {
        auto item = jsArray->GetValueAt(i);
        if (!item->IsObject()) {
            LOGE("tool bar item is not object");
            continue;
        }

        auto itemObject = JSRef<JSObject>::Cast(item);
        auto toolBarItem = AceType::MakeRefPtr<ToolBarItem>();
        auto itemValueObject = itemObject->GetProperty("value");
        if (itemValueObject->IsString()) {
            toolBarItem->value = itemValueObject->ToString();
        }

        auto itemIconObject = itemObject->GetProperty("icon");
        if (itemIconObject->IsString()) {
            toolBarItem->icon = itemIconObject->ToString();
        }

        auto itemActionValue = itemObject->GetProperty("action");
        if (itemActionValue->IsFunction()) {
            auto onClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(itemActionValue));
            toolBarItem->action = EventMarker(
                [func = std::move(onClickFunc)]() {
                    ACE_SCORING_EVENT("Navigation.toolBarItemClick");
                    func->Execute();
                });
            auto onClickWithParamFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(itemActionValue));
            toolBarItem->actionWithParam = EventMarker(
                [func = std::move(onClickWithParamFunc)](const BaseEventInfo* info) {
                    ACE_SCORING_EVENT("Navigation.menuItemButtonClick");
                    func->Execute();
                });
        }
        items.push_back(toolBarItem);
    }
}

} // namespace

void JSNavigation::Create()
{
    auto navigationContainer = AceType::MakeRefPtr<NavigationContainerComponent>();
    ViewStackProcessor::GetInstance()->Push(navigationContainer);
}

void JSNavigation::JSBind(BindingTarget globalObj)
{
    JSClass<JSNavigation>::Declare("Navigation");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSNavigation>::StaticMethod("create", &JSNavigation::Create, opt);
    JSClass<JSNavigation>::StaticMethod("title", &JSNavigation::SetTitle, opt);
    JSClass<JSNavigation>::StaticMethod("subTitle", &JSNavigation::SetSubTitle, opt);
    JSClass<JSNavigation>::StaticMethod("titleMode", &JSNavigation::SetTitleMode, opt);
    JSClass<JSNavigation>::StaticMethod("hideTitleBar", &JSNavigation::SetHideTitleBar, opt);
    JSClass<JSNavigation>::StaticMethod("hideBackButton", &JSNavigation::SetHideBackButton, opt);
    JSClass<JSNavigation>::StaticMethod("hideToolBar", &JSNavigation::SetHideToolBar, opt);
    JSClass<JSNavigation>::StaticMethod("toolBar", &JSNavigation::SetToolBar);
    JSClass<JSNavigation>::StaticMethod("menus", &JSNavigation::SetMenus);
    JSClass<JSNavigation>::StaticMethod("menuCount", &JSNavigation::SetMenuCount);
    // keep compatible, need remove after
    JSClass<JSNavigation>::StaticMethod("onTitleModeChanged", &JSNavigation::SetOnTitleModeChanged);
    JSClass<JSNavigation>::StaticMethod("onTitleModeChange", &JSNavigation::SetOnTitleModeChanged);
    JSClass<JSNavigation>::Inherit<JSContainerBase>();
    JSClass<JSNavigation>::Inherit<JSViewAbstract>();
    JSClass<JSNavigation>::Bind(globalObj);
}

void JSNavigation::SetTitle(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (!navigationContainer) {
        LOGI("component is not navigationContainer.");
        return;
    }

    if (info[0]->IsString()) {
        navigationContainer->GetDeclaration()->title = info[0]->ToString();
    } else if (info[0]->IsObject()) {
        auto builderObject = JSRef<JSObject>::Cast(info[0])->GetProperty("builder");
        if (builderObject->IsFunction()) {
            ScopedViewStackProcessor builderViewStackProcessor;
            JsFunction jsBuilderFunc(info.This(), JSRef<JSObject>::Cast(builderObject));
            ACE_SCORING_EVENT("Navigation.title.builder");
            jsBuilderFunc.Execute();
            navigationContainer->GetDeclaration()->customTitle = ViewStackProcessor::GetInstance()->Finish();
        }
    } else {
        LOGE("arg is not [String|Function].");
    }
}

void JSNavigation::SetTitleMode(int32_t value)
{
    if ((value == static_cast<int32_t>(NavigationTitleMode::FREE)) ||
        (value == static_cast<int32_t>(NavigationTitleMode::FULL)) ||
        (value == static_cast<int32_t>(NavigationTitleMode::MINI))) {
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
        if (navigationContainer) {
            navigationContainer->GetDeclaration()->titleMode = static_cast<NavigationTitleMode>(value);
        }
    } else {
        LOGE("invalid value for titleMode");
    }
}

void JSNavigation::SetSubTitle(const std::string& subTitle)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (navigationContainer) {
        navigationContainer->GetDeclaration()->subTitle = subTitle;
    }
}

void JSNavigation::SetHideTitleBar(bool hide)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (navigationContainer) {
        auto declaration = navigationContainer->GetDeclaration();
        declaration->hideBar = hide;
        declaration->animationOption = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    }
}

void JSNavigation::SetHideBackButton(bool hide)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (navigationContainer) {
        navigationContainer->GetDeclaration()->hideBarBackButton = hide;
    }
}

void JSNavigation::SetHideToolBar(bool hide)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (navigationContainer) {
        auto declaration = navigationContainer->GetDeclaration();
        declaration->hideToolbar = hide;
        declaration->animationOption = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    }
}

void JSNavigation::SetToolBar(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least one argument");
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (!navigationContainer) {
        LOGI("component is not navigationContainer.");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not a object.");
        return;
    }
    auto builderFuncParam = JSRef<JSObject>::Cast(info[0])->GetProperty("builder");
    if (builderFuncParam->IsFunction()) {
        ScopedViewStackProcessor builderViewStackProcessor;
        JsFunction jsBuilderFunc(builderFuncParam);
        jsBuilderFunc.Execute();
        RefPtr<Component> builderGeneratedRootComponent = ViewStackProcessor::GetInstance()->Finish();
        navigationContainer->GetDeclaration()->toolBarBuilder = builderGeneratedRootComponent;
        return;
    }

    auto itemsValue = JSRef<JSObject>::Cast(info[0])->GetProperty("items");
    if (!itemsValue->IsObject() || !itemsValue->IsArray()) {
        LOGE("arg format error: not find items");
        return;
    }
    ParseToolBarItems(JSRef<JSArray>::Cast(itemsValue), navigationContainer->GetDeclaration()->toolbarItems);
}

void JSNavigation::SetMenus(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least one argument");
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (!navigationContainer) {
        LOGI("component is not navigationContainer.");
        return;
    }

    if (info[0]->IsArray()) {
        ParseToolBarItems(JSRef<JSArray>::Cast(info[0]), navigationContainer->GetDeclaration()->menuItems);
    } else if (info[0]->IsObject()) {
        auto builderObject = JSRef<JSObject>::Cast(info[0])->GetProperty("builder");
        if (builderObject->IsFunction()) {
            ScopedViewStackProcessor builderViewStackProcessor;
            JsFunction jsBuilderFunc(info.This(), JSRef<JSObject>::Cast(builderObject));
            {
                ACE_SCORING_EVENT("Navigation.menu.builder");
                jsBuilderFunc.Execute();
            }
            navigationContainer->GetDeclaration()->customMenus = ViewStackProcessor::GetInstance()->Finish();
        }
    } else {
        LOGE("arg is not [String|Function].");
    }
}

void JSNavigation::SetMenuCount(int32_t menuCount)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
    if (navigationContainer) {
        navigationContainer->SetMenuCount(menuCount);
    }
}

void JSNavigation::SetOnTitleModeChanged(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least one argument");
        return;
    }
    if (info[0]->IsFunction()) {
        auto changeHandler = AceType::MakeRefPtr<JsEventFunction<NavigationTitleModeChangeEvent, 1>>(
            JSRef<JSFunc>::Cast(info[0]), TitleModeChangeEventToJSValue);
        auto eventMarker = EventMarker([executionContext = info.GetExecutionContext(), func = std::move(changeHandler)](
                                           const BaseEventInfo* baseInfo) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(executionContext);
            auto eventInfo = TypeInfoHelper::DynamicCast<NavigationTitleModeChangeEvent>(baseInfo);
            if (!eventInfo) {
                LOGE("HandleChangeEvent eventInfo == nullptr");
                return;
            }
            ACE_SCORING_EVENT("Navigation.onTitleModeChanged");
            func->Execute(*eventInfo);
        });
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto navigationContainer = AceType::DynamicCast<OHOS::Ace::NavigationContainerComponent>(component);
        if (navigationContainer) {
            navigationContainer->GetDeclaration()->titleModeChangedEvent = eventMarker;
        }
    }
    info.ReturnSelf();
}

} // namespace OHOS::Ace::Framework
