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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_RENDER_BLOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_RENDER_BLOCK_H

#include "core/components/slider/block_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderBlock : public RenderNode {
    DECLARE_ACE_TYPE(RenderBlock, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    const Color& GetBlockColor() const
    {
        return blockColor_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        hoverColor_ = hoverColor;
    }

    Size Measure();

    void SetFocus(bool focus)
    {
        isFocus_ = focus;
    }

    bool GetFocus() const
    {
        return isFocus_;
    }

    void SetRadiusScale(double radiusScale)
    {
        if (radiusScale >= 1.0) {
            radiusScale_ = radiusScale;
        }
    }

    const Dimension& GetBlockSize() const
    {
        return blockSize_;
    }

    const Dimension& GetHotRegionWidth() const
    {
        return hotRegionWidth_;
    }

    const Dimension& GetHotRegionHeight() const
    {
        return hotRegionHeight_;
    }

    void SetHover(bool isHover)
    {
        isHover_ = isHover;
    }

protected:
    bool isFocus_ = false;
    bool isHover_ = false;
    double radiusScale_ = 1.0;
    Dimension blockSize_;
    Dimension hotRegionWidth_;
    Dimension hotRegionHeight_;

private:
    Color blockColor_;
    Color hoverColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_RENDER_BLOCK_H
