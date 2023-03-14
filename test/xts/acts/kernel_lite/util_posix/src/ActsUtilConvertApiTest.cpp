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

#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>
#include <string.h>

#include "gtest/gtest.h"
#include "log.h"
#include "utils.h"

using namespace testing::ext;

class ActsUtilConvertApiTest : public testing::Test {
public:
    locale_t g_aucaNewloc;
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
        g_aucaNewloc = newlocale(LC_ALL_MASK, "", (locale_t)0);
    }
    // Testcase teardown
    virtual void TearDown()
    {
        freelocale(g_aucaNewloc);
    }
};

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_0100
* @tc.name       test _tolower api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolower0100, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'A';
    returnVal = _tolower(paraChar);
    LOGD("    _tolower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: _tolower  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_0200
* @tc.name       test _tolower api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolower0200, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = '5';
    returnVal = _tolower(paraChar);
    LOGD("    _tolower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE('5' == returnVal) << "ErrInfo: _tolower  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_0300
* @tc.name       test _tolower api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolower0300, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'z';
    returnVal = _tolower(paraChar);
    LOGD("    _tolower c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE('z' == returnVal) << "ErrInfo: _tolower  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_0310
 * @tc.name       test _tolower api abnormal scenario with two uppercase characters
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testTolower0310, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = 'AZ';
    returnVal = _tolower(paraChar);
    LOGD("    _tolower c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    EXPECT_FALSE('az' == returnVal) << "ErrInfo: _tolower  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_0320
 * @tc.name       test _tolower api abnormal scenario with multiple digital characters
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testTolower0320, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = '22';
    returnVal = _tolower(paraChar);
    LOGD("    _tolower c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    EXPECT_FALSE('22' == returnVal) << "ErrInfo: _tolower  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_0330
 * @tc.name       test _tolower api abnormal scenario with special symbols characters
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testTolower0330, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = '@';
    returnVal = _tolower(paraChar);
    LOGD("    _tolower c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    ASSERT_TRUE(96 == returnVal) << "ErrInfo: _tolower  returnVal:='" << returnVal << "'";
}


/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_0400
* @tc.name       test _toupper api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToupper0400, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'a';
    returnVal = _toupper(paraChar);
    LOGD("    _toupper c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: _toupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_0500
* @tc.name       test _toupper api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToupper0500, Function | MediumTest | Level1) {
    char paraChar;
    int returnVal;

    paraChar = 'Z';
    returnVal = _toupper(paraChar);
    LOGD("    _toupper c:='%c',   returnVal:='%c'\n", paraChar, returnVal);
    ASSERT_TRUE('Z' == returnVal) << "ErrInfo: _toupper  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_0510
 * @tc.name       test _toupper api abnormal scenario with special symbols characters
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testToupper0510, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = '@';
    returnVal = _toupper(paraChar);
    LOGD("    _toupper c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    ASSERT_TRUE('@' == returnVal) << "ErrInfo: _toupper  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_0520
 * @tc.name       test _toupper api with ASCII
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testToupper0520, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = 50;
    returnVal = _toupper(paraChar);
    LOGD("    _toupper c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    ASSERT_TRUE(18 == returnVal) << "ErrInfo: _toupper  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_0530
 * @tc.name       test _toupper api abnormal scenario with multiple lowercase characters
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testToupper0530, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = 'aa';
    returnVal = _toupper(paraChar);
    LOGD("    _toupper c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    EXPECT_FALSE('AA' == returnVal) << "ErrInfo: _toupper  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_0540
 * @tc.name       test _toupper api abnormal scenario with multiple lowercase characters combination
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testToupper0540, Function | MediumTest | Level1)
{
    char paraChar;
    int returnVal;

    paraChar = 'az';
    returnVal = _toupper(paraChar);
    LOGD("    _toupper c:='%c',   returnVal:='%c'/n", paraChar, returnVal);
    EXPECT_FALSE('AZ' == returnVal) << "ErrInfo: _toupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0600
* @tc.name       test atof api with string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtof0600, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "abcde";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'\n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0700
* @tc.name       test atof api with empty string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtof0700, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'\n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0800
* @tc.name       test atof api with digit2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtof0800, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "12345";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'\n", paraChar, returnVal);
    ASSERT_TRUE(12345.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0810
 * @tc.name       test atof api with digit1
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtof0810, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "xtstest abcde";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'/n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0820
 * @tc.name       test atof api with digit3
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtof0820, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "xtstest 123456";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'/n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0830
 * @tc.name       test atof api with digit4
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtof0830, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "123456    ";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'/n", paraChar, returnVal);
    ASSERT_TRUE(123456.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0840
 * @tc.name       test atof api with digit5
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtof0840, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "      abcde";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'/n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0850
 * @tc.name       test atof api with digit6
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtof0850, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "      abcde     ";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'/n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOF_0860
 * @tc.name       test atof api with digit7
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtof0860, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    double returnVal;

    paraChar = "a";
    returnVal = atof(paraChar);
    LOGD("    atof c:='%s',   returnVal:='%f'/n", paraChar, returnVal);
    ASSERT_TRUE(0.000000 == returnVal) << "ErrInfo: atof  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOL_0900
* @tc.name       test atol api with string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtol0900, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    long int returnVal;

    paraChar = "abcde";
    returnVal = atol(paraChar);
    LOGD("    atol c:='%s',   returnVal:='%ld'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: atol  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOL_1000
* @tc.name       test atol api with empty string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtol1000, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    long int returnVal;

    paraChar = "";
    returnVal = atol(paraChar);
    LOGD("    atol c:='%s',   returnVal:='%ld'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: atol  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOL_1100
* @tc.name       test atol api with digit9
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtol1100, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    long int returnVal;

    paraChar = "12345";
    returnVal = atol(paraChar);
    LOGD("    atol c:='%s',   returnVal:='%ld'\n", paraChar, returnVal);
    ASSERT_TRUE(12345 == returnVal) << "ErrInfo: atol  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOL_0100
 * @tc.name       test atol api with digit8
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtol0100, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    long int returnVal;

    paraChar = "12345.111";
    returnVal = atol(paraChar);
    LOGD("    atol c:='%s',   returnVal:='%ld'/n", paraChar, returnVal);
    ASSERT_TRUE(12345 == returnVal) << "ErrInfo: atol  returnVal:='" << returnVal << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOL_0200
 * @tc.name       test atol api with digit10
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testAtol0200, Function | MediumTest | Level1)
{
    const char *paraChar = nullptr;
    long int returnVal;

    paraChar = "-12345";
    returnVal = atol(paraChar);
    LOGD("    atol c:='%s',   returnVal:='%ld'/n", paraChar, returnVal);
    ASSERT_TRUE(-12345 == returnVal) << "ErrInfo: atol  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOLL_1200
* @tc.name       test atoll api with string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtoll1200, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    long long int returnVal;

    paraChar = "abcde";
    returnVal = atoll(paraChar);
    LOGD("    atoll c:='%s',   returnVal:='%lld'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: atoll  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOLL_1300
* @tc.name       test atoll api with empty string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtoll1300, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    long long int returnVal;

    paraChar = "";
    returnVal = atoll(paraChar);
    LOGD("    atoll c:='%s',   returnVal:='%lld'\n", paraChar, returnVal);
    ASSERT_TRUE(0 == returnVal) << "ErrInfo: atoll  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ATOLL_1400
* @tc.name       test atoll api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testAtoll1400, Function | MediumTest | Level1) {
    const char *paraChar = nullptr;
    long long int returnVal;

    paraChar = "1234567890";
    returnVal = atoll(paraChar);
    LOGD("    atoll c:='%s',   returnVal:='%lld'\n", paraChar, returnVal);
    ASSERT_TRUE(1234567890 == returnVal) << "ErrInfo: atoll  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ECVT_1500
* @tc.name       test ecvt api with decimal double
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testEcvt1500, Function | MediumTest | Level1) {
    char *returnVal = nullptr;
    double paraValue;
    int digitCount;
    int paraDec;
    int paraSign;

    paraValue = 12345.6789;
    digitCount = 10;
    returnVal = ecvt(paraValue, digitCount, &paraDec, &paraSign);
    LOGD("    ecvt  paraValue:='%f',  digitCount:='%d',  paraDec = '%d',  paraSign = '%d',  returnVal:='%s'\n",
        paraValue, digitCount, paraDec, paraSign, returnVal);
    ASSERT_TRUE(strcmp(returnVal, "1234567890") == 0 && paraSign == 0 && paraDec == 5) <<
        "ErrInfo: ecvt returnVal:='" << returnVal << "',  paraDec:='" << paraDec << "', paraSign:='" << paraSign << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_ECVT_1600
* @tc.name       test ecvt api with exponent double2
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testEcvt1600, Function | MediumTest | Level1) {
    char *returnVal = nullptr;
    double paraValue;
    int digitCount;
    int paraDec;
    int paraSign;

    paraValue = -0.123e4;
    digitCount = 5;
    returnVal = ecvt(paraValue, digitCount, &paraDec, &paraSign);
    LOGD("    ecvt  paraValue:='%d',  digitCount:='%d',  paraDec = '%d',  paraSign = '%d',  returnVal:='%s'\n",
        paraValue, digitCount, returnVal, paraDec, paraSign);
    ASSERT_TRUE(strcmp(returnVal, "12300") == 0 && paraSign == 1 && paraDec != 0) <<
        "ErrInfo: ecvt returnVal:='" << returnVal << "',  paraDec:='" << paraDec << "', paraSign:='" << paraSign << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ECVT_1610
 * @tc.name       test ecvt api with exponent double1
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testEcvt1610, Function | MediumTest | Level1)
{
    int dec, sign;
    char *returnVal;

    double paraValue = 3.1415926385;
    int precision = 10;

    returnVal = ecvt(paraValue, precision, &dec, &sign);
    LOGD("    ecvt  paraValue:='%f',  precision:='%d',  dec = '%d',  sign = '%d',  returnVal:='%s'/n", paraValue,
        precision, dec, sign, returnVal);
    ASSERT_TRUE(strcmp(returnVal, "3141592639") == 0 && dec == 1 && sign == 0) << "ErrInfo: ecvt returnVal:='" <<
        returnVal << "',  dec:='" << dec << "', sign:='" << sign << "'";
}

/* *
 * @tc.number     SUB_KERNEL_UTIL_CONVERT_ECVT_1630
 * @tc.name       test ecvt api with exponent double3
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsUtilConvertApiTest, testEcvt1630, Function | MediumTest | Level1)
{
    int dec, sign;
    char *returnVal;

    double paraValue = 0.0;
    int precision = 10;

    returnVal = ecvt(paraValue, precision, &dec, &sign);
    LOGD("    ecvt  paraValue:='%f',  precision:='%d',  dec = '%d',  sign = '%d',  returnVal:='%s'/n", paraValue,
        precision, dec, sign, returnVal);
    ASSERT_TRUE(strcmp(returnVal, "0000000000") == 0 && dec == 1 && sign == 0) << "ErrInfo: ecvt returnVal:='" <<
        returnVal << "',  dec:='" << dec << "', sign:='" << sign << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOASCII_1700
* @tc.name       test toascii api with digit
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToascii1700, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 225;
    returnVal = toascii(paraVal);
    LOGD("    toascii returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(97 == returnVal) << "ErrInfo: toascii  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_1800
* @tc.name       test tolower api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolower1800, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = tolower(paraVal);
    LOGD("    tolower returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: tolower  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_1900
* @tc.name       test tolower api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolower1900, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = tolower(paraVal);
    LOGD("    tolower returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: tolower  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_L_2000
* @tc.name       test tolower_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolowerL2000, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = tolower_l(paraVal, g_aucaNewloc);
    LOGD("    tolower_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: tolower_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOLOWER_L_2100
* @tc.name       test tolower_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTolowerL2100, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = tolower_l(paraVal, g_aucaNewloc);
    LOGD("    tolower_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: tolower_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_2200
* @tc.name       test toupper api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToupper2200, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = toupper(paraVal);
    LOGD("    toupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: toupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_2300
* @tc.name       test toupper api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToupper2300, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = toupper(paraVal);
    LOGD("    toupper returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: toupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_L_2400
* @tc.name       test toupper_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToupperL2400, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'A';
    returnVal = toupper_l(paraVal, g_aucaNewloc);
    LOGD("    toupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: toupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOUPPER_L_2500
* @tc.name       test toupper_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testToupperL2500, Function | MediumTest | Level1) {
    char paraVal;
    int returnVal;

    paraVal = 'a';
    returnVal = toupper_l(paraVal, g_aucaNewloc);
    LOGD("    toupper_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: toupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_L64A_2600
* @tc.name       test l64a api with zero
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testL64a2600, Function | MediumTest | Level1) {
    long paraVal;
    char *returnVal;

    paraVal = 0;
    returnVal = l64a(paraVal);
    LOGD("    l64a returnVal:='%s'\n", returnVal);
    ASSERT_TRUE(strcmp("", returnVal) == 0) << "ErrInfo: l64a  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_L64A_2700
* @tc.name       test l64a api with decimal
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testL64a2700, Function | MediumTest | Level1) {
    long paraVal;
    char *returnVal;

    paraVal = 12345;
    returnVal = l64a(paraVal);
    LOGD("    l64a returnVal:='%s'\n", returnVal);
    ASSERT_TRUE(strcmp("t.1", returnVal) == 0) << "ErrInfo: l64a  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWLOWER_2800
* @tc.name       test towlower api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowlower2800, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'A';
    returnVal = towlower(wideChar);
    LOGD("    towlower returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: towlower  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWLOWER_2900
* @tc.name       test towlower api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowlower2900, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'a';
    returnVal = towlower(wideChar);
    LOGD("    towlower returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: towlower  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWLOWER_L_3000
* @tc.name       test towlower_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowlowerL3000, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'A';
    returnVal = towlower_l(wideChar, g_aucaNewloc);
    LOGD("    towlower_l returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: towlower_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWLOWER_L_3100
* @tc.name       test towlower_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowlowerL3100, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'a';
    returnVal = towlower_l(wideChar, g_aucaNewloc);
    LOGD("    towlower_l returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: towlower_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWUPPER_3200
* @tc.name       test towupper api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowupper3200, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'A';
    returnVal = towupper(wideChar);
    LOGD("    towupper returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: towupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWUPPER_3300
* @tc.name       test towupper api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowupper3300, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'a';
    returnVal = towupper(wideChar);
    LOGD("    towupper returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: towupper  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWUPPER_L_3400
* @tc.name       test towupper_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowupperL3400, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'A';
    returnVal = towupper_l(wideChar, g_aucaNewloc);
    LOGD("    towupper_l returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: towupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWUPPER_L_3500
* @tc.name       test towupper_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowupperL3500, Function | MediumTest | Level1) {
    wint_t wideChar;
    wint_t returnVal;

    wideChar = 'a';
    returnVal = towupper_l(wideChar, g_aucaNewloc);
    LOGD("    towupper_l returnVal:='%c'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: towupper_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_MBTOWC_3600
* @tc.name       test mbtowc api with alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testMbtowc3600, Function | MediumTest | Level1) {
    wchar_t wideChar;
    const char *paraStr = "a";
    size_t sizeVal;
    int returnVal;

    wideChar = 'a';
    sizeVal = 1;
    returnVal = mbtowc(&wideChar, paraStr, sizeVal);
    LOGD("    mbtowc returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(1 == returnVal && L'a' == wideChar) << "ErrInfo: mbtowc  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWCTRANS_3700
* @tc.name       test towctrans api with WEOF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowctrans3700, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctrans_t paraDesc;
    wint_t returnVal;

    wideChar = WEOF;
    paraDesc = wctrans("tolower");
    returnVal = towctrans(wideChar, paraDesc);
    LOGD("    towctrans returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(WEOF == returnVal) << "ErrInfo: towctrans  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWCTRANS_3800
* @tc.name       test towctrans api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowctrans3800, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctrans_t paraDesc;
    wint_t returnVal;

    wideChar = L'A';
    paraDesc = wctrans("tolower");
    returnVal = towctrans(wideChar, paraDesc);
    LOGD("    towctrans returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: towctrans  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWCTRANS_3900
* @tc.name       test towctrans api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowctrans3900, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctrans_t paraDesc;
    wint_t returnVal;

    wideChar = L'a';
    paraDesc = wctrans("toupper");
    returnVal = towctrans(wideChar, paraDesc);
    LOGD("    towctrans returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: towctrans  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWCTRANS_L_4000
* @tc.name       test towctrans_l api with WEOF
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowctransL4000, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctrans_t paraDesc;
    wint_t returnVal;

    wideChar = WEOF;
    paraDesc = wctrans_l("tolower", g_aucaNewloc);
    returnVal = towctrans_l(wideChar, paraDesc, g_aucaNewloc);
    LOGD("    towctrans_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(WEOF == returnVal) << "ErrInfo: towctrans_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWCTRANS_L_4100
* @tc.name       test towctrans_l api with upper alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowctransL4100, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctrans_t paraDesc;
    wint_t returnVal;

    wideChar = L'A';
    paraDesc = wctrans_l("tolower", g_aucaNewloc);
    returnVal = towctrans_l(wideChar, paraDesc, g_aucaNewloc);
    LOGD("    towctrans_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: towctrans_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_TOWCTRANS_L_4200
* @tc.name       test towctrans_l api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testTowctransL4200, Function | MediumTest | Level1) {
    wint_t wideChar;
    wctrans_t paraDesc;
    wint_t returnVal;

    wideChar = L'a';
    paraDesc = wctrans_l("toupper", g_aucaNewloc);
    returnVal = towctrans_l(wideChar, paraDesc, g_aucaNewloc);
    LOGD("    towctrans_l returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('A' == returnVal) << "ErrInfo: towctrans_l  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_WCTOB_4300
* @tc.name       test wctob api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testWctob4300, Function | MediumTest | Level1) {
    wint_t wideChar;
    int returnVal;

    wideChar = L'a';
    returnVal = wctob(wideChar);
    LOGD("    wctob returnVal:='%d'\n", returnVal);
    ASSERT_TRUE('a' == returnVal) << "ErrInfo: wctob  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_WCTOMB_4400
* @tc.name       test wctomb api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testWctomb4400, Function | MediumTest | Level1) {
    char paraStr[5];
    wint_t wideChar;
    int returnVal;

    wideChar = L'a';
    returnVal = wctomb(paraStr, wideChar);
    LOGD("    wctomb returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(1 == returnVal && 'a' == paraStr[0]) << "ErrInfo: wctomb  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_MBSTOWCS_4500
* @tc.name       test mbstowcs api with string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testMbstowcs4500, Function | MediumTest | Level1) {
    wchar_t paraDest[10];
    const char *paraSrc = "abcde";
    size_t sizeVal = 6;
    size_t returnVal;

    returnVal = mbstowcs(paraDest, paraSrc, sizeVal);
    LOGD("    mbstowcs returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(5 == returnVal && (0 == wcscmp(paraDest, L"abcde")))
        << "ErrInfo: mbstowcs  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_WCSTOMBS_4600
* @tc.name       test wcstombs api with string
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testWcstombs4600, Function | MediumTest | Level1) {
    char paraDest[10];
    const wchar_t *paraSrc = L"abcde";
    size_t sizeVal = 6;
    size_t returnVal;

    returnVal = wcstombs(paraDest, paraSrc, sizeVal);
    LOGD("    wcstombs returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(5 == returnVal && (0 == strcmp(paraDest, "abcde")))
        << "ErrInfo: wcstombs  returnVal:='" << returnVal << "'";
}

/**
* @tc.number     SUB_KERNEL_UTIL_CONVERT_WCRTOMB_4700
* @tc.name       test wcrtomb api with lower alpha
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ActsUtilConvertApiTest, testWcrtomb4700, Function | MediumTest | Level1) {
    char paraStr[2];
    wchar_t wideChar;
    mbstate_t paraPs;
    size_t returnVal;

    wideChar = L'a';

    returnVal = wcrtomb(paraStr, wideChar, &paraPs);
    LOGD("    wcrtomb returnVal:='%d'\n", returnVal);
    ASSERT_TRUE(1 == returnVal && 'a' == paraStr[0]) << "ErrInfo: wcrtomb  returnVal:='" << returnVal << "'";
}
