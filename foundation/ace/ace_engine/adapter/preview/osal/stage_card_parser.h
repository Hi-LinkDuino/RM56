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

#ifndef FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_STAGE_CARD_PARSER_H
#define FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_STAGE_CARD_PARSER_H

#include <string>

#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "frameworks/base/json/json_util.h"
#include "frameworks/bridge/common/manifest/manifest_window.h"

namespace OHOS::Ace {

class StageCardParser : public Referenced {
public:
    StageCardParser();
    ~StageCardParser() override = default;
    void Parse(const std::string& contents, const std::string& selectUrl);
    const std::string& GetColorMode() const;
    const std::string& GetDefaultDimension() const;
    const std::string& GetDescription() const;
    const std::string& GetFormConfigAbility() const;
    bool GetIsDefault() const;
    const std::string& GetName() const;
    const std::string& GetScheduledUpdateTime() const;
    const std::string& GetSrc() const;
    const std::vector<std::string>& GetSupportDimensions() const;
    uint32_t GetUpdateDuration() const;
    bool GetUpdateEnabled() const;
    WindowConfig& GetWindowConfig() const;

private:
    std::string colorMode_;
    std::string defaultDimension_;
    std::string description_;
    std::string formConfigAbility_;
    bool isDefault_;
    std::string name_;
    std::string scheduledUpdateTime_;
    std::string src_;
    std::vector<std::string> supportDimensions_;
    uint32_t updateDuration_;
    bool updateEnabled_;
    RefPtr<Framework::ManifestWindow> manifestWindow_;
    ACE_DISALLOW_COPY_AND_MOVE(StageCardParser);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_STAGE_CARD_PARSER_H
