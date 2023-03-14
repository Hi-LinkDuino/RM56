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

#include "frameworks/bridge/declarative_frontend/jsview/js_row_split.h"

#include "base/log/ace_trace.h"
#include "core/components/split_container/row_split_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSRowSplit::Create()
{
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;
    auto rowSplit = AceType::MakeRefPtr<RowSplitComponent>(componentChildren);
    ViewStackProcessor::GetInstance()->Push(rowSplit);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetBoxClipFlag(true);
}

void JSRowSplit::JsResizeable(bool resizeable)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto rowSplit = AceType::DynamicCast<RowSplitComponent>(stack->GetMainComponent());
    if (rowSplit == nullptr) {
        LOGE("Get RowSplitComponent failed");
        return;
    }
    rowSplit->SetResizeable(resizeable);
}

void JSRowSplit::JSBind(BindingTarget globalObj)
{
    JSClass<JSRowSplit>::Declare("RowSplit");
    JSClass<JSRowSplit>::StaticMethod("create", &JSRowSplit::Create, MethodOptions::NONE);
    JSClass<JSRowSplit>::StaticMethod("resizeable", &JSRowSplit::JsResizeable, MethodOptions::NONE);
    JSClass<JSRowSplit>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSRowSplit>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSRowSplit>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSRowSplit>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSRowSplit>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSRowSplit>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSRowSplit>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSRowSplit>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSRowSplit>::Inherit<JSContainerBase>();
    JSClass<JSRowSplit>::Inherit<JSViewAbstract>();
    JSClass<JSRowSplit>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
