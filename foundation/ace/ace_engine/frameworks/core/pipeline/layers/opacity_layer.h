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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_OPACITY_LAYER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_OPACITY_LAYER_H

#include "core/pipeline/layers/offset_layer.h"
#include "core/pipeline/layers/scene_builder.h"

namespace OHOS::Ace::Flutter {

class OpacityLayer : public OffsetLayer {
    DECLARE_ACE_TYPE(OpacityLayer, OffsetLayer)
public:
    OpacityLayer(uint8_t alpha, double dx, double dy) : OffsetLayer(0, 0), alpha_(alpha), dx_(dx), dy_(dy) {}
    ~OpacityLayer() override = default;

    void AddToScene(SceneBuilder& builder, double x, double y) override;

    void SetOpacity(uint8_t alpha, double dx, double dy);
    void Dump() override;

private:
    uint8_t alpha_ = 255;
    double dx_ = 0.0;
    double dy_ = 0.0;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_OPACITY_LAYER_H
