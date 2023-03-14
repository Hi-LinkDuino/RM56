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

#include "frameworks/bridge/declarative_frontend/jsview/js_blank.h"

#include "core/components/box/box_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
void JSBlank::Create(const JSCallbackInfo& info)
{
    auto specializedBox = AceType::MakeRefPtr<OHOS::Ace::BoxComponent>();
    // specialized component should be firstly pushed.
    ViewStackProcessor::GetInstance()->Push(specializedBox);
    // Blank fill the remain space.
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flexItem->SetFlexGrow(1);
    flexItem->SetFlexShrink(0);
    flexItem->SetMustStretch(true);

    Dimension flexBasis;
    if (info.Length() >= 1 && ParseJsDimensionVp(info[0], flexBasis)) {
        flexItem->SetFlexBasis(flexBasis);
    }
}

void JSBlank::JSBind(BindingTarget globalObj)
{
    JSClass<JSBlank>::Declare("Blank");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSBlank>::StaticMethod("create", &JSBlank::Create, opt);
    JSClass<JSBlank>::StaticMethod("color", &JSViewAbstract::JsBackgroundColor, opt);
    JSClass<JSBlank>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSBlank>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSBlank>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSBlank>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSBlank>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSBlank>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSBlank>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSBlank>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSBlank>::Inherit<JSViewAbstract>();
    JSClass<JSBlank>::Bind<>(globalObj);
}
} // namespace OHOS::Ace::Framework
