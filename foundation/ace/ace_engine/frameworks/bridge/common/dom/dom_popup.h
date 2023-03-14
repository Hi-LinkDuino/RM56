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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_POPUP_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_POPUP_H

#include "core/components/popup/popup_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMPopup final : public DOMNode {
    DECLARE_ACE_TYPE(DOMPopup, DOMNode);

public:
    DOMPopup(NodeId nodeId, const std::string& nodeName);
    ~DOMPopup() override;

    void BindIdNode(const RefPtr<DOMNode>& idNode);

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

    void InitializeStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return popupChild_;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void ResetInitializedStyle() override;
    void PrepareSpecializedComponent() override;
    RefPtr<Component> CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components) override;

private:
    void RemoveMarker();

    bool clickable_ = true;
    bool hasBackground_ = false;
    Color maskColor_;
    Color backgroundColor_;
    RefPtr<PopupComponent> popupChild_;
    EventMarker clickMarker_;
    EventMarker visibilityChangeEventId_;
    RefPtr<SingleChild> lastComponent_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_POPUP_H
