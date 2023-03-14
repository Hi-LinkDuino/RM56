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

#include "bindings_implementation.h"

namespace OHOS::Ace::Framework {

template<typename T, template<typename> typename ImplDetail>
std::unordered_map<int, IFunctionBinding*> JSClassImpl<T, ImplDetail>::functions_;
template<typename T, template<typename> typename ImplDetail>
int JSClassImpl<T, ImplDetail>::nextFreeId_ = 0;
template<typename T, template<typename> typename ImplDetail>
std::string JSClassImpl<T, ImplDetail>::jsName;

template<typename C, template<typename> typename ImplDetail>
void JSClassImpl<C, ImplDetail>::Declare(const char* name)
{
    jsName = name;
    ImplDetail<C>::Declare(name);
}

template<typename C, template<typename> typename ImplDetail>
template<typename R, typename... Args>
void JSClassImpl<C, ImplDetail>::Method(const char* name, R (C::*func)(Args...), MethodOptions options)
{
    functions_.emplace(nextFreeId_, new FunctionBinding(name, options, func));
    ImplDetail<C>::Method(name, func, nextFreeId_++);
}

template<typename C, template<typename> typename ImplDetail>
template<typename Base, typename R, typename... Args>
void JSClassImpl<C, ImplDetail>::Method(const char* name, R (Base::*func)(Args...), MethodOptions options)
{
    static_assert(std::is_base_of_v<Base, C>, "Trying to bind an unrelated method!");
    functions_.emplace(nextFreeId_, new FunctionBinding(name, options, func));
    ImplDetail<C>::Method(name, func, nextFreeId_++);
}

template<typename C, template<typename> typename ImplDetail>
template<typename R, typename... Args>
void JSClassImpl<C, ImplDetail>::StaticMethod(const char* name, R (C::*func)(Args...), MethodOptions options)
{
    functions_.emplace(nextFreeId, new FunctionBinding(name, options, func));
    ImplDetail<C>::StaticMethod(name, func, nextFreeId++);
}

template<typename C, template<typename> typename ImplDetail>
template<typename Base, typename R, typename... Args>
void JSClassImpl<C, ImplDetail>::StaticMethod(const char* name, R (Base::*func)(Args...), MethodOptions options)
{
    static_assert(std::is_base_of_v<Base, C>, "Trying to bind an unrelated method!");
    functions_.emplace(nextFreeId, new FunctionBinding(name, options, func));
    ImplDetail<C>::StaticMethod(name, func, nextFreeId++);
}

template<typename C, template<typename> typename ImplDetail>
template<typename T>
void JSClassImpl<C, ImplDetail>::CustomMethod(
    const char* name, typename ImplDetail<C>::template MemberFunctionCallback<T> callback)
{
    static_assert(std::is_base_of_v<T, C>, "Trying to bind an unrelated method!");
    functions_.emplace(nextFreeId_, new FunctionBinding(name, MethodOptions::NONE, callback));
    ImplDetail<C>::CustomMethod(name, callback, nextFreeId_++);
}

template<typename C, template<typename> typename ImplDetail>
void JSClassImpl<C, ImplDetail>::CustomMethod(const char* name, typename ImplDetail<C>::FunctionCallback callback)
{
    ImplDetail<C>::CustomMethod(name, callback);
}

template<typename C, template<typename> typename ImplDetail>
template<typename T>
void JSClassImpl<C, ImplDetail>::CustomProperty(
    const char* name, typename ImplDetail<C>::template MemberFunctionGetCallback<T> getter,
    typename ImplDetail<C>::template MemberFunctionSetCallback<T> setter)
{
    int getFuncId;
    int setFuncId;
    static_assert(std::is_base_of_v<T, C>, "Trying to bind an unrelated method!");
    getFunctions_.emplace(nextFreeId_, new FunctionBinding(name, MethodOptions::NONE, getter));
    setFunctions_.emplace(nextFreeId_, new FunctionBinding(name, MethodOptions::NONE, setter));
    functions_.emplace(nextFreeId_, new FunctionBinding(name, MethodOptions::NONE, getter));
    getFuncId = nextFreeId_++;
    functions_.emplace(nextFreeId_, new FunctionBinding(name, MethodOptions::NONE, setter));
    setFuncId = nextFreeId_++;
    ImplDetail<C>::CustomProperty(name, getter, getFuncId, setFuncId);
}

template<typename C, template<typename> typename ImplDetail>
void JSClassImpl<C, ImplDetail>::CustomProperty(const char* name, typename ImplDetail<C>::FunctionGetCallback getter,
    typename ImplDetail<C>::FunctionSetCallback setter)
{
    ImplDetail<C>::CustomProperty(name, getter, setter);
}

template<typename C, template<typename> typename ImplDetail>
template<typename T>
void JSClassImpl<C, ImplDetail>::StaticMethod(
    const char* name, typename ImplDetail<C>::template MemberFunctionCallback<T> callback)
{
    static_assert(std::is_base_of_v<T, C>, "Trying to bind an unrelated method!");
    functions_.emplace(nextFreeId, new FunctionBinding(name, MethodOptions::NONE, callback));
    ImplDetail<C>::StaticMethod(name, callback, nextFreeId++);
}

template<typename C, template<typename> typename ImplDetail>
void JSClassImpl<C, ImplDetail>::StaticMethod(const char* name, typename ImplDetail<C>::FunctionCallback callback)
{
    ImplDetail<C>::StaticMethod(name, callback);
}

template<typename C, template<typename> typename ImplDetail>
template<typename T>
void JSClassImpl<C, ImplDetail>::StaticConstant(const char* name, T value)
{
    ImplDetail<C>::StaticConstant(name, value);
}

template<typename C, template<typename> typename ImplDetail>
void JSClassImpl<C, ImplDetail>::Bind(
    typename ImplDetail<C>::BindingTarget bindTarget, typename ImplDetail<C>::FunctionCallback ctor)
{
    ImplDetail<C>::Bind(bindTarget, ctor);
}

template<typename C, template<typename> typename ImplDetail>
template<typename... Args>
void JSClassImpl<C, ImplDetail>::Bind(typename ImplDetail<C>::BindingTarget bindTarget)
{
    ImplDetail<C>::template Bind<Args...>(bindTarget);
}

template<typename C, template<typename> typename ImplDetail>
template<typename Base>
void JSClassImpl<C, ImplDetail>::Inherit()
{
    static_assert(std::is_base_of_v<Base, C>, "Calling Inherit() on unrelated classes!");
    ImplDetail<C>::template Inherit<Base>();
}

template<typename C, template<typename> typename ImplDetail>
IFunctionBinding* JSClassImpl<C, ImplDetail>::GetFunctionBinding(int id)
{
    return functions_[id];
}

template<typename C, template<typename> typename ImplDetail>
const char* JSClassImpl<C, ImplDetail>::JSName()
{
    return jsName.c_str();
}

}; // namespace OHOS::Ace::Framework
