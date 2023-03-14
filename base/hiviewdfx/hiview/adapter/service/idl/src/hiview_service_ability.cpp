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
#include "hiview_service_ability.h"

#include <mutex>
#include <cstdio>
#include <unistd.h>

#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"

#include "string_util.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiViewSA-HiViewServiceAbility");
namespace {
constexpr int MAXRETRYTIMEOUT = 10;
}
int HiviewServiceAbility::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    auto service = GetOrSetHiviewService(nullptr);
    if (service != nullptr) {
        std::vector<std::string> cmds;
        for (const auto &arg : args) {
            cmds.push_back(StringUtil::ConvertToUTF8(arg));
        }
        service->DumpRequestDispatcher(fd, cmds);
    }
    return 0;
}

HiviewServiceAbility::HiviewServiceAbility() : SystemAbility(DFX_SYS_HIVIEW_ABILITY_ID, true)
{
    HIVIEW_LOGI("begin, cmd : %d", DFX_SYS_HIVIEW_ABILITY_ID);
}

HiviewServiceAbility::~HiviewServiceAbility()
{
    HIVIEW_LOGI("begin, cmd : %d", DFX_SYS_HIVIEW_ABILITY_ID);
}

void HiviewServiceAbility::StartServiceAbility(int sleepS)
{
    HIVIEW_LOGI("called");

    sptr<ISystemAbilityManager> serviceManager;

    int retryTimeout = MAXRETRYTIMEOUT;
    while (retryTimeout > 0) {
        --retryTimeout;
        if (sleepS > 0) {
            sleep(sleepS);
        }

        SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
        serviceManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (serviceManager == nullptr) {
            continue;
        }

        int result = serviceManager->AddSystemAbility(DFX_SYS_HIVIEW_ABILITY_ID, new HiviewServiceAbility());
        if (result != 0) {
            HIVIEW_LOGE("AddSystemAbility error %d", result);
            continue;
        }
        break;
    }

    auto abilityObjext = serviceManager->AsObject();
    if (abilityObjext == nullptr) {
        HIVIEW_LOGE("AsObject() == nullptr");
        return;
    }

    bool ret = abilityObjext->AddDeathRecipient(new HiviewServiceAbilityDeathRecipient());
    if (ret == false) {
        HIVIEW_LOGE("AddDeathRecipient == false");
    }
}

void HiviewServiceAbility::StartService(HiviewService *service)
{
    GetOrSetHiviewService(service);
    StartServiceAbility(0);
    IPCSkeleton::JoinWorkThread();
}

HiviewService *HiviewServiceAbility::GetOrSetHiviewService(HiviewService *service)
{
    static HiviewService *ref = nullptr;
    if (service != nullptr) {
        ref = service;
    }
    return ref;
}

void HiviewServiceAbility::OnDump()
{
    HIVIEW_LOGI("called");
}

void HiviewServiceAbility::OnStart()
{
    HIVIEW_LOGI("called");
}

void HiviewServiceAbility::OnStop()
{
    HIVIEW_LOGI("called");
}

HiviewServiceAbilityDeathRecipient::HiviewServiceAbilityDeathRecipient()
{
    HIVIEW_LOGI("called");
}

HiviewServiceAbilityDeathRecipient::~HiviewServiceAbilityDeathRecipient()
{
    HIVIEW_LOGI("called");
}

void HiviewServiceAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    HIVIEW_LOGI("called");
    if (object == nullptr) {
        return;
    }
    HiviewServiceAbility::StartServiceAbility(1);
}
} // namespace HiviewDFX
} // namespace OHOS
