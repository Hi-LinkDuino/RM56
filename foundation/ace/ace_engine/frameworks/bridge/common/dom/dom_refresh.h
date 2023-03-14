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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_REFRESH_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_REFRESH_H

#include "core/components/flex/flex_component.h"
#include "core/components/refresh/refresh_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMRefresh final : public DOMNode {
    DECLARE_ACE_TYPE(DOMRefresh, DOMNode);

public:
    DOMRefresh(NodeId nodeId, const std::string& nodeName);
    ~DOMRefresh() override = default;

    void InitializeStyle() override;

    void PrepareSpecializedComponent() override;
    RefPtr<Component> GetSpecializedComponent() override;

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void ResetInitializedStyle() override;

private:
    RefPtr<RefreshComponent> refreshChild_;
    RefPtr<ColumnComponent> columnChild_;
    EventMarker refreshEventId_;
    EventMarker pullDownEventId_;

    bool refreshing_ = false;
    bool showLastTime_ = false;
    int32_t friction_ = 0;
    RefreshType refreshType_ = RefreshType::AUTO;

    std::pair<Color, bool> progressColor_ { Color(), false };
    Color backgroundColor_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_REFRESH_H
