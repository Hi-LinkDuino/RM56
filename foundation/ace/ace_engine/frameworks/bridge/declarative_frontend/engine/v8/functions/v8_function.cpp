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

#include "frameworks/bridge/declarative_frontend/engine/v8/functions/v8_function.h"

#include "base/json/json_util.h"
#include "base/log/ace_trace.h"
#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"

namespace OHOS::Ace::Framework {

V8Function::V8Function(v8::Local<v8::Value> jsObject, v8::Local<v8::Function> jsFunction)
{
    isolate_ = v8::Isolate::GetCurrent();
    ctx_.Reset(isolate_, isolate_->GetCurrentContext());
    jsThis_.Reset(isolate_, jsObject);
    jsFunction_.Reset(isolate_, jsFunction);
}

V8Function::~V8Function()
{
    LOGD("Destroy: V8Function");
    ctx_.Reset();
    jsThis_.Reset();
    jsFunction_.Reset();
}

void V8Function::execute()
{
    ACE_DCHECK(isolate_);
    v8::Isolate::Scope isolateScope(isolate_);
    v8::HandleScope handleScope(isolate_);
    auto context = ctx_.Get(isolate_);
    v8::Context::Scope contextScope(context);
    V8Function::executeJS();
}

void V8Function::execute(std::vector<std::string> keys, const std::string& param)
{
    ACE_DCHECK(isolate_);
    v8::Isolate::Scope isolateScope(isolate_);
    v8::HandleScope handleScope(isolate_);
    auto context = ctx_.Get(isolate_);
    v8::Context::Scope contextScope(context);
    LOGI("param : %{public}s", param.c_str());
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(param);
    if (!argsPtr) {
        LOGW("Parse param failed!");
        return;
    }
    v8::Local<v8::Object> eventInfo = v8::Object::New(isolate_);
    for (auto iter = keys.begin(); iter != keys.end(); iter++) {
        const std::string key = *iter;
        const auto value = argsPtr->GetValue(key);
        if (!value) {
            LOGI("key[%{public}s] is not exist.", key.c_str());
            continue;
        }

        if (value->IsString()) {
            eventInfo
                ->Set(context, v8::String::NewFromUtf8(isolate_, key.c_str()).ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate_, value->GetString().c_str()).ToLocalChecked())
                .ToChecked();
        } else if (value->IsNumber()) {
            eventInfo
                ->Set(context, v8::String::NewFromUtf8(isolate_, key.c_str()).ToLocalChecked(),
                    v8::Number::New(isolate_, value->GetDouble()))
                .ToChecked();
        } else if (value->IsObject()) {
            v8::Local<v8::String> objStr =
                v8::String::NewFromUtf8(isolate_, value->ToString().c_str()).ToLocalChecked();
            v8::Local<v8::Value> obj = v8::JSON::Parse(context, objStr).ToLocalChecked();
            eventInfo->Set(context, v8::String::NewFromUtf8(isolate_, key.c_str()).ToLocalChecked(), obj).ToChecked();
        }
    }

    v8::Local<v8::Value> v8Param = eventInfo;
    V8Function::executeJS(1, &v8Param);
}

v8::Local<v8::Value> V8Function::executeJS(int argc, v8::Local<v8::Value>* argv)
{
    ACE_DCHECK(isolate_);
    ACE_FUNCTION_TRACE();

    auto context = isolate_->GetCurrentContext();

    LOGD("JS_CALL: %s", V8Utils::ScopedString(jsFunction_.Get(isolate_)).get());

    v8::TryCatch tryCatch(isolate_);
    v8::Local<v8::Value> result;
    bool success = jsFunction_.Get(isolate_)->Call(context, jsThis_.Get(isolate_), argc, argv).ToLocal(&result);

    if (!success) {
        V8Utils::JsStdDumpErrorAce(isolate_, &tryCatch);
        return v8::Undefined(isolate_);
    }

    return result;
}

} // namespace OHOS::Ace::Framework