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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_ROSEN_RENDER_PROGRESS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_ROSEN_RENDER_PROGRESS_H

#include "core/components/progress/render_progress.h"
#include "core/components/track/render_track.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RosenRenderProgress : public RenderProgress {
    DECLARE_ACE_TYPE(RosenRenderProgress, RenderProgress);

public:
    RosenRenderProgress() = default;
    ~RosenRenderProgress() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

private:
    RefPtr<RenderNode> track_;
    bool initialUpdate_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_ROSEN_RENDER_PROGRESS_H
