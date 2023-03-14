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

#include "core/components/plugin/render_plugin.h"

#include "base/log/event_report.h"

namespace OHOS::Ace {
void RenderPlugin::Update(const RefPtr<Component>& component)
{
    auto plugin = AceType::DynamicCast<PluginComponent>(component);

    Dimension rootWidth = 0.0_vp;
    Dimension rootHeight = 0.0_vp;
    if (plugin) {
        rootWidth = plugin->GetWidth();
        rootHeight = plugin->GetHeight();
    }

    if (rootWidth_ == rootWidth && rootHeight_ == rootHeight) {
        LOGE("same size, not resize render plugin");
        return;
    }
    rootWidth_ = rootWidth;
    rootHeight_ = rootHeight;

    MarkNeedLayout();
}

void RenderPlugin::PerformLayout()
{
    if (!NeedLayout()) {
        return;
    }

    if (rootWidth_.IsValid() && rootHeight_.IsValid()) {
        drawSize_ = Size(NormalizePercentToPx(rootWidth_, false), NormalizePercentToPx(rootHeight_, true));
    }

    auto pluginContext = GetSubPipelineContext();
    auto context = GetContext().Upgrade();
    if (pluginContext && context) {
        pluginContext->SetRootSize(context->GetDensity(), drawSize_.Width(), drawSize_.Height());
    }

    SetLayoutSize(drawSize_);
    SetNeedLayout(false);
    MarkNeedRender();
}

bool RenderPlugin::TouchTest(const Point& globalPoint,
    const Point& parentLocalPoint, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    auto context = GetContext().Upgrade();
    if (context) {
        auto pluginContext = GetSubPipelineContext();
        if (pluginContext) {
            double x = globalPoint.GetX() - pluginContext->GetPluginEventOffset().GetX();
            double y = globalPoint.GetY() - pluginContext->GetPluginEventOffset().GetY();
            if (x <= drawSize_.Width() && y <= drawSize_.Height()) {
                context->SetTouchPipeline(WeakPtr<PipelineContext>(pluginContext));
            }
        }
    }
    return true;
}
}; // namespace OHOS::Ace
