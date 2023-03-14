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

#include "frameworks/bridge/common/dom/dom_stack.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/common/ace_application_info.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace::Framework {

DOMStack::DOMStack(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    stackChild_ = AceType::MakeRefPtr<StackComponent>(
        alignment_, stackFit_, Overflow::OBSERVABLE, std::list<RefPtr<Component>>());
}

void DOMStack::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    LOGD("DOMStack Add Child");
    // If child has absolute position, the stack should be as large as the box component, so that the position is
    // correct in front-end.
    auto childDeclaration = child->GetDeclaration();
    if (childDeclaration && childDeclaration->HasPositionStyle() &&
        stackChild_->GetMainStackSize() != MainStackSize::MAX) {
        stackChild_->SetMainStackSize(MainStackSize::MATCH_CHILDREN);
    }
    stackChild_->InsertChild(slot, child->GetRootComponent());
}

void DOMStack::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    ACE_DCHECK(child);
    LOGD("DOMStack remove child");
    stackChild_->RemoveChild(child->GetRootComponent());
}

bool DOMStack::GetAxisOffset(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMStack&)> stackStyleOperators[] = {
        { DOM_ALIGN_ITEMS,
            [](const std::string& val, DOMStack& stack) {
              if (val == DOM_ALIGN_ITEMS_END) {
                  stack.crossAxisAlign_ = END;
              } else if (val == DOM_ALIGN_ITEMS_CENTER) {
                  stack.crossAxisAlign_ = CENTER;
              } else {
                  stack.crossAxisAlign_ = START;
              }
            } },
        { DOM_FLEX_DIRECTION,
            [](const std::string& val, DOMStack& stack) {
              if (val == DOM_FLEX_COLUMN) {
                  stack.direction_ = FlexDirection::COLUMN;
              } else {
                  stack.direction_ = FlexDirection::ROW;
              }
            } },
        { DOM_JUSTIFY_CONTENT,
            [](const std::string& val, DOMStack& stack) {
              if (val == DOM_JUSTIFY_CONTENT_END) {
                  stack.mainAxisAlign_ = END;
              } else if (val == DOM_JUSTIFY_CONTENT_CENTER || val == DOM_JUSTIFY_CONTENT_AROUND) {
                  stack.mainAxisAlign_ = CENTER;
              } else {
                  stack.mainAxisAlign_ = START;
              }
            } },
    };
    auto operatorIter = BinarySearchFindIndex(stackStyleOperators, ArraySize(stackStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        stackStyleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

bool DOMStack::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return GetAxisOffset(style);
}

void DOMStack::PrepareSpecializedComponent()
{
    if (FlexDirection::COLUMN == direction_) {
        alignment_ = AlignArray[mainAxisAlign_][crossAxisAlign_];
    } else {
        alignment_ = AlignArray[crossAxisAlign_][mainAxisAlign_];
    }
    LOGD("DOMStack Vertical:%{private}lf ,Horizontal:%{private}lf", alignment_.GetVertical(),
         alignment_.GetHorizontal());
    if (boxComponent_->GetWidthDimension().IsValid() && boxComponent_->GetHeightDimension().IsValid()) {
        stackChild_->SetMainStackSize(MainStackSize::MAX);
    } else if (boxComponent_->GetWidthDimension().IsValid()) {
        stackChild_->SetMainStackSize(MainStackSize::MAX_X);
    } else if (boxComponent_->GetHeightDimension().IsValid()) {
        stackChild_->SetMainStackSize(MainStackSize::MAX_Y);
    }
    stackChild_->SetAlignment(alignment_);
    SetAlignment(alignment_);
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));
    if (!overflowStyle.IsValid()) {
        return;
    }
    if (overflowStyle.overflow == Overflow::SCROLL) {
        stackChild_->SetMainStackSize(MainStackSize::MATCH_CHILDREN);
    }
}

void DOMStack::CompositeComponents()
{
    DOMNode::CompositeComponents();

    if (!declaration_) {
        return;
    }
    scroll_.Reset();
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));
    if (!overflowStyle.IsValid()) {
        return;
    }

    bool isRootScroll =
        isRootNode_ && (!declaration_->HasOverflowStyle() || overflowStyle.overflow == Overflow::SCROLL);
    bool isCard = AceApplicationInfo::GetInstance().GetIsCardType();
    if (isRootScroll && !isCard) {
        auto rootChild = rootComponent_->GetChild();
        scroll_ = AceType::MakeRefPtr<ScrollComponent>(rootChild);
        scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor, overflowStyle.scrollBarWidth,
                               overflowStyle.edgeEffect);
        declaration_->SetPositionController(scroll_->GetScrollPositionController());
        rootComponent_->SetChild(scroll_);
    }
}

} // namespace OHOS::Ace::Framework
