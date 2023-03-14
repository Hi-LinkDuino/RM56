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
#include "faultlogger_client.h"

#include <unistd.h>

#include "hilog/log_cpp.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "refbase.h"

#include "faultlog_info_ohos.h"
#include "faultlog_query_result.h"
#include "faultlog_query_result_impl.h"

#include "faultlogger_service_proxy.h"
#include "ifaultlogger_service.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD002D10, "FaultloggerClient"};
constexpr int32_t DFX_HIVIEW_FAULTLOGGER_ID = 1202;
sptr<FaultLoggerServiceProxy> GetFaultloggerService()
{
    sptr<ISystemAbilityManager> serviceManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (serviceManager == nullptr) {
        OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "Failed to find samgr, exit.");
        return nullptr;
    }

    auto service = serviceManager->CheckSystemAbility(DFX_HIVIEW_FAULTLOGGER_ID);
    if (service == nullptr) {
        OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "Failed to find faultlogger service, exit.");
        return nullptr;
    }

    sptr<FaultLoggerServiceProxy> proxy = new FaultLoggerServiceProxy(service);
    return proxy;
}

void AddFaultLog(const FaultLogInfoInner &info)
{
    auto service = GetFaultloggerService();
    if (service == nullptr) {
        OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "Fail to get service.");
        return;
    }

    std::string module = info.module;
    module.erase(std::remove_if(module.begin(), module.end(),
		[](unsigned char c) { return !std::isprint(c); }), module.end());

    FaultLogInfoOhos infoOhos;
    infoOhos.time = info.time;
    infoOhos.uid = info.id;
    infoOhos.pid = info.pid;
    infoOhos.faultLogType = info.faultLogType;
    infoOhos.module = module;
    infoOhos.reason = info.reason;
    infoOhos.summary = info.summary;
    infoOhos.logPath = info.logPath;
    infoOhos.sectionMaps = info.sectionMaps;
    infoOhos.fd = -1;
    service->AddFaultLog(infoOhos);
}

void AddFaultLog(int64_t time, int32_t logType, const std::string &module, const std::string &summary)
{
    auto service = GetFaultloggerService();
    if (service == nullptr) {
        OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "Fail to get service.");
        return;
    }

    FaultLogInfoOhos infoOhos;
    infoOhos.time = time;
    infoOhos.uid = getuid();
    infoOhos.pid = getpid();
    infoOhos.faultLogType = logType;
    infoOhos.module = module;
    infoOhos.summary = summary;
    infoOhos.fd = -1;
    service->AddFaultLog(infoOhos);
}

std::unique_ptr<FaultLogQueryResult> QuerySelfFaultLog(FaultLogType faultType, int32_t maxNum)
{
    auto service = GetFaultloggerService();
    if (service == nullptr) {
        OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "Fail to get service.");
        return nullptr;
    }

    auto result = service->QuerySelfFaultLog(static_cast<int32_t>(faultType), maxNum);
    if (result == nullptr) {
        OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "Fail to query result.");
        return nullptr;
    }

    sptr<FaultLogQueryResultProxy> proxy = new FaultLogQueryResultProxy(result);
    return std::make_unique<FaultLogQueryResult>(new FaultLogQueryResultImpl(proxy));
}
}  // namespace HiviewDFX
}  // namespace OHOS

__attribute__((visibility ("default"))) void AddFaultLog(FaultLogInfoInner* info)
{
    OHOS::HiviewDFX::AddFaultLog(*info);
}