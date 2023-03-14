/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_SUBWINDOW_SUBWINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_SUBWINDOW_SUBWINDOW_H

#include "base/memory/ace_type.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class ACE_EXPORT Subwindow : public AceType {
    DECLARE_ACE_TYPE(Subwindow, AceType)

public:
    static RefPtr<Subwindow> CreateSubwindow(int32_t instanceId);

    virtual void InitContainer() = 0;
    virtual void ShowMenu(const RefPtr<Component>& newComponent) = 0;
    virtual void CloseMenu() = 0;
    virtual void ClearMenu() {};

    int32_t GetSubwindowId()
    {
        return subwindowId_;
    }

    void SetSubwindowId(int32_t id)
    {
        subwindowId_ = id;
    }

private:
    int32_t subwindowId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_SUBWINDOW_SUBWINDOW_H