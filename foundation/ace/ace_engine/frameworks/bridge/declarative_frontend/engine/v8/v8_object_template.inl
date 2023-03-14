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

#include "v8_object_template.h"

namespace OHOS::Ace::Framework {

template<typename T>
void V8ObjectTemplate::Constant(const char* name, T val)
{
    auto isolate = v8::Isolate::GetCurrent();
    proto_.Get(isolate)->Set(v8::Local<v8::Name>::Cast(V8ValueConvertor::toV8Value(name)),
        V8ValueConvertor::toV8Value(val), v8::PropertyAttribute::ReadOnly);
}

} // namespace OHOS::Ace::Framework
