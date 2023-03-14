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

#include "bridge/declarative_frontend/jsview/js_list.h"

#include "bridge/declarative_frontend/engine/functions/js_drag_function.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components_v2/list/list_component.h"
#include "core/components_v2/list/list_position_controller.h"

namespace OHOS::Ace::Framework {
namespace {

using ThisComponent = V2::ListComponent;

constexpr DisplayMode DISPLAY_MODE_TABLE[] = { DisplayMode::OFF, DisplayMode::AUTO, DisplayMode::ON };
constexpr EdgeEffect EDGE_EFFECT_TABLE[] = { EdgeEffect::SPRING, EdgeEffect::FADE, EdgeEffect::NONE };
constexpr Axis DIRECTION_TABLE[] = { Axis::VERTICAL, Axis::HORIZONTAL };

} // namespace

void JSList::SetDirection(int32_t direction)
{
    JSViewSetProperty(&V2::ListComponent::SetDirection, direction, DIRECTION_TABLE, Axis::VERTICAL);
}

void JSList::SetScrollBar(int32_t scrollBar)
{
    JSViewSetProperty(&V2::ListComponent::SetScrollBar, scrollBar, DISPLAY_MODE_TABLE, DisplayMode::OFF);
}

void JSList::SetEdgeEffect(int32_t edgeEffect)
{
    JSViewSetProperty(&V2::ListComponent::SetEdgeEffect, edgeEffect, EDGE_EFFECT_TABLE, EdgeEffect::SPRING);
}

void JSList::SetEditMode(bool editMode)
{
    JSViewSetProperty(&V2::ListComponent::SetEditMode, editMode);
}

void JSList::SetCachedCount(int32_t cachedCount)
{
    JSViewSetProperty(&V2::ListComponent::SetCachedCount, cachedCount);
}

void JSList::Create(const JSCallbackInfo& args)
{
    auto listComponent = AceType::MakeRefPtr<V2::ListComponent>();
    if (args.Length() >= 1 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);

        Dimension space;
        if (ConvertFromJSValue(obj->GetProperty("space"), space) && space.IsValid()) {
            listComponent->SetSpace(space);
        }

        int32_t initialIndex = 0;
        if (ConvertFromJSValue(obj->GetProperty("initialIndex"), initialIndex) && initialIndex >= 0) {
            listComponent->SetInitialIndex(initialIndex);
        }

        JSRef<JSVal> scrollerValue = obj->GetProperty("scroller");
        if (scrollerValue->IsObject()) {
            auto scroller = Referenced::Claim(JSRef<JSObject>::Cast(scrollerValue)->Unwrap<JSScroller>());
            if (scroller) {
                auto listController = AceType::MakeRefPtr<V2::ListPositionController>();
                scroller->SetController(listController);
                listComponent->SetScrollController(listController);

                // Init scroll bar proxy.
                auto proxy = scroller->GetScrollBarProxy();
                if (!proxy) {
                    proxy = AceType::MakeRefPtr<ScrollBarProxy>();
                    scroller->SetScrollBarProxy(proxy);
                }
                listComponent->SetScrollBarProxy(proxy);
            }
        }
    }

    ViewStackProcessor::GetInstance()->Push(listComponent);
    JSInteractableView::SetFocusNode(true);
    args.ReturnSelf();
}

void JSList::SetChainAnimation(bool enableChainAnimation)
{
    JSViewSetProperty(&V2::ListComponent::SetChainAnimation, enableChainAnimation);
}

void JSList::JsWidth(const JSCallbackInfo& info)
{
    JSViewAbstract::JsWidth(info);
    JSViewSetProperty(&V2::ListComponent::SetHasWidth, true);
}

void JSList::JsHeight(const JSCallbackInfo& info)
{
    JSViewAbstract::JsHeight(info);
    JSViewSetProperty(&V2::ListComponent::SetHasHeight, true);
}

void JSList::JSBind(BindingTarget globalObj)
{
    JSClass<JSList>::Declare("List");
    JSClass<JSList>::StaticMethod("create", &JSList::Create);

    JSClass<JSList>::StaticMethod("width", &JSList::JsWidth);
    JSClass<JSList>::StaticMethod("height", &JSList::JsHeight);
    JSClass<JSList>::StaticMethod("listDirection", &JSList::SetDirection);
    JSClass<JSList>::StaticMethod("scrollBar", &JSList::SetScrollBar);
    JSClass<JSList>::StaticMethod("edgeEffect", &JSList::SetEdgeEffect);
    JSClass<JSList>::StaticMethod("divider", &JSList::SetDivider);
    JSClass<JSList>::StaticMethod("editMode", &JSList::SetEditMode);
    JSClass<JSList>::StaticMethod("cachedCount", &JSList::SetCachedCount);
    JSClass<JSList>::StaticMethod("chainAnimation", &JSList::SetChainAnimation);
    JSClass<JSList>::StaticMethod("multiSelectable", &JSList::SetMultiSelectable);

    JSClass<JSList>::StaticMethod("onScroll", &JSList::ScrollCallback);
    JSClass<JSList>::StaticMethod("onReachStart", &JSList::ReachStartCallback);
    JSClass<JSList>::StaticMethod("onReachEnd", &JSList::ReachEndCallback);
    JSClass<JSList>::StaticMethod("onScrollStop", &JSList::ScrollStopCallback);
    JSClass<JSList>::StaticMethod("onItemDelete", &JSList::ItemDeleteCallback);
    JSClass<JSList>::StaticMethod("onItemMove", &JSList::ItemMoveCallback);
    JSClass<JSList>::StaticMethod("onScrollIndex", &JSList::ScrollIndexCallback);

    JSClass<JSList>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSList>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSList>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSList>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSList>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSList>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSList>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);

    JSClass<JSList>::StaticMethod("onItemDragStart", &JSList::ItemDragStartCallback);
    JSClass<JSList>::StaticMethod("onItemDragEnter", &JSList::ItemDragEnterCallback);
    JSClass<JSList>::StaticMethod("onItemDragMove", &JSList::ItemDragMoveCallback);
    JSClass<JSList>::StaticMethod("onItemDragLeave", &JSList::ItemDragLeaveCallback);
    JSClass<JSList>::StaticMethod("onItemDrop", &JSList::ItemDropCallback);
    JSClass<JSList>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSList>::Inherit<JSContainerBase>();
    JSClass<JSList>::Inherit<JSViewAbstract>();
    JSClass<JSList>::Bind(globalObj);
}

void JSList::SetDivider(const JSCallbackInfo& args)
{
    do {
        if (args.Length() < 1 || !args[0]->IsObject()) {
            LOGW("Invalid params");
            break;
        }

        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        Dimension strokeWidth;
        if (!ConvertFromJSValue(obj->GetProperty("strokeWidth"), strokeWidth) && strokeWidth.IsValid()) {
            LOGW("Invalid strokeWidth of divider");
            break;
        }

        auto divider = std::make_unique<V2::ItemDivider>();
        divider->strokeWidth = strokeWidth;
        if (!ConvertFromJSValue(obj->GetProperty("color"), divider->color)) {
            // Failed to get color from param, using default color defined in theme
            RefPtr<ListTheme> listTheme = GetTheme<ListTheme>();
            if (listTheme) {
                divider->color = listTheme->GetDividerColor();
            }
        }
        ConvertFromJSValue(obj->GetProperty("startMargin"), divider->startMargin);
        ConvertFromJSValue(obj->GetProperty("endMargin"), divider->endMargin);

        JSViewSetProperty(&V2::ListComponent::SetItemDivider, std::move(divider));
    } while (0);

    args.ReturnSelf();
}

void JSList::ScrollCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnScroll, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ReachStartCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnReachStart, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ReachEndCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnReachEnd, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ScrollStopCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnScrollStop, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ItemDeleteCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnItemDelete, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ItemMoveCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnItemMove, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ScrollIndexCallback(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&V2::ListComponent::SetOnScrollIndex, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSList::ItemDragStartCallback(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragStart event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragStartId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragFunc)](
                        const ItemDragInfo& dragInfo, int32_t itemIndex) -> RefPtr<Component> {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, nullptr);
        auto ret = func->ItemDragStartExecute(dragInfo, itemIndex);
        if (!ret->IsObject()) {
            LOGE("builder param is not an object.");
            return nullptr;
        }

        auto builderObj = JSRef<JSObject>::Cast(ret);
        auto builder = builderObj->GetProperty("builder");
        if (!builder->IsFunction()) {
            LOGE("builder param is not a function.");
            return nullptr;
        }
        auto builderFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(builder));
        if (!builderFunc) {
            LOGE("builder function is null.");
            return nullptr;
        }
        // use another VSP instance while executing the builder function
        ScopedViewStackProcessor builderViewStackProcessor;
        {
            ACE_SCORING_EVENT("List.onItemDragStart.builder");
            builderFunc->Execute();
        }
        RefPtr<Component> customComponent = ViewStackProcessor::GetInstance()->Finish();
        if (!customComponent) {
            LOGE("Custom component is null.");
            return nullptr;
        }
        return customComponent;
    };
    auto component = AceType::DynamicCast<V2::ListComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<V2::ListComponent>());
        return;
    }
    component->SetOnItemDragStartId(onItemDragStartId);
}

void JSList::ItemDragEnterCallback(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragEnter event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragEnterFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragEnterId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragEnterFunc)](
                                const ItemDragInfo& dragInfo) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("List.onItemDragEnter");
        func->ItemDragEnterExecute(dragInfo);
    };
    auto component = AceType::DynamicCast<V2::ListComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<V2::ListComponent>());
        return;
    }
    component->SetOnItemDragEnterId(onItemDragEnterId);
}

void JSList::ItemDragMoveCallback(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragMove event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragMoveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragMoveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragMoveFunc)](
                            const ItemDragInfo& dragInfo, int32_t itemIndex, int32_t insertIndex) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("List.onItemDragMove");
        func->ItemDragMoveExecute(dragInfo, itemIndex, insertIndex);
    };
    auto component = AceType::DynamicCast<V2::ListComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<V2::ListComponent>());
        return;
    }
    component->SetOnItemDragMoveId(onItemDragMoveId);
}

void JSList::ItemDragLeaveCallback(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragLeave event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragLeaveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragLeaveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragLeaveFunc)](
                                const ItemDragInfo& dragInfo, int32_t itemIndex) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("List.onItemDragLeave");
        func->ItemDragLeaveExecute(dragInfo, itemIndex);
    };
    auto component = AceType::DynamicCast<V2::ListComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<V2::ListComponent>());
        return;
    }
    component->SetOnItemDragLeaveId(onItemDragLeaveId);
}

void JSList::ItemDropCallback(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDrop event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDropFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDropId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDropFunc)](
                        const ItemDragInfo& dragInfo, int32_t itemIndex, int32_t insertIndex, bool isSuccess) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("List.onItemDrop");
        func->ItemDropExecute(dragInfo, itemIndex, insertIndex, isSuccess);
    };
    auto component = AceType::DynamicCast<V2::ListComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<V2::ListComponent>());
        return;
    }
    component->SetOnItemDropId(onItemDropId);
}

void JSList::SetMultiSelectable(bool multiSelectable)
{
    JSViewSetProperty(&V2::ListComponent::SetMultiSelectable, multiSelectable);
}

} // namespace OHOS::Ace::Framework
