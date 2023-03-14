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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_POPUP_LIST_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_POPUP_LIST_COMPONENT_H

#include <list>

#include "base/geometry/axis.h"
#include "base/utils/noncopyable.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace::V2 {
inline constexpr double POPUP_BOX_RADIUS_SIZE = 0.0;
inline constexpr double POPUP_BORDER_RADIUS_SIZE = 12.0;

class ACE_EXPORT PopupListComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(V2::PopupListComponent, ComponentGroup)

public:
    PopupListComponent()
    {
        BuildBackground();
    };
    ~PopupListComponent() = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    ACE_DEFINE_COMPONENT_PROP(Direction, Axis, Axis::VERTICAL);
    ACE_DEFINE_COMPONENT_PROP(EdgeEffect, EdgeEffect, EdgeEffect::SPRING);

    void SetPopupSelectedEvent(const EventMarker& event)
    {
        popupSelectedEvent_ = event;
    }

    const EventMarker& GetPopupSelectedEvent() const
    {
        return popupSelectedEvent_;
    }

private:
    void BuildBackground();

    EventMarker popupSelectedEvent_;

    ACE_DISALLOW_COPY_AND_MOVE(PopupListComponent);
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_POPUP_LIST_COMPONENT_H
