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

#ifndef OHOS_DM_SHOW_CONFIRM_H
#define OHOS_DM_SHOW_CONFIRM_H

#include <cstring>
#include <string>
#include <memory>

#include "dm_auth_manager.h"
#include "dm_ability_manager.h"

namespace OHOS {
namespace DistributedHardware {
class ShowConfirm {
public:
    ShowConfirm();
    ~ShowConfirm();
    void ShowConfirmDialog(const std::string &params,
        std::shared_ptr<DmAuthManager> authManager, std::shared_ptr<DmAbilityManager> dmAbilityMgr);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_SHOW_CONFIRM_H