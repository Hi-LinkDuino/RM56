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

#include "base/geometry/transform_util.h"

namespace OHOS::Ace {
namespace {

float Length3(const float v[3])
{
    double vd[3] = { v[0], v[1], v[2] };
    return static_cast<float>(std::sqrt(vd[0] * vd[0] + vd[1] * vd[1] + vd[2] * vd[2]));
}

template<int n>
float Dot(const float* a, const float* b)
{
    double total = 0.0;
    for (int i = 0; i < n; ++i) {
        total += a[i] * b[i];
    }
    return static_cast<float>(total);
}

template<int n>
void Combine(float* out, const float* a, const float* b, double scaleA, double scaleB)
{
    for (int i = 0; i < n; ++i) {
        out[i] = static_cast<float>(a[i] * scaleA + b[i] * scaleB);
    }
}

void Cross3(float out[3], const float a[3], const float b[3])
{
    float x = a[1] * b[2] - a[2] * b[1];
    float y = a[2] * b[0] - a[0] * b[2];
    float z = a[0] * b[1] - a[1] * b[0];
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

// Returns false if the matrix cannot be normalized.
bool Normalize(Matrix4& m)
{
    if (NearZero(m.Get(3, 3))) {
        return false;
    }
    float scale = 1.0f / m.Get(3, 3);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            auto value = m.Get(i, j) * scale;
            m.Set(i, j, value);
        }
    }
    return true;
}

Matrix4 BuildPerspectiveMatrix(const DecomposedTransform& decomp)
{
    Matrix4 matrix = Matrix4::CreateIdentity();

    for (int i = 0; i < 4; i++) {
        matrix.Set(3, i, decomp.perspective[i]);
    }
    return matrix;
}

Matrix4 BuildTranslationMatrix(const DecomposedTransform& decomp)
{
    Matrix4 matrix = Matrix4::CreateIdentity();
    float dx = decomp.translate[0];
    float dy = decomp.translate[1];
    float dz = decomp.translate[2];
    if (NearZero(dx) && NearZero(dy) && NearZero(dz)) {
        return matrix;
    }

    matrix.Set(0, 3, dx);
    matrix.Set(1, 3, dy);
    matrix.Set(2, 3, dz);
    return matrix;
}

Matrix4 BuildRotationMatrix(const DecomposedTransform& decomp)
{
    Matrix4 matrix4;
    double x = decomp.quaternion.GetX();
    double y = decomp.quaternion.GetY();
    double z = decomp.quaternion.GetZ();
    double w = decomp.quaternion.GetW();

    matrix4.Set(0, 0, static_cast<float>(1.0 - 2.0 * (y * y + z * z)));
    matrix4.Set(1, 0, static_cast<float>(2.0 * (x * y + z * w)));
    matrix4.Set(2, 0, static_cast<float>(2.0 * (x * z - y * w)));
    matrix4.Set(3, 0, 0);

    matrix4.Set(0, 1, static_cast<float>(2.0 * (x * y - z * w)));
    matrix4.Set(1, 1, static_cast<float>(1.0 - 2.0 * (x * x + z * z)));
    matrix4.Set(2, 1, static_cast<float>(2.0 * (y * z + x * w)));
    matrix4.Set(3, 1, 0);

    matrix4.Set(0, 2, static_cast<float>(2.0 * (x * z + y * w)));
    matrix4.Set(1, 2, static_cast<float>(2.0 * (y * z - x * w)));
    matrix4.Set(2, 2, static_cast<float>(1.0 - 2.0 * (x * x + y * y)));
    matrix4.Set(3, 2, 0);

    matrix4.Set(0, 3, 0);
    matrix4.Set(1, 3, 0);
    matrix4.Set(2, 3, 0);
    matrix4.Set(3, 3, 1);

    return matrix4;
}

Matrix4 BuildSkewMatrix(const DecomposedTransform& decomp)
{
    Matrix4 matrix = Matrix4::CreateIdentity();

    Matrix4 temp = Matrix4::CreateIdentity();
    if (decomp.skew[2]) {
        temp.Set(1, 2, decomp.skew[2]);
        matrix = matrix * temp;
    }

    if (decomp.skew[1]) {
        temp.Set(1, 2, 0);
        temp.Set(0, 2, decomp.skew[1]);
        matrix = matrix * temp;
    }

    if (decomp.skew[0]) {
        temp.Set(0, 2, 0);
        temp.Set(0, 1, decomp.skew[0]);
        matrix = matrix * temp;
    }
    return matrix;
}

Matrix4 BuildScaleMatrix(const DecomposedTransform& decomp)
{
    Matrix4 matrix = Matrix4::CreateIdentity();
    matrix.SetScale(decomp.scale[0], decomp.scale[1], decomp.scale[2]);
    return matrix;
}

Matrix4 ComposeTransform(const Matrix4& perspective, const Matrix4& translation, const Matrix4& rotation,
    const Matrix4& skew, const Matrix4& scale)
{
    Matrix4 matrix = Matrix4::CreateIdentity();
    matrix = matrix * perspective;
    matrix = matrix * translation;
    matrix = matrix * rotation;
    matrix = matrix * skew;
    matrix = matrix * scale;
    return matrix;
}

} // namespace

TranslateOperation TranslateOperation::Blend(
    const TranslateOperation& to, const TranslateOperation& from, float progress)
{
    TranslateOperation ret;
    float scaleA = progress;
    float scaleB = 1 - progress;
    ret.dx = to.dx * scaleA + from.dx * scaleB;
    ret.dy = to.dy * scaleA + from.dy * scaleB;
    ret.dz = to.dz * scaleA + from.dz * scaleB;

    return ret;
}

ScaleOperation ScaleOperation::Blend(const ScaleOperation& to, const ScaleOperation& from, float progress)
{
    ScaleOperation ret;
    float scaleA = progress;
    float scaleB = 1 - progress;
    ret.scaleX = to.scaleX * scaleA + from.scaleX * scaleB;
    ret.scaleY = to.scaleY * scaleA + from.scaleY * scaleB;
    ret.scaleZ = to.scaleZ * scaleA + from.scaleZ * scaleB;
    return ret;
}

SkewOperation SkewOperation::Blend(const SkewOperation& to, const SkewOperation& from, float progress)
{
    SkewOperation ret;
    float scaleA = progress;
    float scaleB = 1 - progress;
    ret.skewX = to.skewX * scaleA + from.skewX * scaleB;
    ret.skewY = to.skewY * scaleA + from.skewY * scaleB;
    return ret;
}

RotateOperation RotateOperation::Blend(const RotateOperation& to, const RotateOperation& from, float progress)
{
    RotateOperation ret;
    float scaleA = progress;
    float scaleB = 1 - progress;
    ret.angle = to.angle * scaleA + from.angle * scaleB;
    ret.dx = to.dx;
    ret.dy = to.dy;
    ret.dz = to.dz;
    // rotate vector is (0,0,0) is error
    if (NearZero(ret.dx) && NearZero(ret.dy) && NearZero(ret.dz)) {
        ret.dx = from.dx;
        ret.dy = from.dy;
        ret.dz = from.dz;
    }
    return ret;
}

PerspectiveOperation PerspectiveOperation::Blend(
    const PerspectiveOperation& to, const PerspectiveOperation& from, float progress)
{
    PerspectiveOperation ret;
    ret.distance = from.distance + (to.distance - from.distance) * progress;
    return ret;
}

TransformOperation TransformOperation::Blend(
    const TransformOperation& to, const TransformOperation& from, float progress)
{
    TransformOperation ret;
    if (to.type_ == from.type_ && to.type_ == TransformOperationType::UNDEFINED) {
        return ret;
    } else if (to.type_ == TransformOperationType::UNDEFINED) {
        ret.type_ = from.type_;
        BlendInner(Create(ret.type_), from, progress, ret);
    } else if (from.type_ == TransformOperationType::UNDEFINED) {
        ret.type_ = to.type_;
        BlendInner(to, Create(ret.type_), progress, ret);
    } else if (to.type_ == from.type_) {
        ret.type_ = to.type_;
        BlendInner(to, from, progress, ret);
    }
    return ret;
}

TransformOperation TransformOperation::Create(TransformOperationType type)
{
    TransformOperation ret;
    ret.type_ = type;
    switch (ret.type_) {
        case TransformOperationType::TRANSLATE:
            ret.translateOperation_ = TranslateOperation();
            break;
        case TransformOperationType::SCALE:
            ret.scaleOperation_ = ScaleOperation();
            break;
        case TransformOperationType::SKEW:
            ret.skewOperation_ = SkewOperation();
            break;
        case TransformOperationType::ROTATE:
            ret.rotateOperation_ = RotateOperation();
            break;
        case TransformOperationType::MATRIX:
            ret.matrix4_ = Matrix4::CreateIdentity();
            break;
        case TransformOperationType::PERSPECTIVE:
            ret.perspectiveOperation_ = PerspectiveOperation();
            break;
        case TransformOperationType::UNDEFINED:
            break;
        default:
            break;
    }
    return ret;
}

void TransformOperation::BlendInner(
    const TransformOperation& to, const TransformOperation& from, float progress, TransformOperation& ret)
{
    switch (ret.type_) {
        case TransformOperationType::TRANSLATE:
            ret.translateOperation_ =
                TranslateOperation::Blend(to.translateOperation_, from.translateOperation_, progress);
            break;
        case TransformOperationType::SCALE:
            ret.scaleOperation_ = ScaleOperation::Blend(to.scaleOperation_, from.scaleOperation_, progress);
            break;
        case TransformOperationType::SKEW:
            ret.skewOperation_ = SkewOperation::Blend(to.skewOperation_, from.skewOperation_, progress);
            break;
        case TransformOperationType::ROTATE:
            ret.rotateOperation_ = RotateOperation::Blend(to.rotateOperation_, from.rotateOperation_, progress);
            break;
        case TransformOperationType::MATRIX: {
            DecomposedTransform toTransform;
            DecomposedTransform fromTransform;
            if (TransformUtil::DecomposeTransform(toTransform, to.matrix4_) &&
                TransformUtil::DecomposeTransform(fromTransform, from.matrix4_)) {
                auto result = TransformUtil::BlendDecomposedTransforms(toTransform, fromTransform, progress);
                ret.matrix4_ = TransformUtil::ComposeTransform(result);
            } else {
                LOGE("DecomposeTransform failed");
            }
            break;
        }
        case TransformOperationType::PERSPECTIVE:
            ret.perspectiveOperation_ =
                PerspectiveOperation::Blend(to.perspectiveOperation_, from.perspectiveOperation_, progress);
            break;
        case TransformOperationType::UNDEFINED:
            break;
        default:
            break;
    }
}

std::string DecomposedTransform::ToString() const
{
    std::string out;
    out.append("translate: ")
        .append(std::to_string(translate[0]))
        .append(" ")
        .append(std::to_string(translate[1]))
        .append(" ")
        .append(std::to_string(translate[2]))
        .append("\n")
        .append("scale: ")
        .append(std::to_string(scale[0]))
        .append(" ")
        .append(std::to_string(scale[1]))
        .append(" ")
        .append(std::to_string(scale[2]))
        .append("\n")
        .append("skew: ")
        .append(std::to_string(skew[0]))
        .append(" ")
        .append(std::to_string(skew[1]))
        .append(" ")
        .append(std::to_string(skew[2]))
        .append("\n")
        .append("perspective: ")
        .append(std::to_string(perspective[0]))
        .append(" ")
        .append(std::to_string(perspective[1]))
        .append(" ")
        .append(std::to_string(perspective[2]))
        .append(" ")
        .append(std::to_string(perspective[3]))
        .append("\n")
        .append("quaternion: ")
        .append(std::to_string(quaternion.GetX()))
        .append(" ")
        .append(std::to_string(quaternion.GetY()))
        .append(" ")
        .append(std::to_string(quaternion.GetZ()))
        .append(" ")
        .append(std::to_string(quaternion.GetW()))
        .append("\n");
    return out;
}

TransformOperations TransformOperations::Blend(
    const TransformOperations& to, const TransformOperations& from, float progress)
{
    TransformOperations result;
    to.BlendInner(from, progress, result);
    return result;
}

std::size_t TransformOperations::MatchingLength(const TransformOperations& to, const TransformOperations& from) const
{
    auto numOperations = std::min(to.operations_.size(), from.operations_.size());

    for (std::size_t i = 0; i < numOperations; i++) {
        auto& first = to.operations_[i];
        auto& second = from.operations_[i];

        if (first.type_ != TransformOperationType::UNDEFINED || second.type_ != TransformOperationType::UNDEFINED) {
            if (first.type_ != second.type_) {
                return i;
            }
        }
    }
    return std::max(to.operations_.size(), from.operations_.size());
}

void TransformOperations::ParseOperationsToMatrix(std::vector<TransformOperation>& operations)
{
    for (auto& operation : operations) {
        ParseOperationToMatrix(operation);
    }
}

void TransformOperations::ParseOperationToMatrix(TransformOperation& operation)
{
    switch (operation.type_) {
        case TransformOperationType::TRANSLATE: {
            auto& translate = operation.translateOperation_;
            float dx = translate.dx.Value();
            float dy = translate.dy.Value();
            float dz = translate.dz.Value();
            operation.matrix4_ = Matrix4::CreateTranslate(dx, dy, dz);
            break;
        }
        case TransformOperationType::SCALE: {
            auto& scale = operation.scaleOperation_;
            operation.matrix4_ = Matrix4::CreateScale(scale.scaleX, scale.scaleY, scale.scaleZ);
            break;
        }
        case TransformOperationType::SKEW: {
            auto& skew = operation.skewOperation_;
            operation.matrix4_ = Matrix4::CreateSkew(skew.skewX, skew.skewY);
            break;
        }
        case TransformOperationType::ROTATE: {
            auto& rotate = operation.rotateOperation_;
            operation.matrix4_ = Matrix4::CreateRotate(rotate.angle, rotate.dx, rotate.dy, rotate.dz);
            break;
        }
        case TransformOperationType::PERSPECTIVE: {
            auto& perspective = operation.perspectiveOperation_;
            double distance = perspective.distance.Value();
            operation.matrix4_ = Matrix4::CreatePerspective(distance);
            break;
        }
        case TransformOperationType::MATRIX:
        case TransformOperationType::UNDEFINED:
            break;
        default:
            break;
    }
}

void TransformOperations::BlendInner(const TransformOperations& from, float progress, TransformOperations& out) const
{
    auto matchPrefix = MatchingLength(*this, from);
    auto fromSize = from.operations_.size();
    auto toSize = operations_.size();

    // find most match type transform
    for (std::size_t i = 0; i < matchPrefix; i++) {
        const auto& fromTransformOperation = i >= fromSize ? TransformOperation() : from.operations_[i];
        const auto& toTransformOperation = i >= toSize ? TransformOperation() : operations_[i];
        out.operations_.push_back(TransformOperation::Blend(toTransformOperation, fromTransformOperation, progress));
    }
    // type not match
    if (matchPrefix < std::max(fromSize, toSize)) {
        TransformOperation fromTransformOperation;
        fromTransformOperation.type_ = TransformOperationType::MATRIX;
        fromTransformOperation.matrix4_ = from.ComputerRemaining(matchPrefix);
        TransformOperation toTransformOperation;
        toTransformOperation.type_ = TransformOperationType::MATRIX;
        toTransformOperation.matrix4_ = ComputerRemaining(matchPrefix);
        out.operations_.push_back(TransformOperation::Blend(toTransformOperation, fromTransformOperation, progress));
    }
}

Matrix4 TransformOperations::ComputerRemaining(std::size_t startOffset) const
{
    Matrix4 result = Matrix4::CreateIdentity();
    for (auto i = startOffset; i < operations_.size(); i++) {
        result = result * operations_[i].matrix4_;
    }
    return result;
}

DecomposedTransform TransformUtil::BlendDecomposedTransforms(
    const DecomposedTransform& to, const DecomposedTransform& from, double progress)
{
    DecomposedTransform ret;
    Combine<3>(ret.translate, to.translate, from.translate, progress, 1.0 - progress);
    Combine<3>(ret.scale, to.scale, from.scale, progress, 1.0 - progress);
    Combine<3>(ret.skew, to.skew, from.skew, progress, 1.0 - progress);
    Combine<4>(ret.perspective, to.perspective, from.perspective, progress, 1.0 - progress);
    ret.quaternion = from.quaternion.Slerp(to.quaternion, progress);
    return ret;
}

bool TransformUtil::DecomposeTransform(DecomposedTransform& out, const Matrix4& transform)
{
    Matrix4 matrix = transform;

    if (!Normalize(matrix)) {
        return false;
    }

    Matrix4 perspectiveMatrix = matrix;
    for (int i = 0; i < 3; i++) {
        perspectiveMatrix.Set(3, i, 0.0);
    }
    perspectiveMatrix.Set(3, 3, 1.0);

    if (NearZero(std::abs(perspectiveMatrix.Determinant()))) {
        return false;
    }

    if (!NearZero(matrix.Get(3, 0)) || !NearZero(matrix.Get(3, 1)) || !NearZero(matrix.Get(3, 2))) {
        double rhs[4] = { matrix.Get(3, 0), matrix.Get(3, 1), matrix.Get(3, 2), matrix.Get(3, 3) };

        Matrix4 inversePerspectiveMatrix = Matrix4::Invert(perspectiveMatrix);
        Matrix4 transposedInversePerspectiveMatrix = inversePerspectiveMatrix;

        transposedInversePerspectiveMatrix.Transpose();
        transposedInversePerspectiveMatrix.MapScalars(rhs, 4);

        for (int32_t i = 0; i < 4; i++) {
            out.perspective[i] = rhs[i];
        }
    } else {
        // No perspective.
        for (int i = 0; i < 3; ++i)
            out.perspective[i] = 0.0;
        out.perspective[3] = 1.0;
    }

    for (int32_t i = 0; i < 3; i++) {
        out.translate[i] = matrix.Get(i, 3);
    }

    // Copy of matrix is stored in column major order to facilitate column-level
    // operations.
    float column[3][3];
    for (int32_t i = 0; i < 3; i++) {
        for (int32_t j = 0; j < 3; j++) {
            column[i][j] = matrix.Get(j, i);
        }
    }

    // Compute X scale factor and normalize first column.
    out.scale[0] = Length3(column[0]);
    if (out.scale[0] != 0.0) {
        column[0][0] /= out.scale[0];
        column[0][1] /= out.scale[0];
        column[0][2] /= out.scale[0];
    }

    // Compute XY shear factor and make 2nd column orthogonal to 1st.
    out.skew[0] = Dot<3>(column[0], column[1]);
    Combine<3>(column[1], column[1], column[0], 1.0, -out.skew[0]);

    // Now, compute Y scale and normalize 2nd column.
    out.scale[1] = Length3(column[1]);
    if (out.scale[1] != 0.0) {
        column[1][0] /= out.scale[1];
        column[1][1] /= out.scale[1];
        column[1][2] /= out.scale[1];
    }

    out.skew[0] /= out.scale[1];

    // Compute XZ and YZ shears, orthogonalize the 3rd column.
    out.skew[1] = Dot<3>(column[0], column[2]);
    Combine<3>(column[2], column[2], column[0], 1.0, -out.skew[1]);
    out.skew[2] = Dot<3>(column[1], column[2]);
    Combine<3>(column[2], column[2], column[1], 1.0, -out.skew[2]);

    // Next, get Z scale and normalize the 3rd column.
    out.scale[2] = Length3(column[2]);
    if (out.scale[2] != 0.0) {
        column[2][0] /= out.scale[2];
        column[2][1] /= out.scale[2];
        column[2][2] /= out.scale[2];
    }

    out.skew[1] /= out.scale[2];
    out.skew[2] /= out.scale[2];

    // At this point, the matrix is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
    // only 1 axis is flipped when the determinant is negative. Verify if it is
    // correct to flip all of the scales and matrix elements, as this introduces
    // rotation for the simple case of a single axis scale inversion.
    float pdum3[3];
    Cross3(pdum3, column[1], column[2]);
    if (Dot<3>(column[0], pdum3) < 0) {
        for (int i = 0; i < 3; i++) {
            out.scale[i] *= -1.0;
            for (int j = 0; j < 3; ++j)
                column[i][j] *= -1.0;
        }
    }

    // See https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion.
    // Note: deviating from spec (http://www.w3.org/TR/css3-transforms/)
    // which has a degenerate case of zero off-diagonal elements in the
    // orthonormal matrix, which leads to errors in determining the sign
    // of the quaternions.
    double q_xx = column[0][0];
    double q_xy = column[1][0];
    double q_xz = column[2][0];
    double q_yx = column[0][1];
    double q_yy = column[1][1];
    double q_yz = column[2][1];
    double q_zx = column[0][2];
    double q_zy = column[1][2];
    double q_zz = column[2][2];

    double r, s, t, x, y, z, w;
    t = q_xx + q_yy + q_zz;
    if (t > 0) {
        r = std::sqrt(1.0 + t);
        s = 0.5 / r;
        w = 0.5 * r;
        x = (q_zy - q_yz) * s;
        y = (q_xz - q_zx) * s;
        z = (q_yx - q_xy) * s;
    } else if (q_xx > q_yy && q_xx > q_zz) {
        r = std::sqrt(1.0 + q_xx - q_yy - q_zz);
        s = 0.5 / r;
        x = 0.5 * r;
        y = (q_xy + q_yx) * s;
        z = (q_xz + q_zx) * s;
        w = (q_zy - q_yz) * s;
    } else if (q_yy > q_zz) {
        r = std::sqrt(1.0 - q_xx + q_yy - q_zz);
        s = 0.5 / r;
        x = (q_xy + q_yx) * s;
        y = 0.5 * r;
        z = (q_yz + q_zy) * s;
        w = (q_xz - q_zx) * s;
    } else {
        r = std::sqrt(1.0 - q_xx - q_yy + q_zz);
        s = 0.5 / r;
        x = (q_xz + q_zx) * s;
        y = (q_yz + q_zy) * s;
        z = 0.5 * r;
        w = (q_yx - q_xy) * s;
    }

    out.quaternion.SetX(static_cast<float>(x));
    out.quaternion.SetY(static_cast<float>(y));
    out.quaternion.SetZ(static_cast<float>(z));
    out.quaternion.SetW(static_cast<float>(w));

    return true;
}

Matrix4 TransformUtil::ComposeTransform(const struct DecomposedTransform& decomp)
{
    Matrix4 perspective = BuildPerspectiveMatrix(decomp);
    Matrix4 translation = BuildTranslationMatrix(decomp);
    Matrix4 rotation = BuildRotationMatrix(decomp);
    Matrix4 skew = BuildSkewMatrix(decomp);
    Matrix4 scale = BuildScaleMatrix(decomp);

    return OHOS::Ace::ComposeTransform(perspective, translation, rotation, skew, scale);
}

} // namespace OHOS::Ace