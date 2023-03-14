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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_ROSEN_RENDER_CLIP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_ROSEN_RENDER_CLIP_H

#include "core/components/clip/render_clip.h"

namespace OHOS::Ace {

class RosenRenderClip : public RenderClip {
    DECLARE_ACE_TYPE(RosenRenderClip, RenderClip);

public:
    RosenRenderClip() = default;
    ~RosenRenderClip() override = default;

    bool HasGeometryProperties() const override
    {
        return true;
    }
    void SyncGeometryProperties() override;

    void SetOffsetX(double offsetX) override;
    void SetOffsetY(double offsetY) override;
    void SetWidth(double width) override;
    void SetHeight(double height) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLIP_FLUTTER_RENDER_CLIP_H
