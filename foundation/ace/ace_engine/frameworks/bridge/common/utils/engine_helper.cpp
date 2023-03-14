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

#include "frameworks/bridge/common/utils/engine_helper.h"

#include "base/subwindow/subwindow_manager.h"
#include "core/common/container.h"

namespace OHOS::Ace {
std::shared_mutex EngineHelper::mutex_;
std::unordered_map<int32_t, WeakPtr<Framework::JsEngine>> EngineHelper::engineWeakMap_;

void EngineHelper::AddEngine(int32_t id, WeakPtr<Framework::JsEngine> engine)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    engineWeakMap_.emplace(id, engine);
}

RefPtr<Framework::JsEngine> EngineHelper::GetEngine(int32_t id)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (id >= MIN_SUBCONTAINER_ID && id < MIN_PLUGIN_SUBCONTAINER_ID) {
        id = SubwindowManager::GetInstance()->GetParentContainerId(id);
    }
    auto iter = engineWeakMap_.find(id);
    if (iter != engineWeakMap_.end()) {
        return iter->second.Upgrade();
    }
    return nullptr;
}

void EngineHelper::RemoveEngine(int32_t id)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    engineWeakMap_.erase(id);
}

RefPtr<Framework::JsEngine> EngineHelper::GetCurrentEngine()
{
    return GetEngine(Container::CurrentId());
}

Framework::FrontendDelegate* EngineHelper::GetCurrentDelegate()
{
    auto engine = GetCurrentEngine();
    if (engine) {
        return engine->GetFrontend();
    }
    return nullptr;
}

} // namespace OHOS::Ace