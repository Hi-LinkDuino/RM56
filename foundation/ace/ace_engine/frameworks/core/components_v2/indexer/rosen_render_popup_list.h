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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_ROSEN_RENDER_POPUP_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_ROSEN_RENDER_POPUP_LIST_H

#include "base/utils/noncopyable.h"
#include "core/components_v2/indexer/render_popup_list.h"
#include "core/pipeline/base/render_context.h"

namespace OHOS::Ace::V2 {

class RosenRenderPopupList : public RenderPopupList {
    DECLARE_ACE_TYPE(V2::RosenRenderPopupList, RenderPopupList);

public:
    RosenRenderPopupList() = default;
    ~RosenRenderPopupList() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

private:

    ACE_DISALLOW_COPY_AND_MOVE(RosenRenderPopupList);
};

} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_ROSEN_RENDER_POPUP_LIST_H
