/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ipc_server_listenermgr.h"

#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(IpcServerListenermgr);

int32_t IpcServerListenermgr::RegisterListener(std::string &pkgName, const CommonSvcId *svcId)
{
    if (pkgName == "" || svcId == nullptr) {
        LOGE("invalid param");
        return DM_FAILED;
    }
    LOGI("new listener register:%s", pkgName.c_str());
    std::lock_guard<std::mutex> autoLock(lock_);
    dmListenerMap_[pkgName] = *svcId;
    return DM_OK;
}

int32_t IpcServerListenermgr::GetListenerByPkgName(std::string &pkgName, CommonSvcId *svcId)
{
    if (pkgName == "" || svcId == nullptr) {
        LOGE("invalid param");
        return DM_FAILED;
    }
    std::lock_guard<std::mutex> autoLock(lock_);
    std::map<std::string, CommonSvcId>::iterator iter = dmListenerMap_.find(pkgName);
    if (iter == dmListenerMap_.end()) {
        LOGE("listener not found for pkg:%s", pkgName.c_str());
        return DM_FAILED;
    }
    *svcId = iter->second;
    return DM_OK;
}

int32_t IpcServerListenermgr::UnregisterListener(std::string &pkgName)
{
    std::lock_guard<std::mutex> autoLock(lock_);
    dmListenerMap_.erase(pkgName);
    return DM_OK;
}

const std::map<std::string, CommonSvcId> &IpcServerListenermgr::GetAllListeners()
{
    return dmListenerMap_;
}
} // namespace DistributedHardware
} // namespace OHOS
