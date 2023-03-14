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

#include "abilityms_slite_client.h"

#include "ability_errors.h"
#include "ability_mgr_service.h"
#include "ability_service.h"
#include "abilityms_log.h"
#include "adapter.h"
#include "cmsis_os2.h"
#include "samgr_lite.h"
#include "securec.h"
#include "want_utils.h"
#include "utils.h"

namespace OHOS {
bool AbilityMsClient::Initialize() const
{
    if (amsProxy_ != nullptr) {
        return true;
    }
    int retry = RETRY_TIMES;
    while (retry--) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_SLITE_FEATURE);
        if (iUnknown == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "iUnknown is null");
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }

        (void)iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&amsProxy_);
        if (amsProxy_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "ams proxy is null");
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }
        return true;
    }
    return false;
}

int AbilityMsClient::StartAbility(const Want *want) const
{
    if (want == nullptr || want->element == nullptr || !Initialize()) {
        return PARAM_CHECK_ERROR;
    }

    AbilityMgrService *service = AbilityMgrService::GetInstance();
    if (service == nullptr) {
        return PARAM_CHECK_ERROR;
    }

    // 申请want内存，在服务端用完释放
    Want *info = static_cast<Want *>(AdapterMalloc(sizeof(Want)));
    if (info == nullptr) {
        return MEMORY_MALLOC_ERROR;
    }
    info->element = nullptr;
    info->data = nullptr;
    info->dataLength = 0;
    SetWantElement(info, *(want->element));
    SetWantData(info, want->data, want->dataLength);
    AbilityService::GetInstance().want_ = info;
    Request request = {
        .msgId = START_ABILITY,
        .len = 0,
        .data = nullptr,
        .msgValue = 0,
    };
    return SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
}

int AbilityMsClient::TerminateAbility(uint64_t token) const
{
    AbilityMgrService *service = AbilityMgrService::GetInstance();
    if (service == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = TERMINATE_ABILITY,
        .len = 0,
        .data = nullptr,
        .msgValue = static_cast<uint32_t>(token & 0xFFFF),
    };
    return SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
}

int AbilityMsClient::SchedulerLifecycleDone(uint64_t token, int state) const
{
    AbilityMgrService *service = AbilityMgrService::GetInstance();
    if (service == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = ABILITY_TRANSACTION_DONE,
        .len = 0,
        .data = nullptr,
        .msgValue = static_cast<uint32_t>((token & 0xFF) | (state << 8)),
    };
    return SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
}

int AbilityMsClient::ForceStopBundle(uint64_t token) const
{
    AbilityMgrService *service = AbilityMgrService::GetInstance();
    if (service == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = TERMINATE_APP,
        .len = 0,
        .data = nullptr,
        .msgValue = static_cast<uint32_t>(token & 0xFFFF),
    };
    return SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
}

ElementName *AbilityMsClient::GetTopAbility() const
{
    if (!Initialize()) {
        return nullptr;
    }
    return amsProxy_->GetTopAbility();
}

int AbilityMsClient::ForceStop(char *bundlename) const
{
    AbilityMgrService *service = AbilityMgrService::GetInstance();
    if (service == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    char* name = Utils::Strdup(bundlename);
    Request request = {
        .msgId = TERMINATE_APP_BY_BUNDLENAME,
        .len = (int16)strlen(name),
        .data = reinterpret_cast<void *>(name),
    };

    return SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
}
} //  namespace OHOS
