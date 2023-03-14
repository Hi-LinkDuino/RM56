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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_COUNTER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_COUNTER_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/counter/counter_component.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSCounter : public JSContainerBase {
public:
    static void Create();
    static void JSBind(BindingTarget globalObj);
    static void JsOnInc(const JSCallbackInfo& args);
    static void JsOnDec(const JSCallbackInfo& args);
    static void JSHeight(const JSCallbackInfo& args);
    static void JSWidth(const JSCallbackInfo& args);
    static void JSControlwidth(const JSCallbackInfo& args);
    static void JSStateChange(bool state);
    static void SetSize(const JSCallbackInfo& args);
    static void JsBackgroundColor(const JSCallbackInfo& args);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_COUNTER_H
