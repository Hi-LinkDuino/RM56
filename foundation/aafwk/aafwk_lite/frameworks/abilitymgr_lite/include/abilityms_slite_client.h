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

#ifndef OHOS_ABILITYMS_SLITE_CLIENT_H
#define OHOS_ABILITYMS_SLITE_CLIENT_H

#include "iproxy_client.h"
#include "nocopyable.h"
#include "want.h"
#include "ability_service_interface.h"

namespace OHOS {
class AbilityMsClient {
public:
    static AbilityMsClient &GetInstance()
    {
        static AbilityMsClient instance;
        return instance;
    }

    ~AbilityMsClient() = default;

    bool Initialize() const;

    int StartAbility(const Want *want) const;

    int TerminateAbility(uint64_t token) const;

    int SchedulerLifecycleDone(uint64_t token, int state) const;

    int ForceStopBundle(uint64_t token) const;

    int ForceStop(char *bundlename) const;

    ElementName *GetTopAbility() const;

private:
    AbilityMsClient() = default;

    AmsSliteInterface *amsProxy_ { nullptr };

    DISALLOW_COPY_AND_MOVE(AbilityMsClient);
};
} // namespace OHOS

#endif // OHOS_ABILITYMS_SLITE_CLIENT_H
