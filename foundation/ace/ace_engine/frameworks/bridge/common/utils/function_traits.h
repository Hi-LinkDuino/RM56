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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_FUNCTION_TRAITS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_FUNCTION_TRAITS_H

#include <tuple>

template<typename T>
struct function_traits;

template<typename Class, typename ReturnType, typename... Args>
struct function_traits<ReturnType (Class::*)(Args...)> {
    static constexpr const size_t argc = sizeof...(Args);
    typedef ReturnType returnType;
    using ArgumentPack = std::tuple<Args...>;
    template<size_t i>
    struct argv {
        typedef typename std::tuple_element<i, ArgumentPack>::type type;
    };
};

namespace FunctionUtils {

template<typename C, typename F, typename T, size_t... I>
auto CallMemberFunction(C* instance, F func, T&& tuple, std::index_sequence<I...>)
{
    return (instance->*func)(std::get<I>(std::forward<T>(tuple))...);
}

template<typename C, typename R, typename Tuple, typename... Types>
auto CallMemberFunction(C* instance, R (C::*func)(Types...), Tuple&& tup)
{
    return CallMemberFunction(instance, func, std::forward<Tuple>(tup), std::make_index_sequence<sizeof...(Types)> {});
}

template<typename F, typename T, size_t... I>
auto CallStaticMemberFunction(F func, T&& tuple, std::index_sequence<I...>)
{
    return func(std::get<I>(std::forward<T>(tuple))...);
}

template<typename R, typename Tuple, typename... Types>
auto CallStaticMemberFunction(R (*func)(Types...), Tuple&& tup)
{
    return CallStaticMemberFunction(func, std::forward<Tuple>(tup), std::make_index_sequence<sizeof...(Types)> {});
}

template<class T, class Tuple, size_t... Is>
T* ConstructFromTuple(Tuple&& tuple, std::index_sequence<Is...>)
{
    return new T { std::get<Is>(std::forward<Tuple>(tuple))... };
}

template<class T, class Tuple>
T* ConstructFromTuple(Tuple&& tuple)
{
    return ConstructFromTuple<T>(
        std::forward<Tuple>(tuple), std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value> {});
}

}; // namespace FunctionUtils

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_FUNCTION_TRAITS_H