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
#include "faultlog_query_result_impl.h"

#include <memory>
#include <string>

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"

#include "refbase.h"

#include "faultlog_info_ohos.h"
#include "faultlogger_service_proxy.h"

namespace OHOS {
namespace HiviewDFX {
constexpr int32_t DFX_HIVIEW_FAULTLOGGER_ID = 1202;
FaultLogQueryResultImpl::~FaultLogQueryResultImpl()
{
    sptr<ISystemAbilityManager> serviceManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (serviceManager == nullptr) {
        return;
    }

    auto service = serviceManager->CheckSystemAbility(DFX_HIVIEW_FAULTLOGGER_ID);
    if (service == nullptr) {
        return;
    }

    FaultLoggerServiceProxy faultloggerService(service);
    faultloggerService.Destroy();
}

std::unique_ptr<FaultLogInfo> FaultLogQueryResultImpl::GetNext()
{
    if (impl_ == nullptr) {
        return nullptr;
    }

    auto info = impl_->GetNext();
    if (info == nullptr) {
        return nullptr;
    }

    auto ret = std::make_unique<FaultLogInfo>();
    ret->SetRawFileDescriptor(info->fd);
    ret->SetId(info->uid);
    ret->SetProcessId(info->pid);
    ret->SetTimeStamp(info->time);
    ret->SetFaultType(info->faultLogType);
    ret->SetModuleName(info->module);
    ret->SetFaultSummary(info->summary);
    ret->SetFaultReason(info->reason);
    return ret;
}

bool FaultLogQueryResultImpl::HasNext()
{
    if (impl_ == nullptr) {
        return false;
    }

    return impl_->HasNext();
}
}  // namespace HiviewDFX
}  // namespace OHOS