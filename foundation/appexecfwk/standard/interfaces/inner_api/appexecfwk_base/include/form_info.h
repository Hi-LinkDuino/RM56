/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_FORM_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_FORM_INFO_H

#include <string>
#include "extension_ability_info.h"
#include "extension_form_info.h"
#include "form_info_base.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
enum class FormType {
    JAVA = 0,
    JS = 1,
};

struct FormInfo : public Parcelable {
    std::string package;
    std::string bundleName;
    std::string originalBundleName;
    std::string relatedBundleName;
    std::string moduleName;  // the "module.distro.moduleName" in config.json
    std::string abilityName;
    std::string name;
    std::string description;
    std::string jsComponentName;
    std::string deepLink;
    std::string formConfigAbility;
    std::string scheduledUpdateTime = "";
    std::string src;
    FormWindow window;
    int32_t descriptionId = 0;
    int32_t updateDuration = 0;
    int32_t defaultDimension = 0;
    bool defaultFlag = false;
    bool formVisibleNotify = false;
    bool updateEnabled = false;
    FormType type = FormType::JS;
    std::vector<int32_t> supportDimensions;
    FormsColorMode colorMode = FormsColorMode::AUTO_MODE;
    std::vector<std::string> landscapeLayouts;
    std::vector<std::string> portraitLayouts;
    std::vector<FormCustomizeData> customizeDatas;

    FormInfo() = default;
    explicit FormInfo(const ExtensionAbilityInfo &abilityInfo, const ExtensionFormInfo &formInfo);

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static FormInfo *Unmarshalling(Parcel &parcel);
    bool IsValid() const;

private:
    bool ReadCustomizeData(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_FORM_H
