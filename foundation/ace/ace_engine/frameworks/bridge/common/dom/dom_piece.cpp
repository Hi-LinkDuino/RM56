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

#include "frameworks/bridge/common/dom/dom_piece.h"

#include "core/components/declaration/piece/piece_declaration.h"

namespace OHOS::Ace::Framework {

DOMPiece::DOMPiece(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    pieceChild_ = AceType::MakeRefPtr<PieceComponent>();
}

void DOMPiece::ResetInitializedStyle()
{
    if (declaration_) {
        declaration_->InitializeStyle();
    }
}

void DOMPiece::PrepareSpecializedComponent()
{
    auto declaration = AceType::DynamicCast<PieceDeclaration>(declaration_);
    if (!declaration) {
        return;
    }

    pieceChild_->SetTextDirection(declaration->IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    if (!declaration->HasBackGroundColor() && declaration->HasBackground()) {
        declaration->GetBackDecoration()->SetBackgroundColor(Color::TRANSPARENT);
    }
    declaration->SetMargin(GetBoxComponent()->GetMargin());
    if (boxComponent_->GetBackDecoration()) {
        declaration->SetBorder(boxComponent_->GetBackDecoration()->GetBorder());
    }

    // If content is not exist, clear style.
    if (!declaration->HasContent()) {
        boxComponent_->SetWidth(0.0);
        boxComponent_->SetHeight(0.0);
        boxComponent_->SetPadding(Edge());
        boxComponent_->SetMargin(Edge());
    }

    pieceChild_->SetDeclaration(AceType::DynamicCast<PieceDeclaration>(declaration_));
}

RefPtr<Component> DOMPiece::GetSpecializedComponent()
{
    return pieceChild_;
}

}; // namespace OHOS::Ace::Framework
