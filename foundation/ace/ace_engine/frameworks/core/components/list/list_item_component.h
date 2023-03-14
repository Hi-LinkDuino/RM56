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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_COMPONENT_H

#include "base/utils/macros.h"
#include "core/animation/animation_pub.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

inline const Dimension DIVIDER_DEFAULT_HEIGHT = 1.0_vp;
inline constexpr int32_t LIST_ITEM_FLAG_FROM_CHILD = 2;
inline constexpr int32_t LIST_ITEM_FLAG_IN_RANGE = 4;
inline constexpr int32_t LIST_ITEM_FLAG_DYNAMIC = 8;
inline constexpr int32_t LIST_ITEM_OP_NONE = 0;
inline constexpr int32_t LIST_ITEM_OP_ADD = 1;
inline constexpr int32_t LIST_ITEM_OP_REMOVE = 2;

enum class StickyMode {
    NONE,
    NORMAL,
    OPACITY,
};

class ACE_EXPORT ListItemComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ListItemComponent, SoleChildComponent);

public:
    ListItemComponent(const std::string& type, const RefPtr<Component>& child);
    ~ListItemComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    const std::string& GetType() const
    {
        return type_;
    }

    void SetType(const std::string& type)
    {
        type_ = type;
    }

    void SetIndex(int32_t index)
    {
        index_ = index;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetFlags(uint32_t flags)
    {
        flags_ = flags;
    }

    uint32_t GetFlags() const
    {
        return flags_;
    }

    void AddFlag(uint32_t flag)
    {
        flags_ |= flag;
    }

    void RemoveFlag(uint32_t flag)
    {
        flags_ &= ~flag;
    }

    bool TestFlag(uint32_t flag) const
    {
        return (flags_ & flag);
    }

    void SetFocusAnimationColor(const Color& color)
    {
        focusAnimationColor_ = color;
    }

    const Color& GetFocusAnimationColor() const
    {
        return focusAnimationColor_;
    }

    void SetTopLeftRadius(const Radius& topLeftRadius)
    {
        if (!topLeftRadius.IsValid()) {
            LOGD("Invalid radius x:%{public}lf, y:%{public}lf", topLeftRadius.GetX().Value(),
                topLeftRadius.GetY().Value());
            return;
        }
        topLeftRadius_ = topLeftRadius;
        LOGD("[Focus] SetTopLeftRadius, radius: %{public}lf", topLeftRadius_.GetX().Value());
    }

    void SetTopRightRadius(const Radius& topRightRadius)
    {
        if (!topRightRadius.IsValid()) {
            LOGD("Invalid radius x:%{public}lf, y:%{public}lf", topRightRadius.GetX().Value(),
                topRightRadius.GetY().Value());
            return;
        }
        topRightRadius_ = topRightRadius;
    }

    void SetBottomLeftRadius(const Radius& bottomLeftRadius)
    {
        if (!bottomLeftRadius.IsValid()) {
            LOGD("Invalid radius x:%{public}f, y:%{public}f", bottomLeftRadius.GetX().Value(),
                bottomLeftRadius.GetY().Value());
            return;
        }
        bottomLeftRadius_ = bottomLeftRadius;
    }

    void SetBottomRightRadius(const Radius& bottomRightRadius)
    {
        if (!bottomRightRadius.IsValid()) {
            LOGD("Invalid radius x:%{public}f, y:%{public}f", bottomRightRadius.GetX().Value(),
                bottomRightRadius.GetY().Value());
            return;
        }
        bottomRightRadius_ = bottomRightRadius;
    }

    const Radius& GetTopLeftRadius() const
    {
        return topLeftRadius_;
    }

    const Radius& GetTopRightRadius() const
    {
        return topRightRadius_;
    }

    const Radius& GetBottomLeftRadius() const
    {
        return bottomLeftRadius_;
    }

    const Radius& GetBottomRightRadius() const
    {
        return bottomRightRadius_;
    }

    void SetColumnSpan(int32_t columnSpan)
    {
        if (columnSpan <= 0) {
            return;
        }
        columnSpan_ = columnSpan;
    }

    int32_t GetColumnSpan() const
    {
        return columnSpan_;
    }

    void SetClickEventId(const EventMarker& clickEventId)
    {
        clickEventId_ = clickEventId;
    }

    const EventMarker& GetClickEventId() const
    {
        return clickEventId_;
    }

    void SetStickyEventId(const EventMarker& stickyEventId)
    {
        stickyEventId_ = stickyEventId;
    }

    const EventMarker& GetStickyEventId() const
    {
        return stickyEventId_;
    }

    static RefPtr<ListItemComponent> GetListItem(const RefPtr<Component>& component);

    void SetOperation(int32_t op)
    {
        op_ = op;
    }

    int32_t GetOperation() const
    {
        return op_;
    }

    const std::string& GetIndexKey() const
    {
        return indexKey_;
    }

    void SetIndexKey(const std::string& indexKey)
    {
        indexKey_ = indexKey;
    }

    bool GetSticky() const
    {
        return sticky_;
    }

    void SetSticky(bool sticky)
    {
        sticky_ = sticky;
    }

    StickyMode GetStickyMode() const
    {
        return stickyMode_;
    }

    void SetStickyMode(StickyMode mode)
    {
        stickyMode_ = mode;
    }

    bool NeedVibrate() const
    {
        return needVibrate_;
    }

    void MarkNeedVibrate(bool needVibrate)
    {
        needVibrate_ = needVibrate;
    }

    bool IsRotationVibrate() const
    {
        return rotationVibrate_;
    }

    void MarkNeedRotationVibrate(bool needVibrate)
    {
        rotationVibrate_ = needVibrate;
    }

    bool IsTitle() const
    {
        return isTitle_;
    }

    void MarkTitle(bool isTitle)
    {
        isTitle_ = isTitle;
    }

    bool GetSupportClick() const
    {
        return supportClick_;
    }

    void SetSupportClick(bool isClick)
    {
        supportClick_ = isClick;
    }

    bool GetSupportScale() const
    {
        return supportScale_;
    }

    void SetSupportScale(bool isScale)
    {
        supportScale_ = isScale;
    }

    bool GetSupportOpacity() const
    {
        return supportOpacity_;
    }

    void SetSupportOpacity(bool isOpacity)
    {
        supportOpacity_ = isOpacity;
    }

    bool GetPrimary() const
    {
        return primary_;
    }

    void SetPrimary(bool primary)
    {
        primary_ = primary;
    }

    const Dimension& GetStickyRadius() const
    {
        return stickyRadius_;
    }

    void SetStickyRadius(const Dimension& stickyRadius)
    {
        stickyRadius_ = stickyRadius;
    }

    bool NeedDivider() const
    {
        return needDivider_;
    }

    void MarkNeedDivider(bool needDivider)
    {
        needDivider_ = needDivider;
    }

    const Dimension& GetDividerOrigin() const
    {
        return dividerOrigin_;
    }

    void SetDividerOrigin(const Dimension& origin)
    {
        dividerOrigin_ = origin;
    }

    const Dimension& GetDividerLength() const
    {
        return dividerLength_;
    }

    void SetDividerLength(const Dimension& length)
    {
        dividerLength_ = length;
    }

    const Color& GetDividerColor() const
    {
        return dividerColor_;
    }

    void SetDividerColor(const Color& color)
    {
        dividerColor_ = color;
    }

    const Dimension& GetDividerHeight() const
    {
        return dividerHeight_;
    }

    void SetDividerHeight(const Dimension& dividerHeight)
    {
        dividerHeight_ = dividerHeight;
    }

    bool IsActive() const
    {
        return isActive_;
    }

    void SetIsActive(bool isActive)
    {
        isActive_ = isActive;
    }

    void SetTransitionEffect(TransitionEffect transitionEffect)
    {
        transitionEffect_ = transitionEffect;
    }

    TransitionEffect GetTransitionEffect() const
    {
        return transitionEffect_;
    }

    FlexAlign GetAlignSelf() const
    {
        return alignSelf_;
    }

    void SetAlignSelf(FlexAlign alignSelf)
    {
        alignSelf_ = alignSelf;
    }

    const Color& GetClickColor() const
    {
        return clickColor_;
    }
    void SetClickColor(const Color& clickColor)
    {
        clickColor_ = clickColor;
    }

    int32_t GetKey() const
    {
        return key_;
    }

    void SetKey(int32_t key)
    {
        key_ = key;
    }

private:
    int32_t index_ = -1; // invalid index
    int32_t columnSpan_ = DEFAULT_COLUMN_SPAN;
    uint32_t flags_ = 0;
    int32_t op_ = LIST_ITEM_OP_NONE;
    FlexAlign alignSelf_ = FlexAlign::AUTO;

    std::string type_;
    Color focusAnimationColor_ = Color::WHITE;
    Radius topLeftRadius_;
    Radius topRightRadius_;
    Radius bottomLeftRadius_;
    Radius bottomRightRadius_;
    std::string indexKey_;
    int32_t key_ = -1;
    bool needVibrate_ = true;
    bool rotationVibrate_ = false;
    bool supportScale_ = true;
    bool supportOpacity_ = false;
    bool supportClick_ = true;
    bool sticky_ = false;
    bool isTitle_ = false;
    StickyMode stickyMode_ = StickyMode::NONE;
    Dimension stickyRadius_;
    bool primary_ = false;
    bool needDivider_ = false;
    bool isActive_ = false;
    Dimension dividerOrigin_;
    Dimension dividerLength_;
    Dimension dividerHeight_ = DIVIDER_DEFAULT_HEIGHT;
    Color dividerColor_;
    Color clickColor_ = Color::TRANSPARENT;

    EventMarker clickEventId_;
    EventMarker stickyEventId_;

    TransitionEffect transitionEffect_ = TransitionEffect::NONE;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_COMPONENT_H
