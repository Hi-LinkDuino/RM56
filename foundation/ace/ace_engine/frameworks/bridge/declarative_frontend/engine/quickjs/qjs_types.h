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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_TYPES_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_TYPES_H

#include <string>
#include <variant>

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_fwd.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_value_conversions.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/quickjs-libc.h"

#ifdef __cplusplus
}
#endif // __cplusplus

#define FAKE_PTR_FOR_FUNCTION_ACCESS(klass) \
    const klass* operator->() const         \
    {                                       \
        return this;                        \
    }

#define ENABLE_CAST_FROM_THIS(klass)         \
    static klass Cast(const QJSValue& other) \
    {                                        \
        return klass(other.GetHandle());     \
    }

namespace OHOS::Ace::Framework {

using FunctionCallback = JSValue (*)(JSContext*, JSValueConst, int, JSValueConst*);

class QJSValue {
public:
    QJSValue();
    explicit QJSValue(JSValue rhs);
    virtual ~QJSValue();
    QJSValue(const QJSValue& rhs);
    QJSValue(QJSValue&& rhs);
    QJSValue& operator=(const QJSValue& rhs);
    QJSValue& operator=(QJSValue&& rhs);
    // operator JSValue() const;
    QJSValue Dup() const;
    void Free();
    void Reset();
    bool IsEmpty() const;
    bool IsFunction() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsBoolean() const;
    bool IsArray() const;
    bool IsObject() const;
    bool IsUndefined() const;
    bool IsNull() const;
    std::string ToString() const;
    bool ToBoolean() const;
    JSContext* GetJsContext() const;

    template<typename T>
    T ToNumber() const;

    JSValue GetHandle() const
    {
        return value_;
    }

    FAKE_PTR_FOR_FUNCTION_ACCESS(QJSValue)

private:
    JSValue value_;
};

class QJSArray : public QJSValue {
public:
    QJSArray();
    explicit QJSArray(JSValue arr);
    ~QJSArray() override = default;
    QJSRef<QJSValue> GetValueAt(size_t index) const;
    void SetValueAt(size_t index, QJSRef<QJSValue> value) const;
    size_t Length() const;
    FAKE_PTR_FOR_FUNCTION_ACCESS(QJSArray)
    ENABLE_CAST_FROM_THIS(QJSArray)

    static QJSArray New();
};

class QJSObject : public QJSValue {
public:
    QJSObject();
    explicit QJSObject(JSValue val);
    ~QJSObject() override = default;

    template<typename U>
    U* Unwrap() const;

    template<typename U>
    void Wrap(U* data) const;

    template<typename T>
    void SetProperty(const char* prop, const T value) const;
    void SetPropertyJsonObject(const char* prop, const char* value) const;
    void SetPropertyObject(const char* prop, QJSRef<QJSValue> value) const;
    QJSRef<QJSArray> GetPropertyNames() const;
    QJSRef<QJSValue> GetProperty(const char* prop) const;
    QJSRef<QJSValue> ToJsonObject(const char* value) const;

    FAKE_PTR_FOR_FUNCTION_ACCESS(QJSObject)
    ENABLE_CAST_FROM_THIS(QJSObject)

    static QJSObject New();
};

class QJSObjTemplate : public QJSValue {
public:
    QJSObjTemplate() = default;
    explicit QJSObjTemplate(JSValue val);

    void SetInternalFieldCount(int32_t count) const;
    QJSRef<QJSObject> NewInstance() const;
    static JSValue New();

    FAKE_PTR_FOR_FUNCTION_ACCESS(QJSObjTemplate)
};

class QJSFunction : public QJSValue {
public:
    QJSFunction();
    explicit QJSFunction(JSValue val);
    ~QJSFunction() override = default;

    QJSRef<QJSValue> Call(QJSRef<QJSValue> thisVal, int argc = 0, QJSRef<QJSValue> argv[] = nullptr) const;
    static JSValue New(FunctionCallback func);

    FAKE_PTR_FOR_FUNCTION_ACCESS(QJSFunction)
    ENABLE_CAST_FROM_THIS(QJSFunction)
private:
    JSContext* ctx_ = nullptr;
};

struct QJSExecutionContext {
    JSContext* context;
};

class QJSCallbackInfo {
public:
    QJSCallbackInfo(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv);
    ~QJSCallbackInfo() = default;
    QJSCallbackInfo(const QJSCallbackInfo&) = delete;
    QJSCallbackInfo& operator=(const QJSCallbackInfo&) = delete;

    QJSRef<QJSValue> operator[](size_t index) const;
    QJSRef<QJSObject> This() const;
    int Length() const;

    template<typename T>
    void SetReturnValue(T* instance) const;

    template<typename T>
    void SetReturnValue(QJSRef<T> existing) const;

    void ReturnSelf() const;

    std::variant<void*, JSValue> GetReturnValue()
    {
        return retVal_;
    }

    QJSExecutionContext GetExecutionContext() const
    {
        return QJSExecutionContext { ctx_ };
    }

private:
    JSContext* ctx_;
    JSValueConst thisObj_;
    int argc_;
    JSValueConst* argv_;

    mutable std::variant<void*, JSValue> retVal_;
};

class QJSGCMarkCallbackInfo {
public:
    QJSGCMarkCallbackInfo(JSRuntime* rt, JS_MarkFunc* markFunc);
    ~QJSGCMarkCallbackInfo() = default;

    QJSGCMarkCallbackInfo(const QJSGCMarkCallbackInfo&) = delete;
    QJSGCMarkCallbackInfo& operator=(const QJSGCMarkCallbackInfo&) = delete;

    template<typename T>
    void Mark(const QJSRef<T>& val) const;
    void Mark(JSValue val) const;

private:
    JSRuntime* rt_;
    JS_MarkFunc* markFunc_;
};

class QJSException {
public:
    template<typename... Args>
    static void Throw(const char* format, Args... args);
    template<typename... Args>
    static void ThrowRangeError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowReferenceError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowSyntaxError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowTypeError(const char* format, Args... args);
};

} // namespace OHOS::Ace::Framework

#include "qjs_types.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_TYPES_H
