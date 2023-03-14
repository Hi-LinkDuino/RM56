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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_FLUTTER_NODE_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_FLUTTER_NODE_FACTORY_H

#include <list>
#include <string>

#include "base/memory/ace_type.h"
#include "core/pipeline/base/factories/render_node_factory.h"

namespace OHOS::Ace {

constexpr size_t TEXT_CACHE_SIZE = 100;
constexpr size_t IMAGE_CACHE_SIZE = 100;
constexpr size_t BOX_CACHE_SIZE = 200;

class FlutterTextFactory final : public virtual RenderNodeFactory {
    DECLARE_ACE_TYPE(FlutterTextFactory, RenderNodeFactory);

public:
    FlutterTextFactory(): RenderNodeFactory(TEXT_CACHE_SIZE) {}

    ACE_DISALLOW_COPY_AND_MOVE(FlutterTextFactory);
};

class FlutterImageFactory final : public virtual RenderNodeFactory {
    DECLARE_ACE_TYPE(FlutterImageFactory, RenderNodeFactory);

public:
    FlutterImageFactory(): RenderNodeFactory(IMAGE_CACHE_SIZE) {}

    ACE_DISALLOW_COPY_AND_MOVE(FlutterImageFactory);
};

class FlutterBoxFactory final : public virtual RenderNodeFactory {
    DECLARE_ACE_TYPE(FlutterBoxFactory, RenderNodeFactory);

public:
    FlutterBoxFactory(): RenderNodeFactory(BOX_CACHE_SIZE) {}

    ACE_DISALLOW_COPY_AND_MOVE(FlutterBoxFactory);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_FLUTTER_NODE_FACTORY_H
