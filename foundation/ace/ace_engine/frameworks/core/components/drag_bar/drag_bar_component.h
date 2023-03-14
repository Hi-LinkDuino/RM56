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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_DRAG_BAR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_DRAG_BAR_COMPONENT_H

#include "core/components/common/layout/constants.h"
#include "core/components/image/image_component.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class DragBarComponent : public RenderComponent {
    DECLARE_ACE_TYPE(DragBarComponent, RenderComponent);

public:
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetPanelMode(PanelMode mode)
    {
        mode_ = mode;
    }

    PanelMode GetPanelMode() const
    {
        return mode_;
    }

    bool HasDragBar() const
    {
        return hasDragBar_;
    }

    void SetHasDragBar(bool hasDragBar)
    {
        hasDragBar_ = hasDragBar;
    }

private:
    PanelMode mode_ = PanelMode::FULL;
    bool hasDragBar_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_DRAG_BAR_COMPONENT_H
