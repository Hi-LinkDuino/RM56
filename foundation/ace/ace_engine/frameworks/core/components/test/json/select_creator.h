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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_SELECT_CREATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_SELECT_CREATOR_H

#include "base/json/json_util.h"
#include "core/components/select/select_theme.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class SelectCreator {
public:
    static RefPtr<Component> CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory);

private:
    static bool GetDoubleValue(const JsonValue& json, const std::string& name, double& outValue);
    static bool GetStringValue(const JsonValue& json, const std::string& name, std::string& outValue);
    static bool GetColorValue(const JsonValue& json, const std::string& name, Color& outValue);

    static void CreateCommonAttribute(const RefPtr<Component>& component, const JsonValue& componentJson);
    static void CreateCommonColorAttribute(const RefPtr<Component>& component, const JsonValue& componentJson);
    static void CreateOptionsAttribute(
        const RefPtr<Component>& component, const JsonValue& componentJson, const RefPtr<ThemeManager>& themeManager);
    static void CreateOptionCommonAttribute(const RefPtr<Component>& component, const JsonValue& componentJson);

    static void AdjustTextBySelectedOption(const RefPtr<Component>& component);

    static RefPtr<SelectTheme> theme_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_SELECT_CREATOR_H
