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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine_loader.h"

#include "base/utils/macros.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_xcomponent_bridge.h"

namespace OHOS::Ace::Framework {

JsiEngineLoader::JsiEngineLoader() = default;
JsiEngineLoader::~JsiEngineLoader() = default;

RefPtr<JsEngine> JsiEngineLoader::CreateJsEngine(int32_t instanceId) const
{
    return AceType::MakeRefPtr<JsiEngine>(instanceId);
}

RefPtr<BaseCanvasBridge> JsiEngineLoader::CreateCanvasBridge() const
{
    return AceType::MakeRefPtr<JsiCanvasBridge>();
}

RefPtr<BaseXComponentBridge> JsiEngineLoader::CreateXComponentBridge() const
{
    return AceType::MakeRefPtr<JsiXComponentBridge>();
}

#if defined(BUILT_IN_JS_ENGINE)
JsEngineLoader& JsEngineLoader::Get(const char*)
{
    return JsiEngineLoader::GetInstance();
}
#else
extern "C" ACE_EXPORT void* OHOS_ACE_GetJsEngineLoader()
{
    return &JsiEngineLoader::GetInstance();
}
#endif

} // namespace OHOS::Ace::Framework
