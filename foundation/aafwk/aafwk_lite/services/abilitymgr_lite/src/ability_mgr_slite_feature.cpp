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

#include "ability_mgr_slite_feature.h"

#include "ability_errors.h"
#include "ability_info.h"
#include "ability_message_id.h"
#include "ability_service_interface.h"
#include "abilityms_log.h"
#include "adapter.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "securec.h"
#include "utils.h"
#include "want_utils.h"

namespace OHOS {
AbilityMgrSliteFeatureImpl g_amsSliteImpl = {
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .StartAbility = AbilityMgrSliteFeature::StartAbility,
    .TerminateAbility = AbilityMgrSliteFeature::TerminateAbility,
    .SchedulerLifecycleDone = AbilityMgrSliteFeature::SchedulerLifecycleDone,
    .ForceStopBundle = AbilityMgrSliteFeature::ForceStopBundle,
    .GetTopAbility = AbilityMgrSliteFeature::GetTopAbility,
    DEFAULT_IUNKNOWN_ENTRY_END
};

static void Init()
{
    SamgrLite *samgrLite = SAMGR_GetInstance();
    CHECK_NULLPTR_RETURN(samgrLite, "AbilityMgrSliteFeature", "get samgr error");
    BOOL result = samgrLite->RegisterFeature(AMS_SERVICE, AbilityMgrSliteFeature::GetInstance());
    if (result == FALSE) {
        PRINTE("AbilityMgrSliteFeature", "ams register feature failure");
        return;
    }
    g_amsSliteImpl.ams = AbilityMgrSliteFeature::GetInstance();
    auto publicApi = GET_IUNKNOWN(g_amsSliteImpl);
    CHECK_NULLPTR_RETURN(publicApi, "AbilityMgrSliteFeatureLite", "publicApi is nullptr");
    BOOL apiResult = samgrLite->RegisterFeatureApi(AMS_SERVICE, AMS_SLITE_FEATURE, publicApi);
    PRINTI("AbilityMgrSliteFeature", "ams feature init %{public}s", apiResult ? "success" : "failure");
}
SYSEX_FEATURE_INIT(Init);

AbilityMgrSliteFeature::AbilityMgrSliteFeature() : Feature(), identity_()
{
    this->Feature::GetName = AbilityMgrSliteFeature::GetFeatureName;
    this->Feature::OnInitialize = AbilityMgrSliteFeature::OnFeatureInitialize;
    this->Feature::OnStop = AbilityMgrSliteFeature::OnFeatureStop;
    this->Feature::OnMessage = AbilityMgrSliteFeature::OnFeatureMessage;
}

const char *AbilityMgrSliteFeature::GetFeatureName(Feature *feature)
{
    (void) feature;
    return AMS_SLITE_FEATURE;
}

void AbilityMgrSliteFeature::OnFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    CHECK_NULLPTR_RETURN(feature, "AbilityMgrSliteFeature", "initialize fail");
    (static_cast<AbilityMgrSliteFeature *>(feature))->identity_ = identity;
    AbilityService::GetInstance().StartLauncher();
}

void AbilityMgrSliteFeature::OnFeatureStop(Feature *feature, Identity identity)
{
    (void) feature;
    (void) identity;
}

BOOL AbilityMgrSliteFeature::OnFeatureMessage(Feature *feature, Request *request)
{
    if (feature == nullptr || request == nullptr) {
        return FALSE;
    }
    return TRUE;
}

int32 AbilityMgrSliteFeature::StartAbility(const Want *want)
{
    int32 ret = AbilityService::GetInstance().StartAbility(want);
    ClearWant(const_cast<Want *>(want));
    return ret;
}

int32 AbilityMgrSliteFeature::TerminateAbility(uint64_t token)
{
    uint16_t slitToken = token & 0xFFFF;
    return AbilityService::GetInstance().TerminateAbility(slitToken);
}

int32 AbilityMgrSliteFeature::SchedulerLifecycleDone(uint64_t token, int state)
{
    return AbilityService::GetInstance().SchedulerLifecycleDone(token, state);
}

int32 AbilityMgrSliteFeature::ForceStopBundle(uint64_t token)
{
    uint16_t slitToken = token & 0xFFFF;
    return AbilityService::GetInstance().ForceStopBundle(slitToken);
}

ElementName *AbilityMgrSliteFeature::GetTopAbility()
{
    return AbilityService::GetInstance().GetTopAbility();
}
} // namespace OHOS
