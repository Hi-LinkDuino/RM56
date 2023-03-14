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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_FLUTTER_RENDER_PICKER_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_FLUTTER_RENDER_PICKER_BASE_H

#include "core/components/picker/render_picker_base.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/gradient_layer.h"

namespace OHOS::Ace {

class FlutterRenderPickerBase : public RenderPickerBase {
    DECLARE_ACE_TYPE(FlutterRenderPickerBase, RenderPickerBase);

public:
    FlutterRenderPickerBase() = default;
    ~FlutterRenderPickerBase() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    RenderLayer GetRenderLayer() override;

private:
    void PaintGradient(ScopedCanvas& canvas, const Offset& offset, const Rect& rect, const RefPtr<PickerTheme>& theme);
    Rect GetOptionsRect(const Offset& offset, const RefPtr<RenderPickerColumn>& pickerColumn);

    RefPtr<Flutter::GradientLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_FLUTTER_RENDER_PICKER_BASE_H
