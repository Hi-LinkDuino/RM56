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

#include "bridge/declarative_frontend/jsview/js_foreach.h"

#include <string>

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/functions/js_foreach_function.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/foreach/for_each_component.h"

namespace OHOS::Ace::Framework {

void JSForEach::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 4 || !info[2]->IsObject() || !info[3]->IsFunction() ||
        (!info[0]->IsNumber() && !info[0]->IsString()) || info[1]->IsUndefined() || !info[1]->IsObject()) {
        LOGE("invalid arguments for ForEach");
        return;
    }

    JSRef<JSObject> jsArray = JSRef<JSObject>::Cast(info[2]);
    JSRef<JSVal> jsViewMapperFunc = info[3];
    JSRef<JSVal> jsIdentityMapperFunc;
    RefPtr<JsForEachFunction> jsForEachFunction;
    if (info.Length() > 4 && info[4]->IsFunction()) {
        jsIdentityMapperFunc = info[4];
        jsForEachFunction = AceType::MakeRefPtr<JsForEachFunction>(
            jsArray, JSRef<JSFunc>::Cast(jsIdentityMapperFunc), JSRef<JSFunc>::Cast(jsViewMapperFunc));
    } else {
        jsForEachFunction = AceType::MakeRefPtr<JsForEachFunction>(jsArray, JSRef<JSFunc>::Cast(jsViewMapperFunc));
    }

    auto viewStack = ViewStackProcessor::GetInstance();
    std::string viewId = viewStack->ProcessViewId(info[0]->ToString());
    viewStack->Push(AceType::MakeRefPtr<ForEachComponent>(viewId, "ForEach"));

    std::vector<std::string> keys = jsForEachFunction->ExecuteIdentityMapper();
    for (size_t i = 0; i < keys.size(); i++) {
        keys[i].insert(0, "-");
        keys[i].insert(0, info[0]->ToString());
        viewStack->PushKey(keys[i]);

        viewStack->Push(AceType::MakeRefPtr<MultiComposedComponent>(viewStack->GetKey(), "ForEachItem"));
        jsForEachFunction->ExecuteBuilderForIndex(i);
        viewStack->PopContainer();

        viewStack->PopKey();
    }
}

void JSForEach::Pop()
{
    ViewStackProcessor::GetInstance()->PopContainer();
}

void JSForEach::JSBind(BindingTarget globalObj)
{
    JSClass<JSForEach>::Declare("ForEach");
    JSClass<JSForEach>::StaticMethod("create", &JSForEach::Create);
    JSClass<JSForEach>::StaticMethod("pop", &JSForEach::Pop);
    JSClass<JSForEach>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
