/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * @file depict_curve.cpp
 *
 * @brief Defines Building curve transformation pipes
 * The curve drawing is usually approximated by a series of short line segments,
 * which is the only efficient method to draw the curve
 * @since 1.0
 * @version 1.0
 */

#include "gfx_utils/diagram/depiction/depict_curve.h"

namespace OHOS {
/**
 * Reset the status attribute of a path segment
 * @path_id is a path ID, calculated from 0
 * @since 1.0
 * @version 1.0
 */
void DepictCurve::Rewind(uint32_t pathId)
{
    source_->Rewind(pathId);
    lastX_ = 0.0f;
    lastY_ = 0.0f;
    quadraticBezier_.Reset();
    cubicBezier_.Reset();
}

/**
 * According to PATH_CMD command returns the vertex coordinates generated in each stage
 * @since 1.0
 * @version 1.0
 */
uint32_t DepictCurve::GenerateVertex(float* x, float* y)
{
    if (!IsStop(quadraticBezier_.GenerateVertex(x, y))) {
        lastX_ = *x;
        lastY_ = *y;
        return PATH_CMD_LINE_TO;
    }

    if (!IsStop(cubicBezier_.GenerateVertex(x, y))) {
        lastX_ = *x;
        lastY_ = *y;
        return PATH_CMD_LINE_TO;
    }

    float control2X = 0;
    float control2Y = 0;
    float endX = 0;
    float endY = 0;

    uint32_t cmd = source_->GenerateVertex(x, y);
    switch (cmd) {
        case PATH_CMD_CURVE3:
            source_->GenerateVertex(&endX, &endY);

            quadraticBezier_.Init(lastX_, lastY_, *x, *y, endX, endY);

            quadraticBezier_.GenerateVertex(x, y); // First call returns path_cmd_move_to
            quadraticBezier_.GenerateVertex(x, y); // This is the first vertex of the curve
            cmd = PATH_CMD_LINE_TO;
            break;

        case PATH_CMD_CURVE4:
            source_->GenerateVertex(&control2X, &control2Y);
            source_->GenerateVertex(&endX, &endY);

            cubicBezier_.Init(lastX_, lastY_, *x, *y, control2X, control2Y, endX, endY);

            cubicBezier_.GenerateVertex(x, y); // First call returns path_cmd_move_to
            cubicBezier_.GenerateVertex(x, y); // This is the first vertex of the curve
            cmd = PATH_CMD_LINE_TO;
            break;
    }
    lastX_ = *x;
    lastY_ = *y;
    return cmd;
}
} // namespace OHOS
