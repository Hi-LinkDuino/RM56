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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_TYPES_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_TYPES_H

#include "v8.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_fwd.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_value_conversions.h"

#define FAKE_PTR_FOR_FUNCTION_ACCESS(klass) \
    const klass* operator->() const         \
    {                                       \
        return this;                        \
    }

namespace OHOS::Ace::Framework {

template<typename T>
using CopyablePersistent = v8::Persistent<T, v8::CopyablePersistentTraits<T>>;
using FunctionCallback = void (*)(const v8::FunctionCallbackInfo<v8::Value>&);

/**
 * @brief A wrapper around a copyable-persistent V8 handle, inherited by the rest of the specialized wrappers
 *
 * @tparam T A v8 type (v8::Value, v8::Object etc.)
 */
template<typename T>
class V8Type {
public:
    V8Type();
    V8Type(const V8Type& rhs);
    V8Type(V8Type&& rhs);
    virtual ~V8Type();

    explicit V8Type(v8::Local<T> val);
    explicit V8Type(CopyablePersistent<T> other);

    template<typename S>
    explicit V8Type(v8::Local<S> val);

    V8Type& operator=(const V8Type& rhs);
    V8Type& operator=(V8Type&& rhs);

    template<typename S>
    static V8Type<T> Cast(const V8Type<S>& that)
    {
        return V8Type<T>(that.GetHandle());
    }

    void SetWeak();
    v8::Local<T> GetHandle() const;
    v8::Local<T> operator->() const;
    bool IsEmpty() const;
    bool IsWeak() const;
    void Reset();
    operator v8::Local<T>() const;

    static V8Type<T> New();

private:
    CopyablePersistent<T> handle_;
};

/**
 * @brief A wrapper around v8::Value
 *
 */
class V8Value : public V8Type<v8::Value> {
public:
    V8Value();
    explicit V8Value(v8::Local<v8::Value> val);
    ~V8Value() override = default;

    bool IsFunction() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsBoolean() const;
    bool IsObject() const;
    bool IsArray() const;
    bool IsUndefined() const;
    bool IsNull() const;
    std::string ToString() const;
    bool ToBoolean() const;

    template<typename T>
    T ToNumber() const;

    FAKE_PTR_FOR_FUNCTION_ACCESS(V8Value)
};

/**
 * @brief A wrapper around a v8::Array
 *
 */
class V8Array : public V8Type<v8::Array> {
public:
    V8Array();
    explicit V8Array(v8::Local<v8::Array> arr);
    ~V8Array() override = default;
    V8Ref<V8Value> GetValueAt(size_t index) const;
    void SetValueAt(size_t index, V8Ref<V8Value> value) const;
    size_t Length() const;
    FAKE_PTR_FOR_FUNCTION_ACCESS(V8Array)
};

/**
 * @brief A wrapper around v8::Object
 *
 */
class V8Object : public V8Type<v8::Object> {
public:
    V8Object();
    ~V8Object() override = default;
    enum InternalFieldIndex { INSTANCE = 0 };

    FAKE_PTR_FOR_FUNCTION_ACCESS(V8Object)

    template<typename T>
    T* Unwrap() const;

    template<typename T>
    void Wrap(T* data) const;

    V8Ref<V8Array> GetPropertyNames() const;
    V8Ref<V8Value> GetProperty(const char* prop) const;

    template<typename T>
    void SetProperty(const char* prop, const T value) const;
    void SetPropertyJsonObject(const char* prop, const char* value) const;
    void SetPropertyObject(const char* prop, V8Ref<V8Value> value) const;

    explicit V8Object(v8::Local<v8::Object> obj);
};

/**
 * @brief A wrapper around v8::Function
 *
 */
class V8Funktion : public V8Type<v8::Function> {
public:
    V8Funktion();
    explicit V8Funktion(v8::Local<v8::Function> obj);
    ~V8Funktion() override = default;

    V8Ref<V8Value> Call(V8Ref<V8Value> thisVal, int argc = 0, V8Ref<V8Value> argv[] = nullptr) const;
    static v8::Local<v8::Function> New(FunctionCallback func);

    FAKE_PTR_FOR_FUNCTION_ACCESS(V8Funktion)

private:
    v8::Isolate* isolate_ = nullptr;
    CopyablePersistent<v8::Context> ctx_;
};

class V8ObjTemplate : public V8Type<v8::ObjectTemplate> {
public:
    V8ObjTemplate() = default;
    explicit V8ObjTemplate(v8::Local<v8::ObjectTemplate> obj) : V8Type(obj) {}

    void SetInternalFieldCount(int32_t count) const;
    V8Ref<V8Object> NewInstance() const;
    static v8::Local<v8::ObjectTemplate> New();

    FAKE_PTR_FOR_FUNCTION_ACCESS(V8ObjTemplate)
};

struct V8ExecutionContext {
    v8::Isolate* isolate;
    CopyablePersistent<v8::Context> context;
};

/**
 * @brief A wrapper around v8::FunctionCallbackInfo<v8::Value>
 *
 */
class V8CallbackInfo {
public:
    V8CallbackInfo(const v8::FunctionCallbackInfo<v8::Value>& info);
    ~V8CallbackInfo() = default;
    explicit V8CallbackInfo(const V8CallbackInfo&) = delete;
    V8CallbackInfo& operator=(const V8CallbackInfo&) = delete;

    V8Ref<V8Value> operator[](size_t index) const;
    V8Ref<V8Object> This() const;
    int Length() const;

    template<typename T>
    void SetReturnValue(T* instance) const;

    template<typename T>
    void SetReturnValue(V8Ref<T> val) const;

    void ReturnSelf() const;

    V8ExecutionContext GetExecutionContext() const
    {
        return V8ExecutionContext { isolate_, ctx_ };
    }

private:
    const v8::FunctionCallbackInfo<v8::Value>& info_;
    CopyablePersistent<v8::Context> ctx_;
    v8::Isolate* isolate_;
};

/**
 * @brief A placeholder class for GC mark callbacks. V8 provides optional second-pass callback to handles
 * but for this moment we do not need it.
 *
 */
class V8GCMarkCallbackInfo {
public:
    template<typename T>
    void Mark(const V8Ref<T>& val) const
    {}
};

class V8Exception {
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

#include "v8_types.inl"

#endif