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

#include "core/pipeline/base/factories/render_node_factory.h"

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

RenderNodeFactory::~RenderNodeFactory()
{
    for (auto it = nodeCache_.begin(); it != nodeCache_.end(); ++it) {
        delete *it;
    }
    nodeCache_.clear();
}

RefPtr<RenderNode> RenderNodeFactory::Get()
{
    return GetNodeFromCache();
}

bool RenderNodeFactory::Recycle(RenderNode* node)
{
    if (USE_CACHE && nodeCache_.size() < cacheSize_) {
        LOGD("Recycle add in cache");
        nodeCache_.emplace_back(node);
        return true;
    }
    return false;
}

RefPtr<RenderNode> RenderNodeFactory::GetNodeFromCache()
{
    if (!nodeCache_.empty()) {
        auto node = *(nodeCache_.rbegin());
        nodeCache_.pop_back();
        LOGD("GetNodeFromCache get node");
        return Referenced::Claim(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace