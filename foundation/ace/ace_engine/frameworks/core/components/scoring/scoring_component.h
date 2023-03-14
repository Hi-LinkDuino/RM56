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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCORING_SCORING_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCORING_SCORING_COMPONENT_H

#include <string>

#include "base/utils/utils.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {
class ACE_EXPORT ScoringComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ScoringComponent, SoleChildComponent);

public:
    explicit ScoringComponent(const std::string& name, const std::string& pageName) : name_(name), pageName_(pageName)
    {}
    ~ScoringComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    std::string GetName() const
    {
        return name_;
    }

    std::string GetPageName() const
    {
        return pageName_;
    }

private:
    std::string name_;
    std::string pageName_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCORING_SCORING_COMPONENT_H
