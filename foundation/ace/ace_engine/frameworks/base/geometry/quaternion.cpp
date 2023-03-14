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

#include "base/geometry/quaternion.h"

#include <cmath>

namespace OHOS::Ace {
namespace {

constexpr double KEPSILON = 1e-5;

} // namespace

Quaternion Quaternion::Slerp(const Quaternion& to, double t) const
{
    if (t < 0.0 || t > 1.0) {
        // https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
        // a scalar between 0.0 (at from) and 1.0 (at to)
        return *this;
    }

    Quaternion from = *this;

    double cosHalfAngle = from.x_ * to.x_ + from.y_ * to.y_ + from.z_ * to.z_ + from.w_ * to.w_;
    if (cosHalfAngle < 0.0) {
        // Since the half angle is > 90 degrees, the full rotation angle would
        // exceed 180 degrees. The quaternions (x, y, z, w) and (-x, -y, -z, -w)
        // represent the same rotation. Flipping the orientation of either
        // quaternion ensures that the half angle is less than 90 and that we are
        // taking the shortest path.
        from = from.flip();
        cosHalfAngle = -cosHalfAngle;
    }

    // Ensure that acos is well behaved at the boundary.
    if (cosHalfAngle > 1.0) {
        cosHalfAngle = 1.0;
    }

    double sinHalfAngle = std::sqrt(1.0 - cosHalfAngle * cosHalfAngle);
    if (sinHalfAngle < KEPSILON) {
        // Quaternions share common axis and angle.
        return *this;
    }

    double half_angle = std::acos(cosHalfAngle);

    double scaleA = std::sin((1.0 - t) * half_angle) / sinHalfAngle;
    double scaleB = std::sin(t * half_angle) / sinHalfAngle;

    return (scaleA * from) + (scaleB * to);
}

} // namespace OHOS::Ace
