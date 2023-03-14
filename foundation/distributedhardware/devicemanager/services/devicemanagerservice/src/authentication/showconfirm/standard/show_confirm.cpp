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

#include "show_confirm.h"

#include "dm_auth_manager.h"
#include "dm_constants.h"
#include "dm_log.h"
#ifdef SUPPORT_GRAPHICS
#include "ui_service_mgr_client.h"
#endif

namespace OHOS {
namespace DistributedHardware {
ShowConfirm::ShowConfirm()
{
    LOGI("ShowConfirm ace constructor");
}

ShowConfirm::~ShowConfirm()
{
}

void ShowConfirm::ShowConfirmDialog(const std::string &params, std::shared_ptr<DmAuthManager> authManager,
                                    std::shared_ptr<DmAbilityManager> dmAbilityMgr)
{
#ifdef SUPPORT_GRAPHICS
    LOGI("ShowConfirm ace start");
    if (authManager == nullptr) {
        LOGE("ShowConfirm::dmAbilityManager is null");
        return;
    }
    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        "config_dialog_service",
        params,
        OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
        ACE_X, ACE_Y, ACE_WIDTH, ACE_HEIGHT,
        [authManager](int32_t id, const std::string& event, const std::string& params) {
            if (params == EVENT_INIT_CODE) {
                LOGI("Dialog start id:%d,event:%s,parms:%s", id, event.c_str(), params.c_str());
                authManager->SetPageId(id);
            } else {
                Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
                LOGI("CancelDialog start id:%d,event:%s,parms:%s", id, event.c_str(), params.c_str());
                authManager->StartAuthProcess(atoi(params.c_str()));
            }
        });
    LOGI("ShowConfirm ace end");
#endif
}
} // namespace DistributedHardware
} // namespace OHOS