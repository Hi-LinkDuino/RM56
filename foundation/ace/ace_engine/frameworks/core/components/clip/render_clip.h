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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_RENDER_CLIP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_RENDER_CLIP_H

#include "core/animation/property_animatable.h"
#include "core/components/common/properties/radius.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderClip : public RenderNode {
    DECLARE_ACE_TYPE(RenderClip, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    bool IsFollowChildSize() const
    {
        return followChildSize_;
    }

    virtual void SetOffsetX(double offsetX)
    {
        if (offsetX < 0.0) {
            return;
        }
        offsetX_ = offsetX;
        MarkNeedRender();
    }

    virtual void SetOffsetY(double offsetY)
    {
        if (offsetY < 0.0) {
            return;
        }
        offsetY_ = offsetY;
        MarkNeedRender();
    }

    Rect GetClipRect(const Offset& offset) const;

    virtual void SetWidth(double width)
    {
        if (width < 0.0) {
            return;
        }
        width_ = width;
        MarkNeedRender();
    }

    virtual void SetHeight(double height)
    {
        if (height < 0.0) {
            return;
        }
        height_ = height;
        MarkNeedRender();
    }

    double GetHeight() const
    {
        return height_;
    }

    void SetShadowBoxOffset(const Offset& shadowBoxOffset)
    {
        shadowBoxOffset_ = shadowBoxOffset;
    }

    void Dump() override;

    void SetClipRadius(const Radius& clipRadius)
    {
        topLeftRadius_ = clipRadius;
        topRightRadius_ = clipRadius;
        bottomLeftRadius_ = clipRadius;
        bottomRightRadius_ = clipRadius;
        MarkNeedRender();
    }

protected:
    double width_ = 0.0;
    double height_ = 0.0;
    double offsetX_ = 0.0;
    double offsetY_ = 0.0;

    Radius topLeftRadius_;
    Radius topRightRadius_;
    Radius bottomLeftRadius_;
    Radius bottomRightRadius_;

private:
    FloatPropertyAnimatable::SetterMap GetFloatPropertySetterMap() override;
    FloatPropertyAnimatable::GetterMap GetFloatPropertyGetterMap() override;
    void UpdateBoxForShadowAnimation();
    bool followChildSize_ = false;
    bool clipWithShadow_ = false;
    Offset shadowBoxOffset_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_RENDER_CLIP_H
