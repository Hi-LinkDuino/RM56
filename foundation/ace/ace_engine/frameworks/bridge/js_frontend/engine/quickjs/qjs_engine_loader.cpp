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

#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine_loader.h"

#include "base/utils/macros.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_xcomponent_bridge.h"

namespace OHOS::Ace::Framework {

QjsEngineLoader::QjsEngineLoader() = default;
QjsEngineLoader::~QjsEngineLoader() = default;

RefPtr<JsEngine> QjsEngineLoader::CreateJsEngine(int32_t instanceId) const
{
    return AceType::MakeRefPtr<QjsEngine>(instanceId);
}

RefPtr<BaseCanvasBridge> QjsEngineLoader::CreateCanvasBridge() const
{
    return AceType::MakeRefPtr<CanvasBridge>();
}

RefPtr<BaseXComponentBridge> QjsEngineLoader::CreateXComponentBridge() const
{
    return AceType::MakeRefPtr<QjsXComponentBridge>();
}

#if defined(BUILT_IN_JS_ENGINE)
JsEngineLoader& JsEngineLoader::Get(const char*)
{
    return QjsEngineLoader::GetInstance();
}
#else
extern "C" ACE_EXPORT void* OHOS_ACE_GetJsEngineLoader()
{
    return &QjsEngineLoader::GetInstance();
}
#endif

} // namespace OHOS::Ace::Framework
