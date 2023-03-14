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

#include "core/common/container_scope.h"

namespace OHOS::Ace {

#ifndef WINDOWS_PLATFORM
thread_local int32_t ContainerScope::currentId_ = -1;
#else
int32_t ContainerScope::currentId_ = -1;
#endif
std::function<void(int32_t)> ContainerScope::updateScopeNotify_;

int32_t ContainerScope::CurrentId()
{
    return currentId_;
}

void ContainerScope::UpdateCurrent(int32_t id)
{
    currentId_ = id;
    if (updateScopeNotify_) {
        updateScopeNotify_(id);
    }
}

void ContainerScope::SetScopeNotify(std::function<void(int32_t)>&& notify)
{
    updateScopeNotify_ = std::move(notify);
}

} // namespace OHOS::Ace