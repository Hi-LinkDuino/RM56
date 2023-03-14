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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_RENDER_POSITIONED_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_RENDER_POSITIONED_H

#include "core/animation/property_animatable.h"
#include "core/components/proxy/render_proxy.h"

namespace OHOS::Ace {

using UpdatePositionFunc = std::function<void(const std::function<void(const Dimension&, const Dimension&)>&)>;

class ACE_EXPORT RenderPositioned : public RenderProxy {
    DECLARE_ACE_TYPE(RenderPositioned, RenderProxy);

public:
    void Update(const RefPtr<Component>& component) override;

    static RefPtr<RenderNode> Create();

    const Dimension& GetBottom() const override
    {
        return bottom_;
    }

    const Dimension& GetTop() const override
    {
        return top_;
    }

    double GetHeight() const
    {
        return height_;
    }

    double GetWidth() const
    {
        return width_;
    }

    const Dimension& GetLeft() const override
    {
        return left_;
    }

    const Dimension& GetRight() const override
    {
        return right_;
    }

    bool HasLeft() const override
    {
        return hasLeft_;
    }

    bool HasTop() const override
    {
        return hasTop_;
    }

    bool HasRight() const override
    {
        return hasRight_;
    }

    bool HasBottom() const override
    {
        return hasBottom_;
    }

    void SetLeft(const Dimension& left) override; // add for animation

    void SetTop(const Dimension& top) override; // add for animation

    void SetRight(const Dimension& right) override; // add for animation

    void SetBottom(const Dimension& bottom) override; // add for animation

protected:
    Dimension bottom_;
    Dimension top_;
    Dimension left_;
    Dimension right_;
    double height_ = 0.0;
    double width_ = 0.0;

    bool hasLeft_ = false;
    bool hasTop_ = false;
    bool hasRight_ = false;
    bool hasBottom_ = false;

    UpdatePositionFunc updatePositionFunc_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_RENDER_POSITIONED_H
