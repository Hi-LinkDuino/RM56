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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_LEAST_SQUARE_IMPL_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_LEAST_SQUARE_IMPL_H

#include <vector>

#include "base/utils/macros.h"

namespace OHOS::Ace {
/**
 * @brief Least square method of four parametres.
 * the function template is a3 * x^3 + a2 * x^2 + a1 * x + a0 = y with four;
 * the function template is 0 * x^3 + a2 * x^2 + a1 * x + a0 = y with three.
 */
class ACE_EXPORT LeastSquareImpl {
public:
    /**
     * @brief Construct a new Least Square Impl object.
     * @param paramsNum the right number is 4 or 3.
     */
    explicit LeastSquareImpl(int32_t paramsNum) : paramsNum_(paramsNum) {}

    /**
     * @brief Construct a new Least Square Impl object.
     * @param paramsNum the right number is 4 or 3.
     */
    LeastSquareImpl(int32_t paramsNum, int32_t countNum) : paramsNum_(paramsNum), countNum_(countNum) {}

    LeastSquareImpl() = default;
    ~LeastSquareImpl() = default;

    void UpdatePoint(double xVal, double yVal)
    {
        isResolved_ = false;
        xVals_.emplace_back(xVal);
        yVals_.emplace_back(yVal);
    }

    /**
     * @brief Set the Count Num which to compute.
     *
     * @param countNum the compute number.
     */
    void SetCountNum(int32_t countNum)
    {
        countNum_ = countNum;
    }

    /**
     * @brief Get the Least Square Params object
     *
     * @param params the four values of vector.
     * @return true get the least square result.
     * @return false failed to get the least square result.
     */
    bool GetLeastSquareParams(std::vector<double>& params);

    inline const std::vector<double>& GetXVals() const
    {
        return xVals_;
    }

    inline const std::vector<double>& GetYVals() const
    {
        return yVals_;
    }

    inline int32_t GetTrackNum() const
    {
        return xVals_.size();
    }

    void Reset()
    {
        xVals_.clear();
        yVals_.clear();
        params_.clear();
        isResolved_ = false;
    }

private:
    std::vector<double> xVals_;
    std::vector<double> yVals_;
    std::vector<double> params_;
    int32_t paramsNum_ = 4;
    int32_t countNum_ = 4;
    bool isResolved_ = false;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_LEAST_SQUARE_IMPL_H
