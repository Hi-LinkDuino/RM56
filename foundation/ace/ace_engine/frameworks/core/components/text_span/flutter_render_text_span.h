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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_FLUTTER_RENDER_TEXT_SPAN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_FLUTTER_RENDER_TEXT_SPAN_H

#include "flutter/lib/ui/text/paragraph_builder.h"

#include "core/components/text_span/render_text_span.h"
#include "core/gestures/gesture_type.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class FlutterRenderTextSpan final : public RenderTextSpan {
    DECLARE_ACE_TYPE(FlutterRenderTextSpan, RenderTextSpan);

public:
    FlutterRenderTextSpan() = default;
    ~FlutterRenderTextSpan() override = default;

    // Called by parent to add text with style to builder.
    void UpdateText(
        txt::ParagraphBuilder& builder, std::map<int32_t, std::map<GestureType, EventMarker>>& touchRegions);

private:
    void UpdateTouchRegions(std::map<int32_t, std::map<GestureType, EventMarker>>& touchRegions);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_FLUTTER_RENDER_TEXT_SPAN_H
