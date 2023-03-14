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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TAB_CONTENT_ITEM_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TAB_CONTENT_ITEM_ELEMENT_H

#include "core/components_v2/tabs/tabs_component.h"

namespace OHOS::Ace::V2 {

class TabContentItemElement : public ColumnElement {
    DECLARE_ACE_TYPE(TabContentItemElement, ColumnElement);

public:
    void Update() override;

    const std::string& GetIcon() const
    {
        return barIcon_;
    }

    const std::string& GetText() const
    {
        return barText_;
    }

private:
    std::string barIcon_;
    std::string barText_;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TAB_CONTENT_ITEM_ELEMENT_H
