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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_FLUTTER_RENDER_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_FLUTTER_RENDER_FACTORY_H

#include "core/pipeline/base/factories/flutter_node_factory.h"
#include "core/pipeline/base/factories/render_factory.h"

namespace OHOS::Ace {

class FlutterRenderFactory final : public virtual RenderFactory {
    DECLARE_ACE_TYPE(FlutterRenderFactory, RenderFactory);

public:
    FlutterRenderFactory() = default;
    ~FlutterRenderFactory() override = default;

    RefPtr<RenderNodeFactory>& GetRenderTextFactory() override
    {
        if (!textFactory_) {
            textFactory_ = AceType::MakeRefPtr<FlutterTextFactory>();
        }
        return textFactory_;
    }

    RefPtr<RenderNodeFactory>& GetRenderImageFactory() override
    {
        if (!imageFactory_) {
            imageFactory_ = AceType::MakeRefPtr<FlutterImageFactory>();
        }
        return imageFactory_;
    }

    RefPtr<RenderNodeFactory>& GetRenderBoxFactory() override
    {
        if (!boxFactory_) {
            boxFactory_ = AceType::MakeRefPtr<FlutterBoxFactory>();
        }
        return boxFactory_;
    }

    RefPtr<RenderNodeFactory>& GetRenderFlexFactory() override
    {
        if (!flexFactory_) {
            flexFactory_ = AceType::MakeRefPtr<RenderFlexFactory>();
        }
        return flexFactory_;
    }

    RefPtr<RenderNodeFactory>& GetRenderWrapFactory() override
    {
        if (!wrapFactory_) {
            wrapFactory_ = AceType::MakeRefPtr<RenderWrapFactory>();
        }
        return wrapFactory_;
    }

    RefPtr<RenderNodeFactory>& GetRenderFlexItemFactory() override
    {
        if (!flexItemFactory_) {
            flexItemFactory_ = AceType::MakeRefPtr<RenderFlexItemFactory>();
        }
        return flexItemFactory_;
    }

private:
    RefPtr<RenderNodeFactory> textFactory_;
    RefPtr<RenderNodeFactory> imageFactory_;
    RefPtr<RenderNodeFactory> boxFactory_;
    RefPtr<RenderNodeFactory> flexFactory_;
    RefPtr<RenderNodeFactory> flexItemFactory_;
    RefPtr<RenderNodeFactory> wrapFactory_;

    ACE_DISALLOW_COPY_AND_MOVE(FlutterRenderFactory);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_FLUTTER_RENDER_FACTORY_H
