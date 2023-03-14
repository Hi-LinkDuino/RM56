/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TIMER_FLUTTER_RENDER_TEXT_TIMER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TIMER_FLUTTER_RENDER_TEXT_TIMER_H

#include "core/components/texttimer/render_texttimer.h"
#include "core/pipeline/layers/gradient_layer.h"

namespace OHOS::Ace {
class FlutterRenderTextTimer final : public RenderTextTimer {
    DECLARE_ACE_TYPE(FlutterRenderTextTimer, RenderTextTimer);

public:
    FlutterRenderTextTimer() = default;
    ~FlutterRenderTextTimer() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    RenderLayer GetRenderLayer() override;

private:
    RefPtr<Flutter::GradientLayer> layer_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_FLUTTER_RENDER_TEXT_TIMER_H
