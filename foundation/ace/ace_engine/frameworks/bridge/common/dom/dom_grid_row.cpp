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

#include "frameworks/bridge/common/dom/dom_grid_row.h"

#include "core/components/common/layout/grid_system_manager.h"
#include "frameworks/bridge/common/dom/dom_grid_column.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DomGridRow::DomGridRow(NodeId nodeId, const std::string& nodeName) : DOMDiv(nodeId, nodeName) {}

void DomGridRow::SetParentGridInfo(const RefPtr<GridContainerInfo>& parent)
{
    if (columnType_ != GridColumnType::NONE) {
        auto info = GridSystemManager::GetInstance().GetInfoByType(columnType_);
        gridContainerInfo_ = info->GetParent();
        boxComponent_->SetGridLayoutInfo(gridContainerInfo_);
    } else {
        gridContainerInfo_ = parent;
    }
}

void DomGridRow::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    DOMDiv::OnChildNodeAdded(child, slot);

    auto gridColumn = AceType::DynamicCast<DomGridColumn>(child);
    if (gridColumn) {
        gridColumn->SetParentGridInfo(gridContainerInfo_);
    }
}

void DomGridRow::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    ACE_DCHECK(child);
    DOMDiv::OnChildNodeRemoved(child);
    auto gridColumn = AceType::DynamicCast<DomGridColumn>(child);
    if (gridColumn) {
        gridColumn->SetParentGridInfo(nullptr);
    }
}

bool DomGridRow::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_GRID_COLUMN_TYPE) {
        auto iter = GridColumnTypeMap.find(attr.second);
        if (iter != GridColumnTypeMap.end()) {
            columnType_ = iter->second;
        }
        return true;
    }
    return false;
}

bool DomGridRow::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const std::list<const char*> notSupportStyles {
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

    return DOMDiv::SetSpecializedStyle(style);
}

} // namespace OHOS::Ace::Framework
