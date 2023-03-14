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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_CONTAINER_SCOPE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_CONTAINER_SCOPE_H

#include <functional>
#include <stdint.h>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

class ACE_EXPORT ContainerScope {
public:
    explicit ContainerScope(int32_t id)
    {
        restoreId_ = ContainerScope::CurrentId();
        ContainerScope::UpdateCurrent(id);
    }

    ~ContainerScope()
    {
        ContainerScope::UpdateCurrent(restoreId_);
    }

    static int32_t CurrentId();

    static void UpdateCurrent(int32_t id);

    static void SetScopeNotify(std::function<void(int32_t)>&& notify);

private:
#ifndef WINDOWS_PLATFORM
    static thread_local int32_t currentId_;
#else
    static int32_t currentId_;
#endif
    int32_t restoreId_ = -1;
    static std::function<void(int32_t)> updateScopeNotify_;

    ACE_DISALLOW_COPY_AND_MOVE(ContainerScope);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_CONTAINER_SCOPE_H
