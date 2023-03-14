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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_ROSEN_RENDER_ROOT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_ROSEN_RENDER_ROOT_H

#include "core/components/root/render_root.h"

namespace OHOS::Ace {

class RosenRenderRoot : public RenderRoot {
    DECLARE_ACE_TYPE(RosenRenderRoot, RenderRoot);

public:
    RosenRenderRoot() = default;
    ~RosenRenderRoot() override = default;

    std::shared_ptr<RSNode> CreateRSNode() const override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void FinishRender(const std::unique_ptr<DrawDelegate>& delegate, const Rect& dirty) override;
    void SyncGeometryProperties() override;

    BridgeType GetBridgeType() const override
    {
        return BridgeType::ROSEN;
    }

private:
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_ROSEN_RENDER_ROOT_H
