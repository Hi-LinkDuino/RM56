/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "gfx_utils/transform.h"

#include "gfx_utils/graphic_math.h"
namespace OHOS {
constexpr uint8_t VERTEX_NUM_MIN = 3;

TransformMap::TransformMap()
{
    scale_ = Matrix4<float>::Scale(Vector3<float>(1.0f, 1.0f, 1.0f), Vector3<float>(0, 0, 0));
    rotate_ = Matrix4<float>::Rotate(0, Vector3<float>(0, 0, 0), Vector3<float>(0, 0, 0));
    shear_ = Matrix4<float>::Shear(Vector2<float>(0, 0), Vector2<float>(0, 0), Vector2<float>(0, 0));
    translate_ = Matrix4<float>::Translate(Vector3<float>(0, 0, 0));
    trans_[ROTATE] = &rotate_;
    trans_[SCALE] = &scale_;
    trans_[SHEAR] = &shear_;
    trans_[TRANSLATE] = &translate_;
    opOrder_[ROTATE] = ROTATE;
    opOrder_[SCALE] = SCALE;
    opOrder_[SHEAR] = SHEAR;
    opOrder_[TRANSLATE] = TRANSLATE;

    UpdateMap();
}

TransformMap::TransformMap(const Rect& rect)
{
    rect_ = rect;
    polygon_ = rect;
    scale_ = Matrix4<float>::Scale(Vector3<float>(1.0f, 1.0f, 1.0f), Vector3<float>(0, 0, 0));
    rotate_ = Matrix4<float>::Rotate(0, Vector3<float>(0, 0, 0), Vector3<float>(0, 0, 0));
    shear_ = Matrix4<float>::Shear(Vector2<float>(0, 0), Vector2<float>(0, 0), Vector2<float>(0, 0));
    translate_ = Matrix4<float>::Translate(Vector3<float>(0, 0, 0));
    trans_[ROTATE] = &rotate_;
    trans_[SCALE] = &scale_;
    trans_[SHEAR] = &shear_;
    trans_[TRANSLATE] = &translate_;
    opOrder_[ROTATE] = ROTATE;
    opOrder_[SCALE] = SCALE;
    opOrder_[SHEAR] = SHEAR;
    opOrder_[TRANSLATE] = TRANSLATE;

    UpdateMap();
}

bool TransformMap::GetClockWise() const
{
    int16_t count = 0;
    int32_t c;

    uint8_t vertexNum = polygon_.GetVertexNum();
    if (vertexNum < VERTEX_NUM_MIN) {
        return false;
    }

    uint8_t i = 0;
    uint8_t j;
    uint8_t k;
    for (; i < vertexNum; i++) {
        j = (i + 1) % vertexNum; // 1: the next vertex
        k = (i + 2) % vertexNum; // 2: the after next vertex
        c = (static_cast<int32_t>(polygon_[j].x_ - polygon_[i].x_) * (polygon_[k].y_ - polygon_[j].y_)) -
            (static_cast<int32_t>(polygon_[j].y_ - polygon_[i].y_) * (polygon_[k].x_ - polygon_[j].x_));
        if (c < 0) {
            count--;
        } else if (c > 0) {
            count++;
        }
    }
    if (count > 0) {
        return true;
    }
    return false;
}

void TransformMap::SetTransMapRect(const Rect& rect)
{
    rect_ = rect;
    polygon_ = rect;
    if (isInternalMatrix_) {
        UpdateMap();
    } else {
        SetMatrix(matrixOrig_);
    }
}

void TransformMap::Scale(const Vector2<float>& scale, const Vector2<float>& pivot)
{
    Scale(Vector3<float>(scale.x_, scale.y_, 1.0f), Vector3<float>(pivot.x_, pivot.y_, 0));
}

void TransformMap::Scale(const Vector3<float>& scale, const Vector3<float>& pivot)
{
    scaleCoeff_ = scale;
    scalePivot_ = pivot;
    AddOp(SCALE);
    UpdateMap();
}

bool TransformMap::IsInvalid() const
{
    if (isInvalid_ || isIdentity_) {
        return true;
    }

    for (uint8_t i = 0; i < polygon_.GetVertexNum(); i++) {
        if (polygon_[i].x_ != 0 || polygon_[i].y_ != 0) {
            return false;
        }
    }
    return true;
}

void TransformMap::Rotate(int16_t angle, const Vector2<float>& pivot)
{
    Rotate(angle, Vector3<float>(pivot.x_, pivot.y_, 0), Vector3<float>(pivot.x_, pivot.y_, 1.0f));
}

void TransformMap::Rotate(int16_t angle, const Vector3<float>& rotatePivotStart, const Vector3<float>& rotatePivotEnd)
{
    angle_ = angle;
    rotatePivotStart_ = rotatePivotStart;
    rotatePivotEnd_ = rotatePivotEnd;
    AddOp(ROTATE);
    UpdateMap();
}

void TransformMap::Translate(const Vector2<int16_t>& trans)
{
    Translate(Vector3<int16_t>(trans.x_, trans.y_, 0));
}

void TransformMap::Translate(const Vector3<int16_t>& trans)
{
    translate_ = Matrix4<float>::Translate(Vector3<float>(trans.x_, trans.y_, trans.z_));
    AddOp(TRANSLATE);
    UpdateMap();
}

void TransformMap::Shear(const Vector2<float>& shearX, const Vector2<float>& shearY, const Vector2<float>& shearZ)
{
    shearX_ = shearX;
    shearY_ = shearY;
    shearZ_ = shearZ;
    AddOp(SHEAR);
    UpdateMap();
}

bool TransformMap::operator==(const TransformMap& other) const
{
    if (rotate_ == other.rotate_ && translate_ == other.translate_ && scale_ == other.scale_ &&
        rect_ == other.rect_ && matrix_ == other.matrix_) {
        return true;
    }
    return false;
}

void TransformMap::SetCameraDistance(int16_t distance)
{
    cameraDistance_ = distance;
    UpdateMap();
}

void TransformMap::SetCameraPosition(const Vector2<float>& position)
{
    cameraPosition_ = position;
    UpdateMap();
}

bool TransformMap::Is3DTransform() const
{
    return is3d_;
}

void TransformMap::UpdateMap()
{
    trans_[ROTATE] = &rotate_;
    trans_[SCALE] = &scale_;
    trans_[SHEAR] = &shear_;
    trans_[TRANSLATE] = &translate_;
    rotate_ =
        Matrix4<float>::Rotate(angle_,
            Vector3<float>(rotatePivotStart_.x_ + rect_.GetX(), rotatePivotStart_.y_ + rect_.GetY(),
                           rotatePivotStart_.z_),
            Vector3<float>(rotatePivotEnd_.x_ + rect_.GetX(), rotatePivotEnd_.y_ + rect_.GetY(), rotatePivotEnd_.z_));

    scale_ = Matrix4<float>::Scale(scaleCoeff_,
        Vector3<float>(scalePivot_.x_ + rect_.GetX(), scalePivot_.y_ + rect_.GetY(), scalePivot_.z_));

    shear_ = Matrix4<float>::Shear(shearX_, shearY_, shearZ_);
    shear_ = shear_ * Matrix4<float>::Translate(Vector3<float>(-rect_.GetX(), -rect_.GetY(), 0));
    shear_ = Matrix4<float>::Translate(Vector3<float>(rect_.GetX(), rect_.GetY(), 0)) * shear_;

    matrix_ = (*trans_[opOrder_[TRANSLATE]]) * (*trans_[opOrder_[SHEAR]]) * (*trans_[opOrder_[SCALE]]) *
              (*trans_[opOrder_[ROTATE]]);

    float x = rect_.GetX() + cameraPosition_.x_;
    float y = rect_.GetY() + cameraPosition_.y_;
    float z = 0;
    Matrix4<float> translateFromCamera = Matrix4<float>::Translate(Vector3<float>(-x, -y, -z));
    Matrix4<float> translateToCamera = Matrix4<float>::Translate(Vector3<float>(x, y, z));
    Matrix4<float> perspectiveMatrix;
    perspectiveMatrix[2][2] = 0; // 2 : index
    if (!FloatEqual(cameraDistance_, 0)) {
        perspectiveMatrix[2][3] = -1.0f / cameraDistance_; // 2 3 : index
    }
    perspectiveMatrix_ = translateToCamera * (perspectiveMatrix * translateFromCamera);
    matrix_ = perspectiveMatrix_ * matrix_;
    SetMatrix(matrix_, true);
}

void TransformMap::SetMatrix(const Matrix4<float>& matrix, bool isInternalMatrix)
{
    isInternalMatrix_ = isInternalMatrix;
    polygon_ = rect_;
    matrixOrig_ = matrix;
    uint8_t vertexNum = polygon_.GetVertexNum();
    Vector4<float> imgPoint4;
    is3d_ = false;
    for (uint8_t i = 0; i < vertexNum; i++) {
        Vector4<float> point(polygon_[i].x_, polygon_[i].y_, 0, 1.0f);
        imgPoint4 = matrix * point;
        if (!FloatEqual(imgPoint4.w_, 1)) {
            is3d_ = true;
        }
        if (!FloatEqual(imgPoint4.w_, 0)) {
            imgPoint4.x_ /= imgPoint4.w_;
            imgPoint4.y_ /= imgPoint4.w_;
        }
        if (imgPoint4.x_ < COORD_MIN) {
            polygon_[i].x_ = COORD_MIN;
        } else if (imgPoint4.x_ > COORD_MAX) {
            polygon_[i].x_ = COORD_MAX;
        } else {
            polygon_[i].x_ = MATH_ROUND(imgPoint4.x_);
        }

        if (imgPoint4.y_ < COORD_MIN) {
            polygon_[i].y_ = COORD_MIN;
        } else if (imgPoint4.y_ > COORD_MAX) {
            polygon_[i].y_ = COORD_MAX;
        } else {
            polygon_[i].y_ = MATH_ROUND(imgPoint4.y_);
        }
    }
    isIdentity_ = IsIdentity(const_cast<Matrix4<float>&>(matrix));
    Matrix4<float> translate = Matrix4<float>::Translate(Vector3<float>(rect_.GetX(), rect_.GetY(), 0));
    matrix_ = matrix * translate;
    /* 0 1 2 3  : index of matrix */
    Matrix3<float> matrix3(matrix_[0][0], matrix_[0][1], matrix_[0][3],
                           matrix_[1][0], matrix_[1][1], matrix_[1][3],
                           matrix_[3][0], matrix_[3][1], matrix_[3][3]);
    invMatrix_ = matrix3.Inverse();
}

void TransformMap::AddOp(uint8_t op)
{
    uint8_t index = 0;
    for (; index < TRANS_NUM; index++) {
        if (opOrder_[index] == op) {
            break;
        }
    }
    for (; index < TRANSLATE; index++) {
        opOrder_[index] = opOrder_[index + 1];
    }
    opOrder_[TRANSLATE] = op;
}

void Rotate(const Vector2<int16_t>& point, int16_t angle, const Vector2<int16_t>& pivot, Vector2<int16_t>& out)
{
    float sinma = Sin(angle);
    float cosma = Sin(angle + 90); // 90: cos

    int16_t xt = point.x_ - pivot.x_;
    int16_t yt = point.y_ - pivot.y_;

    /* 0.5: round up */
    float temp = cosma * xt - sinma * yt;
    out.x_ = static_cast<int16_t>((temp > 0) ? (temp + 0.5f) : (temp - 0.5f)) + pivot.x_;
    temp = sinma * xt + cosma * yt;
    out.y_ = static_cast<int16_t>((temp > 0) ? (temp + 0.5f) : (temp - 0.5f)) + pivot.y_;
}

void Rotate(const Line& origLine, int16_t angle, const Vector2<int16_t>& pivot, Line& out)
{
    Vector2<int16_t> pt1 = origLine[0];
    Vector2<int16_t> pt2 = origLine[1];

    Rotate(pt1, angle, pivot, out[1]); // 1: the first point of line
    Rotate(pt2, angle, pivot, out[2]); // 2: the second point of line
}

void Rotate(const Rect& origRect, int16_t angle, const Vector2<int16_t>& pivot, Polygon& out)
{
    Vector2<int16_t> pt1 = {origRect.GetLeft(), origRect.GetTop()};
    Vector2<int16_t> pt2 = {origRect.GetRight(), origRect.GetTop()};
    Vector2<int16_t> pt3 = {origRect.GetRight(), origRect.GetBottom()};
    Vector2<int16_t> pt4 = {origRect.GetLeft(), origRect.GetBottom()};

    Rotate(pt1, angle, pivot, out[1]); // 1: the first point
    Rotate(pt2, angle, pivot, out[2]); // 2: the second point
    Rotate(pt3, angle, pivot, out[3]); // 3: the third point
    Rotate(pt4, angle, pivot, out[4]); // 4: the fourth point

    out.SetVertexNum(4); // 4: number of vertex
}
} // namespace OHOS
