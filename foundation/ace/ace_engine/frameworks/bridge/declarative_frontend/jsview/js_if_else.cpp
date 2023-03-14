/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/jsview/js_if_else.h"

#include "base/log/ace_trace.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/ifelse/if_else_component.h"

namespace OHOS::Ace::Framework {

void JSIfElse::Create(const JSCallbackInfo& info)
{
    auto component = AceType::MakeRefPtr<OHOS::Ace::IfElseComponent>(std::string(), "IfElse");
    ViewStackProcessor::GetInstance()->Push(component);
}

void JSIfElse::Pop()
{
    ViewStackProcessor::GetInstance()->PopContainer();
}

void JSIfElse::SetBranchId(int32_t value)
{
    if (value < 0) {
        LOGE("IfElse: branchId(%{public}d) expected positive number", value);
        return;
    }

    auto component = AceType::DynamicCast<IfElseComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    component->SetBranchId(value);
}

void JSIfElse::JSBind(BindingTarget globalObj)
{
    JSClass<JSIfElse>::Declare("If");
    JSClass<JSIfElse>::StaticMethod("create", &JSIfElse::Create);
    JSClass<JSIfElse>::StaticMethod("pop", &JSIfElse::Pop);
    JSClass<JSIfElse>::StaticMethod("branchId", &JSIfElse::SetBranchId);
    JSClass<JSIfElse>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
