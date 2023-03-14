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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DIALOG_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DIALOG_H

#include "core/components/custom_dialog/custom_dialog_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMDialog : public DOMNode {
    DECLARE_ACE_TYPE(DOMDialog, DOMNode);

public:
    DOMDialog(NodeId nodeId, const std::string& nodeName);
    ~DOMDialog() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return dialogChild_;
    }

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

protected:
    void PrepareSpecializedComponent() override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    RefPtr<Component> CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components) override;

private:
    RefPtr<CustomDialogComponent> dialogChild_;
    RefPtr<DisplayComponent> display_;
    std::pair<Dimension, bool> dialogWidth_ = { 0.0_vp, false };
    std::pair<Dimension, bool> dialogHeight_ = { 0.0_vp, false };
    Dimension marginLeft_ = 0.0_vp;
    Dimension marginRight_ = 0.0_vp;
    Dimension marginTop_ = 0.0_vp;
    Dimension marginBottom_ = 0.0_vp;
    bool customizedMargin_ = false;
    bool isShow_ = true;
    bool dragable_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DIALOG_H
