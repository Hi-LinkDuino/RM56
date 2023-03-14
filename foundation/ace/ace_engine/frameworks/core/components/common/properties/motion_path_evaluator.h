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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_MOTION_PATH_EVALUATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_MOTION_PATH_EVALUATOR_H

#include "base/geometry/dimension_offset.h"
#include "base/geometry/offset.h"
#include "base/geometry/transform_util.h"
#include "base/memory/ace_type.h"
#include "core/animation/evaluator.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/motion_path_option.h"

namespace OHOS::Ace {

class DoubleEvaluator;
class DimensionOffsetEvaluator;
class RotateEvaluator;
class TransformOperationsEvaluator;

struct MotionPathPosition {
    Offset offset;
    float rotate = 0.0f;
};

class MotionPathEvaluator final : public AceType {
    DECLARE_ACE_TYPE(MotionPathEvaluator, AceType);

public:
    explicit MotionPathEvaluator(const MotionPathOption& option = MotionPathOption(),
        const Offset& start = Offset(0.0, 0.0), const Offset& end = Offset(0.0, 0.0),
        PositionType type = PositionType::RELATIVE);
    ~MotionPathEvaluator() override = default;

    MotionPathPosition Evaluate(float fraction);

    RefPtr<Evaluator<double>> CreateXEvaluator()
    {
        return AceType::MakeRefPtr<DoubleEvaluator>(AceType::Claim(this), true);
    }

    RefPtr<Evaluator<double>> CreateYEvaluator()
    {
        return AceType::MakeRefPtr<DoubleEvaluator>(AceType::Claim(this), false);
    }

    RefPtr<Evaluator<DimensionOffset>> CreateDimensionOffstEvaluator()
    {
        return AceType::MakeRefPtr<DimensionOffsetEvaluator>(AceType::Claim(this));
    }

    RefPtr<Evaluator<float>> CreateRotateEvaluator()
    {
        return AceType::MakeRefPtr<RotateEvaluator>(AceType::Claim(this));
    }

    RefPtr<Evaluator<TransformOperations>> CreateTransformOperationsEvaluator()
    {
        return AceType::MakeRefPtr<TransformOperationsEvaluator>(AceType::Claim(this));
    }

    void SetStart(const Offset& point)
    {
        startPoint_ = point;
    }

    const Offset& GetStart() const
    {
        return startPoint_;
    }

    void SetEnd(const Offset& point)
    {
        endPoint_ = point;
    }

    const Offset& GetEnd() const
    {
        return endPoint_;
    }

private:
    MotionPathOption motionPathOption_;
    Offset startPoint_ { 0.0, 0.0 };
    Offset endPoint_ { 0.0, 0.0 };
    PositionType positionType_ = PositionType::RELATIVE;
};

// MotionPathEvaluator adapter for dimension
class DoubleEvaluator final : public Evaluator<double> {
public:
    DoubleEvaluator(const RefPtr<MotionPathEvaluator>& evaluator, bool isXAxis)
        : motionPathEvaluator_(evaluator), isXAxis_(isXAxis) {}
    ~DoubleEvaluator() override = default;

    double Evaluate(const double& start, const double& end, float fraction) override;

private:
    RefPtr<MotionPathEvaluator> motionPathEvaluator_;
    bool isXAxis_ = true;
};

// MotionPathEvaluator adapter for DimensionOffset
class DimensionOffsetEvaluator final : public Evaluator<DimensionOffset> {
public:
    DimensionOffsetEvaluator(const RefPtr<MotionPathEvaluator>& evaluator) : motionPathEvaluator_(evaluator) {}
    ~DimensionOffsetEvaluator() override = default;

    DimensionOffset Evaluate(const DimensionOffset& start, const DimensionOffset& end, float fraction) override;

private:
    RefPtr<MotionPathEvaluator> motionPathEvaluator_;
};

// MotionPathEvaluator adapter for rotate
class RotateEvaluator final : public Evaluator<float> {
public:
    explicit RotateEvaluator(const RefPtr<MotionPathEvaluator>& evaluator) : motionPathEvaluator_(evaluator) {}
    ~RotateEvaluator() override = default;

    float Evaluate(const float& start, const float& end, float fraction) override;

private:
    RefPtr<MotionPathEvaluator> motionPathEvaluator_;
};

class TransformOperationsEvaluator final : public Evaluator<TransformOperations> {
public:
    explicit TransformOperationsEvaluator(const RefPtr<MotionPathEvaluator>& evaluator)
        : motionPathEvaluator_(evaluator) {}
    ~TransformOperationsEvaluator() override = default;

    TransformOperations Evaluate(
        const TransformOperations& start, const TransformOperations& end, float fraction) override;

private:
    RefPtr<MotionPathEvaluator> motionPathEvaluator_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_MOTION_PATH_EVALUATOR_H
