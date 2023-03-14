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

#include "bridge/declarative_frontend/interfaces/profiler/js_profiler.h"

#include <memory>

#include "bridge/declarative_frontend/engine/bindings.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "bridge/declarative_frontend/engine/js_types.h"
#include "core/common/container.h"

namespace OHOS::Ace::Framework {

void JSProfiler::JSBind(BindingTarget globalObj)
{
    JSClass<JSProfiler>::Declare("Profiler");
    JSClass<JSProfiler>::StaticMethod("registerVsyncCallback", &JSProfiler::JsRegisterVsyncCallback);
    JSClass<JSProfiler>::StaticMethod("unregisterVsyncCallback", &JSProfiler::JsUnregisterVsyncCallback);
    JSClass<JSProfiler>::Bind<>(globalObj);
}

void JSProfiler::JsRegisterVsyncCallback(const JSCallbackInfo& args)
{
    if (!args[0]->IsFunction()) {
        LOGE("fail to register callback due to args is not function");
        return;
    }
    auto container = Container::Current();
    auto pipelineContext = container ? container->GetPipelineContext() : nullptr;
    if (!pipelineContext) {
        LOGE("fail to register callback due to pipeline is null");
        return;
    }
    auto onVsyncJsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(args[0]));
    auto onVsyncFunc = [execCtx = args.GetExecutionContext(), func = std::move(onVsyncJsFunc)](
                           const std::string& value) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        JSRef<JSVal> params[1];
        params[0] = JSRef<JSVal>::Make(ToJSValue(value));
        func->ExecuteJS(1, params);
    };
    pipelineContext->SetOnVsyncProfiler(onVsyncFunc);
}

void JSProfiler::JsUnregisterVsyncCallback()
{
    auto container = Container::Current();
    auto pipelineContext = container ? container->GetPipelineContext() : nullptr;
    if (!pipelineContext) {
        LOGE("fail to unregister callback due to pipeline is null");
        return;
    }
    pipelineContext->ResetOnVsyncProfiler();
}

} // namespace OHOS::Ace::Framework
