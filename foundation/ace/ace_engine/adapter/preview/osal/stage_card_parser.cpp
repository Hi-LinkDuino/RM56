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

#include "adapter/preview/osal/stage_card_parser.h"

namespace OHOS::Ace {

StageCardParser::StageCardParser() : manifestWindow_(Referenced::MakeRefPtr<Framework::ManifestWindow>())
{}

void StageCardParser::Parse(const std::string& contents, const std::string& selectUrl)
{
    auto rootJson = JsonUtil::ParseJsonString(contents);
    if (!rootJson || !rootJson->IsValid()) {
        LOGE("the form config is illegal");
        return;
    }
    std::unique_ptr<JsonValue> formConfig;
    auto formConfigs = rootJson->GetValue("forms");
    int32_t index = 0;
    if (formConfigs && formConfigs->IsArray()) {
        for (; index < formConfigs->GetArraySize(); ++index) {
            formConfig = formConfigs->GetArrayItem(index);
            if (formConfig && formConfig->Contains("src") && formConfig->GetString("src") == selectUrl) {
                break;
            }
        }
    }
    if (index == formConfigs->GetArraySize()) {
        LOGE("The configuration information for the url %{public}s does not exist", selectUrl.c_str());
        return;
    }

    auto supportDimensions = formConfig->GetValue("supportDimensions");
    if (supportDimensions && supportDimensions->IsArray()) {
        for (index = 0; index < supportDimensions->GetArraySize(); ++index) {
            auto supportDimension = supportDimensions->GetArrayItem(index);
            if (supportDimension && supportDimension->IsString()) {
                supportDimensions_.push_back(supportDimension->GetString());
            }
        }
    }

    colorMode_ = formConfig->GetString("colorMode", "auto");
    defaultDimension_ = formConfig->GetString("defaultDimension");
    description_ = formConfig->GetString("description");
    formConfigAbility_ = formConfig->GetString("formConfigAbility");
    isDefault_ = formConfig->GetBool("updateEnabled", true);
    name_ = formConfig->GetString("name");
    scheduledUpdateTime_ = formConfig->GetString("scheduledUpdateTime");
    src_ = formConfig->GetString("src");
    updateDuration_ = formConfig->GetUInt("updateDuration", 1);
    updateEnabled_ = formConfig->GetBool("updateEnabled", true);
    manifestWindow_->WindowParse(formConfig);
}

const std::string& StageCardParser::GetColorMode() const
{
    return colorMode_;
}

const std::string& StageCardParser::GetDefaultDimension() const
{
    return defaultDimension_;
}

const std::string& StageCardParser::GetDescription() const
{
    return description_;
}

const std::string& StageCardParser::GetFormConfigAbility() const
{
    return formConfigAbility_;
}

bool StageCardParser::GetIsDefault() const
{
    return isDefault_;
}

const std::string& StageCardParser::GetName() const
{
    return name_;
}

const std::string& StageCardParser::GetScheduledUpdateTime() const
{
    return scheduledUpdateTime_;
}

const std::string& StageCardParser::GetSrc() const
{
    return src_;
}

const std::vector<std::string>& StageCardParser::GetSupportDimensions() const
{
    return supportDimensions_;
}

uint32_t StageCardParser::GetUpdateDuration() const
{
    return updateDuration_;
}

bool StageCardParser::GetUpdateEnabled() const
{
    return updateEnabled_;
}

WindowConfig& StageCardParser::GetWindowConfig() const
{
    return manifestWindow_->GetWindowConfig();
}

} // namespace OHOS::Ace::Framework
