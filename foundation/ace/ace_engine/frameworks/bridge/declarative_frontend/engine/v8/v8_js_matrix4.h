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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_JS_MATRIX4_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_JS_MATRIX4_H

#include "third_party/v8/include/v8.h"

namespace OHOS::Ace::Framework {

class JSMatrix4 final {
public:
    JSMatrix4() = default;
    ~JSMatrix4() = default;

    static void CreateMatrix4Object(v8::Local<v8::Object> moduleObj, v8::Isolate* isolate);

    static void Init(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Copy(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Combine(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Identity(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Invert(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Translate(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Scale(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Rotate(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void TransformPoint(const v8::FunctionCallbackInfo<v8::Value>& info);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_JS_MATRIX4_H
