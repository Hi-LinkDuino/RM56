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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_COMPONENT_H

#include "base/i18n/localization.h"
#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components_v2/indexer/indexer_item_component.h"
#include "core/components_v2/indexer/popup_list_component.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace::V2 {
// common data
inline constexpr int32_t INDEXER_INVALID_INDEX = -1;
inline constexpr int32_t INDEXER_ITEM_MAX_COUNT = 29; // [indexer], default max count
inline constexpr uint8_t DEFAULT_OPACITY = 255;
inline constexpr uint8_t ZERO_OPACITY = 0;
inline const std::u16string INDEXER_STR_DOT = StringUtils::Str8ToStr16("•");
inline const std::u16string INDEXER_STR_DOT_EX = StringUtils::Str8ToStr16(".");
inline const std::u16string INDEXER_STR_SHARP = StringUtils::Str8ToStr16("#");

inline constexpr float KEYFRAME_BEGIN = 0.0;
inline constexpr float KEYFRAME_HALF = 0.5;
inline constexpr float KEYFRAME_END = 1.0;
inline constexpr double DEFAULT_OPACITY_IN_PERCENT = 1.0;
inline constexpr double NINETY_OPACITY_IN_PERCENT = 0.9;
inline constexpr double ZERO_OPACITY_IN_PERCENT = 0.0;
// data for list mode
inline constexpr double INDEXER_LIST_ITEM_TEXT_SIZE = 12.0; // list mode, font size (FP)
inline constexpr uint32_t INDEXER_LIST_COLOR = 0x99000000;
inline constexpr uint32_t INDEXER_LIST_ACTIVE_COLOR = 0xFF254FF7;
inline constexpr double INDEXER_DEFAULT_PADDING_X = 10.0;
inline constexpr double INDEXER_DEFAULT_PADDING_Y = 16.0;
inline constexpr double BUBBLE_BOX_SIZE = 56.0;
inline constexpr double BUBBLE_BOX_RADIUS = 12.0;
inline constexpr double ZERO_RADIUS = 0.0;
inline constexpr double BUBBLE_FONT_SIZE = 24.0;
inline constexpr Dimension BUBBLE_POSITION_X = 48.0_vp;
inline constexpr Dimension BUBBLE_POSITION_Y = 96.0_vp;
inline constexpr uint32_t BUBBLE_FONT_COLOR = 0xFF254FF7;
inline constexpr uint32_t BUBBLE_BG_COLOR = 0xFFF1F3F5;
inline constexpr double INDEXER_ITEM_SIZE = 24.0;      // circle mode, item size (VP)
inline constexpr double INDEXER_ITEM_TEXT_SIZE = 12.0; // circle, mode font size (VP)
// data for circle mode
inline constexpr int32_t INDEXER_COLLAPSE_ITEM_COUNT = 4;
inline constexpr double INDEXER_CIRCLE_ITEM_SIZE = 24.0;          // circle mode, item size (VP)
inline constexpr double INDEXER_CIRCLE_ITEM_TEXT_SIZE = 13.0;     // circle, mode font size (VP)
inline constexpr double INDEXER_CIRCLE_ITEM_SHADOW_RADIUS = 27.0; // circle mode, shadow radius (VP)
inline constexpr double BUBBLE_BOX_SIZE_CIRCLE = 46.0;
inline constexpr double BUBBLE_FONT_SIZE_CIRCLE = 19.0;
inline constexpr uint32_t INDEXER_ACTIVE_BG_COLOR = 0x1F0A59F7;

enum class AlignStyle {
    LEFT = 0,
    RIGHT,
};

class ACE_EXPORT IndexerComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(IndexerComponent, ComponentGroup);

public:
    using OnRequestPopupDataFunc = std::function<std::vector<std::string>(std::shared_ptr<IndexerEventInfo>)>;

    IndexerComponent(const std::vector<std::string>& label, int32_t selectedIndex, bool bubble = true)
        : selectedIndex_(selectedIndex), bubbleEnabled_(bubble)
    {
        indexerLabel_ = GetU16StrVector(label);
        itemSize_ = Dimension(INDEXER_ITEM_SIZE, DimensionUnit::VP);
        selectedBgColor_ = Color(INDEXER_ACTIVE_BG_COLOR);

        BuildIndexerAlphabet();
        InitIndexerItemStyle();
        BuildIndexerItems();
        BuildPopupList();
    }

    ~IndexerComponent() override = default;
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    bool IsBubbleEnabled() const
    {
        return bubbleEnabled_;
    }

    void SetBubbleEnabled(bool enable)
    {
        bubbleEnabled_ = enable;
    }

    bool HasCollapseItem() const
    {
        return hasCollapseItem_;
    }

    const RefPtr<TextComponent>& GetBubbleTextComponent() const
    {
        return bubbleText_;
    }

    int32_t GetNonItemCount() const
    {
        return nonItemCount_;
    }

    int32_t GetMaxShowCount() const
    {
        return maxShowCount_;
    }

    const Dimension& GetItemSize() const
    {
        return itemSize_;
    }

    void SetItemSize(const Dimension& size)
    {
        itemSize_ = size;
        UpdateTextStyle();
    }

    int32_t GetItemCount() const
    {
        return itemCount_;
    }

    const TextStyle& GetNormalTextStyle() const
    {
        return normalStyle_;
    }

    void SetNormalTextStyle(const TextStyle& textStyle)
    {
        normalStyle_ = textStyle;
        UpdateTextStyle();
    }

    const TextStyle& GetActiveTextStyle() const
    {
        return activeStyle_;
    }

    void SetActiveTextStyle(const TextStyle& textStyle)
    {
        activeStyle_ = textStyle;
        UpdateTextStyle();
    }

    const TextStyle& GetBubbleTextStyle() const
    {
        return bubbleStyle_;
    }

    void SetBubbleTextStyle(const TextStyle& textStyle)
    {
        bubbleStyle_ = textStyle;
        bubbleText_->SetTextStyle(bubbleStyle_);
    }

    std::vector<std::u16string> GetU16StrVector(const std::vector<std::string>& str)
    {
        std::vector<std::u16string> alphabet;
        for (const auto& item : str) {
            alphabet.push_back(StringUtils::Str8ToStr16(item));
        }
        return alphabet;
    }

    std::list<RefPtr<IndexerItemComponent>> GetIndexerItemsComponents() const
    {
        return listItem_;
    }

    void SetSelectedEvent(const EventMarker& event)
    {
        selectedEvent_ = event;
    }

    const EventMarker& GetSelectedEvent() const
    {
        return selectedEvent_;
    }

    int32_t GetSelectedIndex() const
    {
        return selectedIndex_;
    }

    void SetSelectedIndex(int32_t index)
    {
        selectedIndex_ = index;
    }

    void SetSelectedBackgroundColor(const Color& bgColor)
    {
        selectedBgColor_ = bgColor;
        UpdateTextStyle();
    }

    const Color& GetSelectedBackgroundColor()
    {
        return selectedBgColor_;
    }

    void SetBubbleBackgroundColor(const Color& bgColor)
    {
        if (!bubbleBack_) {
            bubbleBack_ = AceType::MakeRefPtr<Decoration>();
        }
        bubbleBack_->SetBackgroundColor(bgColor.BlendOpacity(NINETY_OPACITY_IN_PERCENT));
    }

    const Color& GetBubbleBackgroundColor()
    {
        return bubbleBack_->GetBackgroundColor();
    }

    void SetAlignStyle(AlignStyle align)
    {
        alignStyle_ = align;
    }

    AlignStyle GetAlignStyle()
    {
        return alignStyle_;
    }

    void SetRequestPopupDataFunc (const OnRequestPopupDataFunc& func)
    {
        requestPopupDataEvent_ = func;

        // if implement OnRequestPopupData function, enable the popup list.
        popupListEnabled_ = true;
    }

    const OnRequestPopupDataFunc& GetRequestPopupDataFunc() const
    {
        return requestPopupDataEvent_;
    }

    void SetPopupListEnabled(bool enable)
    {
        popupListEnabled_ = enable;
    }

    bool IsPopupListEnabled()
    {
        return popupListEnabled_;
    }

    void SetPopupSelectedEvent(const EventMarker& event)
    {
        if (popupList_) {
            popupList_->SetPopupSelectedEvent(event);
        } else {
            LOGW("Not popup list component");
        }
    }

    std::vector<std::string> GetArrayValue() const
    {
        return valueArray_;
    }

protected:
    // init data
    void FormatLabelAlphabet();
    void BuildIndexerAlphabet();

    void InitIndexerItemStyle();
    void BuildIndexerItems();
    void BuildBubbleBox();
    void BuildPopupList();
    void BuildTextItem(const std::u16string& strSection, const std::u16string& strLabel, int32_t itemType = 0);
    void UpdateTextStyle();

    std::vector<std::u16string> indexerLabel_;  // user input sections
    std::vector<std::u16string> sectionsLocal_; // actual sections, exclude dot
    std::vector<std::u16string> labelLocal_;    // sections shown on the screen with dot inside
    std::vector<std::string> valueArray_;
    TextStyle normalStyle_;
    TextStyle activeStyle_;
    TextStyle bubbleStyle_;
    Color selectedBgColor_;
    AlignStyle alignStyle_ = AlignStyle::RIGHT;

    std::list<RefPtr<IndexerItemComponent>> listItem_;
    RefPtr<Decoration> bubbleBack_;
    RefPtr<TextComponent> bubbleText_;
    RefPtr<PopupListComponent> popupList_;

    int32_t itemCount_ = INDEXER_ITEM_MAX_COUNT;    // actual indexer item
    int32_t maxShowCount_ = INDEXER_ITEM_MAX_COUNT; // max item shown on screen
    int32_t nonItemCount_ = 0;                      // not indexer item count such as arc and bubbleBox
    int32_t selectedIndex_ = 0;
    // item size
    Dimension itemSize_;
    bool hasCollapseItem_ = false;
    bool bubbleEnabled_ = true;
    bool popupListEnabled_ = false;

private:
    EventMarker selectedEvent_;
    OnRequestPopupDataFunc requestPopupDataEvent_;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_COMPONENT_H
