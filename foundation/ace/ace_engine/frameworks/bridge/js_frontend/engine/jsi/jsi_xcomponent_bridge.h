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

#include "base/memory/ace_type.h"
#include "frameworks/bridge/common/dom/dom_xcomponent.h"
#include "frameworks/bridge/js_frontend/engine/common/base_xcomponent_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"
#include "frameworks/core/components/common/properties/decoration.h"
#include "frameworks/core/components/xcomponent/native_interface_xcomponent_impl.h"

namespace OHOS::Ace::Framework {
class JsiXComponentBridge : public BaseXComponentBridge {
    DECLARE_ACE_TYPE(JsiXComponentBridge, BaseXComponentBridge)

public:
    JsiXComponentBridge();
    ~JsiXComponentBridge();
    void HandleContext(const shared_ptr<JsRuntime>& runtime, NodeId id, const std::string& args);

    void OnJsEngineDestroy() override
    {
        renderContext_.reset();
    }

    const shared_ptr<JsValue>& GetRenderContext() const
    {
        return renderContext_;
    }

#ifdef OHOS_STANDARD_SYSTEM
    static shared_ptr<JsValue> JsGetXComponentSurfaceId(const shared_ptr<JsRuntime>& runtime, NodeId nodeId);
#endif

private:
    shared_ptr<JsValue> renderContext_;
    OH_NativeXComponent *nativeXComponent_ = nullptr;
    RefPtr<NativeXComponentImpl> nativeXcomponentImpl_;

    bool hasPluginLoaded_ = false;
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_XCOMPONENT_BRIDGE_H
