/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <unistd.h>

#include "hctest.h"
#include "securec.h"
#include "bundle_info.h"
#include "bundle_manager.h"
#include "want.h"

/**
* @brief  register a test suit named BundleMgrTestSuite
* @param  subsystem name is appexecfwk
* @param  module name is  bundlemgr
* @param  test suit name is BundleMgrTestSuite
*/
LITE_TEST_SUIT(appexecfwk, bundlemgr, BundleMgrTestSuite);

static BOOL BundleMgrTestSuiteSetUp(void)
{
    printf("----------test case with BundleMgrTest start-------------\n");
    return TRUE;
}

static BOOL BundleMgrTestSuiteTearDown(void)
{
    printf("----------test case with BundleMgrTest end-------------\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_APPEXECFWK_0001
 * @tc.name      : testClearAbilityInfoLegal
 * @tc.desc      : testClearAbilityInfo parameter legal test with bundle name
 */
LITE_TEST_CASE(BundleMgrTestSuite, testClearAbilityInfoLegal, Function | MediumTest | Level2)
{
    printf("------start testClearAbilityInfo------\n");
    AbilityInfo abilityInfo;
    int result = memset_s(&abilityInfo, sizeof(abilityInfo), 0, sizeof(abilityInfo));
    TEST_ASSERT_TRUE(result == 0);
    abilityInfo.bundleName = "com.openharmony.testjsdemo";
    TEST_ASSERT_EQUAL_STRING(abilityInfo.bundleName, "com.openharmony.testjsdemo");
    ClearAbilityInfo(&abilityInfo);
    TEST_ASSERT_EQUAL_STRING(abilityInfo.bundleName, NULL);
    printf("------end testClearAbilityInfo------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0002
 * @tc.name      : testClearAbilityInfoIllegal
 * @tc.desc      : testClearAbilityInfo parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testClearAbilityInfoIllegal, Function | MediumTest | Level2)
{
    printf("------start testClearAbilityInfoIllegal------\n");
    AbilityInfo abilityInfo = { 0 };
    int result = memset_s(&abilityInfo, sizeof(abilityInfo), 0, sizeof(abilityInfo));
    TEST_ASSERT_TRUE(result == 0);
    abilityInfo.bundleName = "com.openharmony.testjsdemo";
    ClearAbilityInfo(NULL);
    TEST_ASSERT_EQUAL_STRING(abilityInfo.bundleName, "com.openharmony.testjsdemo");
    printf("------end testClearAbilityInfoIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0003
 * @tc.name      : testClearBundleInfoLegal
 * @tc.desc      : testClearBundleInfo parameter legal test with bundle name
 */
LITE_TEST_CASE(BundleMgrTestSuite, testClearBundleInfoLegal, Function | MediumTest | Level2)
{
    printf("------start testClearBundleInfo------\n");
    BundleInfo bundleInfo = { 0 };
    int result = memset_s(&bundleInfo, sizeof(bundleInfo), 0, sizeof(bundleInfo));
    TEST_ASSERT_TRUE(result == 0);
    bundleInfo.bundleName = "com.openharmony.testjsdemo";
    TEST_ASSERT_EQUAL_STRING(bundleInfo.bundleName, "com.openharmony.testjsdemo");
    ClearBundleInfo(&bundleInfo);
    TEST_ASSERT_EQUAL_STRING(bundleInfo.bundleName, NULL);
    printf("------end testClearBundleInfo------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0004
 * @tc.name      : testClearBundleInfoIllegal
 * @tc.desc      : testClearBundleInfo parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testClearBundleInfoIllegal, Function | MediumTest | Level2)
{
    printf("------start testClearBundleInfoIllegal------\n");
    BundleInfo bundleInfo;
    int result = memset_s(&bundleInfo, sizeof(bundleInfo), 0, sizeof(bundleInfo));
    TEST_ASSERT_TRUE(result == 0);
    bundleInfo.bundleName = "com.openharmony.testjsdemo";
    ClearBundleInfo(NULL);
    TEST_ASSERT_EQUAL_STRING(bundleInfo.bundleName, "com.openharmony.testjsdemo");
    printf("------end testClearBundleInfoIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0005
 * @tc.name      : testClearModuleInfoLegal
 * @tc.desc      : ClearAbilityInfo parameter legal test with module info
 */
LITE_TEST_CASE(BundleMgrTestSuite, testClearModuleInfoLegal, Function | MediumTest | Level1)
{
    printf("------start testClearModuleInfo------\n");
    ModuleInfo moduleInfo = { 0 };
    int result = memset_s(&moduleInfo, sizeof(moduleInfo), 0, sizeof(moduleInfo));
    TEST_ASSERT_TRUE(result == 0);
    moduleInfo.moduleName = "test";
    TEST_ASSERT_EQUAL_STRING(moduleInfo.moduleName, "test");
    ClearModuleInfo(&moduleInfo);
    TEST_ASSERT_EQUAL_STRING(moduleInfo.moduleName, NULL);
    printf("------end testClearModuleInfo------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0006
 * @tc.name      : testClearModuleInfoIllegal
 * @tc.desc      : ClearAbilityInfo parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testClearModuleInfoIllegal, Function | MediumTest | Level1)
{
    printf("------start testClearModuleInfoIllegal------\n");
    ModuleInfo moduleInfo = { 0 };
    int result = memset_s(&moduleInfo, sizeof(moduleInfo), 0, sizeof(moduleInfo));
    TEST_ASSERT_TRUE(result == 0);
    moduleInfo.moduleName = "test";
    ClearModuleInfo(NULL);
    TEST_ASSERT_EQUAL_STRING(moduleInfo.moduleName, "test");
    printf("------end testClearModuleInfoIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0007
 * @tc.name      : testSetElementAbilityNameLegal
 * @tc.desc      : testSetElementAbilityName parameter legal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testSetElementAbilityNameLegal, Function | MediumTest | Level0)
{
    printf("------start testSetElementAbilityName------\n");
    Want want = { 0 };
    ElementName element = { 0 };
    SetElementAbilityName(&element, "SecondAbility");
    SetWantElement(&want, element);
    char *aName = "SecondAbility";
    TEST_ASSERT_EQUAL_STRING(want.element->abilityName, aName);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetElementAbilityName------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0008
 * @tc.name      : testSetElementAbilityNameIllegal
 * @tc.desc      : testSetElementAbilityName parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testSetElementAbilityNameIllegal, Function | MediumTest | Level2)
{
    printf("------start testSetElementAbilityNameIllegal------\n");
    Want want = { 0 };
    ElementName element = { 0 };
    char *aName = "";
    SetElementAbilityName(&element, aName);
    SetWantElement(&want, element);
    TEST_ASSERT_EQUAL_STRING(want.element->abilityName, "");
    SetElementAbilityName(&element, NULL);
    SetWantElement(&want, element);
    TEST_ASSERT_EQUAL_STRING(want.element->abilityName, NULL);
    int ret = SetElementAbilityName(NULL, aName);
    TEST_ASSERT_FALSE(ret);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetElementAbilityNameIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0009
 * @tc.name      : testSetElementBundleNameLegal
 * @tc.desc      : testSetElementBundleName parameter legal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testSetElementBundleNameLegal, Function | MediumTest | Level0)
{
    printf("------start testSetElementBundleName------\n");
    Want want = { 0 };
    ElementName element = { 0 };
    SetElementBundleName(&element, "com.openharmony.testjsdemo");
    SetWantElement(&want, element);
    char *bName = "com.openharmony.testjsdemo";
    TEST_ASSERT_EQUAL_STRING(want.element->bundleName, bName);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetElementBundleName------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0010
 * @tc.name      : testSetElementBundleNameIllegal
 * @tc.desc      : testSetElementBundleName parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testSetElementBundleNameIllegal, Function | MediumTest | Level2)
{
    printf("------start testSetElementBundleNameIllegal------\n");
    Want want = { 0 };
    ElementName element = { 0 };
    SetElementBundleName(&element, "");
    SetWantElement(&want, element);
    char *bName = "";
    TEST_ASSERT_EQUAL_STRING(want.element->bundleName, bName);
    SetElementBundleName(&element, NULL);
    SetWantElement(&want, element);
    TEST_ASSERT_EQUAL_STRING(want.element->bundleName, NULL);
    bool ret = SetElementBundleName(NULL, bName);
    TEST_ASSERT_FALSE(ret);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetElementBundleNameIllegal------\n");
}
/**
 * @tc.number    : SUB_APPEXECFWK_0011
 * @tc.name      : testSetElementDeviceIDLegal
 * @tc.desc      : testSetElementDeviceID parameter legal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testSetElementDeviceIDLegal, Function | MediumTest | Level0)
{
    printf("------start testSetElementDeviceID------\n");
    Want want = { 0 };
    ElementName element = { 0 };
    SetElementDeviceID(&element, "0001000");
    SetWantElement(&want, element);
    char *dID = "0001000";
    TEST_ASSERT_EQUAL_STRING(want.element->deviceId, dID);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetElementDeviceID------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0012
 * @tc.name      : testSetElementDeviceIDIllegal
 * @tc.desc      : testSetElementDeviceID parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testSetElementDeviceIDIllegal, Function | MediumTest | Level2)
{
    printf("------start testSetElementDeviceIDIllegal------\n");
    Want want = { 0 };
    ElementName element = { 0 };
    SetElementDeviceID(&element, "");
    SetWantElement(&want, element);
    char *dID = "";
    TEST_ASSERT_EQUAL_STRING(want.element->deviceId, dID);
    SetElementDeviceID(&element, NULL);
    SetWantElement(&want, element);
    TEST_ASSERT_EQUAL_STRING(want.element->deviceId, NULL);
    int ret = SetElementDeviceID(NULL, "0001000");
    TEST_ASSERT_FALSE(ret);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetElementDeviceIDIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0013
 * @tc.name      : testGetBundleInfoIllegal
 * @tc.desc      : GetBundleInfo parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testGetBundleInfoIllegal, Function | MediumTest | Level2)
{
    printf("------start testGetBundleInfoIllegal------\n");
    BundleInfo bundleInfo;
    int result = memset_s(&bundleInfo, sizeof(bundleInfo), 0, sizeof(bundleInfo));
    TEST_ASSERT_TRUE(result == 0);
    const char *bundleName = "com.openharmony.nothishap";
    int flags = 0;
    uint8_t ret = GetBundleInfo(bundleName, flags, &bundleInfo);
    TEST_ASSERT_TRUE(ret == 2);
    ret = GetBundleInfo(NULL, flags, &bundleInfo);
    printf("abilityInfo2 is %d \n", ret);
    TEST_ASSERT_TRUE(ret == 1);
    ret = GetBundleInfo("", flags, &bundleInfo);
    TEST_ASSERT_TRUE(ret == 2);
    ret = GetBundleInfo("com.openharmony.testjsdemo", 2, &bundleInfo);
    sleep(2);
    TEST_ASSERT_TRUE(ret == 2);
    printf("------end testGetBundleInfoIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_0014
 * @tc.name      : testGetBundleInfosIllegal
 * @tc.desc      : GetBundleInfos parameter illegal test
 */
LITE_TEST_CASE(BundleMgrTestSuite, testGetBundleInfosIllegal, Function | MediumTest | Level2)
{
    printf("------start testGetBundleInfosIllegal------\n");
    BundleInfo *bundleInfos = {NULL};
    int *length = NULL;
    int flags = 0;
    uint8_t ret = GetBundleInfos(flags, NULL, length);
    TEST_ASSERT_TRUE(ret == 1);
    ret = GetBundleInfos(flags, &bundleInfos, NULL);
    printf("ret is %d \n", ret);
    TEST_ASSERT_TRUE(ret == 2);
    ret = GetBundleInfos(2, &bundleInfos, length);
    printf("ret is %d \n", ret);
    TEST_ASSERT_TRUE(ret == 2);
    printf("------end testGetBundleInfosIllegal------\n");
}

RUN_TEST_SUITE(BundleMgrTestSuite);
