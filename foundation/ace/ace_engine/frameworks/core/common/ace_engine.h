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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_ENGINE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_ENGINE_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "core/common/ace_page.h"
#include "core/common/container.h"
#include "core/common/frontend.h"
#include "core/common/watch_dog.h"
#include "core/common/window.h"

namespace OHOS::Ace {

class AcePage;

class ACE_EXPORT AceEngine {
public:
    ~AceEngine() = default;

    void AddContainer(int32_t instanceId, const RefPtr<Container>& container);
    void RemoveContainer(int32_t instanceId);
    RefPtr<Container> GetContainer(int32_t instanceId);
    void RegisterToWatchDog(int32_t instanceId, const RefPtr<TaskExecutor>& taskExecutor, bool useUIAsJSThread = false);
    void BuriedBomb(int32_t instanceId, uint64_t bombId);
    void DefusingBomb(int32_t instanceId);
    static AceEngine& Get();
    void Dump(const std::vector<std::string>& params) const;

    void TriggerGarbageCollection();
    void NotifyContainers(const std::function<void(const RefPtr<Container>&)>& callback);

private:
    AceEngine();

    mutable std::mutex mutex_;
    std::unordered_map<int32_t, RefPtr<Container>> containerMap_;
    RefPtr<WatchDog> watchDog_;

    ACE_DISALLOW_COPY_AND_MOVE(AceEngine);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_ENGINE_H
