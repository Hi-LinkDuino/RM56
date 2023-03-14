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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_TEXTURE_LAYER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_TEXTURE_LAYER_H

#include "core/pipeline/layers/offset_layer.h"
#include "core/pipeline/layers/scene_builder.h"

namespace OHOS::Ace::Flutter {

class TextureLayer : public OffsetLayer {
    DECLARE_ACE_TYPE(TextureLayer, OffsetLayer)
public:
    TextureLayer(int64_t textureId, bool freeze, double width, double height, uint8_t opacity)
        : OffsetLayer(0, 0), id_(textureId), freeze_(freeze), width_(width), height_(height), opacity_(opacity)
    {}
    ~TextureLayer() override = default;

    void Update(int64_t textureId, bool freeze, double width, double height, uint8_t opacity);
    void AddToScene(SceneBuilder& builder, double x, double y) override;

    void Dump() override;

private:
    int64_t id_ = 0;
    bool freeze_ = false;
    double width_ = 0.0;
    double height_ = 0.0;
    uint8_t opacity_ = UINT8_MAX;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_TEXTURE_LAYER_H
