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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_EXTENSION_FORM_PROFILE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_EXTENSION_FORM_PROFILE_H

#include <string>
#include <vector>
#include "appexecfwk_errors.h"
#include "extension_form_info.h"

namespace OHOS {
namespace AppExecFwk {
namespace ExtensionFormProfileReader {
const std::string FORMS = "forms";
const std::string NAME = "name";
const std::string DESCRIPTION = "description";
const std::string SRC = "src";
const std::string WINDOW = "window";
const std::string WINDOW_DESIGN_WIDTH = "designWidth";
const std::string WINDOW_AUTO_DESIGN_WIDTH = "autoDesignWidth";
const std::string COLOR_MODE = "colorMode";
const std::string FORM_CONFIG_ABILITY = "formConfigAbility";
const std::string FORM_VISIBLE_NOTIFY = "formVisibleNotify";
const std::string IS_DEFAULT = "isDefault";
const std::string UPDATE_ENABLED = "updateEnabled";
const std::string SCHEDULED_UPDATE_TIME = "scheduledUpdateTime";
const std::string UPDATE_DURATION = "updateDuration";
const std::string DEFAULT_DIMENSION = "defaultDimension";
const std::string SUPPORT_DIMENSIONS = "supportDimensions";
const std::string METADATA = "metadata";
const std::string METADATA_NAME = "name";
const std::string METADATA_VALUE = "value";
}

class ExtensionFormProfile {
public:
    /**
     * @brief Transform the form profile to ExtensionFormInfos.
     * @param formProfile Indicates the string of the form profile.
     * @param info Indicates the obtained ExtensionFormProfileInfo.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    static ErrCode TransformTo(const std::string &formProfile, std::vector<ExtensionFormInfo> &infos);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_EXTENSION_FORM_PROFILE_H
