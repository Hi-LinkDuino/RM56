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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_COMPONENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_COMPONENT_V2_H

#include "core/components/text/text_component.h"

namespace OHOS::Ace {

// A component can show text in declarative.
class ACE_EXPORT TextComponentV2 : public TextComponent {
    DECLARE_ACE_TYPE(TextComponentV2, TextComponent);

public:
    explicit TextComponentV2(const std::string& data) : TextComponent(data) {}
    ~TextComponentV2() override = default;

    void OnChildInserted(const RefPtr<Component>& child, int32_t position) override;
    void OnChildAppended(const RefPtr<Component>& child) override;

private:
    void CheckAndSetChildStyle(const RefPtr<Component>& child);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_COMPONENT_V2_H
