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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ENGINE_LOADER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ENGINE_LOADER_H

#include "base/utils/singleton.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine_loader.h"

namespace OHOS::Ace::Framework {

class V8EngineLoader : public JsEngineLoader, public Singleton<V8EngineLoader> {
    DECLARE_SINGLETON(V8EngineLoader);

public:
    RefPtr<JsEngine> CreateJsEngine(int32_t instanceId) const final;
    RefPtr<JsEngine> CreateJsEngineUsingSharedRuntime(int32_t instanceId, void* runtime) const override
    {
        return nullptr;
    }
    RefPtr<BaseCanvasBridge> CreateCanvasBridge() const final;
    RefPtr<BaseXComponentBridge> CreateXComponentBridge() const final;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ENGINE_LOADER_H
