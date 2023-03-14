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

#include "core/components/proxy/flutter_render_item_proxy.h"

#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

using namespace Flutter;

FlutterRenderItemProxy::FlutterRenderItemProxy() : RenderItemProxy() {}

RenderLayer FlutterRenderItemProxy::GetRenderLayer()
{
    if (!IsRepaintBoundary()) {
        layer_.Reset();
        return nullptr;
    }
    if (!layer_) {
        matrix_ = Matrix4::CreateScale(1.0, 1.0, 1.0);
        layer_ = AceType::MakeRefPtr<TransformLayer>(matrix_, 0.0, 0.0);
    }
    return AceType::RawPtr(layer_);
}

} // namespace OHOS::Ace
