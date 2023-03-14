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

#include <gtest/gtest.h>
#include "string_ex.h"
#include <iostream>
using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsStringTest : public testing::Test
{
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsStringTest::SetUpTestCase(void)
{
}

void UtilsStringTest::TearDownTestCase(void)
{
}

void UtilsStringTest::SetUp(void)
{
}

void UtilsStringTest::TearDown(void)
{
}

/*
* Feature: string_ex
* Function: UpperStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for convert all letters of str  to uppercase
*/
HWTEST_F(UtilsStringTest, test_strupper_01, TestSize.Level0)
{
    string strBase = "strbase";
    string strTemp = "STRBASE";
    string result = UpperStr(strBase);
    EXPECT_EQ(result, strTemp);

    strBase = "StrBase";
    result = UpperStr(strBase);
    EXPECT_EQ(result, strTemp);
}

HWTEST_F(UtilsStringTest, test_strupper_02, TestSize.Level0)
{
    string strBase = "";
    string strTemp = "";
    string result = UpperStr(strBase);
    EXPECT_EQ(result, strTemp);
}

/*
* Feature: string_ex
* Function: LowerStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for convert all letters of str  to lowercase
*/
HWTEST_F(UtilsStringTest, test_strlower_01, TestSize.Level0)
{
    string strBase = "STRbase";
    string strTemp = "strbase";
    string result = LowerStr(strBase);
    EXPECT_EQ(result, strTemp);

    strBase = "StrBase";
    result = LowerStr(strBase);
    EXPECT_EQ(result, strTemp);
}

HWTEST_F(UtilsStringTest, test_strlower_02, TestSize.Level0)
{
    string strBase = "";
    string strTemp = "";
    string result = LowerStr(strBase);
    EXPECT_EQ(result, strTemp);
}

/*
* Feature: string_ex
* Function: ReplaceStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for replace src with dst int strBase
*/
HWTEST_F(UtilsStringTest, test_strreplace_01, TestSize.Level0)
{
    string strBase = "test for replace";
    string src = "for";
    string dst = "with";
    string strTemp = "test with replace";
    string result = ReplaceStr(strBase, src, dst);
    EXPECT_EQ(result, strTemp);

    src = "test for replace";
    dst = "test";
    strTemp = "test";
    result = ReplaceStr(strBase, src, dst);
    EXPECT_EQ(result, strTemp);

    src = "";
    dst = "test";
    result = ReplaceStr(strBase, src, dst);
    EXPECT_EQ(result, strBase);

    src = "for";
    dst = "";
    strTemp = "test  replace";
    result = ReplaceStr(strBase, src, dst);
    EXPECT_EQ(result, strTemp);
}

/*
* Feature: string_ex
* Function: TrimStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for trim str front and end
*/
HWTEST_F(UtilsStringTest, test_strtrim_01, TestSize.Level0)
{
    string strBase = "              test for trim ";
    string strTemp = "test for trim";
    string result = TrimStr(strBase);
    EXPECT_EQ(result, strTemp);
}

HWTEST_F(UtilsStringTest, test_strtrim_02, TestSize.Level0)
{
    string strBase = "test";
    string strTemp = "es";
    string result = TrimStr(strBase, 't');
    EXPECT_EQ(result, strTemp);
}

/*
* Feature: string_ex
* Function: SplitStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for split str by strSep
*/
HWTEST_F(UtilsStringTest, test_strsplit_01, TestSize.Level0)
{
    string strBase = "test for split";
    string strSep = " ";
    string splitResult[3] = { "test", "for", "split" };
    vector<string> strsRet;
    SplitStr(strBase, strSep, strsRet);

    for (int i = 0; i < 3; i++)
    {
        EXPECT_EQ(splitResult[i], strsRet[i]);
    }
}

HWTEST_F(UtilsStringTest, test_strsplit_02, TestSize.Level0)
{
    string strBase = "test for split";
    string strSep = "for";
    string splitResult[2] = { "test", "split" };
    vector<string> strsRet;
    SplitStr(strBase, strSep, strsRet);

    for (int i = 0; i < 2; i++)
    {
        EXPECT_EQ(splitResult[i], strsRet[i]);
    }

    splitResult[0] = "test ";
    splitResult[1] = " split";
    SplitStr(strBase, strSep, strsRet, false, false);
    for (int i = 0; i < 2; i++)
    {
        EXPECT_EQ(splitResult[i], strsRet[i]);
    }
}

HWTEST_F(UtilsStringTest, test_strsplit_03, TestSize.Level0)
{
    string strBase = "test for for split";
    string strSep = "for";
    string splitResult[3] = { "test", "", "split" };
    vector<string> strsRet;
    SplitStr(strBase, strSep, strsRet, true);
    for (int i = 0; i < (int)strsRet.size(); i++)
    {
        EXPECT_EQ(splitResult[i], strsRet[i]);
    }
}


/*
* Feature: string_ex
* Function: IsNumericStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for judge all characters of the string are numbers
*/
HWTEST_F(UtilsStringTest, test_strisnumeric_01, TestSize.Level0)
{
    string strBase = "1234556";
    bool result = IsNumericStr(strBase);
    EXPECT_EQ(result, true);

    strBase = "1234,a";
    result = IsNumericStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "";
    result = IsNumericStr(strBase);
    EXPECT_EQ(result, false);
}

/*
* Feature: string_ex
* Function: IsAlphaStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for judge all characters of the string are alphabet
*/
HWTEST_F(UtilsStringTest, test_strisalpha_01, TestSize.Level0)
{
    string strBase = "1234556";
    bool result = IsAlphaStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "Acedafe";
    result = IsAlphaStr(strBase);
    EXPECT_EQ(result, true);

    strBase = "Acedafe  ";
    result = IsAlphaStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "Acedafe3";
    result = IsAlphaStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "";
    result = IsAlphaStr(strBase);
    EXPECT_EQ(result, false);
}

/*
* Feature: string_ex
* Function: IsUpperStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for judge all characters of the string are uppercase
*/
HWTEST_F(UtilsStringTest, test_IsUpperStr_01, TestSize.Level0)
{
    string strBase = "ABSEFAD";
    bool result = IsUpperStr(strBase);
    EXPECT_EQ(result, true);

    strBase = "Afaefadf";
    result = IsUpperStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "12e13eaefd     ";
    result = IsUpperStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "";
    result = IsUpperStr(strBase);
    EXPECT_EQ(result, false);
}

/*
* Feature: string_ex
* Function: IsLowerStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for judge all characters of the string are lowercase
*/
HWTEST_F(UtilsStringTest, test_IsLowerStr_01, TestSize.Level0)
{
    string strBase = "testlower";
    bool result = IsLowerStr(strBase);
    EXPECT_EQ(result, true);

    strBase = "AAFDeadfkl";
    result = IsLowerStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "12e";
    result = IsLowerStr(strBase);
    EXPECT_EQ(result, false);

    strBase = "";
    result = IsLowerStr(strBase);
    EXPECT_EQ(result, false);
}

/*
* Feature: string_ex
* Function: IsSubStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for judge the sub_str in base_str
*/
HWTEST_F(UtilsStringTest, test_IsSubStr_01, TestSize.Level0)
{
    string strBase = "test for issubstr";
    string strSub = "for";
    bool result = IsSubStr(strBase, strSub);
    EXPECT_EQ(result, true);

    strSub = "fori";
    result = IsSubStr(strBase, strSub);
    EXPECT_EQ(result, false);
}

/*
* Feature: string_ex
* Function: IsSameTextStr
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for judge the strFirst's letter is same with strSecond
*/
HWTEST_F(UtilsStringTest, test_IsSameTextStr_01, TestSize.Level0)
{
    string strFirst = "Test For StrSameText";
    string strSecond = "test for strsametext";
    bool result = IsSameTextStr(strFirst, strSecond);
    EXPECT_EQ(result, true);

    strSecond = "test for strsametex";
    result = IsSameTextStr(strFirst, strSecond);
    EXPECT_EQ(result, false);
}

/*
* Feature: string_ex
* Function: ToString
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for convert int to str
*/
HWTEST_F(UtilsStringTest, test_ToString_01, TestSize.Level0)
{
    int ivalue = 12345;
    string strValue = "12345";
    string result = ToString(ivalue);
    EXPECT_EQ(result, strValue);

    ivalue = -15;
    result = ToString(ivalue);
    EXPECT_EQ(result, "-15");
}

/*
* Feature: string_ex
* Function: StrToInt
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: test for convert str to int
*/
HWTEST_F(UtilsStringTest, test_StrToInt_01, TestSize.Level0)
{
    string strValue = "12345";
    int iValue = 0;
    bool result = StrToInt(strValue, iValue);
    EXPECT_EQ(result, true);
    EXPECT_EQ(iValue, 12345);

    strValue = "123r54";
    result = StrToInt(strValue, iValue);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilsStringTest, test_StrToInt_02, TestSize.Level0)
{
    string strValue = "-12345";
    int iValue = 0;
    bool result = StrToInt(strValue, iValue);
    EXPECT_EQ(result, true);
    EXPECT_EQ(iValue, -12345);

    strValue = "123=     54";
    result = StrToInt(strValue, iValue);
    EXPECT_EQ(result, false);

    string strvalue2;
    result = StrToInt(strvalue2, iValue);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilsStringTest, test_StrToInt_03, TestSize.Level0)
{
    string strValue = "2147483648";
    int ivalue = 0;
    bool result = StrToInt(strValue, ivalue);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilsStringTest, test_StrToInt_04, TestSize.Level0)
{
    string strValue = "             ";
    int iValue = 0;
    bool result = StrToInt(strValue, iValue);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilsStringTest, test_strcovertfailed_01, TestSize.Level0)
{
    char test[] = {192, 157, 47, 106, 97, 18, 97, 47, 115, 1, 2};
    string strValue(test);

    bool ret = IsAsciiString(strValue);
    EXPECT_EQ(ret, false);

    strValue = "1234";
    ret = IsAsciiString(strValue);
    EXPECT_EQ(ret, true);

    strValue = "abcde";
    ret = IsAsciiString(strValue);
    EXPECT_EQ(ret, true);
}


HWTEST_F(UtilsStringTest, test_strcovert_01, TestSize.Level0)
{
    string strValue = "hello world!";
    u16string str16 = Str8ToStr16(strValue);
    EXPECT_EQ(0, strValue.compare(Str16ToStr8(str16)));
}

HWTEST_F(UtilsStringTest, test_strcovert_02, TestSize.Level0)
{
    string str8Value = "hello world!";
    u16string str16Result = u"hello world!";
    u16string str16Value = Str8ToStr16(str8Value);
    EXPECT_EQ(0, str16Result.compare(str16Value));

    str16Result = u"你好";
    string str8Result = Str16ToStr8(str16Result);
    str16Value = Str8ToStr16(str8Result);
    EXPECT_EQ(0, str16Result.compare(str16Value));


    str16Result = u"某某技术有限公司";
    str8Result = Str16ToStr8(str16Result);
    str16Value = Str8ToStr16(str8Result);
    EXPECT_EQ(0, str16Result.compare(str16Value));
}

HWTEST_F(UtilsStringTest, test_strcovert_03, TestSize.Level0)
{
    string str8Value = "1234567890!@#$%^&*().";
    u16string str16Result = u"1234567890!@#$%^&*().";
    u16string str16Value = Str8ToStr16(str8Value);
    EXPECT_EQ(0, str16Result.compare(str16Value));

    string str8Result = Str16ToStr8(str16Value);
    EXPECT_EQ(0, str8Result.compare(str8Value));
}

HWTEST_F(UtilsStringTest, test_strcovert_04, TestSize.Level0)
{
    string str8Value = "1234567890!@#$%^&*().qazxswedcvfr,./;'][";
    u16string str16Result = u"1234567890!@#$%^&*().qazxswedcvfr,./;'][";
    u16string str16Value = Str8ToStr16(str8Value);
    EXPECT_EQ(0, str16Result.compare(str16Value));

    string str8Result = Str16ToStr8(str16Value);
    EXPECT_EQ(0, str8Result.compare(str8Value));
}


HWTEST_F(UtilsStringTest, test_getsubstr_01, TestSize.Level0)
{
    string strBase = "test for {sub str} {sub str1}";
    string left = "{";
    string right = "}";
    string strResult = "sub str";
    string strValue;
    string::size_type pos = GetFirstSubStrBetween(strBase, left, right, strValue);
    EXPECT_EQ(17, (int)pos);
    EXPECT_EQ(strResult, strValue);

    strBase = "test for sub str} {sub str1}";
    strResult = "sub str1";
    pos = GetFirstSubStrBetween(strBase, left, right, strValue);
    EXPECT_EQ(27, (int)pos);
    EXPECT_EQ(strResult, strValue);
}

HWTEST_F(UtilsStringTest, test_getsubstr_02, TestSize.Level0)
{
    string strBase = "test for} {sub str {sub str1";
    string left = "{";
    string right = "}";
    string strValue;
    string::size_type pos = GetFirstSubStrBetween(strBase, left, right, strValue);
    EXPECT_EQ(pos, string::npos);
}


HWTEST_F(UtilsStringTest, test_getsubstr_03, TestSize.Level0)
{
    string strBase = "test for {sub str} {sub str1}";
    string left = "{";
    string right = "}";
    string strResult[2] = { "sub str", "sub str1" };
    vector<string> strValue;
    GetSubStrBetween(strBase, left, right, strValue);
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(strResult[i], strValue[i]);
    }
}

HWTEST_F(UtilsStringTest, test_getsubstr_04, TestSize.Level0)
{
    string strBase = "test for } {sub str {sub str1";
    string left = "{";
    string right = "}";
    string strResult[2] = { "sub str", "sub str1" };
    vector<string> strValue;
    GetSubStrBetween(strBase, left, right, strValue);
    EXPECT_EQ(0, static_cast<int>(strValue.size()));
}

HWTEST_F(UtilsStringTest, DexToHexString_01, TestSize.Level0)
{
    string result = DexToHexString(0);
    EXPECT_EQ(result, "0");

    result = DexToHexString(14);
    EXPECT_EQ(result, "E");

    result = DexToHexString(14, false);
    EXPECT_EQ(result, "e");

    result = DexToHexString(-14, false);
    EXPECT_EQ(result, "fffffff2");

    result = DexToHexString(-14);
    EXPECT_EQ(result, "FFFFFFF2");

    result = DexToHexString(11259375);
    EXPECT_EQ(result, "ABCDEF");

    result = DexToHexString(11259375, false);
    EXPECT_EQ(result, "abcdef");
}
