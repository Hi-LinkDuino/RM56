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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JS_OBJECT_TEMPLATE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JS_OBJECT_TEMPLATE_H

#ifdef USE_V8_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_object_template.h"
#elif USE_QUICKJS_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_object_template.h"
#elif USE_ARK_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_object_template.h"
#endif

namespace OHOS::Ace::Framework {

template<typename ImplDetail>
class JSObjectTemplateImpl : private ImplDetail {
public:
    JSObjectTemplateImpl();
    ~JSObjectTemplateImpl();

    template<typename T>
    void Constant(const char* name, T value);

    auto operator*() const;
    auto NewInstance() const;
};

#ifdef USE_V8_ENGINE
using JSObjectTemplate = JSObjectTemplateImpl<V8ObjectTemplate>;
#elif USE_QUICKJS_ENGINE
using JSObjectTemplate = JSObjectTemplateImpl<QJSObjectTemplate>;
#elif USE_ARK_ENGINE
using JSObjectTemplate = JSObjectTemplateImpl<JsiObjectTemplate>;
#endif

} // namespace OHOS::Ace::Framework

#include "js_object_template.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JS_OBJECT_TEMPLATE_H
