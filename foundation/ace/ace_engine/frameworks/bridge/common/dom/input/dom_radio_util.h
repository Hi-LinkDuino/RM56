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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_RADIO_UTIL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_RADIO_UTIL_H

#include "core/components/checkable/checkable_component.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components/checkable/radio_group_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/component.h"
#include "frameworks/bridge/common/dom/dom_input.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT DOMRadioUtil final {
public:
    DOMRadioUtil() = delete;
    ~DOMRadioUtil() = delete;

    static RefPtr<RadioComponent<std::string>> CreateComponentAndSetChildAttr(
        const std::map<std::string, std::string>& attrs, DOMInput& node);

    static void SetChildAttr(const DOMInput& node, const RefPtr<RadioComponent<std::string>>& component,
        const std::map<std::string, std::string>& attrs);

    static void SetChildStyle(const RefPtr<BoxComponent>& boxComponent,
        const RefPtr<RadioComponent<std::string>>& component, const std::map<std::string, std::string>& styles);

    static void AddChildEvent(const RefPtr<RadioComponent<std::string>>& component, int32_t pageId,
        const std::string& nodeId, const std::vector<std::string>& events);

    static void RemoveRadioComponent(const DOMInput& node, const RefPtr<RadioComponent<std::string>>& component);

private:
    static void InitDefaultValue(const RefPtr<RadioComponent<std::string>>& component);

    static void CheckRadioGroup(
        const DOMInput& node, const RefPtr<RadioComponent<std::string>>& component, const std::string& name);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_RADIO_UTIL_H
