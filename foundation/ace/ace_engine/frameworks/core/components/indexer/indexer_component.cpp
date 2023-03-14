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

#include "core/components/indexer/indexer_component.h"

#include "core/components/arc/arc_component.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/indexer/indexer_element.h"
#include "core/components/indexer/render_indexer_circle.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {

RefPtr<Element> Ace::IndexerComponent::CreateElement()
{
    LOGI("[indexer] CreateElement ");
    return AceType::MakeRefPtr<IndexerElement>();
}

RefPtr<RenderNode> IndexerComponent::CreateRenderNode()
{
    LOGI("[indexer] CreateRenderNode ");
    if (circleMode_) {
        return AceType::MakeRefPtr<RenderIndexerCircle>();
    } else {
        return AceType::MakeRefPtr<RenderIndexer>();
    }
}

void IndexerComponent::FormatLabelAlphabet()
{
    for (auto& item : indexerLabel_) {
        if (item == INDEXER_STR_DOT_EX) {
            item = INDEXER_STR_DOT;
        }
    }
}

void IndexerComponent::BuildDefaultAlphabet()
{
    defaultAlphaLocal_ = GetU16StrVector(alphabetIndexer_);
}

void IndexerComponent::BuildIndexerAlphabet()
{
    BuildDefaultAlphabet();
    std::vector<std::u16string> alphabet = Localization::GetInstance()->GetIndexAlphabet(); // get alphabet
    if (alphabet.empty()) {
        LOGE("fail to build indexer alphabet due to alphabet is empty");
        return;
    }
    FormatLabelAlphabet();
    // alphabet must begin with "#", and items "." can not be neighbors
    if (alphabet[0] != INDEXER_STR_SHARP || indexerLabel_[0] != INDEXER_STR_SHARP) {
        LOGE("fail to build indexer alphabet due to alphabet is wrong");
        return;
    }

    sectionsLocal_.clear();
    labelLocal_.clear();

    std::u16string strItem;
    size_t countAlphabet = 0;
    for (size_t i = 0; i < indexerLabel_.size(); ++i) {
        if (indexerLabel_[i] != INDEXER_STR_DOT) {
            sectionsLocal_.push_back(indexerLabel_[i]);
            labelLocal_.push_back(indexerLabel_[i]);
            ++countAlphabet;
        } else {
            if (i == indexerLabel_.size() - 1) { // while this "." is the last item
                while (countAlphabet < alphabet.size()) {
                    strItem += alphabet[countAlphabet];
                    strItem += INDEXER_STR_SPACE;
                    ++countAlphabet;
                }
                sectionsLocal_.push_back(strItem);
                labelLocal_.push_back(indexerLabel_[i]);
            } else if (indexerLabel_[i + 1] != INDEXER_STR_DOT) { // while the next item is not "."
                while ((countAlphabet < alphabet.size()) && (indexerLabel_[i + 1] != alphabet[countAlphabet])) {
                    strItem += alphabet[countAlphabet];
                    strItem += INDEXER_STR_SPACE;
                    ++countAlphabet;
                }
                sectionsLocal_.push_back(strItem);
                labelLocal_.push_back(indexerLabel_[i]);
            } else {
                // Do nothing while the next item is "." too
            }
        }
        strItem.clear();
    }
}

void IndexerComponent::InitIndexerItemStyle()
{
    if (GetCircleMode()) {
        // Indexer item style when binding list item is not current.
        normalStyle_.SetFontSize(Dimension(INDEXER_CIRCLE_ITEM_TEXT_SIZE, DimensionUnit::FP));
        normalStyle_.SetFontWeight(FontWeight::W400);
        normalStyle_.SetTextColor(Color::WHITE);

        // Indexer item style when binding list item is current.
        activeStyle_ = normalStyle_;
    } else {
        // Indexer item style when binding list item is not current.
        normalStyle_.SetFontSize(Dimension(INDEXER_ITEM_TEXT_SIZE, DimensionUnit::FP));
        normalStyle_.SetFontWeight(FontWeight::W400);
        normalStyle_.SetTextColor(Color(INDEXER_LIST_COLOR));

        // Indexer item style when binding list item is current.
        activeStyle_.SetFontSize(Dimension(INDEXER_LIST_ITEM_TEXT_SIZE, DimensionUnit::FP));
        activeStyle_.SetFontWeight(FontWeight::W500);
        activeStyle_.SetTextColor(Color(INDEXER_LIST_ACTIVE_COLOR));
    }
}

void IndexerComponent::BuildArcItem()
{
    RefPtr<ArcComponent> arcItem = AceType::MakeRefPtr<ArcComponent>();
    arcItem->SetWidth(itemSize_);
    arcItem->SetStartAngle(startPosition_);
    arcItem->SetSweepAngle(arcLength_);
    arcItem->SetColor(Color(INDEXER_CIRCLE_ARC_COLOR));
    arcItem->SetShadowWidth(Dimension(INDEXER_CIRCLE_ITEM_SHADOW_RADIUS, DimensionUnit::VP));
    AppendChild(arcItem);
    nonItemCount_++;
}

void IndexerComponent::BuildBubbleBox()
{
    if (!bubbleEnabled_) {
        return;
    }
    RefPtr<BoxComponent> bubble = AceType::MakeRefPtr<BoxComponent>();
    bubble->SetFlex(BoxFlex::FLEX_NO);
    bubble->SetAlignment(Alignment::CENTER);
    Radius radius = Radius(Dimension(BUBBLE_BOX_SIZE_CIRCLE, DimensionUnit::VP) * HALF);
    RefPtr<Decoration> back = AceType::MakeRefPtr<Decoration>();
    if (circleMode_) {
        bubble->SetWidth(BUBBLE_BOX_SIZE_CIRCLE, DimensionUnit::VP);
        bubble->SetHeight(BUBBLE_BOX_SIZE_CIRCLE, DimensionUnit::VP);
        back->SetBackgroundColor(Color(BUBBLE_BG_COLOR_CIRCLE).BlendOpacity(NINETY_OPACITY_IN_PERCENT));
    } else {
        // for shadow blur region
        bubble->SetMargin(Edge(200));
        bubble->SetWidth(BUBBLE_BOX_SIZE, DimensionUnit::VP);
        bubble->SetHeight(BUBBLE_BOX_SIZE, DimensionUnit::VP);
        radius = Radius(Dimension(BUBBLE_BOX_RADIUS, DimensionUnit::VP));
        back->SetBackgroundColor(Color(BUBBLE_BG_COLOR).BlendOpacity(NINETY_OPACITY_IN_PERCENT));
        back->AddShadow(ShadowConfig::DefaultShadowL);
    }
    back->SetBorderRadius(radius);
    bubble->SetBackDecoration(back);
    bubbleText_ = AceType::MakeRefPtr<TextComponent>(StringUtils::Str16ToStr8(INDEXER_STR_SHARP));
    TextStyle textStyle;
    textStyle.SetTextAlign(TextAlign::CENTER);
    if (circleMode_) {
        textStyle.SetFontSize(Dimension(BUBBLE_FONT_SIZE_CIRCLE, DimensionUnit::FP));
        textStyle.SetTextColor(Color::WHITE);
    } else {
        textStyle.SetFontSize(Dimension(BUBBLE_FONT_SIZE, DimensionUnit::VP));
        textStyle.SetTextColor(Color(BUBBLE_FONT_COLOR));
    }
    bubbleText_->SetTextStyle(textStyle);
    bubble->SetChild(bubbleText_);
    RefPtr<DisplayComponent> displayComponent = AceType::MakeRefPtr<DisplayComponent>(bubble);
    displayComponent->SetOpacity(ZERO_OPACITY);
    AppendChild(displayComponent);
    nonItemCount_++;
}

void IndexerComponent::BuildIndicatorBox()
{
    RefPtr<BoxComponent> indicator = AceType::MakeRefPtr<BoxComponent>();
    indicator->SetWidth(INDEXER_CIRCLE_ITEM_SIZE, DimensionUnit::VP);
    indicator->SetHeight(INDEXER_CIRCLE_ITEM_SIZE, DimensionUnit::VP);
    indicator->SetFlex(BoxFlex::FLEX_NO);
    indicator->SetAlignment(Alignment::CENTER);
    RefPtr<Decoration> front = AceType::MakeRefPtr<Decoration>();
    Radius radius = Radius(itemSize_ * HALF);
    front->SetBorderRadius(radius);
    front->SetBackgroundColor(Color::TRANSPARENT);
    indicator->SetFrontDecoration(front);
    AppendChild(indicator);
    nonItemCount_++;
}

void IndexerComponent::BuildTextItem(const std::u16string& strSection, const std::u16string& strLabel, int32_t itemType)
{
    RefPtr<IndexerItemComponent> textItem =
        AceType::MakeRefPtr<IndexerItemComponent>(strSection, strLabel, itemSize_, circleMode_, false);
    textItem->SetNormalTextStyle(normalStyle_);
    textItem->SetActiveTextStyle(activeStyle_);
    textItem->SetTextStyle(false);
    textItem->SetItemType(itemType);
    if (isFirstItem_ || strSection == INDEXER_STR_SHARP || strSection == INDEXER_STR_COLLECT) {
        textItem->MarkItemPrimary();
        if (!userDefineList_ || strSection != INDEXER_STR_SHARP) {
            isFirstItem_ = false;
        }
    }
    listItem_.push_back(textItem);
    AppendChild(textItem);
    ++itemCount_;
}

void IndexerComponent::BuildCollapseItem()
{
    RefPtr<IndexerItemComponent> collapseItem = AceType::MakeRefPtr<IndexerItemComponent>(
        INDEXER_STR_COLLAPSE, INDEXER_STR_COLLAPSE, itemSize_, circleMode_, true);
    collapseItem->SetNormalTextStyle(normalStyle_);
    collapseItem->SetActiveTextStyle(activeStyle_);
    collapseItem->SetTextStyle(false);
    collapseItem->MarkItemPrimary();
    listItem_.push_back(collapseItem);
    AppendChild(collapseItem);
    ++itemCount_;
    hasCollapseItem_ = true;
}

void IndexerComponent::BuildIndexerItems()
{
    uint32_t length = static_cast<uint32_t>(labelLocal_.size());
    if (length == 0) {
        LOGE("[indexer] invalid section string");
        return;
    }

    // add arc first while circle mode
    if (GetCircleMode()) {
        BuildArcItem();
        BuildIndicatorBox();
    }
    BuildBubbleBox();
    itemCount_ = 0;

    // add “*” item while circle mode
    if (GetCircleMode() && !userDefineList_) {
        BuildTextItem(INDEXER_STR_COLLECT, INDEXER_STR_COLLECT);
    }
    // add "#" first
    if (!userDefineList_) {
        BuildTextItem(INDEXER_STR_SHARP, INDEXER_STR_SHARP);
    }
    // add indexer items except '#'
    isFirstItem_ = true;
    for (uint32_t i = 0; i < length; ++i) {
        std::u16string strItem = labelLocal_[i];
        if (!userDefineList_ && strItem == INDEXER_STR_SHARP) {
            continue;
        }
        BuildTextItem(sectionsLocal_[i], strItem);
    }

    if (circleMode_ && multiLanguageEnabled_) {
        isFirstItem_ = true;
        // add default alphabet indexer
        uint32_t count = defaultAlphaLocal_.size();
        for (uint32_t i = 0; i < count; ++i) {
            std::u16string strItem = defaultAlphaLocal_[i];
            BuildTextItem(strItem, strItem, 1);
        }
        ++maxShowCount_;
        if (userDefineList_ && indexerLabel_[0] != INDEXER_STR_COLLECT) {
            --maxShowCount_;
        }
    }
    isFirstItem_ = false;

    // add collapse item while circle mode
    if (GetCircleMode() && itemCount_ > INDEXER_COLLAPSE_ITEM_COUNT) {
        BuildCollapseItem();
    }
    if (GetCircleMode()) {
        RefPtr<ArcComponent> arc =
            AceType::DynamicCast<ArcComponent>(GetChildren().empty() ? nullptr : GetChildren().front());
        if (arc) {
            double arcLen = itemCount_ < maxShowCount_ ?
                INDEXER_ARC_LENGTH / maxShowCount_ * (itemCount_ - 1) : INDEXER_ARC_LENGTH;
            arc->SetSweepAngle(arcLen);
        }
    }
    LOGI("[indexer] BuildIndexerItems circleMode:%{public}d, itemCount_:%{public}d", circleMode_, itemCount_);
}

RefPtr<IndexerItemComponent> IndexerComponent::GetIndexerItem(const std::string& indexKey)
{
    RefPtr<IndexerItemComponent> ret = nullptr;
    if (indexKey.empty()) {
        LOGE("[indexer] indexKey is NULL");
        return ret;
    }

    LOGI("[indexer] GetIndexerItem section: %{public}s", indexKey.c_str());
    for (const auto& item : listItem_) {
        if (item->IsCorrectItem(indexKey)) {
            LOGI("[indexer] section: %{public}s", StringUtils::Str16ToStr8(item->GetSectionStr()).c_str());
            ret = item;
            break;
        }
    }
    return ret;
}

int32_t IndexerComponent::AddItemIndexKey(const std::string& indexKey, const std::string& headStyle)
{
    int32_t itemIndex = INDEXER_INVALID_INDEX;
    if (!list_) {
        LOGE("[indexer] AddItemIndexKey List ptr is NULL");
        return itemIndex;
    }

    if (indexKey.empty()) {
        LOGI("[indexer] indexKey is NULL, add to Sharp area.");
        // add to sharp section, while indexkey is Null
        return AddItemToSharp(indexKey, headStyle);
    }

    for (const auto& item : listItem_) {
        if (item && (item->IsCorrectItem(indexKey))) {
            // add section head
            if (item->GetKeyCount() == 0 && !GetCircleMode()) {
                AddSectionHead(item, headStyle);
            }
            // add index key
            itemIndex = static_cast<int32_t>(item->AddIndexKey(indexKey));
            break;
        }
    }

    // add to sharp section, while indexkey is number
    if (itemIndex == INVALID_INDEX) {
        itemIndex = AddItemToSharp(indexKey, headStyle);
    } else {
        UpdateSectionIndex();
    }

    return itemIndex;
}

int32_t IndexerComponent::AddItemToSharp(const std::string& indexKey, const std::string& headStyle)
{
    // find the # section
    RefPtr<IndexerItemComponent> itemPtr = nullptr;
    if (!listItem_.empty()) {
        for (const auto& item : listItem_) {
            if (item && (item->GetSectionStr() == INDEXER_STR_SHARP)) {
                itemPtr = item;
                break;
            }
        }
    }
    if (!itemPtr) {
        LOGE("[indexer] AddItemToSharp get # section failed");
        return 0;
    }

    // add section head
    if (itemPtr->GetKeyCount() == 0 && !GetCircleMode()) {
        AddSectionHead(itemPtr, headStyle);
    }
    // add index key
    int32_t itemIndex = static_cast<int32_t>(itemPtr->AddIndexKey(indexKey));
    // update section head index
    UpdateSectionIndex();

    return itemIndex;
}

void IndexerComponent::PrintItemInfo() const
{
    for (const auto& item : listItem_) {
        item->PrintIndexerItem();
    }
}

int32_t IndexerComponent::AddSectionHead(
    const RefPtr<IndexerItemComponent>& indexerItem, const std::string& headStyleStr)
{
    int32_t sectionHeadIndex = INDEXER_INVALID_INDEX;
    if (GetCircleMode()) {
        LOGI("[indexer] Circle Mode, cancel section head adding");
        return sectionHeadIndex;
    }

    if (!list_) {
        LOGE("[indexer] List ptr is NULL");
        return sectionHeadIndex;
    }

    if (!indexerItem) {
        LOGE("[indexer] Invalid indexerItem ");
        return sectionHeadIndex;
    }

    std::string strSectionHeadLabel = StringUtils::Str16ToStr8(indexerItem->GetLabelStr());
    if (strSectionHeadLabel.empty()) {
        LOGE("[indexer] Invalid Section head, strSectionHead:%{public}s", strSectionHeadLabel.c_str());
        return sectionHeadIndex;
    }

    // [indexer] Section head style need check
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>(strSectionHeadLabel);
    TextStyle headStyle;
    if (circleMode_) {
        headStyle.SetFontSize(Dimension(INDEXER_CIRCLE_ITEM_TEXT_SIZE, DimensionUnit::FP));
    } else {
        headStyle.SetFontSize(Dimension(INDEXER_ITEM_TEXT_SIZE, DimensionUnit::FP));
    }
    headStyle.SetFontWeight(FontWeight::W500);
    headStyle.SetTextColor(Color::BLACK);
    text->SetTextStyle(headStyle);
    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    box->SetFlex(BoxFlex::FLEX_X);
    box->SetDeliverMinToChild(false);
    box->SetAlignment(Alignment::CENTER);
    box->SetColor(Color::WHITE);
    box->SetChild(text);
    RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>(headStyleStr, box);
    listItem->SetSticky(true);
    listItem->SetIndexKey(strSectionHeadLabel);
    RefPtr<ComposedComponent> composedItem =
        AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
    sectionHeadIndex = indexerItem->GetSectionIndex();
    list_->InsertChild(sectionHeadIndex, composedItem);
    LOGI("[indexer] strSectionHeadLabel:%{public}s, headStyle:%{public}s index:%{public}d",
        strSectionHeadLabel.c_str(), headStyleStr.c_str(), sectionHeadIndex);
    return sectionHeadIndex;
}

bool IndexerComponent::RemoveSectionHead(int32_t index)
{
    bool ret = false;
    if (GetCircleMode()) {
        LOGD("[indexer] RemoveSectionHead Circle Mode, cancel section head Delete");
        return ret;
    }

    if (!list_) {
        LOGE("[indexer] RemoveSectionHead List ptr is NULL");
        return ret;
    }

    for (const auto& item : list_->GetChildren()) {
        RefPtr<ListItemComponent> listItem = ListItemComponent::GetListItem(item);
        if (listItem && listItem->GetIndex() == index) {
            list_->RemoveChild(item);
            ret = true;
            break;
        }
    }

    return ret;
}

bool IndexerComponent::RemoveItemIndexKey(const std::string& indexKey)
{
    bool ret = false;
    if (!list_) {
        LOGE("[indexer] RemoveItemIndexKey List ptr is NULL");
        return ret;
    }

    if (indexKey.empty()) {
        LOGI("[indexer] RemoveItemIndexKey indexKey is NULL");
        // remove item while the index is null
        return RemoveItemFromSharp(indexKey);
    }

    for (const auto& item : listItem_) {
        if (item && (item->IsCorrectItem(indexKey))) {
            int32_t sectionHeadIndex = item->GetSectionIndex();
            // remove index key
            ret = item->RemoveIndexKey(indexKey);
            // remove section head
            if (item->GetKeyCount() == 0 && !GetCircleMode()) {
                RemoveSectionHead(sectionHeadIndex);
            }
            break;
        }
    }

    if (ret) {
        // update section head index
        UpdateSectionIndex();
    } else {
        // remove item while indexkey is number, low probable case, put it last
        ret = RemoveItemFromSharp(indexKey);
    }

    return ret;
}

bool IndexerComponent::RemoveItemFromSharp(const std::string& indexKey)
{
    // remove item from "#" section
    bool ret = false;
    if (!list_) {
        LOGE("[indexer] RemoveItemFromSharp List ptr is NULL");
        return ret;
    }

    // find section "#"
    RefPtr<IndexerItemComponent> itemPtr = nullptr;
    if (!listItem_.empty()) {
        for (const auto& item : listItem_) {
            if (item && (item->GetSectionStr() == INDEXER_STR_SHARP)) {
                itemPtr = item;
                break;
            }
        }
    }
    if (!itemPtr) {
        LOGE("[indexer] RemoveItemFromSharp get # section failed");
        return ret;
    }

    ret = itemPtr->RemoveIndexKey(indexKey);
    if (!ret) {
        LOGE("[indexer] RemoveItemFromSharp remove IndexKey failed");
        return ret;
    }

    // update section head index
    UpdateSectionIndex();

    return ret;
}

int32_t IndexerComponent::GetSectionIndexInList(const std::string& indexKey)
{
    int32_t index = INVALID_INDEX;
    RefPtr<IndexerItemComponent> item = GetIndexerItem(indexKey);
    if (item) {
        index = item->GetSectionIndex();
    }
    return index;
}

int32_t IndexerComponent::GetSectionIndexInIndexer(int32_t index)
{
    int32_t count = 0;
    if (index <= 0) {
        return count;
    }
    for (const auto& item : listItem_) {
        if (item) {
            if (index < item->GetSectionIndex()) {
                break;
            }
        }
        count++;
    }
    return count - 1;
}

void IndexerComponent::UpdateSectionIndex()
{
    // update section head index
    if (listItem_.empty()) {
        LOGE("[indexer] UpdateSectionIndex listItem_ is empty");
        return;
    }

    int32_t sectionIndex = listItem_.front()->GetSectionIndex();
    for (const auto& item : listItem_) {
        if (!item) {
            continue;
        }
        item->SetSectionIndex(sectionIndex);
        if (item->GetKeyCount() != 0) {
            sectionIndex = sectionIndex + static_cast<int32_t>(item->GetKeyCount()) + 1;
            if (GetCircleMode()) {
                --sectionIndex;
            }
        }
    }
}

} // namespace OHOS::Ace
