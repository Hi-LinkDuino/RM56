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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WRAP_WRAP_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WRAP_WRAP_ELEMENT_H

#include "core/components/common/layout/constants.h"
#include "core/components/flex/flex_element.h"
#include "core/focus/focus_node.h"

namespace OHOS::Ace {

class ACE_EXPORT WrapElement : public FlexElement {
    DECLARE_ACE_TYPE(WrapElement, FlexElement);

public:
    RefPtr<RenderNode> GetCachedRenderNode() override
    {
        auto context = GetContext().Upgrade();
        if (context) {
            return context->GetRenderFactory()->GetRenderWrapFactory()->Get();
        }
        return nullptr;
    }
    void Update() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

private:
    WrapDirection direction_ = WrapDirection::HORIZONTAL;
    bool dialogStretch_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WRAP_WRAP_ELEMENT_H
