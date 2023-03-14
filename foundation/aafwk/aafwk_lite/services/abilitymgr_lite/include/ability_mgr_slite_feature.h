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

#ifndef OHOS_ABILITYMANAGERSERVICE_SLITE_FEATURE_H
#define OHOS_ABILITYMANAGERSERVICE_SLITE_FEATURE_H

#include "ability_service.h"
#include "ability_service_interface.h"
#include "feature.h"
#include "iproxy_client.h"
#include "iunknown.h"
#include "nocopyable.h"

namespace OHOS {
class AbilityMgrSliteFeature : public Feature {
public:
    static int32 StartAbility(const Want *want);
    static int32 TerminateAbility(uint64_t token);
    static int32 SchedulerLifecycleDone(uint64_t token, int state);
    static int32 ForceStopBundle(uint64_t token);
    static ElementName *GetTopAbility();

    static AbilityMgrSliteFeature *GetInstance()
    {
        static AbilityMgrSliteFeature instance;
        return &instance;
    }
    ~AbilityMgrSliteFeature() = default;
private:
    AbilityMgrSliteFeature();
    static const char *GetFeatureName(Feature *feature);
    static void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity);
    static void OnFeatureStop(Feature *feature, Identity identity);
    static BOOL OnFeatureMessage(Feature *feature, Request *request);

private:
    Identity identity_;
    AbilityService *abilityService_;
    DISALLOW_COPY_AND_MOVE(AbilityMgrSliteFeature);
};

typedef struct {
    INHERIT_IUNKNOWNENTRY(AmsSliteInterface);
    AbilityMgrSliteFeature *ams;
} AbilityMgrSliteFeatureImpl;
} // namespace OHOS
#endif // OHOS_ABILITYMANAGERSERVICE_SLITE_FEATURE_H
