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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_REF_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_REF_H

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_types.h"

namespace OHOS::Ace::Framework {

template<typename T>
class QJSRef {
public:
    using wrappedT = JSValue;

    QJSRef() {}

    explicit QJSRef(const T& rhs) : value_(rhs)
    {
        value_.Dup();
    }

    template<typename S>
    QJSRef(const QJSRef<S>& that) : value_(that.Get().GetHandle())
    {
        value_.Dup();
    }

    explicit QJSRef(const QJSWeak<T>& rhs)
    {
        *this = rhs.Lock();
    }

    template<typename... Args>
    static QJSRef<T> Make(Args&&... args)
    {
        return QJSRef<T>(T { std::forward<Args>(args)... });
    }

    template<typename Arg>
    static QJSRef<T> Claim(Arg&& val)
    {
        QJSRef<T> newVal;
        newVal = T { std::forward<Arg>(val) };
        return newVal;
    }

    static QJSRef<T> New()
    {
        return QJSRef<T>::Make(T::New());
    }

    template<typename S>
    static QJSRef<T> New(S param)
    {
        return QJSRef<T>::Make(T::New(param));
    }

    template<typename S>
    static QJSRef<T> Cast(const QJSRef<S>& that)
    {
        return QJSRef<T>(that);
    }

    ~QJSRef()
    {
        value_.Free();
    }

    QJSRef(const QJSRef<T>& rhs) : value_(rhs.value_)
    {
        value_.Dup();
    }

    QJSRef(QJSRef<T>&& rhs) : value_(std::move(rhs.value_))
    {
        rhs.value_.Reset();
    }

    QJSRef<T>& operator=(const QJSRef<T>& rhs)
    {
        value_.Free();
        value_ = rhs.value_;
        value_.Dup();
        return *this;
    }

    QJSRef<T>& operator=(QJSRef<T>&& rhs)
    {
        value_.Free();
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
        value_.Free();
    }

    template<typename U>
    typename std::enable_if_t<std::is_same_v<T, QJSObject>, U*> Unwrap() const
    {
        return value_.template Unwrap<U>();
    }

    T Get() const
    {
        return value_;
    }

    const T& operator->() const
    {
        return value_;
    }

private:
    QJSRef<T>& operator=(const T& val)
    {
        value_.Free();
        value_ = val;
        return *this;
    }

    T value_;
};

template<typename T>
class QJSWeak {
public:
    using wrappedT = JSValue;

    QJSWeak() {}

    ~QJSWeak() {}

    QJSWeak(const QJSWeak<T>& rhs)
    {
        value_ = rhs.value_;
    }

    explicit QJSWeak(const QJSRef<T>& rhs) : value_(rhs.Get()) {}

    QJSWeak(QJSWeak<T>&& rhs) : value_(std::move(rhs)) {}

    QJSWeak<T>& operator=(const QJSWeak<T>& rhs)
    {
        value_ = rhs.value_;
        return *this;
    }

    QJSWeak& operator=(const QJSRef<T>& rhs)
    {
        value_ = rhs.Get();
        return *this;
    }

    QJSWeak<T>& operator=(QJSWeak<T>&& rhs)
    {
        value_ = std::move(rhs.value_);
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

    QJSRef<T> Lock()
    {
        return QJSRef<T>(value_);
    }

private:
    T value_;
};

} // namespace OHOS::Ace::Framework
#endif
