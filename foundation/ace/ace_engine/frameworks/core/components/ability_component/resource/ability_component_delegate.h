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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RESOURCE_ABILITY_COMPONENT_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RESOURCE_ABILITY_COMPONENT_DELEGATE_H

#include "core/components/ability_component/ability_component.h"
#include "core/components/ability_component/resource/ability_component_resource.h"
#include "core/components/common/layout/constants.h"

namespace OHOS::Ace {

class AbilityComponentDelegate : public AbilityComponentResource {
    DECLARE_ACE_TYPE(AbilityComponentDelegate, AbilityComponentResource);

public:
    enum class State : char {
        WAITING_FOR_SIZE,
        CREATING,
        CREATED,
        CREATE_FAILED,
        RELEASED,
    };

    AbilityComponentDelegate() = delete;
    ~AbilityComponentDelegate() override;
    AbilityComponentDelegate(const RefPtr<AbilityComponent>& abilityComponent, const WeakPtr<PipelineContext>& context,
        const std::string& type);

    void CreatePlatformResource(const WeakPtr<PipelineContext>& context, const Rect& rect);
    void ReleasePlatformResource();
    void UpdateRenderRect(const Rect& rect);

private:
    void CreatePluginResource(const WeakPtr<PipelineContext>& context, const Rect& rect);
    void Stop();
    void RegisterEvent();
    void UnregisterEvent();
    void InitControllerImpl();

    // Events of ability component.
    void OnReady() const;
    void OnDestroy() const;
    void OnAbilityCreated(const std::string& param) const;
    void OnAbilityMovedFront() const;
    void OnAbilityWillRemove() const;

    // Implement of methods of ability component.
    void StartAbility(const std::string& want);
    void PerformBackPress();
    int32_t GetStackCount();

    RefPtr<AbilityComponent> abilityComponent_;
    State state_ { State::WAITING_FOR_SIZE };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RESOURCE_ABILITY_COMPONENT_DELEGATE_H
