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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_FUNCTION_H

#include <functional>

#include "third_party/v8/include/v8.h"

#include "base/memory/ace_type.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT V8Function : public virtual AceType {
    DECLARE_ACE_TYPE(V8Function, AceType);

public:
    void execute();
    void execute(std::vector<std::string> keys, const std::string& param);

protected:
    v8::Persistent<v8::Function> jsFunction_;
    v8::Persistent<v8::Value> jsThis_;
    v8::Persistent<v8::Context> ctx_;
    v8::Isolate* isolate_;

    v8::Local<v8::Value> executeJS(int argc = 0, v8::Local<v8::Value>* argv = nullptr);

public:
    V8Function(v8::Local<v8::Value> jsObject, v8::Local<v8::Function> jsRenderFunction);
    ~V8Function();
};

template<class T, int32_t ARGC = 0>
class ACE_EXPORT V8EventFunction : public V8Function {
    DECLARE_ACE_TYPE(V8EventFunction, V8Function);

public:
    using ParseFunc = std::function<v8::Local<v8::Value>(const T&, v8::Isolate*)>;
    V8EventFunction() = delete;
    V8EventFunction(v8::Local<v8::Function> jsFunction, ParseFunc parser)
        : V8Function(v8::Undefined(v8::Isolate::GetCurrent()), jsFunction), parser_(parser)
    {}
    ~V8EventFunction() = default;

    void execute()
    {
        ACE_DCHECK(isolate_);
        v8::Isolate::Scope isolateScope(isolate_);
        v8::HandleScope handleScope(isolate_);
        auto context = ctx_.Get(isolate_);
        v8::Context::Scope contextScope(context);
        V8Function::executeJS();
    }

    void execute(const T& eventInfo)
    {
        ACE_DCHECK(isolate_);
        v8::Isolate::Scope isolateScope(isolate_);
        v8::HandleScope handleScope(isolate_);
        auto context = ctx_.Get(isolate_);
        v8::Context::Scope contextScope(context);
        v8::Local<v8::Value> v8Param;
        if (parser_) {
            v8Param = parser_(eventInfo, isolate_);
        }
        V8Function::executeJS(ARGC, &v8Param);
    }

private:
    ParseFunc parser_;
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_FUNCTION_H