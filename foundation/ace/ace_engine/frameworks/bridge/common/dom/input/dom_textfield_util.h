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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_TEXTFIELD_UTIL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_TEXTFIELD_UTIL_H

#include "core/common/ime/text_input_action.h"
#include "core/common/ime/text_input_type.h"
#include "core/components/text_field/text_field_component.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/component.h"
#include "frameworks/bridge/common/dom/dom_input.h"

namespace OHOS::Ace::Framework {

class DOMTextFieldUtil final {
public:
    DOMTextFieldUtil() = delete;
    ~DOMTextFieldUtil() = delete;

    static RefPtr<TextFieldComponent> CreateComponentAndSetChildAttr(const RefPtr<BoxComponent>& boxComponent,
        const std::string& type, const std::map<std::string, std::string>& attrs, const DOMInput& node);

    static void SetChildAttr(const RefPtr<TextFieldComponent>& component, const RefPtr<BoxComponent>& boxComponent,
        const std::string& type, const std::map<std::string, std::string>& attrs);

    static void SetChildStyle(const RefPtr<BoxComponent>& boxComponent, const RefPtr<TextFieldComponent>& component,
        const std::map<std::string, std::string>& styles, const Border& boxBorder, const DOMInput& node);

    static void UpdateDecorationStyle(const RefPtr<BoxComponent>& boxComponent,
        const RefPtr<TextFieldComponent>& component, const Border& boxBorder, bool hasBoxRadius);

    static void AddChildEvent(const RefPtr<TextFieldComponent>& component, int32_t pageId, const std::string& nodeId,
        const std::vector<std::string>& events);

    static void InitDefaultValue(const RefPtr<BoxComponent>& boxComponent,
                                 const RefPtr<TextFieldComponent>& component, const RefPtr<TextFieldTheme>& theme);

    static void SetDisableStyle(
        const RefPtr<TextFieldComponent>& component, const RefPtr<TextFieldTheme>& theme);

    static void InitController(const RefPtr<TextFieldComponent>& component);

    static bool IsRadiusStyle(const std::string& style);

private:
    static Radius defaultRadius_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_INPUT_DOM_TEXTFIELD_UTIL_H
