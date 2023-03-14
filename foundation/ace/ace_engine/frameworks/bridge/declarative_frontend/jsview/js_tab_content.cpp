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

#include "frameworks/bridge/declarative_frontend/jsview/js_tab_content.h"

#include "base/log/ace_trace.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "frameworks/core/components/padding/padding_component.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension DEFAULT_SMALL_TEXT_FONT_SIZE = 10.0_fp;
constexpr Dimension DEFAULT_SMALL_IMAGE_WIDTH = 24.0_vp;
constexpr Dimension DEFAULT_SMALL_IMAGE_HEIGHT = 26.0_vp;
constexpr Dimension DEFAULT_SINGLE_TEXT_FONT_SIZE = 16.0_fp;
constexpr char DEFAULT_TAB_BAR_NAME[] = "TabBar";

} // namespace

void JSTabContent::Create()
{
    auto tabsComponent = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetTopTabs());
    if (tabsComponent) {
        auto tabBar = tabsComponent->GetTabBarChild();
        std::list<RefPtr<Component>> components;
        auto tabContentItemComponent = AceType::MakeRefPtr<V2::TabContentItemComponent>(components);
        tabContentItemComponent->SetCrossAxisSize(CrossAxisSize::MAX);
        tabContentItemComponent->SetTabsComponent(AceType::WeakClaim(AceType::RawPtr(tabsComponent)));
        tabBar->AppendChild(CreateTabBarLabelComponent(tabContentItemComponent, std::string(DEFAULT_TAB_BAR_NAME)));
        ViewStackProcessor::GetInstance()->Push(tabContentItemComponent);
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        if (box) {
            box->SetBoxClipFlag(true);
        }
    } else {
        LOGE("fail to create tab content due to tabs missing");
    }
}

void JSTabContent::SetTabBar(const JSCallbackInfo& info)
{
    auto tabContentItemComponent =
        AceType::DynamicCast<V2::TabContentItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!tabContentItemComponent) {
        return;
    }
    auto weakTabs = tabContentItemComponent->GetTabsComponent();
    auto tabs = weakTabs.Upgrade();
    if (!tabs) {
        LOGE("can not get Tabs parent component error.");
        return;
    }
    RefPtr<TabBarComponent> tabBar = tabs->GetTabBarChild();
    if (!tabBar) {
        LOGE("can not get TabBar component error.");
        return;
    }
    RefPtr<Component> tabBarChild = nullptr;
    std::string infoStr;
    if (ParseJsString(info[0], infoStr)) {
        auto textVal = infoStr.empty() ? DEFAULT_TAB_BAR_NAME : infoStr;
        auto text = CreateTabBarLabelComponent(tabContentItemComponent, textVal);
        auto defaultTabChild = tabBar->GetChildren().back();
        tabBar->RemoveChildDirectly(defaultTabChild);
        tabBar->AppendChild(text);
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> builderFuncParam = paramObject->GetProperty("builder");
    JSRef<JSVal> textParam = paramObject->GetProperty("text");
    JSRef<JSVal> iconParam = paramObject->GetProperty("icon");
    if (builderFuncParam->IsFunction()) {
        tabBarChild = ProcessTabBarBuilderFunction(tabContentItemComponent, builderFuncParam);
        // for custom build, no need for indicator.
        tabBar->ResetIndicator();
        tabBar->SetAlignment(Alignment::TOP_LEFT);
    } else if (!textParam->IsEmpty() && !iconParam->IsEmpty()) {
        tabBarChild = ProcessTabBarTextIconPair(tabContentItemComponent, textParam, iconParam);
    } else if (!textParam->IsEmpty() && iconParam->IsEmpty()) {
        tabBarChild = ProcessTabBarLabel(tabContentItemComponent, textParam);
    } else {
        LOGE("invalid parameters: expecting either builder func, text & icon pair, or label");
        return;
    }
    auto defaultTabChild = tabBar->GetChildren().back();
    tabBar->RemoveChildDirectly(defaultTabChild);
    tabBar->AppendChild(tabBarChild);
}

RefPtr<Component> JSTabContent::ProcessTabBarBuilderFunction(
    RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSObject> builderFunc)
{
    tabContent->SetBarText("custom");
    ScopedViewStackProcessor builderViewStackProcessor;
    JsFunction jsBuilderFunc(builderFunc);
    ACE_SCORING_EVENT("TabContent.tabBarBuilder");
    jsBuilderFunc.Execute();
    RefPtr<Component> builderGeneratedRootComponent = ViewStackProcessor::GetInstance()->Finish();
    return builderGeneratedRootComponent;
}

RefPtr<TextComponent> JSTabContent::CreateTabBarLabelComponent(
    RefPtr<V2::TabContentItemComponent>& tabContent, const std::string& labelStr)
{
    tabContent->SetBarText(labelStr);
    auto text = AceType::MakeRefPtr<TextComponent>(labelStr);
    auto textStyle = text->GetTextStyle();
    textStyle.SetFontSize(DEFAULT_SINGLE_TEXT_FONT_SIZE);
    textStyle.SetMaxLines(1);
    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    text->SetTextStyle(textStyle);
    text->SetAutoMaxLines(false);
    return text;
}

RefPtr<TextComponent> JSTabContent::ProcessTabBarLabel(
    RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSVal> labelVal)
{
    std::string textStr;
    if (!ParseJsString(labelVal, textStr)) {
        textStr = DEFAULT_TAB_BAR_NAME;
    }
    return CreateTabBarLabelComponent(tabContent, textStr);
}

RefPtr<Component> JSTabContent::ProcessTabBarTextIconPair(
    RefPtr<V2::TabContentItemComponent>& tabContent, JSRef<JSVal> textVal, JSRef<JSVal> iconVal)
{
    std::string iconUri;
    if (!ParseJsMedia(iconVal, iconUri)) {
        return ProcessTabBarLabel(tabContent, textVal);
    }
    std::string textStr;
    if (!ParseJsString(textVal, textStr)) {
        textStr = DEFAULT_TAB_BAR_NAME;
    }
    tabContent->SetBarText(textStr);
    auto imageComponent = AceType::MakeRefPtr<ImageComponent>(iconUri);
    auto box = AceType::MakeRefPtr<BoxComponent>();
    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    padding->SetPadding(Edge(0, 0, 0, 2, DimensionUnit::VP));
    padding->SetChild(imageComponent);
    box->SetChild(padding);
    box->SetWidth(DEFAULT_SMALL_IMAGE_WIDTH);
    box->SetHeight(DEFAULT_SMALL_IMAGE_HEIGHT);
    auto textComponent = AceType::MakeRefPtr<TextComponent>(textStr);
    auto textStyle = textComponent->GetTextStyle();
    textStyle.SetFontSize(DEFAULT_SMALL_TEXT_FONT_SIZE);
    textStyle.SetMaxLines(1);
    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    textComponent->SetTextStyle(textStyle);
    std::list<RefPtr<Component>> children;
    children.emplace_back(box);
    children.emplace_back(textComponent);
    auto columnComponent = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, children);
    columnComponent->SetMainAxisSize(MainAxisSize::MIN);
    return columnComponent;
}

void JSTabContent::JSBind(BindingTarget globalObj)
{
    JSClass<JSTabContent>::Declare("TabContent");
    JSClass<JSTabContent>::StaticMethod("create", &JSTabContent::Create);
    JSClass<JSTabContent>::StaticMethod("tabBar", &JSTabContent::SetTabBar);
    JSClass<JSTabContent>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTabContent>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTabContent>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTabContent>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSTabContent>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTabContent>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTabContent>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTabContent>::StaticMethod("width", &JSTabContent::SetTabContentWidth);
    JSClass<JSTabContent>::StaticMethod("height", &JSTabContent::SetTabContentHeight);
    JSClass<JSTabContent>::StaticMethod("size", &JSTabContent::SetTabContentSize);
    JSClass<JSTabContent>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSTabContent>::Inherit<JSContainerBase>();
    JSClass<JSTabContent>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
