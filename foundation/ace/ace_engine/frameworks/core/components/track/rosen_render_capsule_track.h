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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_ROSEN_RENDER_CAPSULE_TRACK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_ROSEN_RENDER_CAPSULE_TRACK_H

#include "core/components/track/render_track.h"

namespace OHOS::Ace {
class RosenRenderCapsuleTrack : public RenderCapsuleTrack {
    DECLARE_ACE_TYPE(RosenRenderCapsuleTrack, RenderCapsuleTrack);

public:
    RosenRenderCapsuleTrack() = default;
    ~RosenRenderCapsuleTrack() override = default;

    void DrawShape(RenderContext& context, const Offset& offset);
    void DrawCapsuleProgressAnimation(RenderContext& context, const Offset& offset);
    void DrawCapsuleProgressVerticalAnimation(RenderContext& context, const Offset& offset);
    void Paint(RenderContext& context, const Offset& offset) override;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_ROSEN_RENDER_CAPSULE_TRACK_H
