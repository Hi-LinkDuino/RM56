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

#include "frameworks/bridge/declarative_frontend/jsview/js_sheet.h"

#include "core/components/sheet/sheet_component.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSSheet::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    RefPtr<SheetComponent> sheetComponent =
        AceType::MakeRefPtr<OHOS::Ace::SheetComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, children);
    ViewStackProcessor::GetInstance()->Push(sheetComponent);
}

void JSSheet::JSBind(BindingTarget globalObj)
{
    JSClass<JSSheet>::Declare("Sheet");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSheet>::StaticMethod("create", &JSSheet::Create, opt);
    JSClass<JSSheet>::Inherit<JSViewAbstract>();
    JSClass<JSSheet>::Bind(globalObj);
}

} // namespace OHOS::Ace::Framework
