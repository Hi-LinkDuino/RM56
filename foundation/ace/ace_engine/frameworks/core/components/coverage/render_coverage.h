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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COVERAGE_RENDER_COVERAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COVERAGE_RENDER_COVERAGE_H

#include "core/components/coverage/coverage_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderCoverage : public RenderNode {
    DECLARE_ACE_TYPE(RenderCoverage, RenderNode);

public:
    RenderCoverage();
    ~RenderCoverage() override = default;

    static RefPtr<RenderNode> Create();

    const Dimension& GetX() const
    {
        return x_;
    }

    const Alignment& GetAlignment() const
    {
        return align_;
    }

    const Dimension& GetY() const
    {
        return y_;
    }

    const std::string GetTextVal() const
    {
        return title_;
    }

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
private:
    Dimension y_;
    Alignment align_;
    Dimension x_;
    std::string title_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COVERAGE_RENDER_COVERAGE_H
