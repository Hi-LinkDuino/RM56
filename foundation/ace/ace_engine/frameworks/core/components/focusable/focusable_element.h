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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUSABLE_FOCUSABLE_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUSABLE_FOCUSABLE_ELEMENT_H

#include "core/components/box/box_component.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class ACE_EXPORT FocusableElement final : public SoleChildElement, public FocusGroup {
    DECLARE_ACE_TYPE(FocusableElement, SoleChildElement, FocusGroup);

public:
    void Update() override;

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    bool IsChild() const override;
    bool IsFocusable() const override;
    bool OnKeyEvent(const KeyEvent& keyEvent) override;
    bool AcceptFocusByRectOfLastFocus(const Rect& rect) override;

    void DumpFocusTree(int32_t depth) override;
    void OnClick() override;
    void OnFocus() override;
    void OnBlur() override;
    void HandleFocus() override;

protected:
    RefPtr<RenderNode> GetCachedRenderNode() override
    {
        auto context = GetContext().Upgrade();
        if (context) {
            return context->GetRenderFactory()->GetRenderBoxFactory()->Get();
        }
        return nullptr;
    }
    bool IsDeleteDisabled() override
    {
        return isDeleteDisabled_;
    }

private:
    // Deprecated
    RefPtr<BoxComponent> boxStyle_;
    RefPtr<BoxComponent> focusedBoxStyle_;
    bool isNode_{false};
    bool isDeleteDisabled_{false};
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUSABLE_FOCUSABLE_ELEMENT_H
