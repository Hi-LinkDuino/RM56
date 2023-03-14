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

#include "ComplexTest.h"
#include "log.h"
#include "gtest/gtest.h"

using namespace testing::ext;

class MathComplexApiTest : public testing::Test {
};

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CABS_0100
* @tc.name       test cabs api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCabs, Function | MediumTest | Level1) {
    int ret = TestCabs();
    ASSERT_EQ(ret, 0);
}
/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CABSF_0100
* @tc.name       test cabsf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCabsf, Function | MediumTest | Level1) {
    int ret = TestCabsf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CABSL_0100
* @tc.name       test cabsl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCabsl, Function | MediumTest | Level1) {
    int ret = TestCabsl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CACOS_0100
* @tc.name       test cacos api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCacos, Function | MediumTest | Level1) {
    int ret = TestCacos();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CACOSF_0100
* @tc.name       test cacosf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCacosf, Function | MediumTest | Level1) {
    int ret = TestCacosf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CACOSHF_0100
* @tc.name       test cacoshf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCacoshf, Function | MediumTest | Level1) {
    int ret = TestCacoshf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CACOSHL_0100
* @tc.name       test cacoshl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCacoshl, Function | MediumTest | Level1) {
    int ret = TestCacoshl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CACOSL_0100
* @tc.name       test cacosl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCacosl, Function | MediumTest | Level1) {
    int ret = TestCacosl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CARG_0100
* @tc.name       test carg api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCarg, Function | MediumTest | Level1) {
    int ret = TestCarg();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CARGF_0100
* @tc.name       test cargf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCargf, Function | MediumTest | Level1) {
    int ret = TestCargf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CARGL_0100
* @tc.name       test cargl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCargl, Function | MediumTest | Level1) {
    int ret = TestCargl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CASIN_0100
* @tc.name       test casin api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCasin, Function | MediumTest | Level1) {
    int ret = TestCasin();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CASINF_0100
* @tc.name       test casinf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCasinf, Function | MediumTest | Level1) {
    int ret = TestCasinf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CASINH_0100
* @tc.name       test casinh api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCasinh, Function | MediumTest | Level1) {
    int ret = TestCasinh();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CASINHF_0100
* @tc.name       test casinhf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCasinhf, Function | MediumTest | Level1) {
    int ret = TestCasinhf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CASINHL_0100
* @tc.name       test casinhl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCasinhl, Function | MediumTest | Level1) {
    int ret = TestCasinhl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CASINL_0100
* @tc.name       test casinl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCasinl, Function | MediumTest | Level1) {
    int ret = TestCasinl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CATAN_0100
* @tc.name       test catan api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCatan, Function | MediumTest | Level1) {
    int ret = TestCatan();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CATANF_0100
* @tc.name       test catanf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCatanf, Function | MediumTest | Level1) {
    int ret = TestCatanf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CATANH_0100
* @tc.name       test catanh api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCatanh, Function | MediumTest | Level1) {
    int ret = TestCatanh();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CATANHF_0100
* @tc.name       test catanhf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCatanhf, Function | MediumTest | Level1) {
    int ret = TestCatanhf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CATANHL_0100
* @tc.name       test catanhl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCatanhl, Function | MediumTest | Level1) {
    int ret = TestCatanhl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CATANL_0100
* @tc.name       test catanl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCatanl, Function | MediumTest | Level1) {
    int ret = TestCatanl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CCOS_0100
* @tc.name       test ccos api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCcos, Function | MediumTest | Level1) {
    int ret = TestCcos();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CCOSF_0100
* @tc.name       test ccosf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCcosf, Function | MediumTest | Level1) {
    int ret = TestCcosf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CCOSH_0100
* @tc.name       test ccosh api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCcosh, Function | MediumTest | Level1) {
    int ret = TestCcosh();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CCOSHF_0100
* @tc.name       test ccoshf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCcoshf, Function | MediumTest | Level1) {
    int ret = TestCcoshf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CCOSHL_0100
* @tc.name       test ccoshl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCcoshl, Function | MediumTest | Level1) {
    int ret = TestCcoshl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CCOSL_0100
* @tc.name       test ccosl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCcosl, Function | MediumTest | Level1) {
    int ret = TestCcosl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CEXP_0100
* @tc.name       test cexp api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCexp, Function | MediumTest | Level1) {
    int ret = TestCexp();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CEXPF_0100
* @tc.name       test cexpf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCexpf, Function | MediumTest | Level1) {
    int ret = TestCexpf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CEXPL_0100
* @tc.name       test cexpl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCexpl, Function | MediumTest | Level1) {
    int ret = TestCexpl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CIMAG_0100
* @tc.name       test cimag api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCimag, Function | MediumTest | Level1) {
    int ret = TestCimag();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CIMAGF_0100
* @tc.name       test cimagf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCimagf, Function | MediumTest | Level1) {
    int ret = TestCimagf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CIMAGL_0100
* @tc.name       test cimagl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCimagl, Function | MediumTest | Level1) {
    int ret = TestCimagl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CLOG_0100
* @tc.name       test clog api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestClog, Function | MediumTest | Level1) {
    int ret = TestClog();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CLOGF_0100
* @tc.name       test clogf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestClogf, Function | MediumTest | Level1) {
    int ret = TestClogf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CLOGL_0100
* @tc.name       test clogl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestClogl, Function | MediumTest | Level1) {
    int ret = TestClogl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CONJ_0100
* @tc.name       test conj api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testConj, Function | MediumTest | Level1) {
    int ret = TestConj();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CONJF_0100
* @tc.name       test conjf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testConjf, Function | MediumTest | Level1) {
    int ret = TestConjf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CONJL_0100
* @tc.name       test conjl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testConjl, Function | MediumTest | Level1) {
    int ret = TestConjl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CPOW_0100
* @tc.name       test cpow api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCpow, Function | MediumTest | Level1) {
    int ret = TestCpow();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CPOWF_0100
* @tc.name       test cpowf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCpowf, Function | MediumTest | Level1) {
    int ret = TestCpowf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CPOWL_0100
* @tc.name       test cpowl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, TestCpowl, Function | MediumTest | Level1) {
    int ret = TestCpowl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CPROJ_0100
* @tc.name       test cproj api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCproj, Function | MediumTest | Level1) {
    int ret = TestCproj();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CPROJF_0100
* @tc.name       test cprojf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCprojf, Function | MediumTest | Level1) {
    int ret = TestCprojf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CPROJL_0100
* @tc.name       test cprofl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCprojl, Function | MediumTest | Level1) {
    int ret = TestCprojl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CREAL_0100
* @tc.name       test creal api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCreal, Function | MediumTest | Level1) {
    int ret = TestCreal();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CREALF_0100
* @tc.name       test crealf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCrealf, Function | MediumTest | Level1) {
    int ret = TestCrealf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CREALL_0100
* @tc.name       test creall api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCreall, Function | MediumTest | Level1) {
    int ret = TestCreall();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSIN_0100
* @tc.name       test csin api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsin, Function | MediumTest | Level1) {
    int ret = TestCsin();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSINF_0100
* @tc.name       test csinf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsinf, Function | MediumTest | Level1) {
    int ret = TestCsinf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSINH_0100
* @tc.name       test csinh api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsinh, Function | MediumTest | Level1) {
    int ret = TestCsinh();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSINHF_0100
* @tc.name       test csinhf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsinhf, Function | MediumTest | Level1) {
    int ret = TestCsinhf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSINHL_0100
* @tc.name       test csinhl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsinhl, Function | MediumTest | Level1) {
    int ret = TestCsinhl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSINL_0100
* @tc.name       test csinl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsinl, Function | MediumTest | Level1) {
    int ret = TestCsinl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSQRT_0100
* @tc.name       test csqrt api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsqrt, Function | MediumTest | Level1) {
    int ret = TestCsqrt();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSQRTF_0100
* @tc.name       test csqrtf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsqrtf, Function | MediumTest | Level1) {
    int ret = TestCsqrtf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CSQRTL_0100
* @tc.name       test csqrtl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCsqrtl, Function | MediumTest | Level1) {
    int ret = TestCsqrtl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CTAN_0100
* @tc.name       test ctan api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCtan, Function | MediumTest | Level1) {
    int ret = TestCtan();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CTANF_0100
* @tc.name       test ctanf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCtanf, Function | MediumTest | Level1) {
    int ret = TestCtanf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CTANH_0100
* @tc.name       test ctanh api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCtanh, Function | MediumTest | Level1) {
    int ret = TestCtanh();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CTANHF_0100
* @tc.name       test ctanhf api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCtanhf, Function | MediumTest | Level1) {
    int ret = TestCtanhf();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CTANHL_0100
* @tc.name       test ctanhl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCtanhl, Function | MediumTest | Level1) {
    int ret = TestCtanhl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CTANL_0100
* @tc.name       test ctanl api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCtanl, Function | MediumTest | Level1) {
    int ret = TestCtanl();
    ASSERT_EQ(ret, 0);
}

/**
* @tc.number     SUB_KERNEL_MATH_COMPLEX_CACOSH_0100
* @tc.name       test cacosh api
* @tc.desc       [C- SOFTWARE -0100]
*/
HWTEST_F(MathComplexApiTest, testCacosh, Function | MediumTest | Level1) {
    int ret = TestCacosh();
    ASSERT_EQ(ret, 0);
}