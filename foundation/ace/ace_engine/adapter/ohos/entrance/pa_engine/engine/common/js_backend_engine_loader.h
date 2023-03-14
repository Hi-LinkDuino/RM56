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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_COMMON_JS_BACKEND_ENGINE_LOADER_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_COMMON_JS_BACKEND_ENGINE_LOADER_H

#include "adapter/ohos/entrance/pa_engine/engine/common/js_backend_engine.h"

namespace OHOS::Ace {

class JsBackendEngineLoader {
public:
    virtual ~JsBackendEngineLoader() = default;

    static JsBackendEngineLoader& Get(const char* sharedLibrary);

    virtual RefPtr<JsBackendEngine> CreateJsBackendEngine(int32_t instanceId) const = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_COMMON_JS_BACKEND_ENGINE_LOADER_H
