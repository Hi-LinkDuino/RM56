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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_FLUTTER_RENDER_PATTERN_LOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_FLUTTER_RENDER_PATTERN_LOCK_H
#include "flutter/lib/ui/painting/canvas.h"

#include "core/components_v2/pattern_lock/render_pattern_lock.h"
#include "core/pipeline/base/scoped_canvas_state.h"
namespace OHOS::Ace::V2 {
class FlutterRenderPatternLock final : public RenderPatternLock {
    DECLARE_ACE_TYPE(FlutterRenderPatternLock, RenderPatternLock);

public:
    FlutterRenderPatternLock() = default;
    ~FlutterRenderPatternLock() override = default;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    void PaintLockCircle(ScopedCanvas& canvas, const Offset& offset, int16_t x, int16_t y);
    void PaintLockLine(ScopedCanvas& canvas, const Offset& offset);
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_FLUTTER_RENDER_PATTERN_LOCK_H