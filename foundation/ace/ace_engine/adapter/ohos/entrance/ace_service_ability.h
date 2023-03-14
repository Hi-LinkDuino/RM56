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

#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_SERVICE_ABILITY_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_SERVICE_ABILITY_H

#include <string>
#include <vector>

#include "ability.h"
#include "ability_loader.h"
#include "iremote_object.h"
#include "want.h"

#include "adapter/ohos/entrance/pa_engine/pa_backend.h"

namespace OHOS::Ace {

class AceServiceAbility final : public OHOS::AppExecFwk::Ability {
public:
    AceServiceAbility()
    {
        abilityId_ = instanceId_;
        instanceId_++;
    }
    virtual ~AceServiceAbility() = default;

    void OnStart(const OHOS::AAFwk::Want& want) override;
    void OnStop() override;
    sptr<IRemoteObject> OnConnect(const OHOS::AAFwk::Want &want) override;
    void OnDisconnect(const OHOS::AAFwk::Want &want) override;
    void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;

private:
    int32_t abilityId_ = 100000;

    static int32_t instanceId_;
    static const std::string START_PARAMS_KEY;
    static const std::string URI;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_SERVICE_ABILITY_H
