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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_REF_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_REF_H

#include "v8.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_types.h"

namespace OHOS::Ace::Framework {

template<typename T>
class V8Weak;

/**
 *  \brief A class template that holds a strong reference to a javascript value.
 *  Further in the text, when we talk about "javascript value" we refer to its representation within C++.
 *  Such are Value, Object, Array, Function, etc.
 *
 *  A strong reference will increase the reference count of the holding javascript value when constructed,
 *  copied or assigned, and will decrease the reference count when destroyed.
 *
 *  A strong reference to a javascript value should only be held when sharing it among owners that outlive
 *  said value. All other cases should use \p JSWeak. For example a row containing several buttons should
 *  hold a strong reference, but those buttons should hold a weak reference to the parent row if that is
 *  what the application requires, otherwise a circular dependency is established and objects do not get
 *  collected by the garbage collector.
 *
 *  \code{.cpp}
 *  \endcode
 *
 *  3. \p FunctionCallback and \p MemberFunctionCallback corresponding to ESI callback signatures:
 *  \code{.cpp}
 *  \endcode
 *
 *
 *  \tparam A javascript value (Value, Object, Array, Function, etc.)
 *  \example
 *  \code{.cpp}
 *  \endcode
 *
 * \class V8Ref
 * \alias JSRef
 */
template<typename T>
class V8Ref {
public:
    using wrappedT = v8::Local<v8::Object>;

    V8Ref() {}
    explicit V8Ref(const T& val) : object_(val) {}
    ~V8Ref()
    {
        object_.Reset();
    }

    template<typename S>
    V8Ref(const V8Ref<S>& that) : object_(T::Cast(that.Get()))
    {}

    // We do not want implicit conversions from weak to strong reference
    explicit V8Ref(const V8Weak<T>& rhs)
    {
        *this = rhs.Lock();
    }

    template<typename... Args>
    static V8Ref<T> Make(Args&&... args)
    {
        return V8Ref<T>::Claim(T { std::forward<Args>(args)... });
    }

    static V8Ref<T> Claim(T&& val)
    {
        return V8Ref<T>(std::forward<T>(val));
    }

    template<typename S>
    static V8Ref<T> Cast(const V8Ref<S>& that)
    {
        return V8Ref<T>::Make(T::Cast(that.Get()));
    }

    static V8Ref<T> New()
    {
        return V8Ref<T>::Make(T::New());
    }

    template<typename S>
    static V8Ref<T> New(S param)
    {
        return V8Ref<T>::Make(T::New(param));
    }

    V8Ref(const V8Ref<T>& rhs) : object_(rhs.object_) {}

    V8Ref(V8Ref<T>&& rhs) : object_(std::move(rhs.object_))
    {
        rhs.object_.Reset();
    }

    V8Ref<T>& operator=(const V8Ref<T>& rhs)
    {
        object_.Reset();
        object_ = rhs.object_;
        return *this;
    }

    V8Ref<T>& operator=(V8Ref<T>&& rhs)
    {
        object_.Reset();
        object_ = std::move(rhs.object_);
        rhs.object_.Reset();
        return *this;
    }

    bool IsEmpty() const
    {
        return object_.IsEmpty();
    }

    void Reset()
    {
        object_.Reset();
    }

    template<typename U>
    typename std::enable_if_t<std::is_same_v<T, V8Object>, U*> Unwrap() const
    {
        return object_.template Unwrap<U>();
    }

    const T& operator->() const
    {
        return object_;
    }

    operator v8::Local<T>() const
    {
        // Implicit conversion, V8Type has operator v8::Local<T>
        return object_;
    }

    T Get() const
    {
        return object_;
    }

private:
    T object_;
};

template<typename T>
class V8Weak {
public:
    using wrappedT = v8::Local<v8::Object>;

    V8Weak() {}
    ~V8Weak()
    {
        object_.Reset();
    }

    V8Weak(const V8Weak<T>& rhs) : object_(rhs.object_)
    {
        object_.SetWeak();
    }

    V8Weak(V8Weak<T>&& rhs) : object_(std::move(rhs.object_))
    {
        object_.SetWeak();
        rhs.object_.Reset();
    }

    explicit V8Weak(const V8Ref<T>& rhs) : object_(rhs.Get())
    {
        object_.SetWeak();
    }

    V8Weak<T>& operator=(const V8Weak<T>& rhs)
    {
        object_.Reset();
        object_ = rhs.object_;
        object_.SetWeak();
        return *this;
    }

    V8Weak<T>& operator=(const V8Ref<T>& rhs)
    {
        object_ = rhs.Get();
        object_.SetWeak();
        return *this;
    }

    V8Weak<T>& operator=(V8Weak<T>&& rhs)
    {
        object_.Reset();
        object_ = std::move(rhs.object_);
        object_.SetWeak();

        rhs.object_.Reset();
        return *this;
    }

    bool IsEmpty() const
    {
        return object_.IsEmpty();
    }

    void Reset()
    {
        object_.Reset();
    }

    V8Ref<T> Lock()
    {
        return V8Ref<T>(object_);
    }

private:
    T object_;
};

} // namespace OHOS::Ace::Framework
#endif
