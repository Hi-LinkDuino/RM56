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

#include "core/pipeline/layers/layer.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace::Flutter {

void Layer::AddChildren(const RefPtr<Layer>& layer)
{
    auto it = std::find(children_.begin(), children_.end(), layer);
    if (it == children_.end()) {
        layer->SetParent(AceType::Claim(this));
        children_.push_back(layer);
    }
}

void Layer::DumpTree(int32_t depth)
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        Dump();
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children_.size());
    }

    for (const auto& item : children_) {
        item->DumpTree(depth + 1);
    }
}

} // namespace OHOS::Ace::Flutter
