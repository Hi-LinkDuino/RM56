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

#include "core/components/form/flutter_render_form.h"

#include "core/components/form/render_form.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

std::unique_ptr<DrawDelegate> FlutterRenderForm::GetDrawDelegate()
{
    auto drawDelegate = std::make_unique<DrawDelegate>();

    drawDelegate->SetDrawFrameCallback([this](RefPtr<Flutter::Layer>& layer, const Rect& dirty) {
        LOGI("form draw delete");
        if (!layer_) {
            layer_ = AceType::MakeRefPtr<Flutter::OffsetLayer>();
        }
        layer_->AddChildren(layer);
        layer->SetParent(layer_);

        MarkNeedLayout(true, true);
        MarkNeedRender(true);
    });

    return drawDelegate;
}

void FlutterRenderForm::RemoveChildren()
{
    if (layer_ && layer_->Size() > 0) {
        layer_->RemoveChildren();
    }
    MarkNeedLayout(true, true);
}

void FlutterRenderForm::NotifyPaintFinish()
{
#ifndef OHOS_STANDARD_SYSTEM
    auto renderPost = GetGlobalOffset();
    auto context = GetContext().Upgrade();
    if (context) {
        auto density = context->GetDensity();
        layer_->SetOffset(
            renderPost.GetX() / density - renderPost.GetX(), renderPost.GetY() / density - renderPost.GetY());
    }
#endif
}

RenderLayer FlutterRenderForm::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::OffsetLayer>();
    }

    return AceType::RawPtr(layer_);
}

} // namespace OHOS::Ace