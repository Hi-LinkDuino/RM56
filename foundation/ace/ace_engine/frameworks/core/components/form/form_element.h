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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_ELEMENT_H

#include "core/pipeline/base/render_element.h"
#include "core/components/form/form_component.h"
#include "core/components/form/sub_container.h"
#include "core/components/form/resource/form_manager_delegate.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class FormElement : public RenderElement {
    DECLARE_ACE_TYPE(FormElement, RenderElement);

public:
    using EventCallback = std::function<void(const std::string&)>;

    FormElement() = default;
    ~FormElement() override;

    void Prepare(const WeakPtr<Element>& parent) override;
    void Update()  override;
    void PerformBuild() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    const RefPtr<SubContainer> GetSubContainer() const
    {
        return subContainer_;
    }

    void HandleOnAcquireEvent(int64_t);
    void HandleOnErrorEvent(const std::string code, const std::string msg);
    void HandleOnRouterEvent(const std::unique_ptr<JsonValue>& action);
    void HandleOnUninstallEvent(int64_t formId);
    void OnActionEvent(const std::string& action) const;

    bool ISAllowUpdate() const
    {
        return cardInfo_.allowUpate;
    }

private:
    void CreateCardContainer();
    void InitEvent(const RefPtr<FormComponent>& component);

private:
    RefPtr<SubContainer> subContainer_;
    RefPtr<FormManagerDelegate> formManagerBridge_;

    EventCallback onAcquireEvent_;
    EventCallback onErrorEvent_;
    EventCallback onUninstallEvent_;
    EventCallback onRouterEvent_;
    RequestFormInfo cardInfo_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_ELEMENT_H
