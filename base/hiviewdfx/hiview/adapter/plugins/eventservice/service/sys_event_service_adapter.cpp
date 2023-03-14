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

#include "sys_event_service_adapter.h"

#include "logger.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-SysEventServiceAdapter");
void SysEventServiceAdapter::StartService(SysEventServiceBase* service, OHOS::HiviewDFX::NotifySysEvent notify)
{
    if (service == nullptr) {
        HIVIEW_LOGE("failed to start SysEventService.");
        return;
    }
    OHOS::HiviewDFX::SysEventServiceOhos::StartService(service, notify);
}

void SysEventServiceAdapter::OnSysEvent(std::shared_ptr<SysEvent> &event)
{
    if (event == nullptr) {
        HIVIEW_LOGE("sys event is nullptr.");
        return;
    }
    OHOS::HiviewDFX::SysEventServiceOhos::GetInstance().OnSysEvent(event);
}

void SysEventServiceAdapter::BindGetTagFunc(const GetTagByDomainNameFunc& getTagFunc)
{
    OHOS::HiviewDFX::SysEventServiceOhos::GetInstance().BindGetTagFunc(getTagFunc);
}
}  // namespace HiviewDFX
}  // namespace OHOS