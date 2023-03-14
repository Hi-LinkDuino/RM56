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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_CLIP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_CLIP_COMPONENT_H

#include "base/utils/utils.h"
#include "core/components/common/properties/radius.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ClipComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ClipComponent, SoleChildComponent);

public:
    explicit ClipComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~ClipComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    const Dimension& GetOffsetX() const
    {
        return offsetX_;
    }

    void SetOffsetX(const Dimension& offsetX)
    {
        offsetX_ = offsetX;
    }

    const Dimension& GetOffsetY() const
    {
        return offsetY_;
    }

    void SetOffsetY(const Dimension& offsetY)
    {
        offsetY_ = offsetY;
    }

    void SetClipRadius(const Radius& clipRadius)
    {
        topLeftRadius_ = clipRadius;
        topRightRadius_ = clipRadius;
        bottomLeftRadius_ = clipRadius;
        bottomRightRadius_ = clipRadius;
    }

    const Radius& GetTopLeftRadius() const
    {
        return topLeftRadius_;
    }

    void SetTopLeftRadius(const Radius& topLeftRadius)
    {
        topLeftRadius_ = topLeftRadius;
    }

    const Radius& GetTopRightRadius() const
    {
        return topRightRadius_;
    }

    void SetTopRightRadius(const Radius& topRightRadius)
    {
        topRightRadius_ = topRightRadius;
    }

    const Radius& GetBottomLeftRadius() const
    {
        return bottomLeftRadius_;
    }

    void SetBottomLeftRadius(const Radius& bottomLeftRadius)
    {
        bottomLeftRadius_ = bottomLeftRadius;
    }

    const Radius& GetBottomRightRadius() const
    {
        return bottomRightRadius_;
    }

    void SetBottomRightRadius(const Radius& bottomRightRadius)
    {
        bottomRightRadius_ = bottomRightRadius;
    }

    bool IsFollowChild() const
    {
        return followChild_;
    }

    void SetFollowChild(bool followChild)
    {
        followChild_ = followChild;
    }

    bool IsClipWithShadow() const
    {
        return clipWithShadow_;
    }

    void SetClipWithShadow(bool clipWithShadow)
    {
        clipWithShadow_ = clipWithShadow;
    }

private:
    Dimension width_ = Dimension(0.0);
    Dimension height_ = Dimension(0.0);
    Dimension offsetX_ = Dimension(0.0);
    Dimension offsetY_ = Dimension(0.0);

    Radius topLeftRadius_;
    Radius topRightRadius_;
    Radius bottomLeftRadius_;
    Radius bottomRightRadius_;
    bool followChild_ = true;
    // Play clip animation with shadow when set true, shadow should set in box outer of tween component.
    bool clipWithShadow_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_CLIP_COMPONENT_H
