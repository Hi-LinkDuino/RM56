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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_FLUTTER_RENDER_SELECT_POPUP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_FLUTTER_RENDER_SELECT_POPUP_H

#include "third_party/skia/include/core/SkCanvas.h"

#include "core/components/select_popup/render_select_popup.h"

namespace OHOS::Ace {

class FlutterRenderSelectPopup : public RenderSelectPopup {
    DECLARE_ACE_TYPE(FlutterRenderSelectPopup, RenderSelectPopup);

public:
    FlutterRenderSelectPopup() = default;
    ~FlutterRenderSelectPopup() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PaintGradient(RenderContext& context, bool isTop);

    double GetOptionLeft() const;
    double GetOptionWidth() const;
    double GetOptionTop() const;
    double GetOptionBottom() const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_FLUTTER_RENDER_SELECT_POPUP_H
