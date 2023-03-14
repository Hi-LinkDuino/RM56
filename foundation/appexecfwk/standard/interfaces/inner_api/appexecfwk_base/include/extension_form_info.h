/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_EXTENSION_FORM_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_EXTENSION_FORM_INFO_H

#include <string>
#include <vector>
#include "form_info_base.h"

namespace OHOS {
namespace AppExecFwk {
struct ExtensionFormInfo {
    std::string name;
    std::string description;
    std::string src;
    FormWindow window;
    FormsColorMode colorMode = FormsColorMode::AUTO_MODE;
    std::string formConfigAbility;
    bool formVisibleNotify = false;
    bool isDefault = false;
    bool updateEnabled = false;
    std::string scheduledUpdateTime = "";
    int32_t updateDuration = 0;
    std::int32_t defaultDimension;
    std::vector<int32_t> supportDimensions {};
    std::vector<FormCustomizeData> metadata {};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_EXTENSION_FORM_INFO_H
