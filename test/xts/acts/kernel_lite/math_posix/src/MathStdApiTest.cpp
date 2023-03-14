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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "log.h"
#include "gtest/gtest.h"
#include "inttypes.h"

using namespace testing::ext;

class MathStdApiTest : public testing::Test {
};

/**
* @tc.number     SUB_KERNEL_MATH_STD_STDLIB_0100
* @tc.name       test abs api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathStdApiTest, testAbs, Function | MediumTest | Level1) {

    const int testCount = 3;
    int testValues[] = {-3, 0, 3};
    int expected[] = {3, 0, 3};
    int ret;
    for (int i = 0; i < testCount; ++i) {
        ret = abs(testValues[i]);
        EXPECT_EQ(ret, expected[i]) << "abs failed";
    }
}

/**
* @tc.number     SUB_KERNEL_MATH_STD_DIV_0100
* @tc.name       test div api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathStdApiTest, testDiv, Function | MediumTest | Level1) {

    const int testCount = 3;
    int numer[]={11, -11, 0};
    int denom[]={2, 2, -1};
    int aquot[] = {5, -5, 0};
    int arem[] = {1, -1, 0};

    div_t ret;
    for (int i = 0; i < testCount; ++i) {
        ret = div(numer[i], denom[i]);
        ASSERT_TRUE(ret.quot == aquot[i] && ret.rem == arem[i]) << " div failed";
    }
}

/**
* @tc.number SUB_KERNEL_MATH_STD_IMAXABS_0100
* @tc.name test imaxabs api
* @tc.desc [C- SOFTWARE -0100]
**/
HWTEST_F(MathStdApiTest, testimaxabs, Function | MediumTest | Level1) {

    const int testCount = 3;
    intmax_t testValues[] = {3765, -1234, 0};
    intmax_t expected[] = {3765, 1234, 0};

    intmax_t ret;
    for (int i = 0; i < testCount; ++i) {
        ret = imaxabs(testValues[i]);
        EXPECT_EQ(ret, expected[i]) << " imaxabs failed";
    }
}

/**
* @tc.number SUB_KERNEL_MATH_STD_IMAXDIV_0100
* @tc.name test imaxdiv api
* @tc.desc [C- SOFTWARE -0100]
**/
HWTEST_F(MathStdApiTest, testimaxdiv, Function | MediumTest | Level1) {

    const int testCount = 4;
    intmax_t numerator[] = {2000000000, 2000000000, -2000000001, -2000000001};
    intmax_t denominator[] = {2, -2, 2, -2};
    intmax_t expected[] = {1000000000, -1000000000, -1000000000, 1000000000};

    imaxdiv_t ret;
    for (int i = 0; i < testCount; ++i) {
        ret = imaxdiv(numerator[i], denominator[i]);
        ASSERT_TRUE(ret.quot == expected[i]) << " imaxdiv failed";
    }
}

/**
* @tc.number SUB_KERNEL_MATH_STD_LABS_0100
* @tc.name test labs api
* @tc.desc [C- SOFTWARE -0100]
**/
HWTEST_F(MathStdApiTest, testlabs, Function | MediumTest | Level1) {

    const int testCount = 3;
    long testValues[] = {214748364, -214748364, 0};
    long expected[] = {214748364, 214748364, 0};

    long ret;
    for (int i = 0; i < testCount; ++i) {
        ret = labs(testValues[i]);
        EXPECT_EQ(ret, expected[i]) << " labs failed";
    }
}

/**
* @tc.number SUB_KERNEL_MATH_STD_LDIV_0100
* @tc.name test ldiv api
* @tc.desc [C- SOFTWARE -0100]
**/
HWTEST_F(MathStdApiTest, testldiv, Function | MediumTest | Level1) {

    const int testCount = 4;
    long numer[] = {20000000, 20000000, -20000001, -20000001};
    long denom[] = {2, -2, 2, -2};
    long aquot[] = {10000000, -10000000, -10000000, 10000000};
    long arem[] = {0, 0, -1, -1};

    ldiv_t ret;
    for (int i = 0; i < testCount; ++i) {
        ret = ldiv(numer[i], denom[i]);
        ASSERT_TRUE(ret.quot == aquot[i] && ret.rem == arem[i]) << " ldiv failed";
    }
}

/**
* @tc.number SUB_KERNEL_MATH_STD_LLABS_0100
* @tc.name test llabs api
* @tc.desc [C- SOFTWARE -0100]
**/
HWTEST_F(MathStdApiTest, testllabs, Function | MediumTest | Level1) {

    const int testCount = 3;
    intmax_t testValues[] = {2147483649, -2147483649, 0};
    intmax_t expected[] = {2147483649, 2147483649, 0};

    float ret;
    for (int i = 0; i < testCount; ++i) {
        ret = llabs(testValues[i]);
        EXPECT_EQ(ret, expected[i]) << " llabs failed";
    }
}

/**
* @tc.number SUB_KERNEL_MATH_STD_LLDIV_0100
* @tc.name test lldiv api
* @tc.desc [C- SOFTWARE -0100]
**/
HWTEST_F(MathStdApiTest, testlldiv, Function | MediumTest | Level1) {

    const int testCount = 4;
    long long numer[] = {20000000000, 20000000000, -20000000001, -20000000001};
    long long denom[] = {2, -2, 2, -2};
    long long aquot[] = {10000000000, -10000000000, -10000000000, 10000000000};
    long long arem[] = {0, 0, -1, -1};

    lldiv_t ret;
    for (int i = 0; i < testCount; ++i) {
        ret = lldiv(numer[i], denom[i]);
        ASSERT_TRUE(ret.quot == aquot[i] && ret.rem == arem[i]) << " lldiv failed";
    }
}
