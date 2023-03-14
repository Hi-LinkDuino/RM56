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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_DROP_FILTER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_DROP_FILTER_COMPONENT_H

#include <string>

#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

// A component can show Gaussian Blur.
class DropFilterComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(DropFilterComponent, SoleChildComponent);

public:
    DropFilterComponent() = default;
    DropFilterComponent(const RefPtr<Component>& child, double sigmaX, double sigmaY);
    ~DropFilterComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    double GetSigmaX() const
    {
        return sigmaX_;
    }

    double GetSigmaY() const
    {
        return sigmaY_;
    }

    void SetSigmaX(double sigmaX)
    {
        sigmaX_ = sigmaX;
    }

    void SetSigmaY(double sigmaY)
    {
        sigmaY_ = sigmaY;
    }

    bool UsePanelTouchRect() const
    {
        return usePanelTouchRect_;
    }

    void SetUsePanelTouchRect(bool usePanelTouchRect)
    {
        usePanelTouchRect_ = usePanelTouchRect;
    }

private:
    double sigmaX_ = 0.0;
    double sigmaY_ = 0.0;

    // use this flag to update DropFilter touch rect in SlidingPanel mode.
    bool usePanelTouchRect_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_DROP_FILTER_COMPONENT_H
