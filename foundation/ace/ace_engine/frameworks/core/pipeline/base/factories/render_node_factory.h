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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_RENDER_NODE_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_RENDER_NODE_FACTORY_H

#include <string>
#include <vector>

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

constexpr size_t FLEX_CACHE_SIZE = 50;
constexpr size_t FLEX_ITEM_CACHE_SIZE = 200;
constexpr bool USE_CACHE = false;
class RenderNode;
class RenderNodeFactory : public virtual AceType {
    DECLARE_ACE_TYPE(RenderNodeFactory, AceType);

public:
    explicit RenderNodeFactory(size_t cacheSize): cacheSize_(cacheSize) {}

    ~RenderNodeFactory() override;

    virtual RefPtr<RenderNode> Get();
    virtual bool Recycle(RenderNode*);

protected:
    RefPtr<RenderNode> GetNodeFromCache();
    std::vector<RenderNode*> nodeCache_;
    size_t cacheSize_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(RenderNodeFactory);
};

class RenderFlexFactory final : public virtual RenderNodeFactory {
    DECLARE_ACE_TYPE(RenderFlexFactory, RenderNodeFactory);

public:
    RenderFlexFactory(): RenderNodeFactory(FLEX_CACHE_SIZE) {}
    ~RenderFlexFactory() override = default;
    ACE_DISALLOW_COPY_AND_MOVE(RenderFlexFactory);
};

class RenderWrapFactory final : public virtual RenderNodeFactory {
    DECLARE_ACE_TYPE(RenderWrapFactory, RenderNodeFactory);

public:
    RenderWrapFactory(): RenderNodeFactory(FLEX_CACHE_SIZE) {}
    ~RenderWrapFactory() override = default;
    ACE_DISALLOW_COPY_AND_MOVE(RenderWrapFactory);
};

class RenderFlexItemFactory final : public virtual RenderNodeFactory {
    DECLARE_ACE_TYPE(RenderFlexItemFactory, RenderNodeFactory);

public:
    RenderFlexItemFactory(): RenderNodeFactory(FLEX_ITEM_CACHE_SIZE) {}
    ~RenderFlexItemFactory() override = default;
    ACE_DISALLOW_COPY_AND_MOVE(RenderFlexItemFactory);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_RENDER_NODE_FACTORY_H
