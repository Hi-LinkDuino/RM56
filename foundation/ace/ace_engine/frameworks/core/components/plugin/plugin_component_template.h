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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_COMPONENT_TEMPLATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_COMPONENT_TEMPLATE_H

#include <string>

#include "base/memory/ace_type.h"

namespace OHOS::Ace {
class ACE_EXPORT PluginComponentTemplate final : public AceType {
    DECLARE_ACE_TYPE(PluginComponentTemplate, AceType);

public:
    PluginComponentTemplate() = default;
    ~PluginComponentTemplate() override = default;

    const std::string& GetSource() const
    {
        return source_;
    }

    void SetSource(const std::string& source)
    {
        source_ = source;
    }

    const std::string& GetAbility() const
    {
        return ability_;
    }

    void SetAbility(const std::string& ability)
    {
        ability_ = ability;
    }

private:
    std::string source_;
    std::string ability_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_COMPONENT_TEMPLATE_H
