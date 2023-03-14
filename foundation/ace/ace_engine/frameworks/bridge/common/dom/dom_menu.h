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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_MENU_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_MENU_H

#include "core/components/menu/menu_component.h"
#include "core/components/text/text_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMMenu final : public DOMNode {
    DECLARE_ACE_TYPE(DOMMenu, DOMNode);

public:
    DOMMenu(NodeId nodeId, const std::string& nodeName);
    ~DOMMenu() override;

    void BindIdNode(const RefPtr<DOMNode>& idNode);

    RefPtr<Component> GetSpecializedComponent() override;

    void InitializeStyle() override;

    void ResetInitializedStyle() override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void PrepareSpecializedComponent() override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

private:
    double GetJsonDouble(const std::unique_ptr<JsonValue>& json, const std::string& name, double defaultValue) const;

    RefPtr<MenuComponent> menuChild_;
    TextStyle titleStyle_;
    EventMarker clickMarkerId_;
    EventMarker focusMarkerId_;
    EventMarker longPressMarkerId_;
    bool isClickType_ = true;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_MENU_H
