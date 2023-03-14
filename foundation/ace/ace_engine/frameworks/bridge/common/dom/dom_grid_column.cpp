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

#include "frameworks/bridge/common/dom/dom_grid_column.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DomGridColumn::DomGridColumn(NodeId nodeId, const std::string& nodeName) : DOMDiv(nodeId, nodeName)
{
    infoBuilder_.SetColumns(1); // default span 1 column
}

RefPtr<Component> DomGridColumn::GetSpecializedComponent()
{
    if (!columnInfo_) {
        columnInfo_ = infoBuilder_.Build();
        boxComponent_->SetGridLayoutInfo(columnInfo_);
        if (!flexItemComponent_) {
            flexItemComponent_ = AceType::MakeRefPtr<FlexItemComponent>();
        }
        flexItemComponent_->SetGridColumnInfoBuilder(boxComponent_->GetGridColumnInfoBuilder());
    }
    return DOMDiv::GetSpecializedComponent();
}

void DomGridColumn::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    DOMDiv::OnChildNodeAdded(child, slot);
}

void DomGridColumn::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    ACE_DCHECK(child);
    DOMDiv::OnChildNodeRemoved(child);
}

bool DomGridColumn::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, DomGridColumn&)> attrOperators[] {
        { DOM_GRID_SIZE_TYPE_LG,
            [](const std::string& value, DomGridColumn& column) {
                uint32_t span = 0;
                Dimension offset = UNDEFINED_DIMENSION;
                column.ParseSpanAndOffset(value, span, offset);
                column.infoBuilder_.SetLgSizeColumn(span, offset);
            } },
        { DOM_GRID_SIZE_TYPE_MD,
            [](const std::string& value, DomGridColumn& column) {
                uint32_t span = 0;
                Dimension offset = UNDEFINED_DIMENSION;
                column.ParseSpanAndOffset(value, span, offset);
                column.infoBuilder_.SetMdSizeColumn(span, offset);
            } },
        { DOM_GRID_COLUMN_OFFSET,
            [](const std::string& value, DomGridColumn& column) {
                auto declaration = column.GetDeclaration();
                if (!declaration) {
                    return;
                }
                auto& positionStyle =
                    declaration->MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid()) {
                    positionStyle.position = PositionType::ABSOLUTE;
                    positionStyle.top = Dimension(0.0, DimensionUnit::PX);
                    positionStyle.left = column.ParseDimension(value);
                    declaration->SetHasLeft(true);
                    declaration->SetHasTop(true);
                    declaration->SetHasPositionStyle(true);
                }
            } },
        { DOM_GRID_SIZE_TYPE_SM,
            [](const std::string& value, DomGridColumn& column) {
                uint32_t span = 0;
                Dimension offset = UNDEFINED_DIMENSION;
                column.ParseSpanAndOffset(value, span, offset);
                column.infoBuilder_.SetSmSizeColumn(span, offset);
            } },
        { DOM_GRID_COLUMN_SPAN,
            [](const std::string& value, DomGridColumn& column) {
                column.infoBuilder_.SetColumns(StringUtils::StringToUint(value));
            } },
        { DOM_GRID_SIZE_TYPE_XS,
            [](const std::string& value, DomGridColumn& column) {
                uint32_t span = 0;
                Dimension offset = UNDEFINED_DIMENSION;
                column.ParseSpanAndOffset(value, span, offset);
                column.infoBuilder_.SetXsSizeColumn(span, offset);
            } },
    };

    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        attrOperators[operatorIter].value(attr.second, *this);
        return true;
    }
    return false;
}

void DomGridColumn::DomGridColumn::ParseSpanAndOffset(const std::string& value, uint32_t& span, Dimension& offset)
{
    span = StringUtils::StringToUint(value);
    if (span > 0) {
        return;
    }

    std::unique_ptr<JsonValue> jsonValue = JsonUtil::ParseJsonString(value);
    if (!jsonValue) {
        LOGE("parse format is error");
        return;
    }

    std::unique_ptr<JsonValue> spanValue = jsonValue->GetValue(DOM_GRID_COLUMN_SPAN);
    if (spanValue && spanValue->IsNumber()) {
        span = spanValue->GetUInt();
    } else {
        LOGW("get span failed");
    }

    std::unique_ptr<JsonValue> offsetValue = jsonValue->GetValue(DOM_GRID_COLUMN_OFFSET);
    if (!offsetValue) {
        return;
    }
    if (offsetValue->IsNumber()) {
        offset.SetValue(offsetValue->GetDouble());
    } else if (offsetValue->IsString()) {
        offset = ParseDimension(offsetValue->GetString());
    }
}

} // namespace OHOS::Ace::Framework
