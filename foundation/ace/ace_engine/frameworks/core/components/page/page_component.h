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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_COMPONENT_H

#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

class PageComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(PageComponent, ComposedComponent);

public:
    PageComponent(int32_t pageId, const std::string& pageUrl, const RefPtr<Component>& child);
    PageComponent(
        int32_t pageId, const std::string& pageUrl, const ComposeId& cardComposeId, const RefPtr<Component>& child);
    ~PageComponent() override = default;

    RefPtr<Element> CreateElement() override;

    int32_t GetPageId()
    {
        return pageId_;
    }

private:
    int32_t pageId_ = -1;
    std::string pageUrl_;
    ComposeId cardComposeId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_COMPONENT_H
