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
#include "hiview_service_adapter.h"

#include "logger.h"

#include "hiview_service_ability.h"

#include "hiview_service.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-ServiceAdapter");
void HiviewServiceAdapter::StartService(HiviewService *service) const
{
    if (service == nullptr) {
        HIVIEW_LOGW("failed to start service.");
        return;
    }

    HIVIEW_LOGD("start service.");
    HiviewServiceAbility::StartService(service);
}
}  // namespace HiviewDFX
}  // namespace OHOS
