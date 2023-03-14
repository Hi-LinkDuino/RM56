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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_types.h"

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine_instance.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_ref.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

void PrintRefCount(JSValue val)
{
    if (JS_VALUE_HAS_REF_COUNT(val)) {
        JSRefCountHeader* p = static_cast<JSRefCountHeader*>(JS_VALUE_GET_PTR(val));
        LOGE("Ref count of value: %d", p->ref_count);
    } else {
        LOGE("no ref count");
    }
}

QJSValue::QJSValue()
{
    value_ = JS_UNDEFINED;
}

QJSValue::QJSValue(JSValue rhs)
{
    value_ = rhs;
}

QJSValue::~QJSValue() {}

QJSValue::QJSValue(const QJSValue& rhs)
{
    value_ = rhs.value_;
}

QJSValue::QJSValue(QJSValue&& rhs)
{
    value_ = std::move(rhs.value_);
    rhs.value_ = JS_UNDEFINED;
}

QJSValue& QJSValue::operator=(const QJSValue& rhs)
{
    value_ = rhs.value_;
    return *this;
}

QJSValue& QJSValue::operator=(QJSValue&& rhs)
{
    value_ = std::move(rhs.value_);
    rhs.value_ = JS_UNDEFINED;
    return *this;
}

QJSValue QJSValue::Dup() const
{
    return QJSValue(JS_DupValue(nullptr, value_));
}

void QJSValue::Free()
{
    JSRuntime* rt = QJSDeclarativeEngineInstance::GetQJSRuntime();
    if (rt == nullptr) {
        LOGE("rt is nullptr");
        return;
    }
    JS_FreeValueRT(rt, value_);
    value_ = JS_UNDEFINED;
}

void QJSValue::Reset()
{
    value_ = JS_UNDEFINED;
}

bool QJSValue::IsEmpty() const
{
    return JS_IsUndefined(value_) || JS_IsNull(value_);
}

bool QJSValue::IsFunction() const
{
    return JS_IsFunction(QJSContext::Current(), value_);
}

bool QJSValue::IsNumber() const
{
    return JS_IsNumber(value_);
}

bool QJSValue::IsString() const
{
    return JS_IsString(value_);
}

bool QJSValue::IsBoolean() const
{
    return JS_IsBool(value_);
}

bool QJSValue::IsObject() const
{
    return JS_IsObject(value_);
}

bool QJSValue::IsArray() const
{
    return JS_IsArray(QJSContext::Current(), value_);
}

bool QJSValue::IsUndefined() const
{
    return JS_IsUndefined(value_);
}

bool QJSValue::IsNull() const
{
    return JS_IsNull(value_);
}

std::string QJSValue::ToString() const
{
    if (!IsObject()) {
        return ScopedString(value_);
    }
    return ScopedString::Stringify(value_);
}

bool QJSValue::ToBoolean() const
{
    bool value = JS_ToBool(QJSContext::Current(), value_) == 1 ? true : false;
    return value;
}

JSContext* QJSValue::GetJsContext() const
{
    return QJSContext::Current();
}

QJSObject::QJSObject() : QJSValue() {}
QJSObject::QJSObject(JSValue val) : QJSValue(val) {}

QJSObject QJSObject::New()
{
    return QJSObject(JS_NewObject(QJSContext::Current()));
}

QJSRef<QJSArray> QJSObject::GetPropertyNames() const
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(QJSContext::Current(), GetHandle(), &pTab, &len)) {
        return QJSRef<QJSArray>::Make(QJSArray::New());
    }
    JS_GetOwnPropertyNames(QJSContext::Current(), &pTab, &len, GetHandle(), JS_GPN_STRING_MASK);
    JSValue value = JS_AtomToValue(QJSContext::Current(), pTab->atom);
    return QJSRef<QJSArray>::Make(value);
}

QJSRef<QJSValue> QJSObject::GetProperty(const char* prop) const
{
    return QJSRef<QJSValue>::Make(JS_GetPropertyStr(QJSContext::Current(), GetHandle(), prop));
}

QJSRef<QJSValue> QJSObject::ToJsonObject(const char* value) const
{
    JSContext* ctx = QJSContext::Current();
    JSValue jsonObj = JS_ParseJSON(ctx, value, strlen(value), "");
    return QJSRef<QJSValue>::Make(jsonObj);
}

void QJSObject::SetPropertyJsonObject(const char* prop, const char* value) const
{
    JSContext* ctx = QJSContext::Current();
    JSValue jsonObj = JS_ParseJSON(ctx, value, strlen(value), "");
    JS_SetPropertyStr(ctx, GetHandle(), prop, jsonObj);
}

void QJSObject::SetPropertyObject(const char* prop, QJSRef<QJSValue> value) const
{
    JS_SetPropertyStr(QJSContext::Current(), GetHandle(), prop, value->GetHandle());
}

JSValue QJSObjTemplate::New()
{
    return JS_NewObject(QJSContext::Current());
}

QJSObjTemplate::QJSObjTemplate(JSValue val) : QJSValue(val) {}

void QJSObjTemplate::SetInternalFieldCount(int32_t count) const {}

QJSRef<QJSObject> QJSObjTemplate::NewInstance() const
{
    return QJSRef<QJSObject>::Make(JS_NewObject(QJSContext::Current()));
}

QJSFunction::QJSFunction() {}
QJSFunction::QJSFunction(JSValue val) : QJSValue(val), ctx_(QJSContext::Current()) {}

QJSRef<QJSValue> QJSFunction::Call(QJSRef<QJSValue> thisVal, int argc, QJSRef<QJSValue> argv[]) const
{
    std::vector<JSValue> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i].Get().GetHandle());
    }
    QJSContext::Scope scp(ctx_);
    JSValue res = JS_Call(QJSContext::Current(), GetHandle(), thisVal.Get().GetHandle(), argc, args.data());
    if (JS_IsException(res)) {
        QJSUtils::JsStdDumpErrorAce(QJSContext::Current());
        JS_FreeValue(QJSContext::Current(), res);
        return QJSRef<QJSValue>::Make(JS_NULL);
    }
    js_std_loop(ctx_);
    return QJSRef<QJSValue>::Make(res);
}

JSValue QJSFunction::New(FunctionCallback func)
{
    return JS_UNDEFINED;
}

QJSArray::QJSArray() {}
QJSArray::QJSArray(JSValue val) : QJSValue(val) {}

QJSArray QJSArray::New()
{
    return QJSArray(JS_NewArray(QJSContext::Current()));
}

QJSRef<QJSValue> QJSArray::GetValueAt(size_t index) const
{
    JSContext* ctx = QJSContext::Current();
    JSValue val = JS_GetPropertyUint32(ctx, GetHandle(), index);
    return QJSRef<QJSValue>::Make(val);
}

void QJSArray::SetValueAt(size_t index, QJSRef<QJSValue> value) const
{
    JSContext* ctx = QJSContext::Current();
    JS_SetPropertyUint32(ctx, GetHandle(), index, value.Get().GetHandle());
}

size_t QJSArray::Length() const
{
    return QJSUtils::JsGetArrayLength(QJSContext::Current(), GetHandle());
}

QJSCallbackInfo::QJSCallbackInfo(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv)
    : ctx_(ctx), thisObj_(thisObj), argc_(argc), argv_(argv)
{}

QJSRef<QJSValue> QJSCallbackInfo::operator[](size_t index) const
{
    QJSRef<QJSValue> val = QJSRef<QJSValue>::Make(argv_[index]);
    return val;
}

QJSRef<QJSObject> QJSCallbackInfo::This() const
{
    return QJSRef<QJSObject>::Make(thisObj_);
}

int QJSCallbackInfo::Length() const
{
    return argc_;
}

void QJSCallbackInfo::ReturnSelf() const
{
    retVal_ = JS_DupValue(nullptr, thisObj_);
}

QJSGCMarkCallbackInfo::QJSGCMarkCallbackInfo(JSRuntime* rt, JS_MarkFunc* markFunc) : rt_(rt), markFunc_(markFunc) {}

void QJSGCMarkCallbackInfo::Mark(JSValue val) const
{
    JS_MarkValue(rt_, val, markFunc_);
}

} // namespace OHOS::Ace::Framework
