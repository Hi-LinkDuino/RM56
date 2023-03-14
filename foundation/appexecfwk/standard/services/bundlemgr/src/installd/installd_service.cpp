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

#include "installd/installd_service.h"

#include <chrono>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <thread>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "system_ability_definition.h"
#include "system_ability_helper.h"

using namespace std::chrono_literals;

namespace OHOS {
namespace AppExecFwk {
InstalldService::InstalldService()
{
    APP_LOGI("installd service instance is created");
}

InstalldService::~InstalldService()
{
    APP_LOGI("installd service instance is destroyed");
}

bool InstalldService::Init()
{
    if (isReady_) {
        APP_LOGW("the installd service is already ready");
        return false;
    }
    // installd service need mask 000
    umask(Constants::INSTALLD_UMASK);
    hostImpl_ = new (std::nothrow) InstalldHostImpl();
    if (hostImpl_ == nullptr) {
        APP_LOGI("InstalldHostImpl Init failed");
        return false;
    }
    if (!InitDir(Constants::THIRD_PARTY_APP_INSTALL_PATH)) {
        APP_LOGI("THIRD_PARTY_APP_INSTALL_PATH Path already exists");
    }
    if (!InitDir(Constants::SYSTEM_APP_INSTALL_PATH)) {
        APP_LOGI("SYSTEM_APP_INSTALL_PATH is already exists");
    }
    if (!InitDir(Constants::EXTRACT_TMP_PATH)) {
        APP_LOGI("EXTRACT_TMP_PATH is already exists");
    }
    if (!InitDir(Constants::HAP_COPY_PATH)) {
        APP_LOGI("HAP_COPY_PATH is already exists");
    }
    return true;
}

bool InstalldService::InitDir(const std::string &path)
{
    if (InstalldOperator::IsExistDir(path)) {
        APP_LOGI("Path already exists");
        return false;
    }
    if (!InstalldOperator::MkOwnerDir(path, true, Constants::BMS_UID, Constants::BMS_GID)) {
        APP_LOGE("create path failed, errno : %{public}d", errno);
        return false;
    }
    return true;
}

void InstalldService::Start()
{
    if (!(Init())) {
        APP_LOGE("init fail");
        return;
    }
    // add installd service to system ability manager.
    // need to retry some times due to installd start faster than system ability manager.
    if (!SystemAbilityHelper::AddSystemAbility(INSTALLD_SERVICE_ID, hostImpl_)) {
        APP_LOGW("installd service fail to register into system ability manager, retry it");
        int32_t tryTimes = 3;
        bool isSuccess = false;
        for (int32_t i = 0; i < tryTimes; i++) {
            std::this_thread::sleep_for(100ms);
            if (SystemAbilityHelper::AddSystemAbility(INSTALLD_SERVICE_ID, hostImpl_)) {
                isSuccess = true;
                break;
            }
        }
        if (!isSuccess) {
            APP_LOGE("installd service fail to register into system ability manager");
            return;
        }
    }
    isReady_ = true;
    APP_LOGI("installd service start successfully");
}

void InstalldService::Stop()
{
    if (!isReady_) {
        APP_LOGW("the installd service is already stopped");
        return;
    }
    // remove installd service from system ability manager.
    // since we can't handle the fail case, just ignore the result.
    SystemAbilityHelper::RemoveSystemAbility(INSTALLD_SERVICE_ID);
    isReady_ = false;
    APP_LOGI("installd service stop successfully");
}
}  // namespace AppExecFwk
}  // namespace OHOS