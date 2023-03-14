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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARED_TRANSITION_SHARED_TRANSITION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARED_TRANSITION_SHARED_TRANSITION_COMPONENT_H

#include "core/animation/shared_transition_effect.h"
#include "core/components/common/properties/tween_option.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

class SharedTransitionEffect;

class ACE_EXPORT SharedTransitionComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(SharedTransitionComponent, ComposedComponent);

public:
    SharedTransitionComponent(const ComposeId& id, const std::string& name, ShareId shareId);
    ~SharedTransitionComponent() override = default;

    void SetOption(const TweenOption& option);

    const TweenOption& GetOption() const;
    const ShareId& GetShareId() const;
    const RefPtr<SharedTransitionEffect>& GetEffect() const;
    void SetEffect(RefPtr<SharedTransitionEffect>& effect);
    RefPtr<Element> CreateElement() override;
    bool IsEnablePopEnter() const;
    void SetEnablePopEnter(bool enablePopEnter);
    bool IsEnablePushEnter() const;
    void SetEnablePushEnter(bool enablePushEnter);
    bool IsEnablePopExit() const;
    void SetEnablePopExit(bool enablePopExit);
    bool IsEnablePushExit() const;
    void SetEnablePushExit(bool enablePushExit);
    void SetShareId(const ShareId& shareId);
    void SetOpacity(float opacity);
    float GetOpacity() const;
    void SetZIndex(int32_t index);
    int32_t GetZIndex() const;

private:
    void InitEnableStatus();

    ShareId shareId_;
    TweenOption option_;
    RefPtr<SharedTransitionEffect> effect_;
    bool enablePopEnter_ = true;    // enable shared transition when page in pop enter scene.
    bool enablePopExit_ = true;
    bool enablePushEnter_ = true;
    bool enablePushExit_ = true;
    float opacity_ = 1.0;
    int32_t zIndex_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARED_TRANSITION_SHARED_TRANSITION_COMPONENT_H
