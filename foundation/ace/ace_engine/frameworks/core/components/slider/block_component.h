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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_BLOCK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_BLOCK_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

// The slider block in slider
class BlockComponent : public RenderComponent {
    DECLARE_ACE_TYPE(BlockComponent, RenderComponent);

public:
    BlockComponent() = default;
    ~BlockComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        // never use element to create element tree.
        return nullptr;
    }

    const Color& GetBlockColor() const
    {
        return blockColor_;
    }

    void SetBlockColor(const Color& color)
    {
        blockColor_ = color;
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

    void SetBlockSize(const Dimension& size)
    {
        blockSize_ = size;
    }

    void SetHotRegionWidth(const Dimension& width)
    {
        hotRegionWidth_ = width;
    }

    void SetHotRegionHeight(const Dimension& height)
    {
        hotRegionHeight_ = height;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    void SetHoverColor(const Color& color)
    {
        hoverColor_ = color;
    }

private:
    Color hoverColor_;
    Color blockColor_;
    Dimension blockSize_;
    Dimension hotRegionWidth_;
    Dimension hotRegionHeight_;
};

class CircleBlock : public BlockComponent {
    DECLARE_ACE_TYPE(CircleBlock, BlockComponent);

public:
    CircleBlock() = default;
    ~CircleBlock() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_BLOCK_COMPONENT_H
