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

#include "core/components/drop_filter/rosen_render_drop_filter.h"

#include "render_service_client/core/ui/rs_node.h"

#include "base/log/dump_log.h"
#include "core/components/drop_filter/drop_filter_component.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderDropFilter::Update(const RefPtr<Component>& component)
{
    RenderDropFilter::Update(component);
    auto rsNode = GetRSNode();
    if (rsNode == nullptr) {
        return;
    }

    auto backFilter = Rosen::RSFilter::CreateBlurFilter(sigmaX_, sigmaY_);
    rsNode->SetBackgroundFilter(backFilter);
}

} // namespace OHOS::Ace
