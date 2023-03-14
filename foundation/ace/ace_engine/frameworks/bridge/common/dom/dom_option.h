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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_OPTION_H

#include "core/components/option/option_component.h"
#include "core/components/select/select_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMOption final : public DOMNode {
    DECLARE_ACE_TYPE(DOMOption, DOMNode);

public:
    DOMOption(NodeId nodeId, const std::string& nodeName);
    ~DOMOption() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return selectOptionComponent_;
    }

    void InitializeStyle() override;

    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void OnSetStyleFinished() override;
#endif

private:
    RefPtr<SelectTheme> theme_;
    RefPtr<OptionComponent> selectOptionComponent_;
    RefPtr<TextComponent> content_;
    RefPtr<ImageComponent> icon_;
    // used for inspector node in PC preview
    std::vector<std::pair<std::string, std::string>> styles_;
    std::vector<std::pair<std::string, std::string>> attrs_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_OPTION_H
