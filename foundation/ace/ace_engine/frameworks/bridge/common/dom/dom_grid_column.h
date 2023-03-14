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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_GRID_COLUMN_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_GRID_COLUMN_H

#include "core/components/common/layout/grid_column_info.h"
#include "frameworks/bridge/common/dom/dom_div.h"

namespace OHOS::Ace::Framework {

class DomGridColumn : public DOMDiv {
    DECLARE_ACE_TYPE(DomGridColumn, DOMDiv);

public:
    DomGridColumn(NodeId nodeId, const std::string& nodeName);
    ~DomGridColumn() override = default;

    RefPtr<Component> GetSpecializedComponent() override;
    void SetParentGridInfo(const RefPtr<GridContainerInfo>& parent)
    {
        infoBuilder_.SetParent(parent);
    }

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    void ParseSpanAndOffset(const std::string& value, uint32_t& span, Dimension& offset);

private:
    RefPtr<GridColumnInfo> columnInfo_;
    GridColumnInfo::Builder infoBuilder_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_GRID_COLUMN_H
