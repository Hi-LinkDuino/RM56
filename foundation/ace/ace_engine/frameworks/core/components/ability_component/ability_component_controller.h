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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_ABILITY_COMPONENT_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_ABILITY_COMPONENT_CONTROLLER_H

#include <string>

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class ACE_EXPORT AbilityComponentController : public AceType {
    DECLARE_ACE_TYPE(AbilityComponentController, AceType);

public:
    AbilityComponentController() = default;
    ~AbilityComponentController() override = default;

    void StartAbility(const std::string& want) const
    {
        if (startAbilityImpl_) {
            startAbilityImpl_(want);
        }
    }

    void PerformBackPress() const
    {
        if (performBackPressImpl_) {
            performBackPressImpl_();
        }
    }

    int32_t GetStackCount() const
    {
        if (getStackCountImpl_) {
            return getStackCountImpl_();
        }
        return 0;
    }

    void SetStartAbilityImpl(const std::function<void(const std::string&)>& startAbilityImpl)
    {
        startAbilityImpl_ = startAbilityImpl;
    }

    void SetPerformBackPressImpl(std::function<void()> performBackPressImpl)
    {
        performBackPressImpl_ = performBackPressImpl;
    }

    void SetGetStackCountImpl(std::function<int32_t()> getStackCountImpl)
    {
        getStackCountImpl_ = getStackCountImpl;
    }

private:
    std::function<void(const std::string&)> startAbilityImpl_;
    std::function<void()> performBackPressImpl_;
    std::function<int32_t()> getStackCountImpl_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_ABILITY_COMPONENT_CONTROLLER_H
