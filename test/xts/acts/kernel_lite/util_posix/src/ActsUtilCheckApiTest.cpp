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

#include <ctype.h>
#include <math.h>
#include <stropts.h>

#include "gtest/gtest.h"
#include "log.h"
#include "utils.h"

using namespace testing::ext;

class ActsUtilCheckApiTest : public testing::Test {
public:
    locale_t g_aucaLocale;
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
        g_aucaLocale = newlocale(LC_ALL_MASK, "", (locale_t)0);
    }
    // Testcase teardown
    virtual void TearDown()
    {
        freelocale(g_aucaLocale);
    }
};

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_0100
* @tc.name       test isalnum api with num
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnum0100, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '2';
    returnVal = isalnum(paraVal);
    LOGD("    isalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_0200
* @tc.name       test isalnum api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnum0200, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isalnum(paraVal);
    LOGD("    isalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_0300
* @tc.name       test isalnum api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnum0300, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = isalnum(paraVal);
    LOGD("    isalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_0400
* @tc.name       test isalnum api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnum0400, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isalnum(paraVal);
    LOGD("    isalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_0500
* @tc.name       test isalnum api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnum0500, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isalnum(paraVal);
    LOGD("    isalnum returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalnum returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_L_0600
* @tc.name       test isalnum_l api with num
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnumL0600, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '2';
    returnVal = isalnum_l(paraVal, g_aucaLocale);
    LOGD("    isalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_L_0700
* @tc.name       test isalnum_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnumL0700, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isalnum_l(paraVal, g_aucaLocale);
    LOGD("    isalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_L_0800
* @tc.name       test isalnum_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnumL0800, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = isalnum_l(paraVal, g_aucaLocale);
    LOGD("    isalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_L_0900
* @tc.name       test isalnum_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnumL0900, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isalnum_l(paraVal, g_aucaLocale);
    LOGD("    isalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALNUM_L_1000
* @tc.name       test isalnum_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalnumL1000, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isalnum_l(paraVal, g_aucaLocale);
    LOGD("    isalnum_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalnum_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_1100
* @tc.name       test isalpha api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalpha1100, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isalpha(paraVal);
    LOGD("    isalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_1200
* @tc.name       test isalpha api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalpha1200, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = isalpha(paraVal);
    LOGD("    isalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_1300
* @tc.name       test isalpha api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalpha1300, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isalpha(paraVal);
    LOGD("    isalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_1400
* @tc.name       test isalpha api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalpha1400, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isalpha(paraVal);
    LOGD("    isalpha returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalpha returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_L_1500
* @tc.name       test isalpha_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalphaL1500, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isalpha_l(paraVal, g_aucaLocale);
    LOGD("    isalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_L_1600
* @tc.name       test isalpha_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalphaL1600, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = isalpha_l(paraVal, g_aucaLocale);
    LOGD("    isalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_L_1700
* @tc.name       test isalpha_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalphaL1700, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isalpha_l(paraVal, g_aucaLocale);
    LOGD("    isalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISALPHA_L_1800
* @tc.name       test isalpha_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsalphaL1800, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isalpha_l(paraVal, g_aucaLocale);
    LOGD("    isalpha_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isalpha_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISASCII_1900
* @tc.name       test isascii api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsascii1900, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isascii(paraVal);
    LOGD("    isascii returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isascii returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISASCII_2000
* @tc.name       test isascii api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsascii2000, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'Z';
    returnVal = isascii(paraVal);
    LOGD("    isascii returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isascii returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISASCII_2100
* @tc.name       test isascii api with special symbol
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsascii2100, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '~';
    returnVal = isascii(paraVal);
    LOGD("    isascii returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isascii returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISASCII_2200
* @tc.name       test isascii api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsascii2200, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 128;
    returnVal = isascii(paraVal);
    LOGD("    isascii returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isascii returnVal:='" << returnVal << "'";
}

// /**
// * @tc.number     SUB_KERNEL_UTIL_CHECK_ISASTREAM_2300
// * @tc.name       test isastream api with digit
// * @tc.desc       [C- SOFTWARE -0200]
// */
// HWTEST_F(ActsUtilCheckApiTest, testIsastream2300, Function | MediumTest | Level1) {
//     int paraVal;
//     int returnVal;

//     paraVal = 128;
//     returnVal = isastream(paraVal);
//     LOGD("    isastream returnVal:='%d'\n", returnVal);
//     ASSERT_TRUE(-1 == returnVal) << "ErrInfo: isastream returnVal:='" << returnVal << "'";
// }

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISATTY_2400
* @tc.name       test isatty api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsatty2400, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 128;
    returnVal = isatty(paraVal);
    LOGD("    isatty returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isatty returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISBLANK_2500
* @tc.name       test isblank api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsblank2500, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isblank(paraVal);
    LOGD("    isblank returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isblank returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISBLANK_2600
* @tc.name       test isblank api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsblank2600, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = isblank(paraVal);
    LOGD("    isblank returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isblank returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISBLANK_L_2700
* @tc.name       test isblank_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsblankL2700, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isblank_l(paraVal, g_aucaLocale);
    LOGD("    isblank_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isblank_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISBLANK_L_2800
* @tc.name       test isblank_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsblankL2800, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = isblank_l(paraVal, g_aucaLocale);
    LOGD("    isblank_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isblank_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISCNTRL_2900
* @tc.name       test iscntrl api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIscntrl2900, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iscntrl(paraVal);
    LOGD("    iscntrl returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iscntrl returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISCNTRL_3000
* @tc.name       test iscntrl api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIscntrl3000, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = iscntrl(paraVal);
    LOGD("    iscntrl returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iscntrl returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISCNTRL_L_3100
* @tc.name       test iscntrl_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIscntrlL3100, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = iscntrl_l(paraVal, g_aucaLocale);
    LOGD("    iscntrl_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: iscntrl_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISCNTRL_L_3200
* @tc.name       test iscntrl_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIscntrlL3200, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = iscntrl_l(paraVal, g_aucaLocale);
    LOGD("    iscntrl_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: iscntrl_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_3300
* @tc.name       test isdigit api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigit3300, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '3';
    returnVal = isdigit(paraVal);
    LOGD("    isdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isdigit returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_3400
* @tc.name       test isdigit api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigitl3400, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isdigit(paraVal);
    LOGD("    isdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isdigit returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_3500
* @tc.name       test isdigit api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigitl3500, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isdigit(paraVal);
    LOGD("    isdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isdigit returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_3600
* @tc.name       test isdigit api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigit3600, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isdigit(paraVal);
    LOGD("    isdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isdigit returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_L_3700
* @tc.name       test isdigit_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigitL3700, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '3';
    returnVal = isdigit_l(paraVal, g_aucaLocale);
    LOGD("    isdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isdigit_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_L_3800
* @tc.name       test isdigit_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigitlL3800, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isdigit_l(paraVal, g_aucaLocale);
    LOGD("    isdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isdigit_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_L_3900
* @tc.name       test isdigit_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigitlL3900, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isdigit_l(paraVal, g_aucaLocale);
    LOGD("    isdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isdigit_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISDIGIT_L_4000
* @tc.name       test isdigit_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsdigitL4000, Function | MediumTest | Level1) {
    int paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isdigit_l(paraVal, g_aucaLocale);
    LOGD("    isdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isdigit_l returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISFINITE_4100
* @tc.name       test isfinite api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsfinite4100, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isfinite(paraVal);
    LOGD("    isfinite returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isfinite returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISFINITE_4200
* @tc.name       test isfinite api with decimal double
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsfinite4200, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 3.1415926;
    returnVal = isfinite(paraVal);
    LOGD("    isfinite returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isfinite returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISFINITE_4300
* @tc.name       test isfinite api with exponent double
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsfinite4300, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 1.26e3;
    returnVal = isfinite(paraVal);
    LOGD("    isfinite returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isfinite returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_4400
* @tc.name       test isgraph api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraph4400, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = isgraph(paraVal);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_4500
* @tc.name       test isgraph api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraph4500, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isgraph(paraVal);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_4600
* @tc.name       test isgraph api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraph4600, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isgraph(paraVal);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_4700
* @tc.name       test isgraph api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraph4700, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isgraph(paraVal);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_L_4800
* @tc.name       test isgraph_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraphL4800, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = isgraph_l(paraVal, g_aucaLocale);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_L_4900
* @tc.name       test isgraph_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraphL4900, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 'z';
    returnVal = isgraph_l(paraVal, g_aucaLocale);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_L_5000
* @tc.name       test isgraph_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraphL5000, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isgraph_l(paraVal, g_aucaLocale);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGRAPH_L_5100
* @tc.name       test isgraph_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgraphL5100, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isgraph_l(paraVal, g_aucaLocale);
    LOGD("    isgraph returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgraph returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGREATER_5200
* @tc.name       test isgreater api para1 is less than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgreater5200, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 1.1;
    paraVal2 = 2.1;
    returnVal = isgreater(paraVal1, paraVal2);
    LOGD("    isgreater returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgreater returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGREATER_5300
* @tc.name       test isgreater api para1 is greater than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgreater5300, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 1.1;
    returnVal = isgreater(paraVal1, paraVal2);
    LOGD("    isgreater returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgreater returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGREATER_5400
* @tc.name       test isgreater api para1 is equal to para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgreater5400, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 2.1;
    returnVal = isgreater(paraVal1, paraVal2);
    LOGD("    isgreater returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgreater returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGREATEREQUAL_5500
* @tc.name       test isgreaterequal api para1 is less than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgreaterequal5500, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 1.1;
    paraVal2 = 2.1;
    returnVal = isgreaterequal(paraVal1, paraVal2);
    LOGD("    isgreaterequal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isgreaterequal returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGREATEREQUAL_5600
* @tc.name       test isgreaterequal api para1 is greater than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgreaterequal5600, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 1.1;
    returnVal = isgreaterequal(paraVal1, paraVal2);
    LOGD("    isgreaterequal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgreaterequal returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISGREATEREQUAL_5700
* @tc.name       test isgreaterequal api para1 is equal to para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsgreaterequal5700, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 2.1;
    returnVal = isgreaterequal(paraVal1, paraVal2);
    LOGD("    isgreaterequal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isgreaterequal returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISINF_5800
* @tc.name       test isinf api with INFINITY
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsinf5800, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = INFINITY;
    returnVal = isinf(paraVal);
    LOGD("    isinf returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(1 == returnVal) << "ErrInfo: isinf  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLESSEQUAL_5900
* @tc.name       test islessequal api para1 is less than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslessequal5900, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 1.1;
    paraVal2 = 2.1;
    returnVal = islessequal(paraVal1, paraVal2);
    LOGD("    islessequal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 != returnVal) << "ErrInfo: islessequal returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLESSEQUAL_6000
* @tc.name       test islessequal api para1 is greater than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslessequal6000, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 1.1;
    returnVal = islessequal(paraVal1, paraVal2);
    LOGD("    islessequal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islessequal returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLESSEQUAL_6100
* @tc.name       test islessequal api para1 is equal to para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslessequal6100, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 2.1;
    returnVal = islessequal(paraVal1, paraVal2);
    LOGD("    islessequal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: islessequal returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLESSGREATER_6200
* @tc.name       test islessgreater api para1 is equal to para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslessgreater6200, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 2.1;
    paraVal2 = 2.1;
    returnVal = islessgreater(paraVal1, paraVal2);
    LOGD("    islessgreater returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islessgreater returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLESSGREATER_6300
* @tc.name       test islessgreater api para1 is less than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslessgreater6300, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 1.1;
    paraVal2 = 2.1;
    returnVal = islessgreater(paraVal1, paraVal2);
    LOGD("    islessgreater returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: islessgreater returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLESSGREATER_6400
* @tc.name       test islessgreater api para1 is greater than para2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslessgreater6400, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = 3.1;
    paraVal2 = 2.1;
    returnVal = islessgreater(paraVal1, paraVal2);
    LOGD("    islessgreater returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: islessgreater returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_6500
* @tc.name       test islower api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslower6500, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'A';
    returnVal = islower(paraChar);
    LOGD("    islower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islower c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_6600
* @tc.name       test islower api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslower6600, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'a';
    returnVal = islower(paraChar);
    LOGD("    islower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: islower c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_6700
* @tc.name       test islower api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslower6700, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = '5';
    returnVal = islower(paraChar);
    LOGD("    islower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islower c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_6800
* @tc.name       test islower api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslower6800, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = ' ';
    returnVal = islower(paraChar);
    LOGD("    islower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islower c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_L_6900
* @tc.name       test islower_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslowerL6900, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'A';
    returnVal = islower_l(paraChar, g_aucaLocale);
    LOGD("    islower_l c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islower_l c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_L_7000
* @tc.name       test islower_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslowerL7000, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'a';
    returnVal = islower_l(paraChar, g_aucaLocale);
    LOGD("    islower_l c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: islower_l c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_L_7100
* @tc.name       test islower_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslowerL7100, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = '5';
    returnVal = islower_l(paraChar, g_aucaLocale);
    LOGD("    islower_l c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islower_l c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISLOWER_L_7200
* @tc.name       test islower_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIslowerL7200, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = ' ';
    returnVal = islower_l(paraChar, g_aucaLocale);
    LOGD("    islower_l c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: islower_l c:='" << paraChar << "',   returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISNAN_7300
* @tc.name       test isnan api with NAN
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsnan7300, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = NAN;
    returnVal = isnan(paraVal);
    LOGD("    isnan returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isnan  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISNORMAL_7400
* @tc.name       test isnormal api with FP_NORMAL
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsnormal7400, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = FP_NORMAL;
    returnVal = isnormal(paraVal);
    LOGD("    isnormal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isnormal  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISNORMAL_7500
* @tc.name       test isnormal api with NAN
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsnormal7500, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = NAN;
    returnVal = isnormal(paraVal);
    LOGD("    isnormal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isnormal  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISNORMAL_7600
* @tc.name       test isnormal api with double
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsnormal7600, Function | MediumTest | Level1) {
    double paraVal;
    int returnVal;

    paraVal = 2.1;
    returnVal = isnormal(paraVal);
    LOGD("    isnormal returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isnormal  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPRINT_7700
* @tc.name       test isprint api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsprint7700, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isprint(paraVal);
    LOGD("    isprint returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isprint  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPRINT_7800
* @tc.name       test isprint api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsprint7800, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isprint(paraVal);
    LOGD("    isprint returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isprint  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPRINT_7900
* @tc.name       test isprint api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsprint7900, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isprint(paraVal);
    LOGD("    isprint returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isprint  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPRINT_L_8000
* @tc.name       test isprint_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsprintL8000, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isprint_l(paraVal, g_aucaLocale);
    LOGD("    isprint_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isprint_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPRINT_L_8100
* @tc.name       test isprint_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsprintL8100, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isprint_l(paraVal, g_aucaLocale);
    LOGD("    isprint_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isprint_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPRINT_L_8200
* @tc.name       test isprint_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsprintL8200, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isprint_l(paraVal, g_aucaLocale);
    LOGD("    isprint_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isprint_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPUNCT_8300
* @tc.name       test ispunct api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIspunct8300, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = ispunct(paraVal);
    LOGD("    ispunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: ispunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPUNCT_8400
* @tc.name       test ispunct api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIspunct8400, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = ispunct(paraVal);
    LOGD("    ispunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: ispunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPUNCT_8500
* @tc.name       test ispunct api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIspunct8500, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = ispunct(paraVal);
    LOGD("    ispunct returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: ispunct  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPUNCT_L_8600
* @tc.name       test ispunct_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIspunctL8600, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = ispunct_l(paraVal, g_aucaLocale);
    LOGD("    ispunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: ispunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPUNCT_L_8700
* @tc.name       test ispunct_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIspunctL8700, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = ispunct_l(paraVal, g_aucaLocale);
    LOGD("    ispunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: ispunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISPUNCT_L_8800
* @tc.name       test ispunct_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIspunctL8800, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = ispunct_l(paraVal, g_aucaLocale);
    LOGD("    ispunct_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: ispunct_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_8900
* @tc.name       test isspace api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspace8900, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isspace(paraVal);
    LOGD("    isspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_9000
* @tc.name       test isspace api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspace9000, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isspace(paraVal);
    LOGD("    isspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_9100
* @tc.name       test isspace api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspace9100, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isspace(paraVal);
    LOGD("    isspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_9200
* @tc.name       test isspace api with CR
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspace9200, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\r';
    returnVal = isspace(paraVal);
    LOGD("    isspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_9300
* @tc.name       test isspace api with form-feed
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspace9300, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\f';
    returnVal = isspace(paraVal);
    LOGD("    isspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_9400
* @tc.name       test isspace api with horizontal tab
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspace9400, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\t';
    returnVal = isspace(paraVal);
    LOGD("    isspace returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_L_9500
* @tc.name       test isspace_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspaceL9500, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isspace_l(paraVal, g_aucaLocale);
    LOGD("    isspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_L_9600
* @tc.name       test isspace_l api with space
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspaceL9600, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = ' ';
    returnVal = isspace_l(paraVal, g_aucaLocale);
    LOGD("    isspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_L_9620
* @tc.name       test isspace_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspaceL9620, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isspace_l(paraVal, g_aucaLocale);
    LOGD("    isspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_L_9640
* @tc.name       test isspace_l api with CR
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspaceL9640, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\r';
    returnVal = isspace_l(paraVal, g_aucaLocale);
    LOGD("    isspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_L_9660
* @tc.name       test isspace_l api with form-feed
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspaceL9660, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\f';
    returnVal = isspace_l(paraVal, g_aucaLocale);
    LOGD("    isspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISSPACE_L_9680
* @tc.name       test isspace_l api with horizontal tab
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsspaceL9680, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\t';
    returnVal = isspace_l(paraVal, g_aucaLocale);
    LOGD("    isspace_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isspace_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUNORDERED_9700
* @tc.name       test isunordered api with NAN and 0
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsunordered9700, Function | MediumTest | Level1) {
    double paraVal1;
    double paraVal2;
    int returnVal;

    paraVal1 = NAN;
    paraVal2 = 0;
    returnVal = isunordered(paraVal1, paraVal2);
    LOGD("    isunordered returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(1 == returnVal) << "ErrInfo: isunordered  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_9720
* @tc.name       test isupper api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupper9720, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = isupper(paraVal);
    LOGD("    isupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_9740
* @tc.name       test isupper api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupper9740, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isupper(paraVal);
    LOGD("    isupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_9760
* @tc.name       test isupper api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupper9760, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '5';
    returnVal = isupper(paraVal);
    LOGD("    isupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_9780
* @tc.name       test isupper api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupper9780, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isupper(paraVal);
    LOGD("    isupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_L_9800
* @tc.name       test isupper_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupperL9800, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = isupper_l(paraVal, g_aucaLocale);
    LOGD("    isupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_L_9820
* @tc.name       test isupper_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupperL9820, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = isupper_l(paraVal, g_aucaLocale);
    LOGD("    isupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_L_9840
* @tc.name       test isupper_l api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupperL9840, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '5';
    returnVal = isupper_l(paraVal, g_aucaLocale);
    LOGD("    isupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISUPPER_L_9860
* @tc.name       test isupper_l api with LF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsupperL9860, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = '\n';
    returnVal = isupper_l(paraVal, g_aucaLocale);
    LOGD("    isupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISXDIGIT_9880
* @tc.name       test isxdigit api with xdigit F
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsxdigit9880, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'F';
    returnVal = isxdigit(paraVal);
    LOGD("    isxdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isxdigit  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISXDIGIT_9900
* @tc.name       test isxdigit api with alpha G
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsxdigit9900, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'G';
    returnVal = isxdigit(paraVal);
    LOGD("    isxdigit returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isxdigit  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISXDIGIT_L_9920
* @tc.name       test isxdigit_l api with xdigit F
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsxdigitL9920, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'F';
    returnVal = isxdigit_l(paraVal, g_aucaLocale);
    LOGD("    isxdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(returnVal != 0) << "ErrInfo: isxdigit_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CHECK_ISXDIGIT_L_9940
* @tc.name       test isxdigit_l api with alpha G
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilCheckApiTest, testIsxdigitL19940, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'G';
    returnVal = isxdigit_l(paraVal, g_aucaLocale);
    LOGD("    isxdigit_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: isxdigit_l  returnVal:='" << returnVal << "'";
}