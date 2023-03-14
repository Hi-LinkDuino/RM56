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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_DIALOG_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_DIALOG_ELEMENT_H

#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class DialogElement : public RenderElement, public FocusGroup {
    DECLARE_ACE_TYPE(DialogElement, RenderElement, FocusGroup);

public:
    DialogElement() = default;
    ~DialogElement() override = default;

    void PerformBuild() override;

    int32_t GetDialogId() const
    {
        return dialogId_;
    }

protected:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

private:
    int32_t dialogId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_DIALOG_ELEMENT_H
