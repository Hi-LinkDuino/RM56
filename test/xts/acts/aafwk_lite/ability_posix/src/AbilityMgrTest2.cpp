/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <log.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "gtest/gtest.h"
#include "securec.h"

#include "ability.h"
#include "ability_connection.h"
#include "ability_env.h"
#include "ability_event_handler.h"
#include "ability_manager.h"
#include "ability_slice.h"
#include "abilityms_client.h"
#include "bundle_manager.h"
#include "want.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS;
static int32_t g_errorCode = -1;
static sem_t g_sem;
static const int32_t WAIT_TIMEOUT = 30;
static bool g_installState = false;
static string g_testPath;

extern "C" {
void __attribute__((weak)) HOS_SystemInit(void){};
}

/* install callback */
static void TestBundleStateCallback(const uint8_t resultCode, const void *resultMessage)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "TestBundleStateCallback resultCode: %d", resultCode);
    HILOG_DEBUG(HILOG_MODULE_APP, "TestBundleStateCallback resultMessage: %s", (char *) resultMessage);
    g_installState = (resultCode == 0);
    g_errorCode = resultCode;
    sem_post(&g_sem);
}

/* *
 * get current dir
 * @return  string current file path of the test suits
 */
static string GetCurDir()
{
    string filePath = "";
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("get file path error");
    } else {
        printf("Current Dir: %s\r\n", buffer);
        filePath = buffer;
        free(buffer);
    }
    return filePath + "/";
}

/* connectAbility callback */
static void OnAbilityConnectDone(ElementName *elementName, SvcIdentity *serviceSid, int resultCode, void *storeData)
{
    printf("OnAbilityConnectDone, serviceSid is %p \n", serviceSid);
    printf("elementName is %s, %s \n", elementName->bundleName, elementName->abilityName);
    ClearElement(elementName);
    if (serviceSid == nullptr) {
        return;
    }
    printf("ipc callback success \n");
    // push data
    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, 0);
    int32_t data1 = 10;
    int32_t data2 = 6;
    IpcIoPushInt32(&request, data1);
    IpcIoPushInt32(&request, data2);
    // send and getReply
    IpcIo reply = {nullptr};
    uintptr_t ptr = 0;
    Transact(NULL, *serviceSid, 0, &request, &reply, LITEIPC_FLAG_DEFAULT, &ptr);
    g_errorCode = IpcIoPopInt32(&reply);
    if (g_errorCode != 0) {
        printf("execute add method, result is %d\n", g_errorCode);
    }
    if (ptr != 0) {
        FreeBuffer(nullptr, reinterpret_cast<void *>(ptr));
    }
    sem_post(&g_sem);
}

static void OnAbilityDisconnectDone(ElementName (void)*elementName, int resultCode, void *storeData)
{
    printf("OnAbilityDisconnectDone\n");
}


static IAbilityConnection g_conn = {
    .OnAbilityConnectDone = OnAbilityConnectDone,
    .OnAbilityDisconnectDone = OnAbilityDisconnectDone
};

class AbilityMgrTest2 : public testing::Test {
protected:
    static void SetUpTestCase(void)
    {
        printf("----------test case with AbilityMgrTest2 start-------------\n");
        HOS_SystemInit();
        AbilityMsClient::GetInstance().Initialize();
        sem_init(&g_sem, 0, 0);
        bool installResult = false;
        InstallParam installParam = { .installLocation = 1, .keepData = false };
        g_testPath = GetCurDir();
#ifdef __LINUX__
        string hapPath = g_testPath + "testnative_hispark_aries_linux.hap";
#else
        string hapPath = g_testPath + "testnative_hispark_aries_liteos.hap";
#endif
        installResult = Install(hapPath.c_str(), &installParam, TestBundleStateCallback);
        struct timespec ts = {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += WAIT_TIMEOUT;
        sem_timedwait(&g_sem, &ts);
        if (installResult) {
            printf("sem exit \n");
        }
    }
    static void TearDownTestCase(void)
    {
        bool uninstallResult = false;
        sem_init(&g_sem, 0, 0);
        InstallParam installParam = { .installLocation = 1, .keepData = false };
        uninstallResult = Uninstall("com.openharmony.testnative", &installParam, TestBundleStateCallback);
        sem_wait(&g_sem);
        if (uninstallResult) {
            printf("sem exit \n");
        }
        printf("----------test case with AbilityMgrTest2 end-------------\n");
    }
};

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0013
 * @tc.name      : testClearElement parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testClearElement, Function | MediumTest | Level0)
{
    printf("------start testClearElement------\n");
    ElementName element = { nullptr };
    bool setResult = SetElementAbilityName(&element, "ServiceAbility");
    if (setResult) {
        char aName[] = "ServiceAbility";
        EXPECT_STREQ(element.abilityName, aName);
        printf("abilityName is %s \n", element.abilityName);
        ClearElement(&element);
        EXPECT_STREQ(element.abilityName, nullptr);
    }
    printf("------end testClearElement------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0014
 * @tc.name      : testClearElement parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testClearElementIllegal, Function | MediumTest | Level2)
{
    printf("------start testClearElementIllegal------\n");
    ElementName element = { nullptr };
    bool setResult = SetElementAbilityName(&element, "ServiceAbility");
    if (setResult) {
        char aName[] = "ServiceAbility";
        EXPECT_STREQ(element.abilityName, aName);
        printf("abilityName is %s \n", element.abilityName);
        ClearElement(nullptr);
        EXPECT_STREQ(element.abilityName, aName);
        printf("AbilityName of element is %s \n", element.abilityName);
    }
    printf("------end testClearElementIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0015
 * @tc.name      : testSetWantElement parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testSetWantElement, Function | MediumTest | Level0)
{
    printf("------start testSetWantElement------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementDeviceID(&element, "0001000");
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    if (element.abilityName != nullptr) {
        bool setResult = SetWantElement(&want, element);
        if (setResult) {
            EXPECT_STREQ(want.element->deviceId, "0001000");
            EXPECT_STREQ(want.element->abilityName, "ServiceAbility");
            EXPECT_STREQ(want.element->bundleName, "com.openharmony.testnative");
        }
    }
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetWantElement------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0016
 * @tc.name      : testSetWantElement parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testSetWantElementIllegal, Function | MediumTest | Level2)
{
    printf("------start testSetWantElementIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    bool setResult = SetWantElement(&want, element);
    if (setResult) {
        EXPECT_STREQ(want.element->deviceId, nullptr);
        EXPECT_STREQ(want.element->abilityName, nullptr);
        EXPECT_STREQ(want.element->bundleName, nullptr);
    }
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testSetWantElementIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0022
 * @tc.name      : testClearWant parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testClearWantIllegal, Function | MediumTest | Level2)
{
    printf("------start testClearWantIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    bool setResult = SetElementAbilityName(&element, "ServiceAbility");
    if (setResult) {
        setResult = SetWantElement(&want, element);
        if (setResult) {
            char aName[] = "ServiceAbility";
            EXPECT_STREQ(want.element->abilityName, aName);
            printf("abilityName is %s \n", want.element->abilityName);
            ClearWant(nullptr);
            EXPECT_STREQ(want.element->abilityName, aName);
        }
    }
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testClearWantIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0025
 * @tc.name      : testWantToUri parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantToUri, Function | MediumTest | Level0)
{
    printf("------start testWantToUri------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementDeviceID(&element, "0001000");
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "SecondAbility");
    if (element.abilityName !=nullptr) {
        bool setResult = SetWantElement(&want, element);
        if (setResult) {
            const char *uri = WantToUri(want);
            printf("uri is %s \n", uri);
            const char *expectResult = "#Want;device=0001000;bundle=com.openharmony.testnative;ability=SecondAbility;end";
            EXPECT_STREQ(uri, expectResult);
            free((void*)uri);
        }
    }
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantToUri------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0026
 * @tc.name      : testWantToUri parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantToUriIllegal, Function | MediumTest | Level2)
{
    printf("------start testWantToUriIllegal------\n");
    Want want = { nullptr };
    const char *uri = WantToUri(want);
    printf("uri is %s \n", uri);
    const char *expectResult = "#Want;device=;bundle=;ability=;end";
    EXPECT_STREQ(uri, expectResult);
    if (uri != nullptr) {
        free((void*)uri);
    }
    printf("------end testWantToUriIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0017
 * @tc.name      : testSetWantDate parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testSetWantDate, Function | MediumTest | Level0)
{
    printf("------start testSetWantDate------\n");
    Want want = { nullptr };
    SetWantData(&want, "test", 5);
    if (want.data != nullptr) {
        printf("data is %s \n", (char*)(want.data));
        printf("dataLength is %d \n", want.dataLength);
        EXPECT_STREQ((char*)(want.data), "test");
        EXPECT_TRUE(want.dataLength == 5);
    }
    ClearWant(&want);
    printf("------end testSetWantDate------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0018
 * @tc.name      : testSetWantDate parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testSetWantDateIllegal, Function | MediumTest | Level2)
{
    printf("------start testSetWantDateIllegal------\n");
    Want want = { nullptr };
    SetWantData(&want, "test", -1);
    printf("dataLength is %d \n", want.dataLength);
    EXPECT_STREQ((char*)(want.data), nullptr);
    EXPECT_TRUE(want.dataLength == 0);
    SetWantData(&want, nullptr, 0);
    printf("dataLength is %d \n", want.dataLength);
    EXPECT_STREQ((char*)(want.data), nullptr);
    EXPECT_TRUE(want.dataLength == 0);
    printf("------end testSetWantDateIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0023
 * @tc.name      : testWantParseUri parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantParseUri, Function | MediumTest | Level0)
{
    printf("------start testWantParseUri------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementDeviceID(&element, "0001000");
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    if (element.abilityName != nullptr) {
        bool setResult = SetWantElement(&want, element);
        if (setResult) {
            const char *uri = WantToUri(want);
            Want *want2 = WantParseUri(uri);
            printf("uri is %s \n", uri);
            if (uri != nullptr) {
                free((void*)uri);
            }
            EXPECT_STREQ(want2->element->deviceId, want.element->deviceId);
            EXPECT_STREQ(want2->element->abilityName, want.element->abilityName);
            EXPECT_STREQ(want2->element->bundleName, want.element->bundleName);
            free(want2);
        }
    }
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantParseUri------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0024
 * @tc.name      : testWantParseUri parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantParseUriIllegal, Function | MediumTest | Level2)
{
    printf("------start testWantParseUriIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, nullptr);
    SetWantElement(&want, element);
    const char *uri = WantToUri(want);
    // empty
    printf("uri is %s \n", uri);
    Want *want2 = WantParseUri(uri);
    if (uri != nullptr) {
        free((void*)uri);
    }
    if (want2 != nullptr) {
        printf("want is %s \n", want2->element->bundleName);
        EXPECT_STREQ(want2->element->deviceId, "");
        EXPECT_STREQ(want2->element->abilityName, "");
        EXPECT_STREQ(want2->element->bundleName, "");
        free(want2);
    }
    // nullptr
    Want *want4 = WantParseUri(nullptr);
    printf("want4 is %p \n", want4);
    EXPECT_TRUE(want4 == nullptr);
    const char *str = "test";
    // error format
    Want *want1 = WantParseUri(str);
    printf("want is %p \n", want1);
    EXPECT_TRUE(want1 == nullptr);
    Want *want3 = WantParseUri("");
    printf("want is %p \n", want3);
    EXPECT_TRUE(want3 == nullptr);
    free(want1);
    free(want3);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantParseUriIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0030
 * @tc.name      : testGetBundleNameIllegal parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testGetBundleNameIllegal, Function | MediumTest | Level1)
{
    printf("------start testGetBundleNameIllegal------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    int result = StartAbility(&want);
    sleep(2);
    printf("ret is %d \n", result);
    const char * bundleName1 = GetBundleName();
    printf("result of GetBundleName is %s \n", bundleName1);
    EXPECT_STREQ(bundleName1, "");
    g_errorCode = StopAbility(&want);
    sleep(2);
    printf("------end testGetBundleNameIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0031
 * @tc.name      : testGetSrcPathIllegal parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testGetSrcPathIllegal, Function | MediumTest | Level1)
{
    printf("------start testGetSrcPathIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    int result = StartAbility(&want);
    sleep(2);
    printf("ret is %d \n", result);
    const char * srcPath = GetSrcPath();
    printf("result of GetSrcPath is %s \n", srcPath);
    EXPECT_STREQ(srcPath, "");
    g_errorCode = StopAbility(&want);
    sleep(2);
    printf("------end testGetSrcPathIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0032
 * @tc.name      : testGetDataPath parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testGetDataPathIllegal, Function | MediumTest | Level1)
{
    printf("------start testGetDataPathIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    int result = StartAbility(&want);
    sleep(2);
    printf("ret is %d \n", result);
    const char * dataPath = GetDataPath();
    printf("result of GetDataPath is %s \n", dataPath);
    EXPECT_STREQ(dataPath, "");
    g_errorCode = StopAbility(&want);
    sleep(2);
    printf("------end testGetDataPathIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0019
 * @tc.name      : testDump parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testDumpIllegal, Function | MediumTest | Level1)
{
    printf("------start testDump------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    Ability *ability = new Ability();
    int result = ability->StartAbility(want);
    sleep(2);
    printf("ret is %d \n", result);
    EXPECT_EQ(result, 0);
    char *extra = (char*)"test";
    ability->Dump(extra);
    g_errorCode = StopAbility(&want);
    sleep(2);
    printf("------end testDump------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0033
 * @tc.name      : testStartAbility parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testStartAbility, Function | MediumTest | Level1)
{
    printf("------start testStartAbility------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    int result = StartAbility(&want);
    sleep(2);
    printf("ret is %d \n", result);
    EXPECT_EQ(result, 0);
    g_errorCode = StopAbility(&want);
    sleep(2);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testStartAbility------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0005
 * @tc.name      : testStartAbilityIllegal parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testStartAbilityIllegal, Function | MediumTest | Level2)
{
    printf("------start testStartAbilityIllegal------\n");
    int result = StartAbility(nullptr);
    printf("ret is %d \n", result);
    int expect = -1;
    EXPECT_EQ(result, expect);
    printf("------end testStartAbilityIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0006
 * @tc.name      : testStopAbility parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testStopAbility, Function | MediumTest | Level0)
{
    printf("------start testStopAbility------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    g_errorCode = StartAbility(&want);
    sleep(2);
    printf("ret is %d \n", g_errorCode);
    EXPECT_EQ(g_errorCode, 0);
    g_errorCode = StopAbility(&want);
    sleep(2);
    printf("ret of stop is %d \n", g_errorCode);
    EXPECT_EQ(g_errorCode, 0);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testStopAbility------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0007
 * @tc.name      : testStopAbilityIllegal parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testStopAbilityIllegal, Function | MediumTest | Level2)
{
    printf("------start testStopAbilityIllegal------\n");
    g_errorCode = StopAbility(nullptr);
    printf("ret of stop is %d \n", g_errorCode);
    EXPECT_EQ(g_errorCode, -1);
    printf("------end testStopAbilityIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0008
 * @tc.name      : testConnectAbility parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testConnectAbility, Function | MediumTest | Level1)
{
    printf("------start testConnectAbility------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    sem_init(&g_sem, 0, 0);
    int result = ConnectAbility(&want, &g_conn, this);
    struct timespec ts = {};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WAIT_TIMEOUT;
    sem_timedwait(&g_sem, &ts);
    printf("sem exit \n");
    printf("ret is %d \n ", result);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(g_errorCode, 16);
    DisconnectAbility(&g_conn);
    sleep(1);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testConnectAbility------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0010
 * @tc.name      : testWantMathBundle
 * @tc.desc      : [C- SOFTWARE -0100]
 * @tc.author    : lijiashan
 */
HWTEST_F(AbilityMgrTest2, testDisConnectAbility, Function | MediumTest | Level1)
{
    printf("------start testDisConnectAbility------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    sem_init(&g_sem, 0, 0);
    int result = ConnectAbility(&want, &g_conn, this);
    struct timespec ts = {};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WAIT_TIMEOUT;
    sem_timedwait(&g_sem, &ts);
    printf("sem exit \n");
    printf("ret of connect is %d \n ", result);
    EXPECT_EQ(g_errorCode, 16);
    if (g_errorCode == 16) {
        result = DisconnectAbility(&g_conn);
        sleep(2);
        EXPECT_EQ(result, 0);
        printf("ret of disconnect is %d \n ", result);
    }
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testDisConnectAbility------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0009
 * @tc.name      : testConnectAbilityIllegal parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testConnectAbilityIllegal, Function | MediumTest | Level1)
{
    printf("------start testConnectAbilityIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    g_errorCode = ConnectAbility(nullptr, &g_conn, this);
    printf("ret1 is %d \n ", g_errorCode);
    EXPECT_EQ(g_errorCode, -1);
    g_errorCode = ConnectAbility(&want, nullptr, this);
    printf("ret2 is %d \n ", g_errorCode);
    EXPECT_EQ(g_errorCode, -1);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testConnectAbilityIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0011
 * @tc.name      : testDisConnectAbilityIllegal parameter illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testDisConnectAbilityIllegal, Function | MediumTest | Level1)
{
    printf("------start testDisConnectAbilityIllegal------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    sem_init(&g_sem, 0, 0);
    int result = ConnectAbility(&want, &g_conn, this);
    struct timespec ts = {};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WAIT_TIMEOUT;
    sem_timedwait(&g_sem, &ts);
    printf("sem exit \n");
    printf("ret is of connect is %d \n ", g_errorCode);
    EXPECT_EQ(g_errorCode, 16);
    EXPECT_EQ(result, 0);
    g_errorCode = DisconnectAbility(nullptr);
    int expect = -10;
    EXPECT_EQ(g_errorCode, expect);
    printf("ret of disconnect is %d \n ", g_errorCode);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testDisConnectAbilityIllegal------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0012
 * @tc.name      : testTerminateAbility parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testTerminateAbility, Function | MediumTest | Level1)
{
    printf("------start testTerminateAbility------\n");
    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementAbilityName(&element, "ServiceAbility");
    SetWantElement(&want, element);
    int result1 = StartAbility(&want);
    sleep(2);
    printf("result1 of startAbility is %d \n", result1);
    EXPECT_EQ(result1, 0);
    Ability *ability = new Ability();
    int result2 = ability->TerminateAbility();
    sleep(2);
    printf("result2 of TerminateAbility is %d \n", result2);
    EXPECT_EQ(result2, 0);
    ClearElement(&element);
    ClearWant(&want);
    delete ability;
    printf("------end testTerminateAbility------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_WANT_0001
 * @tc.name      : test Want Match BundleInfo
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantMatchBundle, Function | MediumTest | Level1)
{
    printf("------start testWantMathBundle------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementAbilityName(&element, "ServiceAbility");
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetElementDeviceID(&element, "");
    SetWantElement(&want, element);
    AbilityInfo abilityInfo;
    (void)memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo));
    g_errorCode = QueryAbilityInfo(&want, &abilityInfo);
    printf("ret of query is %d \n", g_errorCode);
    EXPECT_EQ(g_errorCode, 0);
    if (g_errorCode == 0) {
        printf("abilityInfo.name is %s \n", abilityInfo.name);
    }
    int result = StartAbility(&want);
    sleep(2);
    printf("result of startAbility is %d \n", result);
    EXPECT_EQ(result, 0);
    printf("element is %s \n", want.element->bundleName);
    printf("element is %s \n", want.element->abilityName);
    StopAbility(&want);
    sleep(1);
    g_errorCode = StopAbility(&want);
    sleep(2);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantMathBundle------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_WANT_0004
 * @tc.name      : test Want Not Match BundleInfo
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantNotMathBundle, Function | MediumTest | Level2)
{
    printf("------start testWantNotMathBundle------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    std::string aName = "NoThisAbility";
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementAbilityName(&element, aName.c_str());
    SetElementBundleName(&element, "com.openharmony.nothishap");
    SetWantElement(&want, element);
    AbilityInfo abilityInfo;
    (void)memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo));
    g_errorCode = QueryAbilityInfo(&want, &abilityInfo);
    printf("ret is %d \n", g_errorCode);
    EXPECT_TRUE(g_errorCode != 0);
    int result = StartAbility(&want);
    sleep(2);
    printf("result of startAbility is %d \n", result);
    EXPECT_TRUE(result == 0);
    printf("element is %s \n", want.element->bundleName);
    g_errorCode = StopAbility(&want);
    sleep(2);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantNotMathBundle------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_WANT_0002
 * @tc.name      : testWantOnlyMathBundle
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantOnlyMathBundleName, Function | MediumTest | Level1)
{
    printf("------start testWantOnlyMathBundleName------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    std::string aName = "Ability";
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementAbilityName(&element, aName.c_str());
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetWantElement(&want, element);
    AbilityInfo abilityInfo;
    (void)memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo));
    g_errorCode = QueryAbilityInfo(&want, &abilityInfo);
    printf("ret is %d \n", g_errorCode);
    EXPECT_TRUE(g_errorCode != 0);
    int result = StartAbility(&want);
    sleep(2);
    printf("result of startAbility is %d \n", result);
    EXPECT_TRUE(result == 0);
    printf("element is %s \n", want.element->bundleName);
    g_errorCode = StopAbility(&want);
    sleep(2);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantOnlyMathBundleName------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_WANT_0003
 * @tc.name      : testWantOnlyMathAbility
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantOnlyMathAbility, Function | MediumTest | Level1)
{
    printf("------start testWantOnlyMathAbility------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    std::string aName = "ServiceAbility";
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementAbilityName(&element, aName.c_str());
    SetElementBundleName(&element, "com.openharmony.test");
    SetWantElement(&want, element);
    AbilityInfo abilityInfo;
    (void)memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo));
    g_errorCode = QueryAbilityInfo(&want, &abilityInfo);
    printf("ret is %d \n", g_errorCode);
    EXPECT_TRUE(g_errorCode != 0);
    int result = StartAbility(&want);
    sleep(2);
    printf("result of startAbility is %d \n", result);
    EXPECT_TRUE(result == 0);
    printf("element is %s \n", want.element->abilityName);
    g_errorCode = StopAbility(&want);
    sleep(2);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantOnlyMathAbility------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_WANT_0005
 * @tc.name      : test WantData Match DataLength
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantDataMatchLength, Function | MediumTest | Level1)
{
    printf("------start testWantDataMatchLength------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementAbilityName(&element, "ServiceAbility");
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetWantElement(&want, element);
    SetWantData(&want, "test", 5);
    AbilityInfo abilityInfo;
    (void)memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo));
    g_errorCode = QueryAbilityInfo(&want, &abilityInfo);
    printf("ret is %d \n", g_errorCode);
    EXPECT_TRUE(g_errorCode == 0);
    int result = StartAbility(&want);
    sleep(2);
    printf("result of startAbility is %d \n", result);
    EXPECT_TRUE(result == 0);
    EXPECT_STREQ((char*)(want.data), "test");
    EXPECT_EQ(want.dataLength, 5);
    StopAbility(&want);
    sleep(1);
    printf("------end testWantDataMatchLength------\n");
}


/**
 * @tc.number    : SUB_APPEXECFWK_AMS_WANT_0006
 * @tc.name      : test WantData Not Match DataLength
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testWantDataNotMatchLength, Function | MediumTest | Level2)
{
    printf("------start testWantDataNotMatchLength------\n");
    Want want;
    (void)memset_s(&want, sizeof(Want), 0, sizeof(Want));
    ElementName element;
    (void)memset_s(&element, sizeof(ElementName), 0, sizeof(ElementName));
    SetElementAbilityName(&element, "ServiceAbility");
    SetElementBundleName(&element, "com.openharmony.testnative");
    SetWantElement(&want, element);
    SetWantData(&want, "test", 3);
    int result = StartAbility(&want);
    sleep(2);
    printf("result of startAbility is %d \n", result);
    EXPECT_TRUE(result == 0);
    EXPECT_EQ(want.dataLength, 3);
    g_errorCode = StopAbility(&want);
    sleep(2);
    ClearElement(&element);
    ClearWant(&want);
    printf("------end testWantDataNotMatchLength------\n");
}

/**
 * @tc.number    : SUB_APPEXECFWK_AMS_API_0040
 * @tc.name      : PostTask parameter illegal test that callback is null
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AbilityMgrTest2, testPostTask, Function | MediumTest | Level1)
{
    printf("------start testPostTask------\n");
#ifdef __LINUX__
    string hapPath = g_testPath + "testnative_hispark_aries_linux.hap";
#else
    string hapPath = g_testPath + "testnative_hispark_aries_liteos.hap";
#endif
    AbilityEventHandler eventHandler1;
    auto task = [this, hapPath]{
        sem_init(&g_sem, 0, 0);
        InstallParam installParam = { .installLocation = 1, .keepData = false };
        bool installResult = Install(hapPath.c_str(), &installParam, TestBundleStateCallback);
        sem_wait(&g_sem);
        printf("installResult is %d \n", installResult);
        EXPECT_TRUE(installResult);

        AbilityEventHandler *eventHandler2 = AbilityEventHandler::GetCurrentHandler();
        eventHandler2->PostQuit();
    };
    eventHandler1.PostTask(task);
    eventHandler1.Run();
    printf("------end testPostTask------\n");
}
