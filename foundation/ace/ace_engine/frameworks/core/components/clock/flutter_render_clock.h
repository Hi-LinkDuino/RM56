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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_FLUTTER_RENDER_CLOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_FLUTTER_RENDER_CLOCK_H

#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/painting/image.h"

#include "core/components/clock/render_clock.h"
#include "core/pipeline/layers/offset_layer.h"

namespace OHOS::Ace {

class FlutterRenderClock final : public RenderClock {
    DECLARE_ACE_TYPE(FlutterRenderClock, RenderClock);

public:
    FlutterRenderClock() = default;
    ~FlutterRenderClock() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PaintShadowLight(RenderContext& context, const Offset& renderOffset, double faceRadius);
    void RenderDigit(RenderContext& context, const Offset& center);
};

class FlutterRenderClockHand : public RenderClockHand {
    DECLARE_ACE_TYPE(FlutterRenderClockHand, RenderClockHand);

public:
    FlutterRenderClockHand() = default;
    ~FlutterRenderClockHand() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void RequestRenderForNextSecond() override;
    void RenderHand(RenderContext& context, const Offset& offset, const RefPtr<RenderImage>& renderHand,
        const Offset& rotateCenter, double rotateAngle);

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    RenderLayer GetRenderLayer() override
    {
        if (IsRepaintBoundary()) {
            if (!layer_) {
                layer_ = AceType::MakeRefPtr<Flutter::OffsetLayer>();
            }
        } else {
            layer_ = nullptr;
        }
        return AceType::RawPtr(layer_);
    }

protected:
    void OnAppShow() override;
    void OnPreDraw() override;

private:
    RefPtr<Flutter::OffsetLayer> layer_;
    int32_t curMinute_ = -1;
    int32_t curHour_ = -1;
    bool onPreDraw_ = false;
    int32_t stableCnt_ = 0; // used for check weather clock out of screen stably.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_FLUTTER_RENDER_CLOCK_H
