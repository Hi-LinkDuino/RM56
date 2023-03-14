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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RENDER_FORM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RENDER_FORM_H

#include "core/components/form/form_component.h"
#include "core/components/form/sub_container.h"
#include "core/pipeline/base/render_sub_container.h"

namespace OHOS::Ace {

class RenderForm : public RenderSubContainer {
DECLARE_ACE_TYPE(RenderForm, RenderSubContainer);

public:
    static RefPtr<RenderNode> Create();

    ~RenderForm() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    virtual std::unique_ptr<DrawDelegate> GetDrawDelegate()
    {
        return nullptr;
    }
    virtual void RemoveChildren() {}

    bool TouchTest(const Point& globalPoint,
        const Point& parentLocalPoint, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    void SetSubContainer(const WeakPtr<SubContainer>& container)
    {
        subContainer_ = container;
    }

    RefPtr<PipelineContext> GetSubPipelineContext() override
    {
        auto context = subContainer_.Upgrade();
        if (context) {
            return context->GetPipelineContext();
        }
        return nullptr;
    }

private:
    Dimension rootWidht_ = 0.0_vp;
    Dimension rootHeight_ = 0.0_vp;
    Size drawSize_;
    WeakPtr<SubContainer> subContainer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RENDER_FORM_H
