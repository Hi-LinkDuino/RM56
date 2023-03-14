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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_WATCH_DOG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_WATCH_DOG_H

#include <unordered_map>

#include "base/thread/task_executor.h"

namespace OHOS::Ace {

class ThreadWatcher;

struct Watchers {
    RefPtr<ThreadWatcher> jsWatcher;
    RefPtr<ThreadWatcher> uiWatcher;
};

class WatchDog final : public Referenced {
public:
    WatchDog();
    ~WatchDog() override;

    void Register(int32_t instanceId, const RefPtr<TaskExecutor>& taskExecutor, bool useUIAsJSThread);
    void Unregister(int32_t instanceId);
    void BuriedBomb(int32_t instanceId, uint64_t bombId);
    void DefusingBomb(int32_t instanceId);

private:
    std::unordered_map<int32_t, Watchers> watchMap_;

    ACE_DISALLOW_COPY_AND_MOVE(WatchDog);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_WATCH_DOG_H
