/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
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

#include <wctype.h>

#include "gtest/gtest.h"
#include "log.h"
#include "utils.h"

using namespace testing::ext;

class ActsUtilWideCheckApiTest : public testing::Test {
public:
    locale_t g_auwcaLocale;
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
    }
    // Testcase setup
    virtual void SetUp()
    {
        g_auwcaLocale = newlocale(LC_ALL_MASK, "", (locale_t)0);
    }
    // Testcase teardown
    virtual void TearDown()
    {
        freelocale(g_auwcaLocale);
    }
};

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_0100
* @tc.name       test iswalnum api with num
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnum0100, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '2';
    returnVal = iswalnum(paraVal);
    LOGD("    iswalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_0200
* @tc.name       test iswalnum api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnum0200, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = iswalnum(paraVal);
    LOGD("    iswalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_0300
* @tc.name       test iswalnum api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnum0300, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = iswalnum(paraVal);
    LOGD("    iswalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_0400
* @tc.name       test iswalnum api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnum0400, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = iswalnum(paraVal);
    LOGD("    iswalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_0500
* @tc.name       test iswalnum api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnum0500, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iswalnum(paraVal);
    LOGD("    iswalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_L_0600
* @tc.name       test iswalnum_l api with num
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnumL0600, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '2';
    returnVal = iswalnum_l(paraVal, g_auwcaLocale);
    LOGD("    iswalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_L_0700
* @tc.name       test iswalnum_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnumL0700, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = iswalnum_l(paraVal, g_auwcaLocale);
    LOGD("    iswalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_L_0800
* @tc.name       test iswalnum_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnumL0800, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = iswalnum_l(paraVal, g_auwcaLocale);
    LOGD("    iswalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_L_0900
* @tc.name       test iswalnum_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnumL0900, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = iswalnum_l(paraVal, g_auwcaLocale);
    LOGD("    iswalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALNUM_L_1000
* @tc.name       test iswalnum_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalnumL1000, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iswalnum_l(paraVal, g_auwcaLocale);
    LOGD("    iswalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_1100
* @tc.name       test iswalpha api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalpha1100, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = iswalpha(paraVal);
    LOGD("    iswalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_1200
* @tc.name       test iswalpha api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalpha1200, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = iswalpha(paraVal);
    LOGD("    iswalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_1300
* @tc.name       test iswalpha api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalpha1300, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = iswalpha(paraVal);
    LOGD("    iswalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_1400
* @tc.name       test iswalpha api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalpha1400, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iswalpha(paraVal);
    LOGD("    iswalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_L_1500
* @tc.name       test iswalpha_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalphaL1500, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = iswalpha_l(paraVal, g_auwcaLocale);
    LOGD("    iswalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_L_1600
* @tc.name       test iswalpha_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalphaL1600, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = iswalpha_l(paraVal, g_auwcaLocale);
    LOGD("    iswalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_L_1700
* @tc.name       test iswalpha_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalphaL1700, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = iswalpha_l(paraVal, g_auwcaLocale);
    LOGD("    iswalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWALPHA_L_1800
* @tc.name       test iswalpha_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswalphaL1800, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iswalpha_l(paraVal, g_auwcaLocale);
    LOGD("    iswalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWBLANK_1900
* @tc.name       test iswblank api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswblank1900, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = iswblank(paraVal);
    LOGD("    iswblank returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswblank returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWBLANK_2000
* @tc.name       test iswblank api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswblank2000, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = iswblank(paraVal);
    LOGD("    iswblank returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswblank returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWBLANK_L_2100
* @tc.name       test iswblank_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswblankL2100, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = iswblank_l(paraVal, g_auwcaLocale);
    LOGD("    iswblank_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswblank_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWBLANK_L_2200
* @tc.name       test iswblank_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswblankL2200, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = iswblank_l(paraVal, g_auwcaLocale);
    LOGD("    iswblank_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswblank_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCNTRL_2300
* @tc.name       test iswcntrl api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswcntrl2300, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iswcntrl(paraVal);
    LOGD("    iswcntrl returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswcntrl returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCNTRL_2400
* @tc.name       test iswcntrl api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswcntrl2400, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = iswcntrl(paraVal);
    LOGD("    iswcntrl returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswcntrl returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCNTRL_L_2500
* @tc.name       test iswcntrl_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswcntrlL2500, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iswcntrl_l(paraVal, g_auwcaLocale);
    LOGD("    iswcntrl_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswcntrl_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCNTRL_L_2600
* @tc.name       test iswcntrl_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswcntrlL2600, Function | MediumTest | Level1) {
    wint_t paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = iswcntrl_l(paraVal, g_auwcaLocale);
    LOGD("    iswcntrl_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswcntrl_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCTYPE_2700
* @tc.name       test iswctype api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswctype2700, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctype_t paraDesc;
    int returnVal;

    wideChar = 'A';
    paraDesc = wctype("alnum");
    returnVal = iswctype(wideChar, paraDesc);
    LOGD("    iswctype returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswctype returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCTYPE_2800
* @tc.name       test iswctype api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswctype2800, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctype_t paraDesc;
    int returnVal;

    wideChar = '3';
    paraDesc = wctype("alnum");
    returnVal = iswctype(wideChar, paraDesc);
    LOGD("    iswctype returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswctype returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCTYPE_L_2900
* @tc.name       test iswctype_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswctypeL2900, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctype_t paraDesc;
    int returnVal;

    wideChar = 'A';
    paraDesc = wctype("alnum");
    returnVal = iswctype_l(wideChar, paraDesc, g_auwcaLocale);
    LOGD("    iswctype_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswctype_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWCTYPE_L_3000
* @tc.name       test iswctype_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswctypeL3000, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctype_t paraDesc;
    int returnVal;

    wideChar = '3';
    paraDesc = wctype("alnum");
    returnVal = iswctype_l(wideChar, paraDesc, g_auwcaLocale);
    LOGD("    iswctype_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswctype_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWDIGIT_3100
* @tc.name       test iswdigit api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswdigit3100, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '3';
    returnVal = iswdigit(wideChar);
    LOGD("    iswdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswdigit returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWDIGIT_3200
* @tc.name       test iswdigit api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswdigit3200, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswdigit(wideChar);
    LOGD("    iswdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswdigit returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWDIGIT_L_3300
* @tc.name       test iswdigit_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswdigitL3300, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '3';
    returnVal = iswdigit_l(wideChar, g_auwcaLocale);
    LOGD("    iswdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswdigit_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWDIGIT_L_3400
* @tc.name       test iswdigit api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswdigitL3400, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswdigit_l(wideChar, g_auwcaLocale);
    LOGD("    iswdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswdigit_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWGRAPH_3500
* @tc.name       test iswgraph api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswgraph3500, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswgraph(wideChar);
    LOGD("    iswgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWGRAPH_3600
* @tc.name       test iswgraph api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswgraph3600, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswgraph(wideChar);
    LOGD("    iswgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWGRAPH_L_3700
* @tc.name       test iswgraph_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswgraphL3700, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswgraph_l(wideChar, g_auwcaLocale);
    LOGD("    iswgraph_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswgraph_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWGRAPH_L_3800
* @tc.name       test iswgraph_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswgraphL3800, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswgraph_l(wideChar, g_auwcaLocale);
    LOGD("    iswgraph_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswgraph_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWLOWER_3900
* @tc.name       test iswlower api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswlower3900, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswlower(wideChar);
    LOGD("    iswlower c:='%c',   returnVal:='%c'\n", wideChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswlower c:='" << wideChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWLOWER_4000
* @tc.name       test islower api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswlower4000, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswlower(wideChar);
    LOGD("    iswlower c:='%c',   returnVal:='%c'\n", wideChar, returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswlower c:='" << wideChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWLOWER_4100
* @tc.name       test islower api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswlower4100, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '5';
    returnVal = iswlower(wideChar);
    LOGD("    iswlower c:='%c',   returnVal:='%c'\n", wideChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswlower c:='" << wideChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWLOWER_L_4200
* @tc.name       test iswlower_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswlowerL4200, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswlower_l(wideChar, g_auwcaLocale);
    LOGD("    iswlower_l c:='%c',   returnVal:='%c'\n", wideChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswlower_l c:='" << wideChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWLOWER_L_4300
* @tc.name       test iswlower_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswlowerL4300, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswlower_l(wideChar, g_auwcaLocale);
    LOGD("    iswlower_l c:='%c',   returnVal:='%c'\n", wideChar, returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswlower_l c:='" << wideChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWLOWER_L_4400
* @tc.name       test iswlower_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswlowerL4400, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '5';
    returnVal = iswlower_l(wideChar, g_auwcaLocale);
    LOGD("    iswlower_l c:='%c',   returnVal:='%c'\n", wideChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswlower_l c:='" << wideChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPRINT_4500
* @tc.name       test iswprint api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswprint4500, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswprint(wideChar);
    LOGD("    iswprint returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswprint  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPRINT_4600
* @tc.name       test iswprint api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswprint4600, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswprint(wideChar);
    LOGD("    iswprint returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswprint  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPRINT_L_4700
* @tc.name       test iswprint_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswprintL4700, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswprint_l(wideChar, g_auwcaLocale);
    LOGD("    iswprint_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswprint_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPRINT_L_4800
* @tc.name       test iswprint_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswprintL4800, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswprint_l(wideChar, g_auwcaLocale);
    LOGD("    iswprint_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswprint_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_4900
* @tc.name       test iswpunct api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunct4900, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = ' ';
    returnVal = iswpunct(wideChar);
    LOGD("    iswpunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_5000
* @tc.name       test iswpunct api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunct5000, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswpunct(wideChar);
    LOGD("    iswpunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_5100
* @tc.name       test iswpunct api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunct5100, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '3';
    returnVal = iswpunct(wideChar);
    LOGD("    iswpunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_5200
* @tc.name       test iswpunct api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunct5200, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswpunct(wideChar);
    LOGD("    iswpunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_L_5300
* @tc.name       test iswpunct_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunctL5300, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = ' ';
    returnVal = iswpunct_l(wideChar, g_auwcaLocale);
    LOGD("    iswpunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_L_5400
* @tc.name       test iswpunct_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunctL5400, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswpunct_l(wideChar, g_auwcaLocale);
    LOGD("    iswpunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_L_5500
* @tc.name       test iswpunct_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunctL5500, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '3';
    returnVal = iswpunct_l(wideChar, g_auwcaLocale);
    LOGD("    iswpunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWPUNCT_L_5600
* @tc.name       test iswpunct_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswpunctL5600, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswpunct_l(wideChar, g_auwcaLocale);
    LOGD("    iswpunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswpunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_5700
* @tc.name       test iswspace api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace5700, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_5800
* @tc.name       test iswspace api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace5800, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = ' ';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_5900
* @tc.name       test iswspace api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace5900, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_6000
* @tc.name       test iswspace api with CR
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace6000, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\r';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_6100
* @tc.name       test iswspace api with form-feed
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace6100, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\f';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_6200
* @tc.name       test iswspace api with horizontal tab
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace6200, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\t';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_6300
* @tc.name       test iswspace api with vertical tab
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspace6300, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\v';
    returnVal = iswspace(wideChar);
    LOGD("    iswspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_6400
* @tc.name       test iswspace_l api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL6400, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_6500
* @tc.name       test iswspace_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL6500, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = ' ';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_6600
* @tc.name       test iswspace_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL6600, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_6700
* @tc.name       test iswspace_l api with CR
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL6700, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\r';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_6800
* @tc.name       test iswspace_l api with form-feed
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL6800, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\f';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_6900
* @tc.name       test iswspace_l api with horizontal tab
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL6900, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\t';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWSPACE_L_7000
* @tc.name       test iswspace_l api with vertical tab
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswspaceL7000, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\v';
    returnVal = iswspace_l(wideChar, g_auwcaLocale);
    LOGD("    iswspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_7100
* @tc.name       test iswupper api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupper7100, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswupper(wideChar);
    LOGD("    iswupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_7200
* @tc.name       test iswupper api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupper7200, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswupper(wideChar);
    LOGD("    iswupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_7300
* @tc.name       test iswupper api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupper7300, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '5';
    returnVal = iswupper(wideChar);
    LOGD("    iswupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_7400
* @tc.name       test iswupper api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupper7400, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswupper(wideChar);
    LOGD("    iswupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_L_7500
* @tc.name       test iswupper_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupperL7500, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'A';
    returnVal = iswupper_l(wideChar, g_auwcaLocale);
    LOGD("    iswupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_L_7600
* @tc.name       test iswupper_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupperL7600, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'a';
    returnVal = iswupper_l(wideChar, g_auwcaLocale);
    LOGD("    iswupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_L_7700
* @tc.name       test iswupper_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupperL7700, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '5';
    returnVal = iswupper_l(wideChar, g_auwcaLocale);
    LOGD("    iswupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWUPPER_L_7800
* @tc.name       test iswupper_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswupperL7800, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = '\n';
    returnVal = iswupper_l(wideChar, g_auwcaLocale);
    LOGD("    iswupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWXDIGIT_7900
* @tc.name       test iswxdigit api with xdigit F
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswxdigit7900, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'F';
    returnVal = iswxdigit(wideChar);
    LOGD("    iswxdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswxdigit  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWXDIGIT_8000
* @tc.name       test iswxdigit api with alpha G
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswxdigit8000, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'G';
    returnVal = iswxdigit(wideChar);
    LOGD("    iswxdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswxdigit  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWXDIGIT_L_8100
* @tc.name       test iswxdigit_l api with xdigit F
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswxdigitL8100, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'F';
    returnVal = iswxdigit_l(wideChar, g_auwcaLocale);
    LOGD("    iswxdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iswxdigit_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_WIDECHECK_ISWXDIGIT_L_8200
* @tc.name       test iswxdigit_l api with alpha G
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilWideCheckApiTest, testIswxdigitL8200, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = 'G';
    returnVal = iswxdigit_l(wideChar, g_auwcaLocale);
    LOGD("    iswxdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iswxdigit_l  returnVal:='" << returnVal << "'";
}