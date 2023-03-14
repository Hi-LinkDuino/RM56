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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_types.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_ref.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_value_conversions.h"

namespace OHOS::Ace::Framework {

V8Value::V8Value() : V8Type(v8::Undefined(v8::Isolate::GetCurrent())) {}

V8Value::V8Value(v8::Local<v8::Value> val) : V8Type(val) {}

bool V8Value::IsFunction() const
{
    return GetHandle()->IsFunction();
}
bool V8Value::IsNumber() const
{
    return GetHandle()->IsNumber();
}
bool V8Value::IsString() const
{
    return GetHandle()->IsString();
}
bool V8Value::IsBoolean() const
{
    return GetHandle()->IsBoolean();
}
bool V8Value::IsObject() const
{
    return GetHandle()->IsObject();
}
bool V8Value::IsArray() const
{
    return GetHandle()->IsArray();
}
bool V8Value::IsUndefined() const
{
    return GetHandle()->IsUndefined();
}

bool V8Value::IsNull() const
{
    return GetHandle()->IsNull();
}

std::string V8Value::ToString() const
{
    if (!IsObject()) {
        return V8Utils::ScopedString(GetHandle());
    }

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    return V8Utils::ScopedString(v8::JSON::Stringify(context, GetHandle()).ToLocalChecked());
}

bool V8Value::ToBoolean() const
{
    return GetHandle()->BooleanValue(v8::Isolate::GetCurrent());
}

V8Ref<V8Array> V8Object::GetPropertyNames() const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Array> val = GetHandle()->GetPropertyNames(context).ToLocalChecked();
    V8Ref<V8Array> result = V8Ref<V8Array>::Make(val);
    return result;
}

V8Ref<V8Value> V8Object::GetProperty(const char* prop) const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> val =
        GetHandle()->Get(context, V8ValueConvertor::toV8Value<std::string>(prop)).ToLocalChecked();
    V8Ref<V8Value> result = V8Ref<V8Value>::Make(val);
    return result;
}

void V8Object::SetPropertyJsonObject(const char* prop, const char* value) const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::String> objStr = v8::String::NewFromUtf8(isolate, value).ToLocalChecked();
    v8::Local<v8::Value> obj = v8::JSON::Parse(context, objStr).ToLocalChecked();
    GetHandle()->Set(context, V8ValueConvertor::toV8Value<std::string>(prop), obj).ToChecked();
}

void V8Object::SetPropertyObject(const char* prop, V8Ref<V8Value> value) const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    GetHandle()->Set(context, V8ValueConvertor::toV8Value<std::string>(prop), value->GetHandle()).ToChecked();
}

V8Object::V8Object() : V8Type() {}
V8Object::V8Object(v8::Local<v8::Object> obj) : V8Type(obj) {}

V8Funktion::V8Funktion() {}
V8Funktion::V8Funktion(v8::Local<v8::Function> obj) : V8Type(obj)
{
    isolate_ = v8::Isolate::GetCurrent();
    ctx_.Reset(isolate_, isolate_->GetCurrentContext());
}

V8Ref<V8Value> V8Funktion::Call(V8Ref<V8Value> thisVal, int argc, V8Ref<V8Value> argv[]) const
{
    std::vector<v8::Local<v8::Value>> localArgv;
    v8::Isolate::Scope iScp(isolate_);
    v8::HandleScope scp(isolate_);
    auto context = ctx_.Get(isolate_);
    v8::Context::Scope ctxScp(context);

    for (int i = 0; i < argc; ++i) {
        localArgv.push_back(argv[i].Get().GetHandle());
    }

    v8::TryCatch tryCatch(isolate_);
    v8::Local<v8::Value> result;
    v8::Local<v8::Value> jsThis = v8::Undefined(isolate_);
    if (!thisVal.IsEmpty()) {
        jsThis = thisVal.Get().GetHandle();
    }

    bool success = GetHandle()->Call(context, jsThis, argc, localArgv.data()).ToLocal(&result);
    if (!success) {
        V8Utils::JsStdDumpErrorAce(isolate_, &tryCatch);
    }

    if (result.IsEmpty()) {
        return V8Ref<V8Value>();
    }

    return V8Ref<V8Value>::Make(result);
}

v8::Local<v8::Function> V8Funktion::New(FunctionCallback func)
{
    auto isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    return v8::Function::New(context, func).ToLocalChecked();
}

V8Array::V8Array() {}
V8Array::V8Array(v8::Local<v8::Array> arr) : V8Type(arr) {}

V8Ref<V8Value> V8Array::GetValueAt(size_t index) const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    v8::MaybeLocal<v8::Value> val = GetHandle()->Get(context, index);
    if (val.IsEmpty()) {
        return V8Ref<V8Value>();
    }

    return V8Ref<V8Value>::Make(val.ToLocalChecked());
}

void V8Array::SetValueAt(size_t index, V8Ref<V8Value> value) const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    GetHandle()->Set(context, index, value->GetHandle()).ToChecked();
}

size_t V8Array::Length() const
{
    return GetHandle()->Length();
}

V8CallbackInfo::V8CallbackInfo(const v8::FunctionCallbackInfo<v8::Value>& info) : info_(info)
{
    isolate_ = info.GetIsolate();
    ctx_.Reset(isolate_, isolate_->GetCurrentContext());
    ctx_.SetWeak();
}

V8Ref<V8Value> V8CallbackInfo::operator[](size_t index) const
{
    return V8Ref<V8Value>::Make(info_[index]);
}

V8Ref<V8Object> V8CallbackInfo::This() const
{
    return V8Ref<V8Object>::Make(info_.This());
}

void V8CallbackInfo::ReturnSelf() const
{
    return info_.GetReturnValue().Set(info_.This());
}

int V8CallbackInfo::Length() const
{
    return info_.Length();
}

void V8ObjTemplate::SetInternalFieldCount(int count) const
{
    GetHandle()->SetInternalFieldCount(count);
}

V8Ref<V8Object> V8ObjTemplate::NewInstance() const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    v8::MaybeLocal<v8::Object> val = GetHandle()->NewInstance(context);
    if (val.IsEmpty()) {
        return V8Ref<V8Object>();
    }
    return V8Ref<V8Object>::Make(val.ToLocalChecked());
}

v8::Local<v8::ObjectTemplate> V8ObjTemplate::New()
{
    auto isolate = v8::Isolate::GetCurrent();
    return v8::ObjectTemplate::New(isolate);
}

} // namespace OHOS::Ace::Framework