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

#ifndef FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_OFFSCREEN_RENDERING_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_OFFSCREEN_RENDERING_CONTEXT_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings_defines.h"
#include "bridge/declarative_frontend/jsview/js_canvas_renderer.h"

#include "frameworks/bridge/declarative_frontend/jsview/js_rendering_context_settings.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSOffscreenRenderingContext : public JSCanvasRenderer {
public:
    JSOffscreenRenderingContext();
    ~JSOffscreenRenderingContext() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSOffscreenRenderingContext* controller);

    void JsTransferToImageBitmap(const JSCallbackInfo& info);
    static RefPtr<OffscreenCanvas> GetOffscreenCanvas(int32_t id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return offscreenCanvasMap_[id];
    }
    uint32_t GetId()
    {
        return id;
    }

    ACE_DISALLOW_COPY_AND_MOVE(JSOffscreenRenderingContext);

private:
    static std::mutex mutex_;
    static std::unordered_map<uint32_t, RefPtr<OffscreenCanvas>> offscreenCanvasMap_;
    static uint32_t offscreenCanvasCount_;
    uint32_t id;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_OFFSCREEN_RENDERING_CONTEXT_H