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

#include "installd_death_recipient.h"

#include "app_log_wrapper.h"
#include "installd_client.h"

namespace OHOS {
namespace AppExecFwk {
void InstalldDeathRecipient::OnRemoteDied([[maybe_unused]] const wptr<IRemoteObject> &remote)
{
    APP_LOGI("installd service died, remove the proxy object");
    InstalldClient::GetInstance()->ResetInstalldProxy();
}
}  // namespace AppExecFwk
}  // namespace OHOS