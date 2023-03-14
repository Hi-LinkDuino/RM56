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

#include "frameworks/bridge/declarative_frontend/jsview/js_column_split.h"

#include "base/log/ace_trace.h"
#include "core/components/split_container/column_split_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSColumnSplit::Create()
{
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;
    auto columnSplit = AceType::MakeRefPtr<ColumnSplitComponent>(componentChildren);
    ViewStackProcessor::GetInstance()->Push(columnSplit);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetBoxClipFlag(true);
}

void JSColumnSplit::JsResizeable(bool resizeable)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto columnSplit = AceType::DynamicCast<ColumnSplitComponent>(stack->GetMainComponent());
    if (columnSplit == nullptr) {
        LOGE("Get ColumnSplitComponent failed");
        return;
    }
    columnSplit->SetResizeable(resizeable);
}

void JSColumnSplit::JSBind(BindingTarget globalObj)
{
    JSClass<JSColumnSplit>::Declare("ColumnSplit");
    JSClass<JSColumnSplit>::StaticMethod("create", &JSColumnSplit::Create, MethodOptions::NONE);
    JSClass<JSColumnSplit>::StaticMethod("resizeable", &JSColumnSplit::JsResizeable, MethodOptions::NONE);
    JSClass<JSColumnSplit>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSColumnSplit>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSColumnSplit>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSColumnSplit>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSColumnSplit>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSColumnSplit>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSColumnSplit>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSColumnSplit>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSColumnSplit>::Inherit<JSContainerBase>();
    JSClass<JSColumnSplit>::Inherit<JSViewAbstract>();
    JSClass<JSColumnSplit>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
