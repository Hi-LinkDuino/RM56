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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_mouse_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_grid_item.h"

#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSGridItem::Create()
{
    auto itemComponent = AceType::MakeRefPtr<GridLayoutItemComponent>();
    ViewStackProcessor::GetInstance()->Push(itemComponent);
}

void JSGridItem::SetColumnStart(int32_t columnStart)
{
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (gridItem) {
        gridItem->SetColumnIndex(columnStart);
    }
}

void JSGridItem::SetColumnEnd(int32_t columnEnd)
{
    // column end must be set after start. loader needs to make the method in order.
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (gridItem) {
        gridItem->SetColumnSpan(columnEnd - gridItem->GetColumnIndex() + 1);
    }
}

void JSGridItem::SetRowStart(int32_t rowStart)
{
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (gridItem) {
        gridItem->SetRowIndex(rowStart);
    }
}

void JSGridItem::SetRowEnd(int32_t rowEnd)
{
    // row end must be set after start. loader needs to make the method in order.
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (gridItem) {
        gridItem->SetRowSpan(rowEnd - gridItem->GetRowIndex() + 1);
    }
}

void JSGridItem::ForceRebuild(bool forceRebuild)
{
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (gridItem) {
        gridItem->SetForceRebuild(forceRebuild);
    }
}

void JSGridItem::JSBind(BindingTarget globalObj)
{
    LOGD("GridItem:JSBind");
    JSClass<JSGridItem>::Declare("GridItem");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSGridItem>::StaticMethod("create", &JSGridItem::Create, opt);
    JSClass<JSGridItem>::StaticMethod("columnStart", &JSGridItem::SetColumnStart, opt);
    JSClass<JSGridItem>::StaticMethod("columnEnd", &JSGridItem::SetColumnEnd, opt);
    JSClass<JSGridItem>::StaticMethod("rowStart", &JSGridItem::SetRowStart, opt);
    JSClass<JSGridItem>::StaticMethod("rowEnd", &JSGridItem::SetRowEnd, opt);
    JSClass<JSGridItem>::StaticMethod("forceRebuild", &JSGridItem::ForceRebuild, opt);
    JSClass<JSGridItem>::StaticMethod("selectable", &JSGridItem::SetSelectable, opt);
    JSClass<JSGridItem>::StaticMethod("onSelect", &JSGridItem::SelectCallback);
    JSClass<JSGridItem>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSGridItem>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSGridItem>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSGridItem>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSGridItem>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSGridItem>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSGridItem>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSGridItem>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSGridItem>::Inherit<JSContainerBase>();
    JSClass<JSGridItem>::Inherit<JSViewAbstract>();
    JSClass<JSGridItem>::Bind<>(globalObj);
}

void JSGridItem::SetSelectable(bool selectable)
{
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (gridItem) {
        gridItem->SetSelectable(selectable);
    }
}

void JSGridItem::SelectCallback(const JSCallbackInfo& args)
{
    if (!args[0]->IsFunction()) {
        LOGE("fail to bind onSelect event due to info is not function");
        return;
    }

    RefPtr<JsMouseFunction> jsOnSelectFunc = AceType::MakeRefPtr<JsMouseFunction>(JSRef<JSFunc>::Cast(args[0]));
    auto onSelectId = [execCtx = args.GetExecutionContext(), func = std::move(jsOnSelectFunc)](bool isSelected) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        func->SelectExecute(isSelected);
    };
    auto gridItem =
        AceType::DynamicCast<GridLayoutItemComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!gridItem) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<GridLayoutItemComponent>());
        return;
    }
    gridItem->SetOnSelectId(onSelectId);
}

} // namespace OHOS::Ace::Framework
