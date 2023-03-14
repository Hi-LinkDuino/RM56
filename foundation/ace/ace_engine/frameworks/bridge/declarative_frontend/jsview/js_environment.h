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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ENVIRONMENT_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ENVIRONMENT_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/js_types.h"

namespace OHOS::Ace::Framework {

class JSEnvironment final : public Referenced {
public:
    void GetAccessibilityEnabled(const JSCallbackInfo& args);
    void GetColorMode(const JSCallbackInfo& args);
    void GetFontScale(const JSCallbackInfo& args);
    void GetFontWeightScale(const JSCallbackInfo& args);
    void GetLayoutDirection(const JSCallbackInfo& args);
    void GetLanguageCode(const JSCallbackInfo& args);
    void onChange(const JSCallbackInfo& args);

    static void ConstructorCallback(const JSCallbackInfo& args);
    static void DestructorCallback(JSEnvironment* obj);

    static void JSBind(BindingTarget globalObj);

private:
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ENVIRONMENT_H
