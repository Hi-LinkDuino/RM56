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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_RENDER_SINGLE_CHILD_SCROLL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_RENDER_SINGLE_CHILD_SCROLL_H

#include "core/components/scroll/render_scroll.h"

namespace OHOS::Ace {

class RenderSingleChildScroll : public RenderScroll {
    DECLARE_ACE_TYPE(RenderSingleChildScroll, RenderScroll);

public:
    static RefPtr<RenderNode> Create();

    void MoveChildToViewPort(const Size& size, const Offset& childOffset, const Offset& effectOffset);
    bool IsUseOnly() override;

protected:
    void PerformLayout() override;
    void Update(const RefPtr<Component>& component) override;
    void AdjustTouchRestrict(TouchRestrict& touchRestrict) override;

private:
    LayoutParam MakeInnerLayoutParam() const;
    bool CalculateMainScrollExtent(const Size& itemSize);
    void UpdateAccessibilityAttr();
    void PostForceMakeNeedLayout();

    bool isEffectSetted_ = false;
    double childLastMainSize_ = -std::numeric_limits<double>::max();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_RENDER_SINGLE_CHILD_SCROLL_H
