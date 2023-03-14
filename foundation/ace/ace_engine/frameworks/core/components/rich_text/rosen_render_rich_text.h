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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_ROSEN_RENDER_RICHTEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_ROSEN_RENDER_RICHTEXT_H

#include "core/components/rich_text/render_rich_text.h"

namespace OHOS::Ace {

class RosenRenderRichText final : public RenderRichText {
    DECLARE_ACE_TYPE(RosenRenderRichText, RenderRichText);

public:
    void DumpTree(int32_t depth) override;

private:
    void PerformLayout() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_ROSEN_RENDER_RICHTEXT_H
