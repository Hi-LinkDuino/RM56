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

#include "frameworks/bridge/declarative_frontend/engine/declarative_engine_loader.h"

#include "base/utils/macros.h"

#ifdef USE_V8_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#endif

#ifdef USE_QUICKJS_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine.h"
#endif

#ifdef USE_ARK_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#endif

namespace OHOS::Ace::Framework {

DeclarativeEngineLoader::DeclarativeEngineLoader() = default;
DeclarativeEngineLoader::~DeclarativeEngineLoader() = default;

RefPtr<JsEngine> DeclarativeEngineLoader::CreateJsEngine(int32_t instanceId) const
{
#ifdef USE_V8_ENGINE
    return AceType::MakeRefPtr<V8DeclarativeEngine>(instanceId);
#endif

#ifdef USE_QUICKJS_ENGINE
    return AceType::MakeRefPtr<QJSDeclarativeEngine>(instanceId);
#endif

#ifdef USE_ARK_ENGINE
    return AceType::MakeRefPtr<JsiDeclarativeEngine>(instanceId);
#endif
}

RefPtr<JsEngine> DeclarativeEngineLoader::CreateJsEngineUsingSharedRuntime(int32_t instanceId, void* runtime) const
{
#ifdef USE_ARK_ENGINE
    LOGI("CreateJsEngineUsingSharedRuntime id:%{public}d runtime:%{public}p", instanceId, runtime);
    return AceType::MakeRefPtr<JsiDeclarativeEngine>(instanceId, runtime);
#else
    return nullptr;
#endif
}

RefPtr<BaseCanvasBridge> DeclarativeEngineLoader::CreateCanvasBridge() const
{
    return nullptr;
}

RefPtr<BaseXComponentBridge> DeclarativeEngineLoader::CreateXComponentBridge() const
{
    return nullptr;
}

#if defined(BUILT_IN_JS_ENGINE)
JsEngineLoader& JsEngineLoader::GetDeclarative(const char*)
{
    return DeclarativeEngineLoader::GetInstance();
}
#else
extern "C" ACE_EXPORT void* OHOS_ACE_GetJsEngineLoader()
{
    return &DeclarativeEngineLoader::GetInstance();
}
#endif

} // namespace OHOS::Ace::Framework