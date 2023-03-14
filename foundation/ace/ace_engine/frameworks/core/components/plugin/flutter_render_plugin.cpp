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

#include "core/components/plugin/flutter_render_plugin.h"

#include "core/components/plugin/render_plugin.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

std::unique_ptr<DrawDelegate> FlutterRenderPlugin::GetDrawDelegate()
{
    auto drawDelegate = std::make_unique<DrawDelegate>();

    drawDelegate->SetDrawFrameCallback([this](RefPtr<Flutter::Layer>& layer, const Rect& dirty) {
        if (!layer_) {
            layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(
                0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Flutter::Clip::HARD_EDGE);
        }
        layer_->AddChildren(layer);
        layer->SetParent(layer_);

        MarkNeedLayout(true, true);
        MarkNeedRender(true);
    });

    return drawDelegate;
}

void FlutterRenderPlugin::RemoveChildren()
{
    if (layer_ && layer_->Size() > 0) {
        layer_->RemoveChildren();
    }
    MarkNeedLayout(true, true);
}

void FlutterRenderPlugin::NotifyPaintFinish()
{
    auto context = GetContext().Upgrade();
    if (context) {
        auto pluginContext = GetSubPipelineContext();
        if (pluginContext) {
            layer_->SetOffset(pluginContext->GetPluginOffset().GetX(), pluginContext->GetPluginOffset().GetY());
        }
    }
}

RenderLayer FlutterRenderPlugin::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Flutter::Clip::HARD_EDGE);
    }

    return AceType::RawPtr(layer_);
}
} // namespace OHOS::Ace
