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

#ifndef FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_RENDERING_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_RENDERING_CONTEXT_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings_defines.h"
#include "bridge/declarative_frontend/jsview/js_canvas_renderer.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_rendering_context_settings.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSRenderingContext : public JSCanvasRenderer {
public:
    JSRenderingContext();
    ~JSRenderingContext() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSRenderingContext* controller);

    void JsToDataUrl(const JSCallbackInfo& info);
    void JsGetWidth(const JSCallbackInfo& info);
    void JsGetHeight(const JSCallbackInfo& info);
    void JsSetHeight(const JSCallbackInfo& info);
    void JsSetWidth(const JSCallbackInfo& info);
    void JsTransferFromImageBitmap(const JSCallbackInfo& info);

    ACE_DISALLOW_COPY_AND_MOVE(JSRenderingContext);

};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_RENDERING_CONTEXT_H
