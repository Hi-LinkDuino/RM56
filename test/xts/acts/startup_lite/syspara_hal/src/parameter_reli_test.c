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

#include "ohos_types.h"
#include <securec.h>
#include "hctest.h"
#include "parameter.h"
#include "parameter_utils.h"

#define QUERY_TIMES    50

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is parameter
 * @param        : test suit name is ParameterReliTestSuite
 */
LITE_TEST_SUIT(utils, parameter, ParameterReliTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL ParameterReliTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL ParameterReliTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_PARAMETER_5800
 * @tc.name      : Obtaining ProductType for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli001, Function | MediumTest | Level1)
{
    const char* value1 = GetDeviceType();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetDeviceType();
    }
    const char* value2 = GetDeviceType();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_5900
 * @tc.name      : Obtaining Manufacture for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli002, Function | MediumTest | Level1)
{
    const char* value1 = GetManufacture();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetManufacture();
    }
    const char* value2 = GetManufacture();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6000
 * @tc.name      : Obtaining Brand for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli003, Function | MediumTest | Level1)
{
    const char* value1 = GetBrand();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBrand();
    }
    const char* value2 = GetBrand();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6100
 * @tc.name      : Obtaining MarketName for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli004, Function | MediumTest | Level1)
{
    const char* value1 = GetMarketName();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetMarketName();
    }
    const char* value2 = GetMarketName();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6200
 * @tc.name      : Obtaining ProductSeries for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli005, Function | MediumTest | Level1)
{
    const char* value1 = GetProductSeries();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetProductSeries();
    }
    const char* value2 = GetProductSeries();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6300
 * @tc.name      : Obtaining ProductModel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli006, Function | MediumTest | Level1)
{
    const char* value1 = GetProductModel();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetProductModel();
    }
    const char* value2 = GetProductModel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6400
 * @tc.name      : Obtaining HardwareModel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli007, Function | MediumTest | Level1)
{
    const char* value1 = GetHardwareModel();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetHardwareModel();
    }
    const char* value2 = GetHardwareModel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6500
 * @tc.name      : Obtaining HardwareProfile for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli008, Function | MediumTest | Level1)
{
    const char* value1 = GetHardwareProfile();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetHardwareProfile();
    }
    const char* value2 = GetHardwareProfile();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6600
 * @tc.name      : Obtaining Serial for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli009, Function | MediumTest | Level1)
{
    const char* value1 = GetSerial();
    if (value1 == NULL) {
        printf("The serial number needs to be written\n");
        TEST_IGNORE();
    }
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetSerial();
    }
    const char* value2 = GetSerial();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6700
 * @tc.name      : Obtaining OsName for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli010, Function | MediumTest | Level1)
{
    const char* value1 = GetOSFullName();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetOSFullName();
    }
    const char* value2 = GetOSFullName();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6800
 * @tc.name      : Obtaining DisplayVersion for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli011, Function | MediumTest | Level1)
{
    const char* value1 = GetDisplayVersion();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetDisplayVersion();
    }
    const char* value2 = GetDisplayVersion();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_6900
 * @tc.name      : Obtaining BootloaderVersion for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli012, Function | MediumTest | Level1)
{
    const char* value1 = GetBootloaderVersion();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBootloaderVersion();
    }
    const char* value2 = GetBootloaderVersion();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7000
 * @tc.name      : Obtaining SecurityPatchTag for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli013, Function | MediumTest | Level1)
{
    const char* value1 = GetSecurityPatchTag();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetSecurityPatchTag();
    }
    const char* value2 = GetSecurityPatchTag();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7100
 * @tc.name      : Obtaining AbiList for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli014, Function | MediumTest | Level1)
{
    const char* value1 = GetAbiList();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetAbiList();
    }
    const char* value2 = GetAbiList();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7200
 * @tc.name      : Obtaining FirstApiLevel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli015, Function | MediumTest | Level1)
{
    int value1 = GetFirstApiVersion();
    TEST_ASSERT_NOT_NULL(value1);
    TEST_ASSERT_TRUE((int)value1 == value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    int value = GetFirstApiVersion();
    }
    int value2 = GetFirstApiVersion();
    TEST_ASSERT_EQUAL_INT(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7300
 * @tc.name      : Obtaining IncrementalVersion for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli016, Function | MediumTest | Level1)
{
    const char* value1 = GetIncrementalVersion();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetIncrementalVersion();
    }
    const char* value2 = GetIncrementalVersion();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7400
 * @tc.name      : Obtaining VersionId for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli017, Function | MediumTest | Level1)
{
    const char* value1 = GetVersionId();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetVersionId();
    }
    const char* value2 = GetVersionId();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7500
 * @tc.name      : Obtaining BuildType for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli018, Function | MediumTest | Level1)
{
    const char* value1 = GetBuildType();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBuildType();
    }
    const char* value2 = GetBuildType();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7600
 * @tc.name      : Obtaining BuildUser for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli019, Function | MediumTest | Level1)
{
    const char* value1 = GetBuildUser();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBuildUser();
    }
    const char* value2 = GetBuildUser();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7700
 * @tc.name      : Obtaining BuildHost for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli020, Function | MediumTest | Level1)
{
    const char* value1 = GetBuildHost();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBuildHost();
    }
    const char* value2 = GetBuildHost();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7800
 * @tc.name      : Obtaining BuildTime for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli021, Function | MediumTest | Level1)
{
    const char* value1 = GetBuildTime();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBuildTime();
    }
    const char* value2 = GetBuildTime();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_7900
 * @tc.name      : Obtaining BuildRootHash for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli022, Function | MediumTest | Level1)
{
    const char* value1 = GetBuildRootHash();
    TEST_ASSERT_NOT_NULL(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetBuildRootHash();
    }
    const char* value2 = GetBuildRootHash();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_8000
 * @tc.name      : Obtaining SoftwareModel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli023, Function | MediumTest | Level1)
{
    const char* value1 = GetSoftwareModel();
    AssertNotEmpty(value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    const char* value = GetSoftwareModel();
    }
    const char* value2 = GetSoftwareModel();
    TEST_ASSERT_EQUAL_STRING(value1, value2);
};

/**
 * @tc.number    : SUB_UTILS_PARAMETER_8100
 * @tc.name      : Obtaining SdkApiLevel for multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(ParameterReliTestSuite, testObtainSysParaReli024, Function | MediumTest | Level1)
{
    int value1 = GetSdkApiVersion();
    TEST_ASSERT_NOT_NULL(value1);
    TEST_ASSERT_TRUE((int)value1 == value1);
    for (int i = 0; i < QUERY_TIMES; i++)
    {
    int value = GetSdkApiVersion();
    }
    int value2 = GetSdkApiVersion();
    TEST_ASSERT_EQUAL_INT(value1, value2);
};

RUN_TEST_SUITE(ParameterReliTestSuite);
