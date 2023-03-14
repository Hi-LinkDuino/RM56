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

#include "frameworks/core/components/svg/parse/svg_clip_path.h"

#include "include/pathops/SkPathOps.h"

namespace OHOS::Ace {

RefPtr<SvgNode> SvgClipPath::Create()
{
    return AceType::MakeRefPtr<SvgClipPath>();
}

void SvgClipPath::AppendChild(const RefPtr<SvgNode>& child)
{
    children_.emplace_back(child);
}

SkPath SvgClipPath::AsPath(const Size& viewPort) const
{
    SkPath path;
    for (auto child : children_) {
        const SkPath childPath = child->AsPath(viewPort);
        Op(path, childPath, kUnion_SkPathOp, &path);
    }
    return path;
}

} // namespace OHOS::Ace
