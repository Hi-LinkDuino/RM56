/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_SELF_CALLBACK_H
#define OHOS_ABILITY_SELF_CALLBACK_H

#include <algorithm>
#include <vector>

#include "ability_manager.h"
#include "liteipc_adapter.h"
#include "nocopyable.h"

namespace OHOS {
class AbilitySelfCallback {
public:
    static AbilitySelfCallback &GetInstance()
    {
        static AbilitySelfCallback instance;
        return instance;
    }
    ~AbilitySelfCallback();
    const SvcIdentity *RegisterAbilitySelfCallback(IAbilityStartCallback &iAbilityStartCallback);
    const IAbilityStartCallback GetCallback();
private:
    AbilitySelfCallback() = default;
    static int32_t Callback(const IpcContext *context, void *ipcMsg, IpcIo *io, void *arg);
    int32_t GenerateLocalServiceId();
    IAbilityStartCallback iAbilityStartCallback_ { nullptr };
    SvcIdentity *svcIdentity_ { nullptr };

    DISALLOW_COPY_AND_MOVE(AbilitySelfCallback);
};
} // namespace
#endif // OHOS_ABILITY_SELF_CALLBACK_H