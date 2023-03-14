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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_ROSEN_RENDER_DATA_PANEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_ROSEN_RENDER_DATA_PANEL_H

#include "third_party/skia/include/core/SkCanvas.h"

#include "core/components/data_panel/render_data_panel.h"

namespace OHOS::Ace {

class RosenRenderProgressDataPanel : public RenderProgressDataPanel {
    DECLARE_ACE_TYPE(RosenRenderProgressDataPanel, RenderProgressDataPanel);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

    void PaintLoadingProgress(RenderContext& context, const Offset& offset);
    void PaintEffectedLoadingProgress(RenderContext& context, const Offset& offset);
    void PaintRingProgress(RenderContext& context, const Offset& offset);

    bool IsRepaintBoundary() const override
    {
        return RenderNode::IsRepaintBoundary();
    }

private:
};

class RosenRenderPercentageDataPanel : public RenderPercentageDataPanel {
    DECLARE_ACE_TYPE(RosenRenderPercentageDataPanel, RenderPercentageDataPanel);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

    bool IsRepaintBoundary() const override
    {
        return RenderNode::IsRepaintBoundary();
    }
    void PaintLinearProgress(RenderContext& context, const Offset& offset);
    void PaintColorSegment(RenderContext& context, const Offset& leftTop, double segmentValue, double xSegment,
        double height, const Color segmentStartColor, const Color segmentEndColor);
    void PaintBackground(RenderContext& context, const Offset& leftTop, double totalWidth, double height);
    void PaintSpace(RenderContext& context, const Offset& leftTop, double spaceWidth, double xSpace, double height);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_ROSEN_RENDER_DATA_PANEL_H
