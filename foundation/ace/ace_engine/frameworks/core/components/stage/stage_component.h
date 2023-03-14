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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_STAGE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_STAGE_COMPONENT_H

#include "base/utils/system_properties.h"
#include "core/components/stack/stack_component.h"
#include "core/components/stage/render_stage.h"
#include "core/components/stage/stage_element.h"

namespace OHOS::Ace {

class StageComponent : public StackComponent {
    DECLARE_ACE_TYPE(StageComponent, StackComponent);

public:
    explicit StageComponent(const std::list<RefPtr<Component>>& children)
        : StackComponent(Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, children)
    {}
    StageComponent(const std::list<RefPtr<Component>>& children, bool isSectionStage)
        : StackComponent(Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, children),
          isSectionStage_(isSectionStage)
    {}
    ~StageComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        if (isSectionStage_) {
            return AceType::MakeRefPtr<SectionStageElement>();
        } else {
            return AceType::MakeRefPtr<StageElement>();
        }
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
            LOGI("device type is watch, create stage render node.");
            return RenderStage::Create();
        }
        return RenderStack::Create();
    }

private:
    bool isSectionStage_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_STAGE_COMPONENT_H
