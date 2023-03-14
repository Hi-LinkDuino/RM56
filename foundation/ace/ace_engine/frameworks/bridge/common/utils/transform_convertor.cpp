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

#include "frameworks/bridge/common/utils/transform_convertor.h"

#include <algorithm>

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t PARAM_SINGLE = 1;
constexpr int32_t PARAM_DOUBLE = 2;
constexpr int32_t PARAM_THREE = 3;
constexpr int32_t PARAM_FOUR = 4;
constexpr int32_t PARAM_SIX = 6;
constexpr int32_t PARAM_SIXTEEN = 16;

} // namespace

const std::vector<std::string> TransformConvertor::TransformKey = {
    DOM_TRANSLATE,
    DOM_TRANSLATE_3D,
    DOM_TRANSLATE_X,
    DOM_TRANSLATE_Y,
    DOM_TRANSLATE_Z,
    DOM_ROTATE,
    DOM_ROTATE_3D,
    DOM_ROTATE_X,
    DOM_ROTATE_Y,
    DOM_ROTATE_Z,
    DOM_SCALE,
    DOM_SCALE_3D,
    DOM_SCALE_X,
    DOM_SCALE_Y,
    DOM_SCALE_Z,
    DOM_SKEW,
    DOM_SKEW_X,
    DOM_SKEW_Y,
    DOM_MATRIX,
    DOM_MATRIX_3D,
    DOM_PERSPECTIVE,
};

void TransformConvertor::Convert(const std::string& key, const std::string& value, double time)
{
    auto iter = TransformConvertorMap.find(key);
    if (iter != TransformConvertorMap.end()) {
        iter->second(value, time, *this);
    }
}

void TransformConvertor::InsertIdentityKeyframe(double time)
{
    noneKeyframeTimes_.push_back(static_cast<float>(time));
}

void TransformConvertor::AddAnimationToTweenOption(TweenOption& option) const
{
    auto noneKeyframes = std::list<RefPtr<Keyframe<TransformOperation>>>();
    TransformOperation operation;
    operation.type_ = TransformOperationType::UNDEFINED;
    for (float time : noneKeyframeTimes_) {
        noneKeyframes.push_back(AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation));
    }
    for (const auto& animation : operationList_) {
        if (!noneKeyframes.empty()) {
            // insert transform: none keyframe into animation
            animation->AddKeyframe(noneKeyframes);
        }
        option.AddTransformAnimation(animation);
    }
}

void TransformConvertor::ApplyCurve(const RefPtr<Curve>& curve)
{
    for (const auto& animation : operationList_) {
        if (animation) {
            animation->SetCurve(curve);
        }
    }
}

void TransformConvertor::ClearAnimations()
{
    operationList_.clear();
    operationMap_.clear();
    noneKeyframeTimes_.clear();
}

void TransformConvertor::AddKeyframe(AnimationType type, const RefPtr<Keyframe<TransformOperation>>& keyframe)
{
    auto& animation = operationMap_[type];
    if (!animation) {
        animation = AceType::MakeRefPtr<KeyframeAnimation<TransformOperation>>();
        operationList_.push_back(animation);
    }
    animation->AddKeyframe(keyframe);
}

void TransformConvertor::AddKeyframe(AnimationType type, double time, const TranslateOperation& translate)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::TRANSLATE;
    operation.translateOperation_ = translate;
    auto keyframe = AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation);
    AddKeyframe(type, keyframe);
}

void TransformConvertor::AddKeyframe(AnimationType type, double time, const SkewOperation& skew)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::SKEW;
    operation.skewOperation_ = skew;
    auto keyframe = AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation);
    AddKeyframe(type, keyframe);
}

void TransformConvertor::AddKeyframe(AnimationType type, double time, const ScaleOperation& scale)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::SCALE;
    operation.scaleOperation_ = scale;
    auto keyframe = AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation);
    AddKeyframe(type, keyframe);
}

void TransformConvertor::AddKeyframe(AnimationType type, double time, const RotateOperation& rotate)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::ROTATE;
    operation.rotateOperation_ = rotate;
    auto keyframe = AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation);
    AddKeyframe(type, keyframe);
}

void TransformConvertor::AddKeyframe(AnimationType type, double time, const Matrix4& matrix)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::MATRIX;
    operation.matrix4_ = matrix;
    auto keyframe = AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation);
    AddKeyframe(type, keyframe);
}

void TransformConvertor::AddKeyframe(AnimationType type, double time, const PerspectiveOperation& distance)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::PERSPECTIVE;
    operation.perspectiveOperation_ = distance;
    auto keyframe = AceType::MakeRefPtr<Keyframe<TransformOperation>>(time, operation);
    AddKeyframe(type, keyframe);
}

const std::unordered_map<std::string, void (*)(const std::string&, const double&, TransformConvertor&)>
    TransformConvertor::TransformConvertorMap = {
        { DOM_TRANSLATE,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_DOUBLE) {
                    auto dx = StringUtils::StringToDimension(offsets[0]);
                    auto dy = StringUtils::StringToDimension(offsets[1]);
                    convertor.AddKeyframe(AnimationType::TRANSLATE, time, TranslateOperation(dx, dy, Dimension {}));
                } else if (offsets.size() == PARAM_SINGLE) {
                    auto dx = StringUtils::StringToDimension(offsets[0]);
                    convertor.AddKeyframe(AnimationType::TRANSLATE, time, TranslateOperation(dx, Dimension {}));
                }
            } },
        { DOM_TRANSLATE_X,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto dx = StringUtils::StringToDimension(typeValue);
                convertor.AddKeyframe(AnimationType::TRANSLATE_X, time, TranslateOperation(dx, Dimension {}));
            } },
        { DOM_TRANSLATE_Y,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto dy = StringUtils::StringToDimension(typeValue);
                convertor.AddKeyframe(AnimationType::TRANSLATE_Y, time, TranslateOperation(Dimension {}, dy));
            } },
        { DOM_TRANSLATE_Z,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto dz = StringUtils::StringToDimension(typeValue);

                convertor.AddKeyframe(
                    AnimationType::TRANSLATE_Z, time, TranslateOperation(Dimension {}, Dimension {}, dz));
            } },
        { DOM_TRANSLATE_3D,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_THREE) {
                    auto dx = StringUtils::StringToDimension(offsets[0]);
                    auto dy = StringUtils::StringToDimension(offsets[1]);
                    auto dz = StringUtils::StringToDimension(offsets[2]);

                    convertor.AddKeyframe(AnimationType::TRANSLATE_3D, time, TranslateOperation(dx, dy, dz));
                }
            } },
        { DOM_ROTATE_3D,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_FOUR) {
                    auto dx = static_cast<float>(StringUtils::StringToDouble(offsets[0]));
                    auto dy = static_cast<float>(StringUtils::StringToDouble(offsets[1]));
                    auto dz = static_cast<float>(StringUtils::StringToDouble(offsets[2]));
                    auto degree = StringUtils::StringToDegree(offsets[3]);
                    convertor.AddKeyframe(
                        AnimationType::ROTATE_3D, time, RotateOperation { dx, dy, dz, static_cast<float>(degree) });
                }
            } },
        { DOM_ROTATE,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto degree = StringUtils::StringToDegree(typeValue);
                convertor.AddKeyframe(
                    AnimationType::ROTATE, time, RotateOperation { 0, 0, 1, static_cast<float>(degree) });
            } },
        { DOM_ROTATE_X,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto degree = StringUtils::StringToDegree(typeValue);
                convertor.AddKeyframe(
                    AnimationType::ROTATE_X, time, RotateOperation { 1, 0, 0, static_cast<float>(degree) });
            } },
        { DOM_ROTATE_Y,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto degree = StringUtils::StringToDegree(typeValue);
                convertor.AddKeyframe(
                    AnimationType::ROTATE_Y, time, RotateOperation { 0, 1, 0, static_cast<float>(degree) });
            } },
        { DOM_ROTATE_Z,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto degree = StringUtils::StringToDegree(typeValue);
                convertor.AddKeyframe(
                    AnimationType::ROTATE_Z, time, RotateOperation { 0, 0, 1, static_cast<float>(degree) });
            } },
        { DOM_SCALE,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                if (typeValue.find(' ') != std::string::npos) {
                    std::vector<std::string> values;
                    StringUtils::StringSpliter(typeValue, ' ', values);
                    if (values.size() == PARAM_DOUBLE) {
                        double scaleValueX = StringUtils::StringToDouble(values[0]);
                        double scaleValueY = StringUtils::StringToDouble(values[1]);
                        convertor.AddKeyframe(
                            AnimationType::SCALE, time, ScaleOperation(scaleValueX, scaleValueY, 1.0f));
                    }
                } else {
                    auto scale = static_cast<float>(StringUtils::StringToDouble(typeValue));
                    convertor.AddKeyframe(AnimationType::SCALE, time, ScaleOperation(scale, scale, 1.0f));
                }
            } },
        { DOM_SCALE_X,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto scaleX = static_cast<float>(StringUtils::StringToDouble(typeValue));
                convertor.AddKeyframe(AnimationType::SCALE_X, time, ScaleOperation(scaleX, 1.0f, 1.0f));
            } },
        { DOM_SCALE_Y,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto scaleY = static_cast<float>(StringUtils::StringToDouble(typeValue));
                convertor.AddKeyframe(AnimationType::SCALE_Y, time, ScaleOperation(1.0f, scaleY, 1.0f));
            } },
        { DOM_SCALE_Z,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto scaleZ = static_cast<float>(StringUtils::StringToDouble(typeValue));
                convertor.AddKeyframe(AnimationType::SCALE_Z, time, ScaleOperation(1.0f, 1.0f, scaleZ));
            } },
        { DOM_SCALE_3D,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_THREE) {
                    auto scaleX = static_cast<float>(StringUtils::StringToDouble(offsets[0]));
                    auto scaleY = static_cast<float>(StringUtils::StringToDouble(offsets[1]));
                    auto scaleZ = static_cast<float>(StringUtils::StringToDouble(offsets[2]));
                    convertor.AddKeyframe(AnimationType::SCALE_3D, time, ScaleOperation(scaleX, scaleY, scaleZ));
                }
            } },
        { DOM_SKEW,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_DOUBLE) {
                    auto degreeX = static_cast<float>(StringUtils::StringToDegree(offsets[0]));
                    auto degreeY = static_cast<float>(StringUtils::StringToDegree(offsets[1]));
                    convertor.AddKeyframe(AnimationType::SKEW, time, SkewOperation { degreeX, degreeY });
                } else if (offsets.size() == PARAM_SINGLE) {
                    auto degreeX = static_cast<float>(StringUtils::StringToDegree(typeValue));
                    convertor.AddKeyframe(AnimationType::SKEW, time, SkewOperation { degreeX, 0.0f });
                }
            } },
        { DOM_SKEW_X,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto degreeX = static_cast<float>(StringUtils::StringToDegree(typeValue));
                convertor.AddKeyframe(AnimationType::SKEW_X, time, SkewOperation { degreeX, 0.0f });
            } },
        { DOM_SKEW_Y,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto degreeY = static_cast<float>(StringUtils::StringToDegree(typeValue));
                convertor.AddKeyframe(AnimationType::SKEW_Y, time, SkewOperation { 0.0f, degreeY });
            } },
        { DOM_MATRIX_3D,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_SIXTEEN) {
                    std::vector<double> matrix;
                    for (const auto& offset : offsets) {
                        matrix.push_back(StringUtils::StringToDouble(offset));
                    }
                    Matrix4 m(matrix[0], matrix[4], matrix[8], matrix[12],
                              matrix[1], matrix[5], matrix[9], matrix[13],
                              matrix[2], matrix[6], matrix[10], matrix[14],
                              matrix[3], matrix[7], matrix[11], matrix[15]);
                    convertor.AddKeyframe(AnimationType::MATRIX_3D, time, m);
                }
            } },
        { DOM_MATRIX,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(typeValue, ' ', offsets);
                if (offsets.size() == PARAM_SIX) {
                    double scaleX = StringUtils::StringToDouble(offsets[0]);
                    double skewY = StringUtils::StringToDouble(offsets[1]);
                    double skewX = StringUtils::StringToDouble(offsets[2]);
                    double scaleY = StringUtils::StringToDouble(offsets[3]);
                    double translateX = StringUtils::StringToDouble(offsets[4]);
                    double translateY = StringUtils::StringToDouble(offsets[5]);
                    Matrix4 matrix = Matrix4::CreateMatrix2D(scaleX, skewY, skewX, scaleY, translateX, translateY);
                    convertor.AddKeyframe(AnimationType::MATRIX_2D, time, matrix);
                }
            } },
        { DOM_PERSPECTIVE,
            [](const std::string& typeValue, const double& time, TransformConvertor& convertor) {
                auto distance = StringUtils::StringToDimension(typeValue);
                convertor.AddKeyframe(AnimationType::PERSPECTIVE, time, PerspectiveOperation(distance));
            } },
    };

} // namespace OHOS::Ace
