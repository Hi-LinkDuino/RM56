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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_OBJECT_TEMPLATE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_OBJECT_TEMPLATE_H

#include "third_party/v8/include/v8.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_value_conversions.h"

namespace OHOS::Ace::Framework {

class V8ObjectTemplate {
public:
    V8ObjectTemplate();
    ~V8ObjectTemplate();

protected:
    v8::Local<v8::ObjectTemplate> operator*() const;

    template<typename T>
    void Constant(const char* name, T val);

    v8::Local<v8::Object> NewInstance() const;

private:
    v8::Persistent<v8::ObjectTemplate> proto_;
};

} // namespace OHOS::Ace::Framework

#include "v8_object_template.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_OBJECT_TEMPLATE_H
