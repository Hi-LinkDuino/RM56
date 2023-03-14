/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_XCOMPONENT_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_XCOMPONENT_BRIDGE_H

#include <unordered_map>

#include "third_party/v8/include/v8.h"

#include "base/memory/ace_type.h"
#include "core/components/common/properties/decoration.h"
#include "frameworks/bridge/js_frontend/engine/common/base_xcomponent_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"
#include "frameworks/core/components/xcomponent/native_interface_xcomponent_impl.h"

namespace OHOS::Ace::Framework {
class V8XComponentBridge : public BaseXComponentBridge {
    DECLARE_ACE_TYPE(V8XComponentBridge, BaseXComponentBridge)

public:
    V8XComponentBridge();
    ~V8XComponentBridge();
    void HandleContext(const v8::Local<v8::Context>& ctx, NodeId id, const std::string& args, JsEngineInstance* engine);

    v8::Local<v8::Context> GetContext() const
    {
        return ctx_.Get(isolate_);
    }

    v8::Local<v8::Object> GetRenderContext() const
    {
        return renderContext_.Get(isolate_);
    }

private:
    v8::Persistent<v8::Object> renderContext_;
    v8::Persistent<v8::Context> ctx_;
    v8::Isolate* isolate_ = nullptr;

    OH_NativeXComponent *nativeXComponent_ = nullptr;
    RefPtr<NativeXComponentImpl> nativeXComponentImpl_;

    bool hasPluginLoaded_ = false;
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_XCOMPONENT_BRIDGE_H
