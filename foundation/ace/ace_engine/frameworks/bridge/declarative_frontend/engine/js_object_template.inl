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

#include "js_object_template.h"

namespace OHOS::Ace::Framework {

template<typename ImplDetail>
JSObjectTemplateImpl<ImplDetail>::JSObjectTemplateImpl() : ImplDetail()
{}

template<typename ImplDetail>
JSObjectTemplateImpl<ImplDetail>::~JSObjectTemplateImpl()
{}

template<typename ImplDetail>
template<typename T>
void JSObjectTemplateImpl<ImplDetail>::Constant(const char* name, T value)
{
    ImplDetail::Constant(name, value);
}

template<typename ImplDetail>
auto JSObjectTemplateImpl<ImplDetail>::operator*() const
{
    return ImplDetail::operator*();
}

template<typename ImplDetail>
auto JSObjectTemplateImpl<ImplDetail>::NewInstance() const
{
    return ImplDetail::NewInstance();
}

} // namespace OHOS::Ace::Framework
