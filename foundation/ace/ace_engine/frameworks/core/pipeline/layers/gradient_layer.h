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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_GRADIENT_LAYER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_GRADIENT_LAYER_H

#include "flutter/lib/ui/dart_wrapper.h"

#include "base/geometry/point.h"
#include "base/geometry/rect.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/layers/offset_layer.h"
#include "core/pipeline/layers/scene_builder.h"

namespace OHOS::Ace::Flutter {

class GradientLayer : public OffsetLayer {
    DECLARE_ACE_TYPE(GradientLayer, OffsetLayer)
public:
    GradientLayer() = default;
    ~GradientLayer() override = default;

    void AddToScene(SceneBuilder& builder, double x, double y) override;


    void SetGradientRect(Rect rect)
    {
        rect_ = rect;
    }

    void SetPoints(const std::vector<Point>& points)
    {
        if (points.empty()) {
            return;
        }
        points_.clear();
        points_.insert(points_.begin(), points.begin(), points.end());
    }

    void SetColors(const std::vector<Color>& colors)
    {
        if (colors.empty()) {
            return;
        }
        colors_.clear();
        colors_.insert(colors_.begin(), colors.begin(), colors.end());
    }

    void SetBlendMode(SkBlendMode blendMode)
    {
        blendMode_ = blendMode;
    }

    void SetAlpha(uint8_t value)
    {
        alpha_ = value;
    }

protected:
    uint8_t alpha_ = 255; // default 255 that means no transparent.
    std::vector<Point> points_;
    std::vector<Color> colors_;
    SkBlendMode blendMode_ = SkBlendMode::kDstIn;
    Rect rect_;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_GRADIENT_LAYER_H
