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

#include "base/geometry/least_square_impl.h"

#include "base/geometry/matrix3.h"
#include "base/geometry/matrix4.h"
#include "base/log/log.h"

namespace OHOS::Ace {
bool LeastSquareImpl::GetLeastSquareParams(std::vector<double>& params)
{
    if (xVals_.size() <= 1 || ((paramsNum_ != Matrix3::DIMENSION) && (paramsNum_ != Matrix4::DIMENSION))) {
        LOGE("size is invalid, %{public}d, %{public}d", static_cast<int32_t>(xVals_.size()), paramsNum_);
        return false;
    }
    params.resize(paramsNum_, 0);
    if (isResolved_) {
        params.assign(params_.begin(), params_.end());
        return true;
    }
    auto countNum = std::min(countNum_, static_cast<int32_t>(std::distance(xVals_.begin(), xVals_.end())));
    std::vector<double> xVals;
    xVals.resize(countNum, 0);
    std::vector<double> yVals;
    yVals.resize(countNum, 0);
    int32_t size = countNum - 1;
    for (auto iter = xVals_.rbegin(); iter != xVals_.rend(); iter++) {
        xVals[size] = *iter;
        size--;
        if (size < 0) {
            break;
        }
    }
    size = countNum - 1;
    for (auto iter = yVals_.rbegin(); iter != yVals_.rend(); iter++) {
        yVals[size] = *iter;
        size--;
        if (size < 0) {
            break;
        }
    }
    if (paramsNum_ == Matrix3::DIMENSION) {
        MatrixN3 matrixn3 { countNum };
        for (auto i = 0; i < countNum; i++) {
            const auto& value = xVals[i];
            matrixn3[i][2] = 1;
            matrixn3[i][1] = value;
            matrixn3[i][0] = value * value;
        }
        Matrix3 invert;
        auto transpose = matrixn3.Transpose();
        if (!(transpose * matrixn3).Invert(invert)) {
            LOGE("fail to invert");
            return false;
        }
        auto matrix3n = invert * transpose;
        auto ret = matrix3n.MapScalars(yVals, params);
        if (ret) {
            params_.assign(params.begin(), params.end());
            isResolved_ = true;
        }
        return ret;
    }
    MatrixN4 matrixn4 { countNum };
    for (auto i = 0; i < countNum; i++) {
        const auto& value = xVals[i];
        matrixn4[i][3] = 1;
        matrixn4[i][2] = value;
        matrixn4[i][1] = value * value;
        matrixn4[i][0] = value * value * value;
    }
    auto transpose = matrixn4.Transpose();
    auto inversMatrix4 = Matrix4::Invert(transpose * matrixn4);
    auto matrix4n = inversMatrix4 * transpose;
    auto ret = matrix4n.MapScalars(yVals, params);
    if (ret) {
        params_.assign(params.begin(), params.end());
        isResolved_ = true;
    }
    return ret;
}
} // namespace OHOS::Ace
