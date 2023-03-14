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

#include "ipc_skeleton.h"

#include "app_log_wrapper.h"
#include "installd/installd_service.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    using namespace OHOS::AppExecFwk;
    std::unique_ptr<InstalldService> service = std::make_unique<InstalldService>();
    if (!service) {
        APP_LOGE("fail to create the installd service");
        std::exit(EXIT_FAILURE);
    }
    service->Start();
    if (!service->IsServiceReady()) {
        APP_LOGW("can't start the installd service, try again");
        service->Start();
        if (!service->IsServiceReady()) {
            APP_LOGE("fail to start the installd service");
            std::exit(EXIT_FAILURE);
        }
    }
    OHOS::IPCSkeleton::JoinWorkThread();
    service->Stop();
    APP_LOGE("installd service stopped");
    return EXIT_SUCCESS;
}