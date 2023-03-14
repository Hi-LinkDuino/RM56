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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_RENDER_DROP_FILTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_RENDER_DROP_FILTER_H

#include "core/components/common/layout/constants.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderDropFilter : public RenderNode {
    DECLARE_ACE_TYPE(RenderDropFilter, RenderNode);

public:
    ~RenderDropFilter() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void UpdateTouchRect() override;
    const Rect& GetTouchRect() override;
    const std::vector<Rect>& GetTouchRectList() override;

protected:
    RenderDropFilter();
    double sigmaX_ = 0.0;
    double sigmaY_ = 0.0;
    bool usePanelTouchRect_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_RENDER_DROP_FILTER_H
