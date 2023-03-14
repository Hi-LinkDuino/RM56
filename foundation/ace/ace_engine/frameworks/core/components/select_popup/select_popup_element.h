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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_SELECT_POPUP_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_SELECT_POPUP_ELEMENT_H

#include "core/pipeline/base/sole_child_element.h"

#include "core/components/tween/tween_element.h"

namespace OHOS::Ace {

class SelectPopupElement : public SoleChildElement {
    DECLARE_ACE_TYPE(SelectPopupElement, SoleChildElement);

public:
    void PerformBuild() override;

private:
    RefPtr<TweenElement> GetTween(const RefPtr<Element>& element) const;
    void OnRefreshAnimation(const TweenOption& option, bool isIn);
    bool isInAnimation_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_SELECT_POPUP_ELEMENT_H
