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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_XCOMPONENT_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_XCOMPONENT_BRIDGE_H

#include <unordered_map>

#include "base/memory/ace_type.h"
#include "core/components/common/properties/decoration.h"
#include "frameworks/bridge/js_frontend/engine/common/base_xcomponent_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "frameworks/core/components/xcomponent/native_interface_xcomponent_impl.h"

namespace OHOS::Ace::Framework {
class QjsXComponentBridge : public BaseXComponentBridge {
    DECLARE_ACE_TYPE(QjsXComponentBridge, BaseXComponentBridge)

public:
    QjsXComponentBridge();
    ~QjsXComponentBridge();

    void HandleContext(JSContext* ctx, NodeId id, const std::string& args);

    JSValue GetRenderContext() const
    {
        return renderContext_;
    }

private:
    JSValue renderContext_;

    OH_NativeXComponent* nativeXComponent_;
    RefPtr<NativeXComponentImpl> nativeXComponentImpl_;

    bool hasPluginLoaded_ = false;
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_XCOMPONENT_BRIDGE_H
