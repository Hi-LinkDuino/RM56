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

#ifndef HIVIEW_SERVICE_ABILITY_H
#define HIVIEW_SERVICE_ABILITY_H

#include <string>
#include <memory>

#include "logger.h"
#include "system_ability.h"
#include "hiview_service_ability_stub.h"
#include "ihiview_service_ability.h"
#include "hiview_service.h"
#include "singleton.h"

namespace OHOS {
namespace HiviewDFX {
class HiviewServiceAbility : public SystemAbility,
    public HiviewServiceAbilityStub {
    DECLARE_SYSTEM_ABILITY(HiviewServiceAbility);

public:
    HiviewServiceAbility();
    ~HiviewServiceAbility();
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    static void StartService(HiviewService *service);
    static void StartServiceAbility(int sleepS);
    static HiviewService* GetOrSetHiviewService(HiviewService *service = nullptr);
protected:
    void OnDump() override;
    void OnStart() override;
    void OnStop() override;
};

class HiviewServiceAbilityDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    HiviewServiceAbilityDeathRecipient();
    ~HiviewServiceAbilityDeathRecipient();
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // IHIVIEW_SERVICE_H