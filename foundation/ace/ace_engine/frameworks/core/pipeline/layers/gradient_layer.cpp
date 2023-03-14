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

#include "core/pipeline/layers/gradient_layer.h"

#include "flutter/lib/ui/painting/gradient.h"

namespace OHOS::Ace::Flutter {
namespace {

const size_t END_POINTS_COUNT = 4;

} // namespace

void GradientLayer::AddToScene(SceneBuilder& builder, double x, double y)
{
    bool pushSuccess = false;
    if ((!points_.empty()) && (!colors_.empty()) && (points_.size() == colors_.size())) {
        auto gradientShader = flutter::CanvasGradient::Create();

        // Unfold first and last points position to a float list.
        tonic::Float32List endPoints(END_POINTS_COUNT);
        endPoints[0] = points_.front().GetX();
        endPoints[1] = points_.front().GetY();
        endPoints[2] = points_.back().GetX();
        endPoints[3] = points_.back().GetY();

        // Convert color list.
        tonic::Int32List colorList(colors_.size());
        for (size_t i = 0; i < colors_.size(); i++) {
            colorList[i] = static_cast<int32_t>(colors_[i].GetValue());
        }

        // Calculate stop point position(from 0.0 to 1.0).
        tonic::Float32List colorStops(points_.size());
        auto endPositionTotal = points_.back().GetX() + points_.back().GetY();
        for (size_t i = 0; i < points_.size(); i++) {
            colorStops[i] = (points_[i].GetX() + points_[i].GetY()) / endPositionTotal;
        }
        tonic::Float64List matrix;
#ifdef USE_SYSTEM_SKIA
        gradientShader->initLinear(endPoints, colorList, colorStops, SkShader::kClamp_TileMode, matrix);
#else
        gradientShader->initLinear(endPoints, colorList, colorStops, SkTileMode::kClamp, matrix);
#endif
        builder.PushShaderMask(gradientShader, x + x_ + rect_.Left(), x + x_ + rect_.Right(), y + y_ + rect_.Top(),
            y + y_ + rect_.Bottom(), static_cast<int32_t>(blendMode_));
        pushSuccess = true;
    }
    builder.PushOpacity(alpha_, 0, 0);
    AddChildToScene(builder, x + x_, y + y_);
    builder.Pop();
    if (pushSuccess) {
        builder.Pop();
    }
}

} // namespace OHOS::Ace::Flutter
