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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_POSITIONED_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_POSITIONED_COMPONENT_H

#include "core/components/positioned/positioned_element.h"
#include "core/components/positioned/render_positioned.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class PositionedComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(PositionedComponent, SoleChildComponent);

public:
    PositionedComponent() = default;
    explicit PositionedComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~PositionedComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderPositioned::Create();
    }

    RefPtr<Element> CreateElement() override
    {
        RefPtr<PositionedElement> positionedElement = AceType::MakeRefPtr<PositionedElement>();
        positionedElement->SetIsAutoFocus(isAutoFocus_);
        return positionedElement;
    }

    const Dimension& GetBottom() const override
    {
        return bottom_;
    }

    void SetBottom(const Dimension& bottom) override
    {
        bottom_ = bottom;
        hasBottom_ = true;
    }

    double GetHeight() const
    {
        return height_;
    }

    void SetHeight(double height)
    {
        height_ = height;
    }

    const Dimension& GetLeft() const override
    {
        return left_;
    }

    void SetLeft(const Dimension& left) override
    {
        left_ = left;
        hasLeft_ = true;
    }

    const Dimension& GetRight() const override
    {
        return right_;
    }

    void SetRight(const Dimension& right) override
    {
        right_ = right;
        hasRight_ = true;
    }

    const Dimension& GetTop() const override
    {
        return top_;
    }

    void SetTop(const Dimension& top) override
    {
        top_ = top;
        hasTop_ = true;
    }

    double GetWidth() const
    {
        return width_;
    }

    void SetWidth(double width)
    {
        width_ = width;
    }

    bool IsAutoFocus() const
    {
        return isAutoFocus_;
    }

    void SetAutoFocus(bool isAutoFocus)
    {
        isAutoFocus_ = isAutoFocus;
    }

    bool HasLeft() const override
    {
        return hasLeft_;
    }

    void SetHasLeft(bool hasLeft)
    {
        hasLeft_ = hasLeft;
    }

    bool HasTop() const override
    {
        return hasTop_;
    }

    void SetHasTop(bool hasTop)
    {
        hasTop_ = hasTop;
    }

    bool HasRight() const override
    {
        return hasRight_;
    }

    void SetHasRight(bool hasRight)
    {
        hasRight_ = hasRight;
    }

    bool HasBottom() const override
    {
        return hasBottom_;
    }

    void SetHasBottom(bool hasBottom)
    {
        hasBottom_ = hasBottom;
    }

    bool HasPositionStyle() const
    {
        return hasPositionStyle_;
    }

    void SetHasPositionStyle(bool hasPositionStyle)
    {
        hasPositionStyle_ = hasPositionStyle;
    }

    const UpdatePositionFunc& GetUpdatePositionFuncId() const
    {
        return updatePositionFunc_;
    }

    void SetUpdatePositionFuncId(const UpdatePositionFunc& updatePositionFunc)
    {
        updatePositionFunc_ = updatePositionFunc;
    }

private:
    Dimension bottom_;
    Dimension left_;
    Dimension right_;
    Dimension top_;
    double height_ = 0.0;
    double width_ = 0.0;

    bool hasLeft_ = false;
    bool hasTop_ = false;
    bool hasRight_ = false;
    bool hasBottom_ = false;
    bool hasPositionStyle_ = false;

    bool isAutoFocus_ = false;

    UpdatePositionFunc updatePositionFunc_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_POSITIONED_COMPONENT_H
