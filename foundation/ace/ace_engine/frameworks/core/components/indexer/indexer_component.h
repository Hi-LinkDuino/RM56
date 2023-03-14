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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_INDEXER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_INDEXER_COMPONENT_H

#include "base/i18n/localization.h"
#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components/indexer/indexer_item_component.h"
#include "core/components/list/list_component.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

// common data
inline constexpr int32_t INDEXER_INVALID_INDEX = -1;
inline constexpr int32_t INDEXER_ITEM_MAX_COUNT = 29; // [indexer], default max count
inline constexpr uint8_t DEFAULT_OPACITY = 255;
inline constexpr uint8_t ZERO_OPACITY = 0;
inline const std::u16string INDEXER_STR_COLLECT = StringUtils::Str8ToStr16("*");
inline const std::u16string INDEXER_STR_DOT = StringUtils::Str8ToStr16("•");
inline const std::u16string INDEXER_STR_DOT_EX = StringUtils::Str8ToStr16(".");
inline const std::u16string INDEXER_STR_SHARP = StringUtils::Str8ToStr16("#");
inline const std::u16string INDEXER_STR_COLLAPSE = StringUtils::Str8ToStr16(">");
inline const std::u16string INDEXER_STR_SPACE = StringUtils::Str8ToStr16(" ");
inline constexpr float KEYFRAME_BEGIN = 0.0;
inline constexpr float KEYFRAME_HALF = 0.5;
inline constexpr float KEYFRAME_END = 1.0;
inline constexpr double DEFAULT_OPACITY_IN_PERCENT = 1.0;
inline constexpr double NINETY_OPACITY_IN_PERCENT = 0.9;
inline constexpr double ZERO_OPACITY_IN_PERCENT = 0.0;
// data for list mode
inline constexpr double INDEXER_LIST_ITEM_TEXT_SIZE = 12.0; // list mode, font size (FP)
inline constexpr uint32_t INDEXER_LIST_COLOR = 0x99000000;
inline constexpr uint32_t INDEXER_LIST_ACTIVE_COLOR = 0xFF0A59F7;
inline constexpr double INDEXER_DEFAULT_PADDING_X = 10.0;
inline constexpr double INDEXER_DEFAULT_PADDING_Y = 16.0;
inline constexpr double BUBBLE_BOX_SIZE = 56.0;
inline constexpr double BUBBLE_BOX_RADIUS = 16.0;
inline constexpr double BUBBLE_FONT_SIZE = 24.0;
inline constexpr Dimension BUBBLE_POSITION_X = Dimension(-84.0, DimensionUnit::VP);
inline constexpr Dimension BUBBLE_POSITION_Y = 48.0_vp;
inline constexpr uint32_t BUBBLE_FONT_COLOR = 0xFF254FF7;
inline constexpr uint32_t BUBBLE_BG_COLOR = 0xFFF1F3F5;
inline constexpr double INDEXER_ITEM_SIZE = 16.0;      // circle mode, item size (VP)
inline constexpr double INDEXER_ITEM_TEXT_SIZE = 12.0; // circle, mode font size (VP)
// data for circle mode
inline constexpr int32_t INDEXER_COLLAPSE_ITEM_COUNT = 4;
inline constexpr double INDEXER_CIRCLE_ITEM_SIZE = 24.0;          // circle mode, item size (VP)
inline constexpr double INDEXER_CIRCLE_ITEM_TEXT_SIZE = 13.0;     // circle, mode font size (VP)
inline constexpr double INDEXER_CIRCLE_ITEM_SHADOW_RADIUS = 27.0; // circle mode, shadow radius (VP)
inline constexpr double INDEXER_ANGLE_TO_RADIAN = 180.0;
inline constexpr double INDEXER_ARC_BEGIN = -0.4488; // default angle
inline constexpr double INDEXER_ARC_LENGTH = 2.0 * M_PI;
inline constexpr double BUBBLE_BOX_SIZE_CIRCLE = 46.0;
inline constexpr double BUBBLE_FONT_SIZE_CIRCLE = 19.0;
inline constexpr uint32_t BUBBLE_BG_COLOR_CIRCLE = 0xFF404040;
inline constexpr uint32_t INDEXER_CIRCLE_ARC_COLOR = 0xFF212121;
inline constexpr uint32_t INDEXER_CIRCLE_ACTIVE_BG_COLOR = 0xFF007DFF;
inline constexpr uint32_t INDEXER_ACTIVE_BG_COLOR = 0x1F0A59F7;

class IndexerComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(IndexerComponent, ComponentGroup);

public:
    IndexerComponent(const RefPtr<ListComponent>& list, bool circleMode, bool bubble = true, bool multiLanguage = false)
        : indexerLabel_(Localization::GetInstance()->GetIndexLetter()), list_(list), circleMode_(circleMode),
          bubbleEnabled_(bubble), multiLanguageEnabled_(multiLanguage)
    {
        if (indexerLabel_.empty()) {
            indexerLabel_ = GetU16StrVector(alphabetDefault_);
        }
        if (circleMode_) {
            itemSize_ = Dimension(INDEXER_CIRCLE_ITEM_SIZE, DimensionUnit::VP);
        } else {
            itemSize_ = Dimension(INDEXER_ITEM_SIZE, DimensionUnit::VP);
        }
        BuildIndexerAlphabet();
        InitIndexerItemStyle();
        BuildIndexerItems();
    }

    IndexerComponent(const RefPtr<ListComponent>& list, bool circleMode, const std::vector<std::string>& label,
        bool bubble = true, bool multiLanguage = false)
        : list_(list), circleMode_(circleMode), bubbleEnabled_(bubble), multiLanguageEnabled_(multiLanguage)
    {
        indexerLabel_ = GetU16StrVector(label);
        userDefineList_ = true;
        if (indexerLabel_.empty()) {
            indexerLabel_ = Localization::GetInstance()->GetIndexLetter();
            userDefineList_ = false;
        }
        if (circleMode_) {
            itemSize_ = Dimension(INDEXER_CIRCLE_ITEM_SIZE, DimensionUnit::VP);
        } else {
            itemSize_ = Dimension(INDEXER_ITEM_SIZE, DimensionUnit::VP);
        }
        BuildIndexerAlphabet();
        InitIndexerItemStyle();
        BuildIndexerItems();
    }

    ~IndexerComponent() override = default;
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    const RefPtr<ScrollPositionController>& GetController() const
    {
        return controller_;
    }

    void SetController(const RefPtr<ScrollPositionController>& controller)
    {
        controller_ = controller;
    }

    // init data
    void FormatLabelAlphabet();
    void BuildIndexerAlphabet();
    void BuildDefaultAlphabet();
    void InitIndexerItemStyle();
    void BuildIndexerItems();
    void BuildArcItem();
    void BuildIndicatorBox();
    void BuildBubbleBox();
    void BuildTextItem(const std::u16string& strSection, const std::u16string& strLabel, int32_t itemType = 0);
    void BuildCollapseItem();

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

    // add item
    int32_t AddItemIndexKey(const std::string& indexKey, const std::string& headStyle);

    // remove item
    bool RemoveItemIndexKey(const std::string& indexKey);

    RefPtr<IndexerItemComponent> GetIndexerItem(const std::string& indexKey);
    int32_t GetSectionIndexInList(const std::string& indexKey);
    int32_t GetSectionIndexInIndexer(int32_t index);

    // for debug
    void PrintItemInfo() const;

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

    bool GetCircleMode() const
    {
        return circleMode_;
    }

    void SetCircleMode(bool circleMode)
    {
        circleMode_ = circleMode;
    }

    const Dimension& GetItemSize() const
    {
        return itemSize_;
    }

    void SetItemSize(const Dimension& size)
    {
        itemSize_ = size;
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
    }

    const TextStyle& GetActiveTextStyle() const
    {
        return activeStyle_;
    }

    void SetActiveTextStyle(const TextStyle& textStyle)
    {
        activeStyle_ = textStyle;
    }

    std::vector<std::u16string> GetU16StrVector(const std::vector<std::string>& str)
    {
        std::vector<std::u16string> alphabet;
        for (const auto& item : str) {
            alphabet.push_back(StringUtils::Str8ToStr16(item));
        }
        return alphabet;
    }
    std::list<RefPtr<IndexerItemComponent>> GetIndexerItemsComponents()
    {
        return listItem_;
    }

    void SetIndexerChange(const EventMarker& indexChange)
    {
        indexChange_ = indexChange;
    }

    const EventMarker& GetIndexerChange() const
    {
        return indexChange_;
    }

protected:
    void UpdateSectionIndex();
    // add
    int32_t AddSectionHead(const RefPtr<IndexerItemComponent>& indexerItem, const std::string& headStyleStr);
    int32_t AddItemToSharp(const std::string& indexKey, const std::string& headStyle);
    // remove
    bool RemoveSectionHead(int32_t index);
    bool RemoveItemFromSharp(const std::string& indexKey);

    std::vector<std::string> alphabetDefault_ = { "#", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

    std::vector<std::string> alphabetIndexer_ = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
        "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

    std::vector<std::u16string> indexerLabel_;  // user input sections
    std::vector<std::u16string> sectionsLocal_; // actual sections, exclude dot
    std::vector<std::u16string> labelLocal_;    // sections shown on the screen with dot inside
    std::vector<std::u16string> defaultAlphaLocal_;
    TextStyle normalStyle_;
    TextStyle activeStyle_;

    std::list<RefPtr<IndexerItemComponent>> listItem_;
    RefPtr<ListComponent> list_;
    RefPtr<ScrollPositionController> controller_;
    RefPtr<TextComponent> bubbleText_;

    int32_t itemCount_ = INDEXER_ITEM_MAX_COUNT;    // actual indexer item
    int32_t maxShowCount_ = INDEXER_ITEM_MAX_COUNT; // max item shown on screen
    int32_t nonItemCount_ = 0;                      // not indexer item count such as arc and bubbleBox
    // item size
    Dimension itemSize_;
    double startPosition_ = INDEXER_ARC_BEGIN; // arc start angle
    double arcLength_ = INDEXER_ARC_LENGTH;    // arc length
    bool circleMode_ = false;                  // list mode or circle mode
    bool userDefineList_ = false;              // user define alphabet list
    bool isFirstItem_ = false;
    bool hasCollapseItem_ = false;
    bool bubbleEnabled_ = true;
    bool multiLanguageEnabled_ = false; // support multi language or not

private:
    EventMarker indexChange_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_INDEXER_COMPONENT_H
