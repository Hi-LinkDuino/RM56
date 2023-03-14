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

#include "bridge/declarative_frontend/jsview/js_scroll.h"

#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/scroll/scroll_component.h"

namespace OHOS::Ace::Framework {
namespace {
    const std::vector<DisplayMode> DISPLAY_MODE = {DisplayMode::OFF, DisplayMode::AUTO, DisplayMode::ON};
}
void JSScroll::Create(const JSCallbackInfo& info)
{
    RefPtr<Component> child;
    auto scrollComponent = AceType::MakeRefPtr<OHOS::Ace::ScrollComponent>(child);
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSScroller* jsScroller = JSRef<JSObject>::Cast(info[0])->Unwrap<JSScroller>();
        if (jsScroller) {
            auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
            jsScroller->SetController(positionController);
            scrollComponent->SetScrollPositionController(positionController);

            // Init scroll bar proxy.
            auto proxy = jsScroller->GetScrollBarProxy();
            if (!proxy) {
                proxy = AceType::MakeRefPtr<ScrollBarProxy>();
                jsScroller->SetScrollBarProxy(proxy);
            }
            scrollComponent->SetScrollBarProxy(proxy);
        }
    } else {
        auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
        scrollComponent->SetScrollPositionController(positionController);
    }
    // init scroll bar
    std::pair<bool, Color> barColor;
    barColor.first = false;
    std::pair<bool, Dimension> barWidth;
    barWidth.first = false;
    scrollComponent->InitScrollBar(GetTheme<ScrollBarTheme>(), barColor, barWidth, EdgeEffect::NONE);
    ViewStackProcessor::GetInstance()->Push(scrollComponent);
}

void JSScroll::SetScrollable(int32_t value)
{
    if (value >= 0 && value < 4) {  // Number of scrolling methods
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto scrollComponent = AceType::DynamicCast<ScrollComponent>(component);
        if (scrollComponent) {
            scrollComponent->SetAxisDirection((Axis)value);
        }
    } else {
        LOGE("invalid value for SetScrollable");
    }
}

void JSScroll::OnScrollCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScroll = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<ScrollEventInfo>(info);
                if (!eventInfo) {
                    return;
                }
                auto params = ConvertToJSValues(eventInfo->GetScrollX(), eventInfo->GetScrollY());
                func->Call(JSRef<JSObject>(), params.size(), params.data());
            });
        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScroll(onScroll);
        }
    }
    args.SetReturnValue(args.This());
}

void JSScroll::OnScrollEdgeCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScroll = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<ScrollEventInfo>(info);
                if (!eventInfo) {
                    return;
                }
                auto param = ConvertToJSValue(static_cast<int32_t>(eventInfo->GetType()));
                func->Call(JSRef<JSObject>(), 1, &param);
            });
        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScrollEdge(onScroll);
        }
    }
    args.SetReturnValue(args.This());
}

void JSScroll::OnScrollEndCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScrollStop = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                func->Call(JSRef<JSObject>(), 0, nullptr);
            });
        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScrollEnd(onScrollStop);
        }
    }
    args.SetReturnValue(args.This());
}

void JSScroll::JSBind(BindingTarget globalObj)
{
    JSClass<JSScroll>::Declare("Scroll");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSScroll>::StaticMethod("create", &JSScroll::Create, opt);
    JSClass<JSScroll>::StaticMethod("scrollable", &JSScroll::SetScrollable, opt);
    JSClass<JSScroll>::StaticMethod("onScroll", &JSScroll::OnScrollCallback, opt);
    JSClass<JSScroll>::StaticMethod("onScrollEdge", &JSScroll::OnScrollEdgeCallback, opt);
    JSClass<JSScroll>::StaticMethod("onScrollEnd", &JSScroll::OnScrollEndCallback, opt);
    JSClass<JSScroll>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSScroll>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSScroll>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSScroll>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSScroll>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSScroll>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSScroll>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSScroll>::StaticMethod("edgeEffect", &JSScroll::SetEdgeEffect, opt);
    JSClass<JSScroll>::StaticMethod("scrollBar", &JSScroll::SetScrollBar, opt);
    JSClass<JSScroll>::StaticMethod("scrollBarColor", &JSScroll::SetScrollBarColor, opt);
    JSClass<JSScroll>::StaticMethod("scrollBarWidth", &JSScroll::SetScrollBarWidth, opt);
    JSClass<JSScroll>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSScroll>::Inherit<JSContainerBase>();
    JSClass<JSScroll>::Inherit<JSViewAbstract>();
    JSClass<JSScroll>::Bind<>(globalObj);
}

void JSScroll::SetScrollBar(int displayMode)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent) {
        return;
    }
    if (displayMode >= 0 && displayMode < static_cast<int32_t>(DISPLAY_MODE.size())) {
        scrollComponent->SetDisplayMode(DISPLAY_MODE[displayMode]);
    }
}

void JSScroll::SetScrollBarWidth(const std::string& scrollBarWidth)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent || scrollBarWidth.empty()) {
        return;
    }
    scrollComponent->SetScrollBarWidth(StringUtils::StringToDimension(scrollBarWidth));
}

void JSScroll::SetScrollBarColor(const  std::string& scrollBarColor)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent || scrollBarColor.empty()) {
        return;
    }
    scrollComponent->SetScrollBarColor(Color::FromString(scrollBarColor));
}

void JSScroll::SetEdgeEffect(int edgeEffect)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent) {
        return;
    }
    RefPtr<ScrollEdgeEffect> scrollEdgeEffect;
    if (edgeEffect == 0) {
        scrollEdgeEffect = AceType::MakeRefPtr<ScrollSpringEffect>();
    } else if (edgeEffect == 1) {
        scrollEdgeEffect = AceType::MakeRefPtr<ScrollFadeEffect>(Color::GRAY);
    } else {
        scrollEdgeEffect = AceType::MakeRefPtr<ScrollEdgeEffect>(EdgeEffect::NONE);
    }
    scrollComponent->SetScrollEffect(scrollEdgeEffect);
}

} // namespace OHOS::Ace::Framework
