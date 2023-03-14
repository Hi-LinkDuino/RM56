/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_CHECKED_CAST_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_CHECKED_CAST_H

#include <exception>
#include <typeinfo>
#include <cassert>
#include "app_log_wrapper.h"

namespace hidden {
// workaroud for T& equally matching f(T&) and f(T const&)
// so we do the following: T const& never matches f(T&), therefore:
// f(T&, LookUpHelper2 const&) and f(T const&, LookUpHelper const&)
// f(T&, LookUpHelper2 const&) does match both but f(T&, LookUpHelper2 const&) is a
// perfect match (no upcast for LookUpHelper needed)

struct LookUpHelper {};
struct LookUpHelper2 : public hidden::LookUpHelper {};

// IsPtr - partial specialization only
template<typename T>
struct IsPtr {
    enum { value = false };
};
template<typename T>
struct IsPtr<T *> {
    enum { value = true };
};
}  // namespace hidden

// bad_checked_cast is thrown if cast is bad
// see boost::lexical_cast
class bad_checked_cast : std::bad_cast {
public:
    bad_checked_cast() : from(0), to(0)
    {}

    bad_checked_cast(std::type_info const &from, std::type_info const &to) : from(&from), to(&to)
    {}
    virtual ~bad_checked_cast()
    {}
    std::type_info const &source_type() const
    {
        return *from;
    }

    std::type_info const &target_type() const
    {
        return *to;
    }

    char const *what() const throw()
    {
        return "bad checked cast: source is not a target type";
    }

private:
    std::type_info const *from;
    std::type_info const *to;
};
#ifdef CHECKED_CAST_DO_ASSERT
#define BAD_CHECKED_CAST(from, to) assert(false)
#else
#define BAD_CHECKED_CAST(from, to)
#endif

// implementation
namespace hidden {
template<typename T, typename X, bool isPtr>
struct checked_cast_impl;

// pointer variant
template<typename T, typename X>
struct checked_cast_impl<T, X, true> {
    static T cast(X &x, hidden::LookUpHelper2 const &)
    {
#ifdef CHECKED_CAST_SAFE_CONVERSATION
        T t = dynamic_cast<T>(x);
        // check cross cast
        if (t != static_cast<T>(x)) {
            BAD_CHECKED_CAST(x, T);
        }
        return t;
#else
        return static_cast<T>(x);
#endif
    }

    static T cast(X const &x, hidden::LookUpHelper const &)
    {
#ifdef CHECKED_CAST_SAFE_CONVERSATION
        T t = dynamic_cast<T>(x);

        // check cross cast
        if (t != static_cast<T>(x)) {
            BAD_CHECKED_CAST(x, T);
        }
        return t;
#else
        return static_cast<T>(x);
#endif
    }
};

template<typename T, typename X>
struct checked_cast_impl<T, X, false> {
    static T cast(X &x, hidden::LookUpHelper2 const &)
    {
#ifdef CHECKED_CAST_SAFE_CONVERSATION
        T t = dynamic_cast<T>(x);
        // check cross cast
        if (&t != &static_cast<T>(x)) {
            APP_LOGE("!!!bad cast!!!");
        }
        return t;
#else
        return static_cast<T>(x);
#endif
    }

    static T cast(X const &x, hidden::LookUpHelper const &)
    {
#ifdef CHECKED_CAST_SAFE_CONVERSATION
        T t = dynamic_cast<T>(x);
        // check cross cast
        if (&t != &static_cast<T>(x)) {
            std::bad_cast();
        }
        return t;
#else
        return static_cast<T>(x);
#endif
    }
};

}  // namespace hidden

template<typename T, typename X>
inline T checked_cast(X &x)
{
    return hidden::checked_cast_impl<T, X, hidden::IsPtr<X>::value>::cast(x, hidden::LookUpHelper2());
}
template<typename T, typename X>
inline T checked_cast(X const &x)
{
    return hidden::checked_cast_impl<T, X, hidden::IsPtr<X>::value>::cast(x, hidden::LookUpHelper2());
}

#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_CHECKED_CAST_H
