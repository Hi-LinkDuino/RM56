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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_ITEM_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_ITEM_ELEMENT_H

#include "core/components/select_popup/select_popup_component.h"
#include "core/components/tool_bar/tool_bar_component.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class ToolBarItemElement : public SoleChildElement, public FocusNode {
    DECLARE_ACE_TYPE(ToolBarItemElement, SoleChildElement, FocusNode);

public:
    using OptionChildrenCallBack = std::function<std::list<RefPtr<Component>>()>;

    void OnTargetCallback(const Offset& leftTop, const Offset& rightBottom);
    void OnOptionCallback(std::size_t index);
    void Prepare(const WeakPtr<Element>& parent) override;
    void Update() override;
    void OnFocus() override;
    void OnBlur() override;
    void OnClick() override;

private:
    void IniteSelectPopup();

    bool isEndItem_ = false;
    RefPtr<SelectPopupComponent> popup_;
    OptionChildrenCallBack optionChildrenCallBack_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_ITEM_ELEMENT_H
