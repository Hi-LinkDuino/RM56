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

#include "core/components/indexer/indexer_item_component.h"

#include "core/components/indexer/indexer_component.h"
#include "core/components/indexer/render_indexer_item.h"
#include "core/components/transform/transform_component.h"

namespace OHOS::Ace {

RefPtr<RenderNode> IndexerItemComponent::CreateRenderNode()
{
    return RenderIndexerItem::Create();
}

void IndexerItemComponent::BuildItem()
{
    box_ = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<Component> tail = box_;
    if (rotate_) {
        image_ = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::INDEXER_ARROW_PNG);
        image_->SetHeight(itemSize_);
        image_->SetWidth(itemSize_);
        box_->SetChild(image_);
        tail = image_;
    } else {
        text_ = AceType::MakeRefPtr<TextComponent>(StringUtils::Str16ToStr8(strLabel_));
        TextStyle textStyle;
        textStyle.SetTextAlign(TextAlign::CENTER);
        textStyle.SetFontWeight(FontWeight::W400);
        text_->SetTextStyle(textStyle);
        box_->SetChild(text_);
        tail = text_;
    }
    box_->SetFlex(BoxFlex::FLEX_NO);
    box_->SetWidth(itemSize_.Value(), itemSize_.Unit());
    box_->SetHeight(itemSize_.Value(), itemSize_.Unit());
    box_->SetAlignment(Alignment::CENTER);
    // generate background
    RefPtr<Decoration> background = AceType::MakeRefPtr<Decoration>();
    background->SetBackgroundColor(Color::TRANSPARENT);
    // generate circle
    Radius radius = circleMode_ ? Radius(itemSize_ / 2.0) : Radius(Dimension(4.0, DimensionUnit::VP));
    background->SetBorderRadius(radius);
    box_->SetBackDecoration(background);
    if (rotate_) {
        RefPtr<TransformComponent> rotate = AceType::MakeRefPtr<TransformComponent>();
        rotate->SetChild(box_);
        Component::MergeRSNode(rotate, tail);
        SetChild(rotate);
    } else {
        Component::MergeRSNode(box_, tail);
        SetChild(box_);
    }
}

bool IndexerItemComponent::IsCorrectItem(const std::string& indexKey) const
{
    // check whether the indexkey matched the section string
    if (strSection_.empty()) {
        LOGE("[indexer] IsCorrectItem invalid strSection_");
        return false;
    }

    if (strSection_ == INDEXER_STR_SHARP) {
        return false;
    }

    if (indexKey.empty()) {
        LOGE("[indexer] IsCorrectItem NULL index key");
        return false;
    }

    std::u16string u16IndexKey = StringUtils::Str8ToStr16(indexKey);
    for (size_t i = 0; i < strSection_.size(); ++i) {
        if (u16IndexKey[0] == strSection_[i]) {
            return true;
        }
    }

    return false;
}

uint32_t IndexerItemComponent::AddIndexKey(const std::string& indexKey)
{
    uint32_t itemIndex = 0;

    if (strSection_.empty()) {
        LOGE("[indexer] AddIndexKey invalid strSection_");
        return itemIndex;
    }

    if (strSection_ == INDEXER_STR_SHARP) {
        return AddIndexKeyForSharp(indexKey);
    }

    if (indexKey.empty()) {
        LOGE("[indexer] AddIndexKey NULL index key");
        return itemIndex;
    }

    if (!IsCorrectItem(indexKey)) {
        LOGE("[indexer] AddIndexKey index key does not match section");
        return itemIndex;
    }

    auto it = indexKey_.begin();
    for (; it != indexKey_.end(); ++it) {
        if (indexKey.compare((*it)) < 0) {
            itemIndex++;
            indexKey_.insert(it, indexKey);
            break;
        } else {
            itemIndex++;
        }
    }

    if (it == indexKey_.end()) {
        indexKey_.push_back(indexKey);
        itemIndex++;
    }

    keyCount_++;
    itemIndex += static_cast<uint32_t>(sectionIndex_);
    if (circleMode_) {
        itemIndex--;
    }

    return itemIndex;
}

uint32_t IndexerItemComponent::AddIndexKeyForSharp(const std::string& indexKey)
{
    uint32_t itemIndex = 0;
    if (strSection_ != INDEXER_STR_SHARP) {
        LOGE("[indexer] AddIndexKeyForSharp the section key is not #, section:%{public}s",
            StringUtils::Str16ToStr8(strSection_).c_str());
        return itemIndex;
    }

    auto item = indexKey_.begin();
    if (indexKey.empty()) {
        // add NULL key
        for (; item != indexKey_.end(); ++item) {
            itemIndex++;
            if (!item->empty()) {
                indexKey_.insert(item, indexKey);
                break;
            }
        }
    } else {
        // add number key
        for (; item != indexKey_.end(); ++item) {
            itemIndex++;
            if (indexKey.compare((*item)) < 0) {
                indexKey_.insert(item, indexKey);
                break;
            }
        }
    }

    if (item == indexKey_.end()) {
        indexKey_.push_back(indexKey);
        itemIndex++;
    }

    keyCount_++;
    itemIndex += static_cast<uint32_t>(sectionIndex_);
    if (circleMode_) {
        itemIndex--;
    }

    return itemIndex;
}

bool IndexerItemComponent::RemoveIndexKey(const std::string& indexKey)
{
    bool ret = false;
    if (strSection_.empty()) {
        LOGE("[indexer] RemoveIndexKey invalid strSection_");
        return ret;
    }

    if (strSection_ == INDEXER_STR_SHARP) {
        return RemoveIndexKeyForSharp(indexKey);
    }

    if (indexKey.empty()) {
        LOGE("[indexer] RemoveIndexKey NULL index key");
        return ret;
    }

    if (!IsCorrectItem(indexKey)) {
        LOGE("[indexer] RemoveIndexKey  index key does not match section");
        return ret;
    }

    auto it = indexKey_.begin();
    for (; it != indexKey_.end(); ++it) {
        if (indexKey == *it) {
            indexKey_.erase(it);
            keyCount_--;
            ret = true;
            break;
        }
    }

    return ret;
}

bool IndexerItemComponent::RemoveIndexKeyForSharp(const std::string& indexKey)
{
    bool ret = false;

    if (strSection_ != INDEXER_STR_SHARP) {
        LOGE("[indexer] RemoveIndexKeyForSharp the section key is not #, section:%{public}s",
            StringUtils::Str16ToStr8(strSection_).c_str());
        return ret;
    }

    auto item = indexKey_.begin();
    if (indexKey.empty()) {
        // remove NULL key
        for (; item != indexKey_.end(); ++item) {
            if (item->empty()) {
                indexKey_.erase(item);
                keyCount_--;
                ret = true;
                break;
            }
        }
    } else {
        // remove number key
        for (; item != indexKey_.end(); ++item) {
            if (indexKey == *item) {
                indexKey_.erase(item);
                keyCount_--;
                ret = true;
                break;
            }
        }
    }

    return ret;
}

void IndexerItemComponent::PrintIndexerItem() const
{
    int count = 1;
    LOGI("[indexer] label:%{public}s section:%{public}s %{public}d",
        StringUtils::Str16ToStr8(strLabel_).c_str(), StringUtils::Str16ToStr8(strSection_).c_str(), sectionIndex_);
    for (auto it = indexKey_.begin(); it != indexKey_.end(); ++it) {
        if (circleMode_) {
            LOGI("[indexer] key:%{public}s %{public}d", it->c_str(), sectionIndex_ + count - 1);
        } else {
            LOGI("[indexer] key:%{public}s %{public}d", it->c_str(), sectionIndex_ + count);
        }
        count++;
    }
}

void IndexerItemComponent::SetTextStyle(bool active)
{
    if (!text_) {
        LOGE("[indexer] Get child text failed.");
        return;
    }
    if (active) {
        text_->SetTextStyle(activeStyle_);
    } else {
        text_->SetTextStyle(normalStyle_);
    }
}

} // namespace OHOS::Ace
