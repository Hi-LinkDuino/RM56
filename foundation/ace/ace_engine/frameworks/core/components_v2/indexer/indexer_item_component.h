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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_ITEM_COMPONENT_H

#include "base/utils/string_utils.h"
#include "core/components/box/box_component.h"
#include "core/components/image/image_component.h"
#include "core/components/text/text_component.h"
#include "core/components_v2/indexer/indexer_event_info.h"

namespace OHOS::Ace::V2 {
class IndexerItemComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(IndexerItemComponent, SoleChildComponent);

public:
    IndexerItemComponent()
    {
        strSection_ = StringUtils::Str8ToStr16("");
        strLabel_ = StringUtils::Str8ToStr16("");
        BuildItem();
    }

    IndexerItemComponent(const std::u16string& strSection, const std::u16string& strLabel, const Dimension& itemSize,
        bool rotate = false)
        : strSection_(strSection), strLabel_(strLabel), itemSize_(itemSize), rotate_(rotate)
    {
        BuildItem();
    }

    ~IndexerItemComponent() override = default;
    RefPtr<RenderNode> CreateRenderNode() override;

    void BuildItem();

    const std::u16string& GetSectionStr() const
    {
        return strSection_;
    }

    const std::u16string& GetLabelStr() const
    {
        return strLabel_;
    }

    int32_t GetSectionIndex() const
    {
        return sectionIndex_;
    }

    void SetSectionIndex(int32_t index)
    {
        sectionIndex_ = index;
    }

    void SetPosition(int32_t position)
    {
        position_ = position;
    }

    int32_t GetPosition() const
    {
        return position_;
    }

    void SetTextStyle(bool active);

    const RefPtr<TextComponent>& GetTextComponent() const
    {
        return text_;
    }

    const Dimension& GetItemSize() const
    {
        return itemSize_;
    }

    void SetItemSize(const Dimension& size)
    {
        itemSize_ = size;
    }

    const TextStyle& GetNormalTextStyle() const
    {
        return normalStyle_;
    }

    void SetNormalTextStyle(const TextStyle& textStyle)
    {
        normalStyle_ = textStyle;
    }

    const TextStyle& GetActiveTextStyle() const
    {
        return activeStyle_;
    }

    void SetActiveTextStyle(const TextStyle& textStyle)
    {
        activeStyle_ = textStyle;
    }

    bool GetRotateFlag() const
    {
        return rotate_;
    }

    void MarkItemPrimary(bool isPrimary = true)
    {
        isPrimary_ = isPrimary;
    }

    bool IsItemPrimary() const
    {
        return isPrimary_;
    }

    int32_t GetItemType() const
    {
        return itemType_;
    }

    void SetItemType(int32_t type)
    {
        itemType_ = type;
    }

    void SetSelectedBackgroundColor(const Color& bgColor)
    {
        selectedBgColor_ = bgColor;
    }

    const Color& GetSelectedBackgroundColor()
    {
        return selectedBgColor_;
    }

    void UpdateSize();

private:
    std::u16string strSection_;
    std::u16string strLabel_; // string show in the section head
    Dimension itemSize_;      // item size
    bool rotate_ = false;
    int32_t sectionIndex_ = 0;
    int32_t position_ = 0;
    int32_t itemType_ = 0;
    RefPtr<TextComponent> text_;
    RefPtr<ImageComponent> image_;
    RefPtr<BoxComponent> box_;
    TextStyle normalStyle_;
    TextStyle activeStyle_;
    Color selectedBgColor_;
    bool isPrimary_ = false;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_ITEM_COMPONENT_H