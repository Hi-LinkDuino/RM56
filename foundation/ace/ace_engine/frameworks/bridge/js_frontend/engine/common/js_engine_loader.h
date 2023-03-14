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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_JS_ENGINE_LOADER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_JS_ENGINE_LOADER_H

#include "frameworks/bridge/js_frontend/engine/common/base_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/common/base_xcomponent_bridge.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"

namespace OHOS::Ace::Framework {

class JsEngineLoader {
public:
    virtual ~JsEngineLoader() = default;

    static JsEngineLoader& Get(const char* sharedLibrary = nullptr);
    static JsEngineLoader& GetDeclarative(const char* sharedLibrary = nullptr);

    virtual RefPtr<JsEngine> CreateJsEngine(int32_t instanceId) const = 0;
    virtual RefPtr<JsEngine> CreateJsEngineUsingSharedRuntime(int32_t instanceId, void* runtime) const = 0;
    virtual RefPtr<BaseCanvasBridge> CreateCanvasBridge() const = 0;
    virtual RefPtr<BaseXComponentBridge> CreateXComponentBridge() const = 0;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_JS_ENGINE_LOADER_H
