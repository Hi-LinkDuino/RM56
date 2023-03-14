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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_TRANSFORM_UTIL_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_TRANSFORM_UTIL_H

#include <utility>
#include <vector>

#include "base/geometry/dimension.h"
#include "base/geometry/matrix4.h"
#include "base/geometry/quaternion.h"

namespace OHOS::Ace {

struct ACE_EXPORT TranslateOperation {
    TranslateOperation() = default;
    TranslateOperation(Dimension dx, Dimension dy, Dimension dz = Dimension {}) : dx(dx), dy(dy), dz(dz) {}
    bool operator==(const TranslateOperation& other) const
    {
        return dx == other.dx && dy == other.dy && dz == other.dz;
    }
    Dimension dx;
    Dimension dy;
    Dimension dz;

    static TranslateOperation Blend(const TranslateOperation& to, const TranslateOperation& from, float progress);
};

struct ACE_EXPORT ScaleOperation {
    ScaleOperation() = default;
    ScaleOperation(float x, float y, float z) : scaleX(x), scaleY(y), scaleZ(z) {}
    bool operator==(const ScaleOperation& other) const
    {
        return NearEqual(scaleX, other.scaleX) && NearEqual(scaleY, other.scaleY) && NearEqual(scaleZ, other.scaleZ);
    }
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;

    static ScaleOperation Blend(const ScaleOperation& to, const ScaleOperation& from, float progress);
};

struct ACE_EXPORT SkewOperation {
    SkewOperation() = default;
    SkewOperation(float x, float y) : skewX(x), skewY(y) {}
    bool operator==(const SkewOperation& other) const
    {
        return NearEqual(skewX, other.skewX) && NearEqual(skewY, other.skewY);
    }
    float skewX = 0.0f;
    float skewY = 0.0f;

    static SkewOperation Blend(const SkewOperation& to, const SkewOperation& from, float progress);
};

struct ACE_EXPORT RotateOperation {
    RotateOperation() = default;
    RotateOperation(float x, float y, float z, float angle) : dx(x), dy(y), dz(z), angle(angle) {}
    bool operator==(const RotateOperation& other) const
    {
        return NearEqual(dx, other.dx) && NearEqual(dy, other.dy) && NearEqual(dz, other.dz) &&
               NearEqual(angle, other.angle);
    }
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
    float angle = 0.0f;

    static RotateOperation Blend(const RotateOperation& to, const RotateOperation& from, float progress);
};

struct ACE_EXPORT PerspectiveOperation {
    PerspectiveOperation() = default;
    explicit PerspectiveOperation(const Dimension& dis) : distance(dis) {}
    bool operator==(const PerspectiveOperation& other) const
    {
        return distance == other.distance;
    }
    Dimension distance;

    static PerspectiveOperation Blend(const PerspectiveOperation& to, const PerspectiveOperation& from, float progress);
};

struct ACE_EXPORT DecomposedTransform {
    float translate[3] = { 0.0f, 0.0f, 0.0f };
    float scale[3] = { 1.0f, 1.0f, 1.0f };
    float skew[3] = { 0.0f, 0.0f, 0.0f };
    float perspective[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    Quaternion quaternion;

    std::string ToString() const;
};

enum class TransformOperationType {
    TRANSLATE,
    SCALE,
    SKEW,
    ROTATE,
    MATRIX,
    PERSPECTIVE,
    UNDEFINED,
};

struct ACE_EXPORT TransformOperation {
    TransformOperation() : type_(TransformOperationType::UNDEFINED) {}

    TransformOperationType type_ = TransformOperationType::UNDEFINED;
    Matrix4 matrix4_ = Matrix4::CreateIdentity();
    union {
        TranslateOperation translateOperation_;
        ScaleOperation scaleOperation_;
        SkewOperation skewOperation_;
        RotateOperation rotateOperation_;
        PerspectiveOperation perspectiveOperation_;
    };

    bool operator==(const TransformOperation& other) const
    {
        if (type_ != other.type_) {
            return false;
        }
        switch (type_) {
            case TransformOperationType::MATRIX: {
                return matrix4_ == other.matrix4_;
            }
            case TransformOperationType::PERSPECTIVE: {
                return perspectiveOperation_ == other.perspectiveOperation_;
            }
            case TransformOperationType::ROTATE: {
                return rotateOperation_ == other.rotateOperation_;
            }
            case TransformOperationType::SCALE: {
                return scaleOperation_ == other.scaleOperation_;
            }
            case TransformOperationType::SKEW: {
                return skewOperation_ == other.skewOperation_;
            }
            case TransformOperationType::TRANSLATE: {
                return translateOperation_ == other.translateOperation_;
            }
            case TransformOperationType::UNDEFINED:
                // fall through
            default:
                return true;
        }
    }

    static TransformOperation Blend(const TransformOperation& to, const TransformOperation& from, float progress);
    static TransformOperation Create(TransformOperationType type);

private:
    static void BlendInner(
        const TransformOperation& to, const TransformOperation& from, float progress, TransformOperation& out);
};

class ACE_EXPORT TransformOperations {
public:
    static void ParseOperationsToMatrix(std::vector<TransformOperation>& operations);

    static void ParseOperationToMatrix(TransformOperation& operations);

    static TransformOperations Blend(const TransformOperations& to, const TransformOperations& from, float progress);

    explicit TransformOperations(std::vector<TransformOperation> operation = std::vector<TransformOperation>())
        : operations_(std::move(operation))
    {}
    ~TransformOperations() = default;

    std::vector<TransformOperation>& GetOperations()
    {
        return operations_;
    }

    const std::vector<TransformOperation>& GetOperations() const
    {
        return operations_;
    }

    Matrix4 ComputerRemaining(std::size_t startOffset) const;

    void SetAlwaysRotate(bool alwaysRotate)
    {
        alwaysRotate_ = alwaysRotate;
    }

    bool operator==(const TransformOperations& other) const
    {
        if (alwaysRotate_) {
            return false;
        }
        if (operations_.size() != other.operations_.size()) {
            return false;
        }
        for (size_t index = 0; index < operations_.size(); index++) {
            if (!(operations_[index] == other.operations_[index])) {
                return false;
            }
        }
        return true;
    }

protected:
    std::vector<TransformOperation> operations_;
    bool alwaysRotate_ = false;

private:
    std::size_t MatchingLength(const TransformOperations& to, const TransformOperations& from) const;
    void BlendInner(const TransformOperations& from, float progress, TransformOperations& out) const;
};

class ACE_EXPORT TransformUtil final {
public:
    static bool DecomposeTransform(DecomposedTransform& out, const Matrix4& transform);
    static DecomposedTransform BlendDecomposedTransforms(
        const DecomposedTransform& to, const DecomposedTransform& from, double progress);
    static Matrix4 ComposeTransform(const DecomposedTransform& decomp);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_TRANSFORM_UTIL_H
