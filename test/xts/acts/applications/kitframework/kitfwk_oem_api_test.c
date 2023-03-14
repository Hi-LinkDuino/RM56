/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <stdbool.h>
#include "cJSON.h"
#include "hctest.h"
#include "oem_auth_config.h"
#include "oem_auth_result_storage.h"
#include "token.h"

#define LOG(format, ...)  printf("%s:" format "\r\n", __FUNCTION__, ##__VA_ARGS__)

#define TEST_TOKEN1 "TEST_TOKEN1abPA4o838zOuuq9R3HBiG2JoYk4X+FIfyYS5iV5DTFiyGuy84eZlr,\
qGPCt5czVOLcquVOs91rfwWT/ZiCCeN7+BXoj6R5ez2NC3JTf5y3wh0kx0twMwmN,0000000000000000,0000"
#define TEST_TOKEN2 "TEST_TOKEN2YisBwKb2M3rsytbhJrDlI348Ch0XHIahlG2CaJUTQyPQlAqRThHa0,\
MbucBf5K9uFnzJyUSj+1u6Ro1jX4xVM0JP4P7FngyAvro4DCmK1Pjq5btHrtceve,0000000000000000,0100"
#define TEST_TICKET "1234156456123134564646631315646465"
#define TEST_AUTH_STATUS ".abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890\
sdi73fabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz\
09uvuyabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz."

#define MAX_SERVER_INFO_LEN  256
#define ENCRYPT_TOKEN_LEN 151
#define ENCRYPT_TICKET_LEN 64
#define STATUS_LEN 320
#define PRODUCT_ID_LEN 4
#define ACKEY_LEN 48
#define PRODUCT_KEY_LEN 32

LITE_TEST_SUIT(applications, KitFwkOemApi, KitFwkOemApiTestSuite);

bool g_isFirstRun = true;

static BOOL KitFwkOemApiTestSuiteSetUp(void)
{
    LOG("========setup========");
    bool ret = OEMIsResetFlagExist();
    if (ret) {
        LOG("======== this is not the first run of test after flash ========\n");
        g_isFirstRun = false;
    } else {
        LOG("======== this is the first run of test after flash ========\n");
    }
    return TRUE;
}

static BOOL KitFwkOemApiTestSuiteTearDown(void)
{
    LOG("========tearDown========");
    int ret = OEMCreateResetFlag();
    if (ret) {
        LOG("======== OEMCreateResetFlag failed! ========");
    } else {
        LOG("======== OEMCreateResetFlag ok! ========");
    }
    return TRUE;
}

#ifndef TOKEN_PERSIST_TEST

/**
 * @tc.name : TestReadAuthServerInfo
 * @tc.desc : test case of OEMReadAuthServerInfo api.
              no assert for this case, comiple ok means test pass.
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestReadAuthServerInfo, Function | MediumTest | Level0)
{
    LOG("----- OEMReadAuthServerInfo api test start -----");
    char* serverInfo = malloc(MAX_SERVER_INFO_LEN + 1);
    if (serverInfo == NULL) {
        LOG("malloc fail, can't do test!");
        TEST_FAIL();
        return;
    }
    int ret = OEMReadAuthServerInfo(serverInfo, MAX_SERVER_INFO_LEN);
    if (ret) {
        LOG("no customized server info.");
    } else {
        LOG("customized server info: %s", serverInfo);
    }
}

/**
 * @tc.name : TestLoadKitInfos
 * @tc.desc : test that string return from OEMLoadKitInfos is a valid json string
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestLoadKitInfos, Function | MediumTest | Level0)
{
    LOG("----- OEMLoadKitInfos api test start -----");
    char* kitInfoMsg = OEMLoadKitInfos();
    if (kitInfoMsg == NULL) {
        LOG("No other kit infos");
        return;
    }
    LOG("kit info: %s", kitInfoMsg);
    cJSON* root = cJSON_Parse(kitInfoMsg);
    free(kitInfoMsg);

    TEST_ASSERT_NOT_NULL_MESSAGE(root, "Invalid json format!");

    cJSON* kitInfoArray = cJSON_GetObjectItem(root, KIT_INFO_JSON_KEY);
    TEST_ASSERT_NOT_NULL_MESSAGE(kitInfoArray, "Invalid json format!");
    cJSON_Delete(root);
}

/**
 * @tc.name : TestIsOverTemperatureLimit
 * @tc.desc : test case of temperature api.
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestIsOverTemperatureLimit, Function | MediumTest | Level0)
{
    LOG("----- temperature api test start -----");
    bool results = OEMIsOverTemperatureLimit();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, results, "OEMIsOverTemperatureLimit should fail in normal state");
}

/**
 * @tc.name: TestResetFlagApi
 * @tc.desc: test case of the reset flag related api: create, check
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestResetFlagApi, Function | MediumTest | Level0)
{
    LOG("----- reset flag api test start -----");
    if (g_isFirstRun) {
        int ret = OEMCreateResetFlag();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to create the reset flag");
        bool flag = OEMIsResetFlagExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, flag, "the reset flag should exist.");
    } else {
        bool flag = OEMIsResetFlagExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, flag, "the reset flag should exist.");
        int ret = OEMDeleteResetFlag();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to delete the Reset flag");
        flag = OEMIsResetFlagExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, flag, "the reset flag should not exist.");
    }
}

/**
 * @tc.name: TestAuthStatusApi
 * @tc.desc: test case of the authStatus related api: read, write, delete, check
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestAuthStatusApi, Function | MediumTest | Level0)
{
    LOG("----- auth status api test start -----");
    uint32_t len = strlen(TEST_AUTH_STATUS);
    char* statusRead = malloc(len + 1);
    if (statusRead == NULL) {
        LOG("malloc fail, can't do test!");
        TEST_FAIL();
        return;
    }
    if (g_isFirstRun) {
        int ret = OEMWriteAuthStatus(TEST_AUTH_STATUS, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to write auth data");
        bool isExist = OEMIsAuthStatusExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, isExist, "authStatus should exist!");
        ret = OEMReadAuthStatus(statusRead, len);
        LOG("statusRead:%s", statusRead);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read authStatus");
        statusRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_AUTH_STATUS, statusRead, "authStatus not match!");
        uint32_t newLen = 0;
        ret = OEMGetAuthStatusFileSize(&newLen);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "failed to get authStatus size");
        // Check whether the obtained length is the same as the initial written length.
        TEST_ASSERT_EQUAL_INT_MESSAGE(len, newLen, "Failed: length not match!");
    } else {
        bool isExist = OEMIsAuthStatusExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, isExist, "authStatus should exist!");
        int ret = OEMReadAuthStatus(statusRead, len);
        LOG("statusRead:%s", statusRead);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read auth status");
        statusRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_AUTH_STATUS, statusRead, "authStatus not match!");
        uint32_t newLen = 0;
        ret = OEMGetAuthStatusFileSize(&newLen);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "failed to get authStatus size");
        TEST_ASSERT_EQUAL_INT_MESSAGE(len, newLen, "Failed: length not match!");

        ret = OEMDeleteAuthStatus();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "failed to delete authStatus");
        isExist = OEMIsAuthStatusExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, isExist, "file still exist!");
    }
    free(statusRead);
}

/**
 * @tc.name: TestTicketApi
 * @tc.desc: test case of the ticket related api: read, write, delete, check
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestTicketApi, Function | MediumTest | Level0)
{
    LOG("----- ticket api test start -----");
    uint32_t len = strlen(TEST_TICKET);
    char* ticketRead = malloc(len + 1);
    if (ticketRead == NULL) {
        LOG("malloc fail, can't do test!");
        TEST_FAIL();
        return;
    }
    if (g_isFirstRun) {
        int ret = OEMWriteTicket(TEST_TICKET, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to write ticket");
        bool isExist = OEMIsTicketExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, isExist, "ticket should exist!");
        ret = OEMReadTicket(ticketRead, len);
        LOG("ticketRead:%s", ticketRead);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to tead ticket");
        ticketRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_TICKET, ticketRead, "ticket not match");
        uint32_t newLen = 0;
        ret = OEMGetTicketFileSize(&newLen);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to get ticket size");
        TEST_ASSERT_EQUAL_INT_MESSAGE(len, newLen, "length not match!");
    } else {
        bool isExist = OEMIsTicketExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, isExist, "ticket not does exist!");
        int ret = OEMReadTicket(ticketRead, len);
        LOG("ticketRead:%s", ticketRead);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read ticket");
        ticketRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_TICKET, ticketRead, "ticket not match");
        uint32_t newLen = 0;
        ret = OEMGetTicketFileSize(&newLen);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "failed to get ticket size");
        TEST_ASSERT_EQUAL_INT_MESSAGE(len, newLen, "length not match!");

        ret = OEMDeleteTicket();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to delete ticket");
        isExist = OEMIsTicketExist();
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, isExist, "file still exist!");
    }
    free(ticketRead);
}

/**
 * @tc.name: TestTokenApi
 * @tc.desc: test case of the token related api: read, write
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestTokenApi, Function | MediumTest | Level0)
{
    LOG("----- token api test start -----");
    uint32_t len = strlen(TEST_TOKEN1);
    char* tokenRead = malloc(len + 1);
    if (tokenRead == NULL) {
        LOG("malloc fail, can't do test!");
        TEST_FAIL();
        return;
    }
    if (g_isFirstRun) {
        int ret = ReadToken(tokenRead, len);
        if (!ret) {
            // print old token if existed, in case it's a valid 'true-token'
            LOG("old token in device:%s", tokenRead);
        }
        // write then read token check
        ret = WriteToken(TEST_TOKEN1, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to write token");
        ret = ReadToken(tokenRead, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read token");
        tokenRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_TOKEN1, tokenRead, "token not match");

        // write then read token check, again. for A-B area check
        ret = WriteToken(TEST_TOKEN2, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to write token");
        ret = ReadToken(tokenRead, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read token");
        tokenRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_TOKEN2, tokenRead, "token not match");
    } else {
        int ret = ReadToken(tokenRead, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read token");
        tokenRead[len] = 0;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_TOKEN2, tokenRead, "token not match");
    }
    free(tokenRead);
}

/**
 * @tc.name: TestGetAckeyApi
 * @tc.desc: Check whether the Ackey can be obtained.
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestGetAckeyApi, Function | MediumTest | Level0)
{
    LOG("----- test get ackey start -----");
    char acKey[ACKEY_LEN];
    unsigned int len = sizeof(acKey);
    int ret = GetAcKey(acKey, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to get the ackey");
}

/**
 * @tc.name: TestGetProdIdApi
 * @tc.desc: Check whether the ProdId can be obtained.
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestGetProdIdApi, Function | MediumTest | Level0)
{
    LOG("----- test getprodid start -----");
    char productId[PRODUCT_ID_LEN];
    int ret = GetProdId(productId, sizeof(productId));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to get the productid");
}

/**
 * @tc.name: TestGetProdKeyApi
 * @tc.desc: Check whether the Prodkey can be obtained.
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestGetProdKeyApi, Function | MediumTest | Level0)
{
    LOG("----- test getproductkey start -----");
    char productKey[PRODUCT_KEY_LEN];
    int ret = GetProdKey(productKey, sizeof(productKey));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to get the productkey");
}

#else  // defined TOKEN_PERSIST_TEST

/**
 * @tc.name: TestIfTokenPersist
 * @tc.desc: test case of the token related api: read, write
 */
LITE_TEST_CASE(KitFwkOemApiTestSuite, TestIfTokenPersist, Function | MediumTest | Level0)
{
    LOG("----- token api test start -----");
    uint32_t len = strlen(TEST_TOKEN2);
    char* tokenRead = malloc(len + 1);
    if (tokenRead == NULL) {
        LOG("malloc fail, can't do test!");
        TEST_FAIL();
        return;
    }
    int ret = ReadToken(tokenRead, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Failed to read token");
    tokenRead[len] = 0;
    LOG("tokenRead:%s", tokenRead);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_TOKEN2, tokenRead, "token not match");

    free(tokenRead);
}
#endif

RUN_TEST_SUITE(KitFwkOemApiTestSuite);
