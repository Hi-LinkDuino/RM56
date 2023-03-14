/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SELECT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SELECT_H

#include "core/components/select/select_component.h"
#include "core/components/select/select_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMSelect final : public DOMNode {
    DECLARE_ACE_TYPE(DOMSelect, DOMNode);

public:
    DOMSelect(NodeId nodeId, const std::string& nodeName);
    ~DOMSelect() override = default;
    void PrepareSpecializedComponent() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return selectComponent_;
    }

    void InitializeStyle() override;

    void ResetInitializedStyle() override;

protected:
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;

    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override
    {
        if (attr.first == DOM_DISABLED) {
            if (selectComponent_) {
                selectComponent_->SetDisabled((attr.second == "true"));
            }
            return true;
        }
        return false;
    }

    void OnSetStyleFinished() override
    {
        if (!selectComponent_) {
            return;
        }
        selectComponent_->FlushRefresh();
    }

    void UpdateBoxSize(const CalcDimension& width, const CalcDimension& height) override;
    void UpdateBoxPadding(const Edge& padding) override;
    void UpdateBoxBorder(const Border& border) override;

private:
    bool IsParentNavigation() const;

    EventMarker onChanged_;
    RefPtr<SelectTheme> theme_;
    RefPtr<SelectComponent> selectComponent_;
    RefPtr<TextComponent> tipText_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SELECT_H
