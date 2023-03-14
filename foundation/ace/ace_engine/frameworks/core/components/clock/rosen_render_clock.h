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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_ROSEN_RENDER_CLOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_ROSEN_RENDER_CLOCK_H

#include "core/components/clock/render_clock.h"

namespace OHOS::Ace {
class RosenRenderClock final : public RenderClock {
    DECLARE_ACE_TYPE(RosenRenderClock, RenderClock);

public:
    RosenRenderClock() = default;
    ~RosenRenderClock() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PerformLayout() override;
    void PaintShadowLight(RenderContext& context, const Offset& renderOffset, double faceRadius);
    void RenderDigit(RenderContext& context, const Offset& offset);
};

class RosenRenderClockHand : public RenderClockHand {
    DECLARE_ACE_TYPE(RosenRenderClockHand, RenderClockHand);

public:
    RosenRenderClockHand() = default;
    ~RosenRenderClockHand() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PerformLayout() override;
    void RequestRenderForNextSecond() override;
    void LayoutHand(const Offset& offset, const RefPtr<RenderImage>& renderHand, double rotateAngle);

    bool IsRepaintBoundary() const override
    {
        return true;
    }

protected:
    void OnAppShow() override;
    void OnPreDraw() override;

private:
    int32_t curMinute_ = -1;
    int32_t curHour_ = -1;
    bool onPreDraw_ = false;
    int32_t stableCnt_ = 0; // used for check weather clock out of screen stably.
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_ROSEN_RENDER_CLOCK_H
