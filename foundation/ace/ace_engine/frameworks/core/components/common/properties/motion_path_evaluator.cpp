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

#include "core/components/common/properties/motion_path_evaluator.h"

#include "base/utils/utils.h"
#include "frameworks/core/components/common/painter/flutter_svg_painter.h"

namespace OHOS::Ace {
namespace {

const char START_X[] = "start.x";
const char START_Y[] = "start.y";
const char END_X[] = "end.x";
const char END_Y[] = "end.y";

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t startPos = 0;
    size_t fromLength = from.length();
    size_t toLength = to.length();
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, fromLength, to);
        startPos += toLength;
    }
    return str;
}

std::string Preprocess(std::string path, const Offset& start, const Offset& end)
{
    path = ReplaceAll(path, START_X, std::to_string(start.GetX()));
    path = ReplaceAll(path, START_Y, std::to_string(start.GetY()));
    path = ReplaceAll(path, END_X, std::to_string(end.GetX()));
    path = ReplaceAll(path, END_Y, std::to_string(end.GetY()));
    return path;
}

} // namespace

MotionPathEvaluator::MotionPathEvaluator(
    const MotionPathOption& option, const Offset& start, const Offset& end, PositionType type)
    : motionPathOption_(option), startPoint_(start), endPoint_(end), positionType_(type)
{
    auto path = Preprocess(motionPathOption_.GetPath(), startPoint_, endPoint_);
    motionPathOption_.SetPath(path);
}

MotionPathPosition MotionPathEvaluator::Evaluate(float fraction)
{
    if (NearEqual(fraction, 1.0f)) {
        fraction = 1.0f;
    }
    if (!motionPathOption_.IsValid()) {
        return MotionPathPosition { .offset = startPoint_ * (1.0f - fraction) + endPoint_ * fraction, .rotate = 0.0f };
    }
    auto progress = motionPathOption_.GetBegin() * (1.0f - fraction) + motionPathOption_.GetEnd() * fraction;
    MotionPathPosition position;
    if (FlutterSvgPainter::GetMotionPathPosition(motionPathOption_.GetPath(), progress, position)) {
        if (positionType_ == PositionType::OFFSET) {
            position.offset += startPoint_;
        }
        return position;
    }
    return MotionPathPosition { .offset = Offset(), .rotate = 0.0f };
}

double DoubleEvaluator::Evaluate(const double& start, const double& end, float fraction)
{
    if (motionPathEvaluator_) {
        auto position = motionPathEvaluator_->Evaluate(fraction);

        if (isXAxis_) {
            return position.offset.GetX();
        } else {
            return position.offset.GetY();
        }
    }
    return 0.0;
}

DimensionOffset DimensionOffsetEvaluator::Evaluate(
    const DimensionOffset& start, const DimensionOffset& end, float fraction)
{
    if (motionPathEvaluator_) {
        auto position = motionPathEvaluator_->Evaluate(fraction);

        return DimensionOffset(Dimension(position.offset.GetX()), Dimension(position.offset.GetY()));
    }

    return DimensionOffset();
}

float RotateEvaluator::Evaluate(const float& start, const float& end, float fraction)
{
    if (motionPathEvaluator_) {
        auto position = motionPathEvaluator_->Evaluate(fraction);
        return position.rotate;
    }
    return 0.0f;
}

TransformOperations TransformOperationsEvaluator::Evaluate(
    const TransformOperations& start, const TransformOperations& end, float fraction)
{
    TransformOperations result;
    if (motionPathEvaluator_) {
        auto position = motionPathEvaluator_->Evaluate(fraction);
        TransformOperation rotation;
        rotation.type_ = TransformOperationType::ROTATE;
        rotation.rotateOperation_ = RotateOperation(0.0f, 0.0f, 1.0f, position.rotate);
        result.GetOperations().push_back(rotation);
    }
    return result;
}

} // namespace OHOS::Ace
