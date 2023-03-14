/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_ENGINE_HELPER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_ENGINE_HELPER_H

#include <mutex>
#include <shared_mutex>

#include "base/memory/referenced.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "bridge/js_frontend/engine/common/js_engine.h"
#include "bridge/js_frontend/frontend_delegate.h"

namespace OHOS::Ace {

class ACE_EXPORT_WITH_PREVIEW EngineHelper final {
public:
    static void AddEngine(int32_t id, WeakPtr<Framework::JsEngine> engine);

    static RefPtr<Framework::JsEngine> GetEngine(int32_t id);

    static void RemoveEngine(int32_t id);

    static RefPtr<Framework::JsEngine> GetCurrentEngine();

    static Framework::FrontendDelegate* GetCurrentDelegate();

private:
    static std::unordered_map<int32_t, WeakPtr<Framework::JsEngine>> engineWeakMap_;

    static std::shared_mutex mutex_;

    ACE_DISALLOW_COPY_AND_MOVE(EngineHelper);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_ENGINE_HELPER_H
