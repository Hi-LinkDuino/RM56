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

#include "frameworks/bridge/common/dom/dom_grid_container.h"

#include "core/components/common/layout/grid_system_manager.h"
#include "frameworks/bridge/common/dom/dom_grid_row.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DomGridContainer::DomGridContainer(NodeId nodeId, const std::string& nodeName) : DOMDiv(nodeId, nodeName)
{
    direction_ = DOM_FLEX_COLUMN;
}

RefPtr<Component> DomGridContainer::GetSpecializedComponent()
{
    if (!gridContainerInfo_) {
        gridContainerInfo_ = infoBuilder_.Build();
        boxComponent_->SetGridLayoutInfo(gridContainerInfo_);
    }
    return DOMDiv::GetSpecializedComponent();
}

void DomGridContainer::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    DOMDiv::OnChildNodeAdded(child, slot);
    auto gridRow = AceType::DynamicCast<DomGridRow>(child);
    if (gridRow) {
        gridRow->SetParentGridInfo(gridContainerInfo_);
    }
}

void DomGridContainer::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    ACE_DCHECK(child);
    DOMDiv::OnChildNodeRemoved(child);
    auto gridRow = AceType::DynamicCast<DomGridRow>(child);
    if (gridRow) {
        gridRow->SetParentGridInfo(nullptr);
    }
}

void DomGridContainer::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    std::string result;
    if (method == DOM_GRID_CONTAINER_GET_COLUMNS) {
        result = std::to_string(gridContainerInfo_->GetColumns());
    } else if (method == DOM_GRID_CONTAINER_GET_COLUMN_WIDTH) {
        result = std::to_string(gridContainerInfo_->GetColumnWidth());
    } else if (method == DOM_GRID_CONTAINER_GET_GUTTER_WIDTH) {
        auto dipScale = GridSystemManager::GetInstance().GetDipScale();
        result = std::to_string(gridContainerInfo_->GetGutterWidth().ConvertToPx(dipScale));
    } else if (method == DOM_GRID_CONTAINER_GET_SIZE_TYPE) {
        auto sizeType = gridContainerInfo_->GetSizeType();
        result.append("\"");
        if (sizeType == GridSizeType::LG) {
            result.append(DOM_GRID_SIZE_TYPE_LG);
        } else if (sizeType == GridSizeType::MD) {
            result.append(DOM_GRID_SIZE_TYPE_MD);
        } else if (sizeType == GridSizeType::SM) {
            result.append(DOM_GRID_SIZE_TYPE_SM);
        }
        result.append("\"");
    } else {
        return;
    }
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != 1) {
        LOGE("parse args error");
        return;
    }
    auto callbackId = argsValue->GetArrayItem(0)->GetString();
    auto content = pipelineContext_.Upgrade();
    if (!content) {
        return;
    }
    content->SendCallbackMessageToFrontend(callbackId, result);
}

bool DomGridContainer::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, DomGridContainer&)> attrOperators[] {
        { DOM_GRID_CONTAINER_COLUMNS,
            [](const std::string& value, DomGridContainer& container) {
                container.infoBuilder_.SetColumns(StringUtils::StringToInt(value));
            } },
        { DOM_GRID_CONTAINER_TEMPLATE,
            [](const std::string& value, DomGridContainer& container) {
                auto templateType = GridTemplateType::NORMAL;
                auto iter = GridTemplateMap.find(value);
                if (iter != GridTemplateMap.end()) {
                    templateType = iter->second;
                }
                container.infoBuilder_.SetGridTemplateType(templateType);
            } },
        { DOM_GRID_CONTAINER_GUTTER,
            [](const std::string& value, DomGridContainer& container) {
                container.infoBuilder_.SetGutterWidth(Dimension(StringUtils::StringToDouble(value), DimensionUnit::PX));
            } },
        { DOM_GRID_CONTAINER_GUTTER_WIDTH,
            [](const std::string& value, DomGridContainer& container) {
                container.infoBuilder_.SetGutterWidth(Dimension(StringUtils::StringToDouble(value), DimensionUnit::PX));
            } },
        { DOM_GRID_CONTAINER_SIZE_TYPE,
            [](const std::string& value, DomGridContainer& container) {
                auto sizeType = GridSizeType::UNDEFINED;
                auto iter = GridSizeTypeMap.find(value);
                if (iter != GridSizeTypeMap.end()) {
                    sizeType = iter->second;
                }
                container.infoBuilder_.SetSizeType(sizeType);
            } },
    };

    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        attrOperators[operatorIter].value(attr.second, *this);
        return true;
    }
    return false;
}

bool DomGridContainer::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const std::list<const char*> notSupportStyles {
        DOM_FLEX_DIRECTION,
        DOM_GRID_COLUMN_END,
        DOM_GRID_COLUMN_START,
        DOM_GRID_COLUMN_GAP,
        DOM_GRID_ROW_END,
        DOM_GRID_ROW_START,
        DOM_GRID_ROW_GAP,
        DOM_GRID_TEMPLATE_COLUMNS,
        DOM_GRID_TEMPLATE_ROWS,
    };

    if (std::find(notSupportStyles.begin(), notSupportStyles.end(), style.first.c_str()) != notSupportStyles.end()) {
        // not support
        return true;
    }
    if (style.first == DOM_MARGIN_LEFT) {
        infoBuilder_.SetMarginLeft(StringToDimension(style.second));
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_MARGIN_RIGHT) {
        infoBuilder_.SetMarginRight(StringToDimension(style.second));
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_MARGIN_START) {
        if (IsRightToLeft()) {
            infoBuilder_.SetMarginRight(StringToDimension(style.second));
        } else {
            infoBuilder_.SetMarginLeft(StringToDimension(style.second));
        }
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_MARGIN_END) {
        if (IsRightToLeft()) {
            infoBuilder_.SetMarginLeft(StringToDimension(style.second));
        } else {
            infoBuilder_.SetMarginRight(StringToDimension(style.second));
        }
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_PADDING_LEFT) {
        infoBuilder_.SetPaddingLeft(StringToDimension(style.second));
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_PADDING_RIGHT) {
        infoBuilder_.SetPaddingRight(StringToDimension(style.second));
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_PADDING_START) {
        if (IsRightToLeft()) {
            infoBuilder_.SetPaddingRight(StringToDimension(style.second));
        } else {
            infoBuilder_.SetPaddingLeft(StringToDimension(style.second));
        }
        return DOMNode::SetSpecializedStyle(style);
    } else if (style.first == DOM_PADDING_END) {
        if (IsRightToLeft()) {
            infoBuilder_.SetPaddingLeft(StringToDimension(style.second));
        } else {
            infoBuilder_.SetPaddingRight(StringToDimension(style.second));
        }
        return DOMNode::SetSpecializedStyle(style);
    }
    return DOMDiv::SetSpecializedStyle(style);
}

} // namespace OHOS::Ace::Framework
