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
#include "faultlogger_service_ohos.h"

#include <functional>
#include <string>
#include <vector>

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"

#include "logger.h"

#include "faultlog_info.h"
#include "faultlog_info_ohos.h"
#include "faultlog_query_result_inner.h"
#include "faultlog_query_result_ohos.h"
#include "faultlogger.h"

DEFINE_LOG_TAG("FaultloggerServiceOhos");
namespace OHOS {
namespace HiviewDFX {
constexpr int32_t UID_SHELL = 2000;
constexpr int32_t UID_ROOT = 0;
constexpr int32_t DFX_ABILITY_FAULTLOGGER_ID = 1202;
void FaultloggerServiceOhos::ClearQueryStub(int32_t uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queries_.erase(uid);
}

int32_t FaultloggerServiceOhos::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    if ((uid != UID_SHELL) && (uid != UID_ROOT)) {
        dprintf(fd, "No permission for uid:%d.\n", uid);
        return -1;
    }

    std::vector<std::string> cmdList;
    std::transform(args.begin(), args.end(), std::back_inserter(cmdList), [](const std::u16string &arg) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
        return convertor.to_bytes(arg);
    });

    auto service = GetOrSetFaultlogger();
    if (service == nullptr) {
        dprintf(fd, "Service is not ready.\n");
        return -1;
    }

    service->Dump(fd, cmdList);
    return 0;
}

void FaultloggerServiceOhos::StartService(OHOS::HiviewDFX::Faultlogger *service)
{
    GetOrSetFaultlogger(service);
    sptr<ISystemAbilityManager> serviceManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (serviceManager == nullptr) {
        HIVIEW_LOGE("Failed to find samgr, exit.");
        return;
    }

    serviceManager->AddSystemAbility(DFX_ABILITY_FAULTLOGGER_ID, &(FaultloggerServiceOhos::GetInstance()));
    HIVIEW_LOGI("FaultLogger Service started.");
}

OHOS::HiviewDFX::Faultlogger *FaultloggerServiceOhos::GetOrSetFaultlogger(
    OHOS::HiviewDFX::Faultlogger *service)
{
    static OHOS::HiviewDFX::Faultlogger *ref = nullptr;
    if (service != nullptr) {
        ref = service;
    }
    return ref;
}

void FaultloggerServiceOhos::AddFaultLog(const FaultLogInfoOhos& info)
{
    auto service = GetOrSetFaultlogger();
    if (service == nullptr) {
        return;
    }

    int32_t uid = IPCSkeleton::GetCallingUid();
    HIVIEW_LOGD("info.uid:%{public}d uid:%{public}d info.pid:%{public}d", info.uid, uid, info.pid);
    if (((uid != static_cast<int32_t>(getuid()))) && (uid != info.uid)) {
        HIVIEW_LOGW("Fail to add fault log, mismatch uid:%{public}d(%{public})", uid, info.uid);
        return;
    }

    FaultLogInfo outInfo;
    outInfo.time = info.time;
    outInfo.id = info.uid;
    outInfo.pid = info.pid;
    outInfo.faultLogType = info.faultLogType;
    outInfo.fd = (info.fd > 0) ? dup(info.fd) : -1;
    outInfo.module = info.module;
    outInfo.reason = info.reason;
    outInfo.summary = info.summary;
    outInfo.logPath = info.logPath;
    outInfo.sectionMap = info.sectionMaps;
    service->AddFaultLog(outInfo);
}

sptr<IRemoteObject> FaultloggerServiceOhos::QuerySelfFaultLog(int32_t faultType, int32_t maxNum)
{
    auto service = GetOrSetFaultlogger();
    if (service == nullptr) {
        return nullptr;
    }

    int32_t uid = IPCSkeleton::GetCallingUid();
    int32_t pid = IPCSkeleton::GetCallingPid();
    std::lock_guard<std::mutex> lock(mutex_);
    if ((queries_.find(uid) != queries_.end()) &&
        (queries_[uid]->pid == pid)) {
        HIVIEW_LOGW("Ongoing query is still alive for uid:%d", uid);
        return nullptr;
    }

    auto queryResult = service->QuerySelfFaultLog(uid, pid, faultType, maxNum);
    if (queryResult == nullptr) {
        HIVIEW_LOGW("Fail to query fault log for uid:%d", uid);
        return nullptr;
    }

    sptr<FaultLogQueryResultOhos> resultRef =
        new FaultLogQueryResultOhos(std::move(queryResult));
    auto queryStub = std::make_unique<FaultLogQuery>();
    queryStub->pid = pid;
    queryStub->ptr = resultRef;
    queries_[uid] = std::move(queryStub);
    return resultRef->AsObject();
}

void FaultloggerServiceOhos::Destroy()
{
    auto service = GetOrSetFaultlogger();
    if (service == nullptr) {
        return;
    }

    int32_t uid = IPCSkeleton::GetCallingUid();
    HIVIEW_LOGI("Destroy Query from uid:%d", uid);
    ClearQueryStub(uid);
}
}  // namespace HiviewDFX
}  // namespace OHOS