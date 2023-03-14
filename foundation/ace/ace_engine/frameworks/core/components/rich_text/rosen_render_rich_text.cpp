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

#include "core/components/rich_text/rosen_render_rich_text.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace {

void RosenRenderRichText::PerformLayout()
{
    RenderRichText::PerformLayout();
}

void RosenRenderRichText::DumpTree(int32_t depth)
{
    auto children = GetChildren();

    if (DumpLog::GetInstance().GetDumpFile() > 0) {
        DumpLog::GetInstance().AddDesc("sourceSize:", " width = ", GetLayoutSize().Width(),
            " height = ", GetLayoutSize().Height());
        DumpLog::GetInstance().AddDesc("globalPosition:", "x = ", GetGlobalOffset().GetX(),
            " y = ", GetGlobalOffset().GetY());
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children.size());
    }

    for (const auto& item : children) {
        item->DumpTree(depth + 1);
    }
}

} // namespace OHOS::Ace
