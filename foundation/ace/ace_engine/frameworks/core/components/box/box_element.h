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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_BOX_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_BOX_ELEMENT_H

#include "core/components/box/box_base_element.h"

namespace OHOS::Ace {

class PageElement;

class BoxElement : public BoxBaseElement {
    DECLARE_ACE_TYPE(BoxElement, BoxBaseElement);

public:
    void PerformBuild() override;

protected:
    RefPtr<RenderNode> GetCachedRenderNode() override
    {
        auto context = GetContext().Upgrade();
        if (context) {
            return context->GetRenderFactory()->GetRenderBoxFactory()->Get();
        }
        return nullptr;
    }

private:
    std::string geometryTransitionId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_BOX_ELEMENT_H
