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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_RENDER_TIP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_RENDER_TIP_H

#include "core/components/arc/arc_component.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr Dimension TIP_SPACING = 8.0_vp;

class RenderTip : public RenderNode {
DECLARE_ACE_TYPE(RenderTip, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    const Size& GetChildSize() const
    {
        return childSize_;
    }

    void SetPadding(const Edge& padding)
    {
        padding_ = padding;
    }

protected:
    Color bgColor_;
    Dimension arrowOffset_;
    Size childSize_;
    Edge padding_;
    Border border_;
    Axis direction_ = Axis::HORIZONTAL;

private:
    void AdaptChildPadding(const Size& childSize, const Size& selfSize);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_RENDER_TIP_H
