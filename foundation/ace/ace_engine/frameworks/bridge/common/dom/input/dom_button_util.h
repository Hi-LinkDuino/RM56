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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_BUTTON_UTIL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_BUTTON_UTIL_H

#include "core/components/button/button_component.h"
#include "core/components/button/button_theme.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/padding/padding_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/component.h"
#include "frameworks/bridge/common/dom/dom_input.h"

namespace OHOS::Ace::Framework {

class DOMButtonUtil final {
public:
    DOMButtonUtil() = delete;
    ~DOMButtonUtil() = delete;

    static RefPtr<ButtonTheme> GetTheme(const NodeId nodeId);

    static RefPtr<ButtonComponent> CreateComponentAndSetChildAttr(
        const std::map<std::string, std::string>& attrs, DOMInput& node);

    static void SetChildAttr(const RefPtr<ButtonComponent>& component, const std::map<std::string, std::string>& attrs,
        const RefPtr<ButtonTheme>& theme);

    static void SetChildStyle(const RefPtr<BoxComponent>& boxComponent, const RefPtr<ButtonComponent>& component,
        const std::map<std::string, std::string>& styles, DOMInput& node);

    static void AddChildEvent(const RefPtr<ButtonComponent>& component, int32_t pageId, const std::string& nodeId,
        const std::vector<std::string>& events);

private:
    static void InitDefaultValue(const RefPtr<ButtonComponent>& component, const RefPtr<TextComponent>& textChild,
        const RefPtr<ButtonTheme>& theme);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_BUTTON_UTIL_H
