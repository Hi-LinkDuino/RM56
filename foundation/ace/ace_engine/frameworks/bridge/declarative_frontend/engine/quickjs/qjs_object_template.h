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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_OBJECT_TEMPLATE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_OBJECT_TEMPLATE_H

#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/quickjs-libc.h"

void* JS_GetOpaqueA(JSValueConst obj, JSClassID* classIds, size_t classIdLen);

JSValueConst JS_NewGlobalCConstructor(
    JSContext* ctx, const char* name, JSCFunction* func, int length, JSValueConst proto);

#ifdef __cplusplus
}
#endif

namespace OHOS::Ace::Framework {

class QJSObjectTemplate {
public:
    QJSObjectTemplate();
    ~QJSObjectTemplate();

protected:
    template<typename T>
    void Constant(const char* constName, T value);

    JSValue operator*() const;
    JSValue NewInstance() const;

private:
    template<typename T>
    void RegisterPropertyWithValue(const char* name, T value, int flags);

    JSValue proto_;
};

} // namespace OHOS::Ace::Framework

#include "qjs_object_template.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_OBJECT_TEMPLATE_H
