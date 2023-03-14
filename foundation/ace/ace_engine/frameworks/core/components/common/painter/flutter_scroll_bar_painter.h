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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_SCROLL_BAR_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_SCROLL_BAR_PAINTER_H

#include "base/memory/ace_type.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

class FlutterScrollBarPainter : public virtual AceType {
    DECLARE_ACE_TYPE(FlutterScrollBarPainter, AceType);

public:
    FlutterScrollBarPainter() = default;
    ~FlutterScrollBarPainter() = default;

    void PaintBar(flutter::Canvas* canvas, const Offset& offset, const Rect& paintRect,
        const RefPtr<ScrollBar>& scrollBar, const Offset& globalOffset, int32_t alpha);

private:
    void PaintCircleBar(
        flutter::Canvas* canvas, const Offset& offset, const Rect& paintRect, const RefPtr<ScrollBar>& scrollBar);
    void PaintRectBar(flutter::Canvas* canvas, const Offset& offset, const RefPtr<ScrollBar>& scrollBar, int32_t alpha);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_SCROLL_BAR_PAINTER_H
