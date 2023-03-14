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
#include <memory>

#include "hiview_platform.h"
#include "hiview_service.h"

#include "defines.h"
#include "logger.h"

DEFINE_LOG_TAG("HiView-Main");
int main(int argc __UNUSED, char* argv[] __UNUSED)
{
    auto& hiview = OHOS::HiviewDFX::HiviewPlatform::GetInstance();
    // process cmdline
    hiview.ProcessArgsRequest(argc, argv);

    // parse configs and load plugin
    if (!hiview.InitEnvironment()) {
        HIVIEW_LOGW("Fail to init plugin environment. exit");
        return -1;
    }

    // start service
    auto hiviewService = std::make_unique<OHOS::HiviewDFX::HiviewService>();
    hiviewService->StartService();
    return 0;
}
