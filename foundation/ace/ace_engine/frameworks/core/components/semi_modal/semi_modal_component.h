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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_COMPONENT_H

#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class SemiModalComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(SemiModalComponent, SoleChildComponent);

public:
    explicit SemiModalComponent(bool isFullScreen) : isFullScreen_(isFullScreen) {}
    ~SemiModalComponent() override = default;

    static RefPtr<Component> Create(RefPtr<Component> child, bool isFullScreen, int32_t modalHeight, uint32_t color);

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    void BuildInnerChild(uint32_t color);

    bool IsFullScreen() const
    {
        return isFullScreen_;
    }

    void SetModalHeight(double height)
    {
        modalHeight_ = height;
    }

    double GetModalHeight() const
    {
        return modalHeight_;
    }

private:
    bool isFullScreen_ = false;

    double modalHeight_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_COMPONENT_H
