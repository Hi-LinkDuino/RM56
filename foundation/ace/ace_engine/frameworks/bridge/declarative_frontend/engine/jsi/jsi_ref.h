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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_REF_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_REF_H

#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_types.h"

namespace OHOS::Ace::Framework {

template<typename T>
class JsiWeak;

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
 * \class JsiRef
 * \alias JSRef
 */
template<typename T>
class JsiRef {
public:
    using wrappedT = panda::Local<panda::ObjectRef>;

    JsiRef() {}
    explicit JsiRef(const T& val) : value_(val) {}
    ~JsiRef()
    {
        value_.Reset();
    }

    template<typename S>
    JsiRef(const JsiRef<S>& that) : value_(T::Cast(that.Get()))
    {}

    // We do not want implicit conversions from weak to strong reference
    explicit JsiRef(const JsiWeak<T>& rhs)
    {
        *this = rhs.Lock();
    }

    template<typename... Args>
    static JsiRef<T> Make(Args&&... args)
    {
        auto obj = T { args... };
        return JsiRef<T>(obj);
    }

    static JsiRef<T> Claim(T&& val)
    {
        return JsiRef<T>(std::forward<T>(val));
    }

    template<typename S>
    static JsiRef<T> Cast(const JsiRef<S>& that)
    {
        return JsiRef<T>::Make(T::Cast(that.Get()));
    }

    static JsiRef<T> New()
    {
        return JsiRef<T>::Make(T::New());
    }

    template<typename S>
    static JsiRef<T> New(S param)
    {
        return JsiRef<T>::Make(T::New(param));
    }

    JsiRef(const JsiRef<T>& rhs) : value_(rhs.value_) {}

    JsiRef(JsiRef<T>&& rhs) : value_(std::move(rhs.value_))
    {
        rhs.value_.Reset();
    }

    JsiRef<T>& operator=(const JsiRef<T>& rhs)
    {
        value_.Reset();
        value_ = rhs.value_;
        return *this;
    }

    JsiRef<T>& operator=(JsiRef<T>&& rhs)
    {
        value_.Reset();
        value_ = std::move(rhs.value_);
        rhs.value_.Reset();
        return *this;
    }

    bool IsEmpty() const
    {
        return value_.IsEmpty();
    }

    void Reset()
    {
        value_.Reset();
    }

    template<typename U>
    typename std::enable_if_t<std::is_same_v<T, JsiObject>, U*> Unwrap() const
    {
        return value_.template Unwrap<U>();
    }

    const T& operator->() const
    {
        return value_;
    }

    T Get() const
    {
        return value_;
    }

private:
    T value_;
};

template<typename T>
class JsiWeak {
public:
    using wrappedT = panda::Local<panda::ObjectRef>;

    JsiWeak() {}
    ~JsiWeak()
    {
        value_.Reset();
    }

    JsiWeak(const JsiWeak<T>& rhs) : value_(rhs.value_)
    {
        value_.SetWeak();
    }

    JsiWeak(JsiWeak<T>&& rhs) : value_(std::move(rhs.value_))
    {
        value_.SetWeak();
        rhs.value_.Reset();
    }

    explicit JsiWeak(const JsiRef<T>& rhs) : value_(rhs.Get())
    {
        value_.SetWeak();
    }

    JsiWeak<T>& operator=(const JsiWeak<T>& rhs)
    {
        value_.Reset();
        value_ = rhs.value_;
        value_.SetWeak();
        return *this;
    }

    JsiWeak<T>& operator=(const JsiRef<T>& rhs)
    {
        value_ = rhs.Get();
        value_.SetWeak();
        return *this;
    }

    JsiWeak<T>& operator=(JsiWeak<T>&& rhs)
    {
        value_.Reset();
        value_ = std::move(rhs.value_);
        value_.SetWeak();

        rhs.value_.Reset();
        return *this;
    }

    bool IsEmpty() const
    {
        return value_.IsEmpty();
    }

    void Reset()
    {
        value_.Reset();
    }

    JsiRef<T> Lock()
    {
        return JsiRef<T>(value_);
    }

private:
    T value_;
};

} // namespace OHOS::Ace::Framework
#endif
