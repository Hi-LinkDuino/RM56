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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LABEL_TARGET_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LABEL_TARGET_H

#include <functional>

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

using LabelTriggerClickHandler = std::function<void()>;
class LabelTrigger : public AceType {
    DECLARE_ACE_TYPE(LabelTrigger, AceType);

public:
    LabelTrigger() = default;
    ~LabelTrigger() override = default;

    void Click()
    {
        if (clickHandler_) {
            clickHandler_();
        }
    }

    LabelTriggerClickHandler clickHandler_;
};

class LabelTarget : public virtual AceType {
    DECLARE_ACE_TYPE(LabelTarget, AceType);

public:
    LabelTarget() : trigger_(AceType::MakeRefPtr<LabelTrigger>()) {}
    ~LabelTarget() override = default;

    const RefPtr<LabelTrigger>& GetTrigger() const
    {
        return trigger_;
    }

private:
    RefPtr<LabelTrigger> trigger_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LABEL_TARGET_H
