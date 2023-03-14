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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_object_template.h"

namespace OHOS::Ace::Framework {

V8ObjectTemplate::V8ObjectTemplate()
{
    auto isolate = v8::Isolate::GetCurrent();
    proto_.Reset(isolate, v8::ObjectTemplate::New(isolate));
}
V8ObjectTemplate::~V8ObjectTemplate()
{
    proto_.Reset();
}

v8::Local<v8::ObjectTemplate> V8ObjectTemplate::operator*() const
{
    auto isolate = v8::Isolate::GetCurrent();
    return proto_.Get(isolate);
}

v8::Local<v8::Object> V8ObjectTemplate::NewInstance() const
{
    auto isolate = v8::Isolate::GetCurrent();
    auto context = isolate->GetCurrentContext();
    return proto_.Get(isolate)->NewInstance(context).ToLocalChecked();
}

} // namespace OHOS::Ace::Framework
