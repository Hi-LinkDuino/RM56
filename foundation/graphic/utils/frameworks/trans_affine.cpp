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
#include "gfx_utils/trans_affine.h"

namespace OHOS {
const uint8_t PARL_INDEX_SIZE = 6;
const TransAffine& TransAffine::ParlToParl(const float* src,
                                           const float* dst)
{
    data_[0] = src[2] - src[0];
    data_[3] = src[3] - src[1];
    data_[1] = src[4] - src[0];
    data_[4] = src[5] - src[1];
    data_[2] = src[0];
    data_[5] = src[1];
    Invert();
    Multiply(TransAffine(dst[2] - dst[0], dst[3] - dst[1],
                         dst[4] - dst[0], dst[5] - dst[1],
                         dst[0], dst[1]));
    return *this;
}

const TransAffine& TransAffine::RectToParl(float x1, float y1,
                                           float x2, float y2,
                                           const float* parl)
{
    float src[PARL_INDEX_SIZE];
    src[0] = x1;
    src[1] = y1;
    src[2] = x2;
    src[3] = y1;
    src[4] = x2;
    src[5] = y2;
    ParlToParl(src, parl);
    return *this;
}

const TransAffine& TransAffine::Multiply(const TransAffine& metrix)
{
    float t0 = data_[0] * metrix.data_[0] + data_[3] * metrix.data_[1];
    float t2 = data_[1] * metrix.data_[0] + data_[4] * metrix.data_[1];
    float t4 = data_[2] * metrix.data_[0] + data_[5] * metrix.data_[1] + metrix.data_[2];
    data_[3] = data_[0] * metrix.data_[3] + data_[3] * metrix.data_[4];
    data_[4] = data_[1] * metrix.data_[3] + data_[4] * metrix.data_[4];
    data_[5] = data_[2] * metrix.data_[3] + data_[5] * metrix.data_[4] + metrix.data_[5];
    data_[0] = t0;
    data_[1] = t2;
    data_[2] = t4;
    return *this;
}

const TransAffine& TransAffine::Invert()
{
    float d = DeterminantReciprocal();

    float t0 = data_[4] * d;
    data_[4] = data_[0] * d;
    data_[3] = -data_[3] * d;
    data_[1] = -data_[1] * d;

    float t4 = -data_[2] * t0 - data_[5] * data_[1];
    data_[5] = -data_[2] * data_[3] - data_[5] * data_[4];

    data_[0] = t0;
    data_[2] = t4;
    return *this;
}

const TransAffine& TransAffine::Reset()
{
    data_[1] = 0;
    data_[2] = 0;
    data_[3] = 0;
    data_[5] = 0;
    data_[6] = 0;
    data_[7] = 0;
    data_[0] = 1;
    data_[4] = 1;
    data_[8] = 1;
    return *this;
}

bool TransAffine::IsIdentity() const
{
    return MATH_FLT_EQUAL(data_[0], 1.0f) &&
           MATH_FLT_EQUAL(data_[3], 0.0f) &&
           MATH_FLT_EQUAL(data_[1], 0.0f) &&
           MATH_FLT_EQUAL(data_[4], 1.0f) &&
           MATH_FLT_EQUAL(data_[2], 0.0f) &&
           MATH_FLT_EQUAL(data_[5], 0.0f);
}

bool TransAffine::IsValid(float epsilon) const
{
    return (MATH_ABS(data_[0]) > epsilon) && (MATH_ABS(data_[4]) > epsilon);
}
} // namespace OHOS
