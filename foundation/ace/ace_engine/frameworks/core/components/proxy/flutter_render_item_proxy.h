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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROXY_FLUTTER_RENDER_ITEM_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROXY_FLUTTER_RENDER_ITEM_PROXY_H

#include "base/geometry/matrix4.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/pipeline/layers/layer.h"

namespace OHOS::Ace {

class FlutterRenderItemProxy : public RenderItemProxy {
    DECLARE_ACE_TYPE(FlutterRenderItemProxy, RenderItemProxy);

public:
    FlutterRenderItemProxy();
    ~FlutterRenderItemProxy() override = default;

    RenderLayer GetRenderLayer() override;

    bool IsRepaintBoundary() const override
    {
        auto child = GetFirstChild();
        if (child && child->IsRepaintBoundary()) {
            return false;
        }
        return true;
    }

private:
    Matrix4 matrix_;
    RefPtr<Flutter::Layer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROXY_FLUTTER_RENDER_ITEM_PROXY_H
