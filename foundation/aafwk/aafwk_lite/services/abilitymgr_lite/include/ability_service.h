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

#ifndef OHOS_ABILITY_SERVICE_H
#define OHOS_ABILITY_SERVICE_H

#include <stdlib.h>

#include "ability_list.h"
#include "ability_record.h"
#include "ability_stack.h"
#include "adapter.h"
#include "js_app_host.h"
#include "nocopyable.h"
#include "want.h"
#include "slite_ability.h"
#include "ability_state.h"

namespace OHOS {
struct AbilitySvcInfo {
    char *bundleName;
    char *path;
    void *data;
    uint16_t dataLength;
};

class AbilityService : public NoCopyable {
public:
    typedef void (AbilityService::*LifecycleFunc)(uint16_t token);

    struct LifecycleFuncStr {
        int32_t state;
        LifecycleFunc func_ptr;
    };

    static AbilityService& GetInstance()
    {
        static AbilityService instance;
        return instance;
    }
    ~AbilityService() override;
    int32_t StartAbility(const Want *want);
    int32_t TerminateAbility(uint16_t token);
    int32_t ForceStop(char *bundlename);
    int32_t ForceStopBundle(uint16_t token);
    int32_t SchedulerLifecycleDone(uint64_t token, int32_t state);
    ElementName *GetTopAbility();
    void setNativeAbility(const SliteAbility *ability);
    void StartLauncher();
    void CleanWant();
    Want *want_ = nullptr;

private:
    static uint16_t GenerateToken();
    AbilityService();
    int32_t StartAbility(AbilitySvcInfo *info);
    int32_t PreCheckStartAbility(const char *bundleName, const char *path, const void *data, uint16_t dataLength);
    bool CheckResponse(const char *bundleName);
    int32_t SchedulerLifecycle(uint64_t token, int32_t state);
    int32_t SchedulerLifecycleInner(const AbilityRecord *record, int32_t state);
    void SchedulerAbilityLifecycle(SliteAbility *ability, const Want &want, int32_t state);
    int32_t CreateAppTask(AbilityRecord *record);
    void OnActiveDone(uint16_t token);
    void OnBackgroundDone(uint16_t token);
    void OnDestroyDone(uint16_t token);
    void DeleteRecordInfo(uint16_t token);
    bool SendMsgToJsAbility(int32_t msgId, const AbilityRecord *record);
    void SetAbilityState(uint64_t token, int32_t state);
    void UpdataRecord(AbilitySvcInfo *info);
    int32_t ForceStopBundleInner(uint16_t token);
    bool IsValidAbility(AbilityInfo *abilityInfo);

    uint16_t pendingToken_ { 0 };
    AbilityList abilityList_ {};
    AbilityStack abilityStack_ {};
    SliteAbility *nativeAbility_ = nullptr;
    static LifecycleFuncStr lifecycleFuncList_[STATE_BACKGROUND + 1];
};
} // namespace OHOS
#endif  // OHOS_ABILITY_SERVICE_H
