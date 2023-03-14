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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_FLEX_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_FLEX_ELEMENT_H

#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class ACE_EXPORT FlexElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(FlexElement, ComponentGroupElement, FocusGroup);

public:
    void Update() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    bool AcceptFocusByRectOfLastFocus(const Rect& rect) override;

protected:
    RefPtr<RenderNode> GetCachedRenderNode() override
    {
        auto context = GetContext().Upgrade();
        if (context) {
            return context->GetRenderFactory()->GetRenderFlexFactory()->Get();
        }
        return nullptr;
    }

    FlexDirection direction_ { FlexDirection::ROW };
};

class RowElement : public FlexElement {
    DECLARE_ACE_TYPE(RowElement, FlexElement);
};

class ColumnElement : public FlexElement {
    DECLARE_ACE_TYPE(ColumnElement, FlexElement);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_FLEX_ELEMENT_H
