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

#include "qjs_object_template.h"

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_function_list_entries_container.h"

namespace OHOS::Ace::Framework {

template<typename T>
void QJSObjectTemplate::Constant(const char* name, T value)
{
    RegisterPropertyWithValue(name, value, 0);
}

template<typename T>
void QJSObjectTemplate::RegisterPropertyWithValue(const char* name, T value, int flags)
{
    JSContext* ctx = QJSContext::Current();
    if constexpr (std::is_same_v<T, int32_t>) {
        JSCFunctionListEntry* funcEntry =
            QJSFunctionListEntriesContainer::GetInstance().New(name, flags, JS_DEF_PROP_INT32, 0);
        funcEntry->u.i32 = value;
        JS_SetPropertyFunctionList(ctx, proto_, funcEntry, 1);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        JSCFunctionListEntry* funcEntry =
            QJSFunctionListEntriesContainer::GetInstance().New(name, flags, JS_DEF_PROP_INT64, 0);
        funcEntry->u.i64 = value;
        JS_SetPropertyFunctionList(ctx, proto_, funcEntry, 1);
    } else if constexpr (std::is_floating_point_v<T>) {
        JSCFunctionListEntry* funcEntry =
            QJSFunctionListEntriesContainer::GetInstance().New(name, flags, JS_DEF_PROP_DOUBLE, 0);
        funcEntry->u.f64 = value;
        JS_SetPropertyFunctionList(ctx, proto_, funcEntry, 1);
    } else if constexpr (std::is_same_v<T, const char*>) {
        JSCFunctionListEntry* funcEntry =
            QJSFunctionListEntriesContainer::GetInstance().New(name, flags, JS_DEF_PROP_STRING, 0);
        funcEntry->u.str = value;
        JS_SetPropertyFunctionList(ctx, proto_, funcEntry, 1);
    }
}

} // namespace OHOS::Ace::Framework
