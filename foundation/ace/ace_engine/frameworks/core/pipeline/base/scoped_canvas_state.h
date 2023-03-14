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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SCOPED_CANVAS_STATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SCOPED_CANVAS_STATE_H

#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

class ScopedCanvas final {
public:
    static ScopedCanvas Create(RenderContext& context)
    {
        auto renderContext = AceType::DynamicCast<FlutterRenderContext>(&context);
        if (!renderContext) {
            LOGE("render context is null");
            return ScopedCanvas(nullptr);
        }
        auto canvas = renderContext->GetCanvas();
        if (!canvas) {
            LOGE("Paint canvas is null");
            return ScopedCanvas(nullptr);
        }
        return ScopedCanvas(canvas);
    }

    explicit ScopedCanvas(flutter::Canvas* canvas) : canvas_(canvas)
    {
        if (canvas_ != nullptr) {
            canvas_->save();
        }
    }

    ~ScopedCanvas()
    {
        if (canvas_) {
            canvas_->restore();
        }
    }

    // Need to check with bool function before call this function.
    flutter::Canvas* operator->() const
    {
        return canvas_;
    }

    operator bool() const
    {
        return canvas_ != nullptr;
    }

    SkCanvas* GetSkCanvas()
    {
        if (canvas_) {
            return canvas_->canvas();
        }
        return nullptr;
    }

    void FlipHorizontal(double horizontalOffset, double drawRectWidth) const
    {
        double horizontalMoveDelta = -1.0 * (horizontalOffset + drawRectWidth / 2.0);
        canvas_->translate(-1.0 * horizontalMoveDelta, 0.0);
        canvas_->scale(-1.0, 1.0);
        canvas_->translate(horizontalMoveDelta, 0.0);
    }

private:
    flutter::Canvas* canvas_ = nullptr;

    ACE_DISALLOW_COPY_AND_MOVE(ScopedCanvas);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SCOPED_CANVAS_STATE_H
