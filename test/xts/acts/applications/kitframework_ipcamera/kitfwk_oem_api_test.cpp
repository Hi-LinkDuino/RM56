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

#include <cstring>
#include <cstdbool>
#include "gtest/gtest.h"
#include "cJSON.h"
#include "oem_auth_config.h"
#include "oem_auth_result_storage.h"
#include "token.h"

using namespace testing::ext;
#define LOG(format, ...)  printf("%s:" format "\r\n", __FUNCTION__, ##__VA_ARGS__)

#define TEST_TOKEN1 "TEST_TOKEN1abPA4o838zOuuq9R3HBiG2JoYk4X+FIfyYS5iV5DTFiyGuy84eZlr,\
qGPCt5czVOLcquVOs91rfwWT/ZiCCeN7+BXoj6R5ez2NC3JTf5y3wh0kx0twMwmN,0000000000000000,0000"
#define TEST_TOKEN2 "TEST_TOKEN2YisBwKb2M3rsytbhJrDlI348Ch0XHIahlG2CaJUTQyPQlAqRThHa0,\
MbucBf5K9uFnzJyUSj+1u6Ro1jX4xVM0JP4P7FngyAvro4DCmK1Pjq5btHrtceve,0000000000000000,0100"
#define TEST_TICKET "1234156456123134564646631315646465"
#define TEST_AUTH_STATUS ".abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890\
sdi73fabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz\
09uvuyabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz."


const int PRODUCT_KEY_LEN = 32;
const int ACKEY_LEN = 48;
const int PRODUCT_ID_LEN = 4;
const int STATUS_LEN = 320;
const int ENCRYPT_TICKET_LEN = 64;
const int ENCRYPT_TOKEN_LEN = 151;
const int MAX_SERVER_INFO_LEN = 256;



bool g_isFirstRun = true;
class OEMAPITEST : public testing::Test {
protected:
    static void SetUpTestCase(void)
    {
        LOG("========setup========");
        bool ret = OEMIsResetFlagExist();
        if (ret) {
            LOG("======== this is not the first run of test after flash ========\n");
            g_isFirstRun = false;
        } else {
            LOG("======== this is the first run of test after flash ========\n");
        }
    }
    static void TearDownTestCase(void)
    {
        LOG("========tearDown========");
        int ret = OEMCreateResetFlag();
        if (ret) {
            LOG("======== OEMCreateResetFlag failed! ========");
        } else {
            LOG("======== OEMCreateResetFlag ok! ========");
        }
    }
};
#ifndef TOKEN_PERSIST_TEST

/**
 * @tc.name : TestReadAuthServerInfo
 * @tc.desc : test case of OEMReadAuthServerInfo api.
              no assert for this case, comiple ok means test pass.
 */
HWTEST_F(OEMAPITEST, TestReadAuthServerInfo, TestSize.Level0) {

    LOG("----- OEMReadAuthServerInfo api test start -----");
    char* serverInfo = (char*)malloc(MAX_SERVER_INFO_LEN + 1);
    if (serverInfo == nullptr) {
        LOG("malloc fail, can't do test!");
        GTEST_FAIL();
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
HWTEST_F(OEMAPITEST, TestLoadKitInfos, TestSize.Level0) {
    LOG("----- OEMLoadKitInfos api test start -----");
    char* kitInfoMsg = OEMLoadKitInfos();
    if (kitInfoMsg == nullptr) {
        LOG("No other kit infos");
        return;
    }
    LOG("kit info: %s", kitInfoMsg);
    cJSON* root = cJSON_Parse(kitInfoMsg);
    free(kitInfoMsg);

    ASSERT_NE(root, nullptr) << "Invalid json format!" << "\n";

    cJSON* kitInfoArray = cJSON_GetObjectItem(root, KIT_INFO_JSON_KEY);
    ASSERT_NE(kitInfoArray, nullptr) << "Invalid json format!" << "\n";
    cJSON_Delete(root);
}
/**
 * @tc.name : TestIsOverTemperatureLimit
 * @tc.desc : test case of temperature api.
 */
HWTEST_F(OEMAPITEST, TestIsOverTemperatureLimit, TestSize.Level0) {
    LOG("----- temperature api test start -----");
    bool results = OEMIsOverTemperatureLimit();
    ASSERT_EQ(0, results) << "OEMIsOverTemperatureLimit should fail in normal state" << "\n";
}
/**
 * @tc.name: TestResetFlagApi
 * @tc.desc: test case of the reset flag related api: create, check
 */
HWTEST_F(OEMAPITEST, TestResetFlagApi, TestSize.Level0) {
    LOG("----- reset flag api test start -----");
    if (g_isFirstRun) {
        int ret = OEMCreateResetFlag();
        ASSERT_EQ(0, ret) << "Failed to create the reset flag" << "\n";
        bool flag = OEMIsResetFlagExist();
        ASSERT_EQ(1, flag) << "the reset flag should exist." << "\n";
    } else {
        bool flag = OEMIsResetFlagExist();
        ASSERT_EQ(1, flag) << "the reset flag should exist." << "\n";
        int ret = OEMDeleteResetFlag();
        ASSERT_EQ(0, ret) << "Failed to delete the Reset flag" << "\n";
        flag = OEMIsResetFlagExist();
        ASSERT_EQ(0, flag) << "the reset flag should not exist." << "\n";
    }
}

/**
 * @tc.name: TestAuthStatusApi
 * @tc.desc: test case of the authStatus related api: read, write, delete, check
 */
HWTEST_F(OEMAPITEST, TestAuthStatusApi, TestSize.Level0) {
    LOG("----- auth status api test start -----");
    uint32_t len = strlen(TEST_AUTH_STATUS);
    char* statusRead = (char*)malloc(len + 1);
    if (statusRead == nullptr) {
        LOG("malloc fail, can't do test!");
        GTEST_FAIL();
        return;
    }
    if (g_isFirstRun) {
        int ret = OEMWriteAuthStatus(TEST_AUTH_STATUS, len);
        ASSERT_EQ(0, ret) << "Failed to write auth data" << "\n";
        bool isExist = OEMIsAuthStatusExist();
        ASSERT_EQ(1, isExist) << "authStatus should exist!" << "\n";
        ret = OEMReadAuthStatus(statusRead, len);
        LOG("statusRead:%s", statusRead);
        ASSERT_EQ(0, ret) << "Failed to read authStatus" << "\n";
        statusRead[len] = 0;
        ASSERT_STREQ(TEST_AUTH_STATUS, statusRead) << "authStatus not match!" << "\n";
        uint32_t newLen = 0;
        ret = OEMGetAuthStatusFileSize(&newLen);
        ASSERT_EQ(0, ret) << "failed to get authStatus size" << "\n";
        // Check whether the obtained length is the same as the initial written length.
        ASSERT_EQ(len, newLen) << "Failed: length not match!" << "\n";
    } else {
        bool isExist = OEMIsAuthStatusExist();
        ASSERT_EQ(1, isExist) << "authStatus should exist!" << "\n";
        int ret = OEMReadAuthStatus(statusRead, len);
        LOG("statusRead:%s", statusRead);
        ASSERT_EQ(0, ret) << "Failed to read authStatus" << "\n";
        statusRead[len] = 0;
        ASSERT_STREQ(TEST_AUTH_STATUS, statusRead) << "authStatus not match!" << "\n";
        uint32_t newLen = 0;
        ret = OEMGetAuthStatusFileSize(&newLen);
        ASSERT_EQ(0, ret) << "failed to get authStatus size" << "\n";
        ASSERT_EQ(len, newLen) << "Failed: length not match!" << "\n";

        ret = OEMDeleteAuthStatus();
        ASSERT_EQ(0, ret) << "failed to delete authStatus" << "\n";
        isExist = OEMIsAuthStatusExist();
        ASSERT_EQ(0, isExist) << "file still exist!" << "\n";
    }
    free(statusRead);
}

/**
 * @tc.name: TestTicketApi
 * @tc.desc: test case of the ticket related api: read, write, delete, check
 */
HWTEST_F(OEMAPITEST, TestTicketApi, TestSize.Level0) {
    LOG("----- ticket api test start -----");
    uint32_t len = strlen(TEST_TICKET);
    char* ticketRead = (char*)malloc(len + 1);
    if (ticketRead == nullptr) {
        LOG("malloc fail, can't do test!");
        GTEST_FAIL();
        return;
    }
    if (g_isFirstRun) {
        int ret = OEMWriteTicket(TEST_TICKET, len);
        ASSERT_EQ(0, ret) << "Failed to write ticket" << "\n";
        bool isExist = OEMIsTicketExist();
        ASSERT_EQ(1, isExist) << "ticket should exist!" << "\n";
        ret = OEMReadTicket(ticketRead, len);
        LOG("ticketRead:%s", ticketRead);
        ASSERT_EQ(0, ret) << "Failed to tead ticket" << "\n";
        ticketRead[len] = 0;
        ASSERT_STREQ(TEST_TICKET, ticketRead) << "ticket not match" << "\n";
        uint32_t newLen = 0;
        ret = OEMGetTicketFileSize(&newLen);
        ASSERT_EQ(0, ret) << "Failed to get ticket size" << "\n";
        ASSERT_EQ(len, newLen) << "length not match!" << "\n";
    } else {
        bool isExist = OEMIsTicketExist();
        ASSERT_EQ(1, isExist) << "ticket not does exist!" << "\n";
        int ret = OEMReadTicket(ticketRead, len);
        LOG("ticketRead:%s", ticketRead);
        ASSERT_EQ(0, ret) << "Failed to read ticket" << "\n";
        ticketRead[len] = 0;
        ASSERT_STREQ(TEST_TICKET, ticketRead) << "ticket not match" << "\n";
        uint32_t newLen = 0;
        ret = OEMGetTicketFileSize(&newLen);
        ASSERT_EQ(0, ret) << "failed to get ticket size" << "\n";
        ASSERT_EQ(len, newLen) << "length not match!" << "\n";

        ret = OEMDeleteTicket();
        ASSERT_EQ(0, ret) << "Failed to delete ticket" << "\n";
        isExist = OEMIsTicketExist();
        ASSERT_EQ(0, isExist) << "file still exist!" << "\n";
    }
    free(ticketRead);
}

/**
 * @tc.name: TestTokenApi
 * @tc.desc: test case of the token related api: read, write
 */
HWTEST_F(OEMAPITEST, TestTokenApi, TestSize.Level0) {
    LOG("----- token api test start -----");
    uint32_t len = strlen(TEST_TOKEN1);
    char* tokenRead = (char*)malloc(len + 1);
    if (tokenRead == nullptr) {
        LOG("malloc fail, can't do test!");
        GTEST_FAIL();
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
        ASSERT_EQ(0, ret) << "Failed to write token" << "\n";
        ret = ReadToken(tokenRead, len);
        ASSERT_EQ(0, ret) << "Failed to read token" << "\n";
        tokenRead[len] = 0;
        ASSERT_STREQ(TEST_TOKEN1, tokenRead) << "token not match" << "\n";

        // write then read token check, again. for A-B area check
        ret = WriteToken(TEST_TOKEN2, len);
        ASSERT_EQ(0, ret) << "Failed to write token" << "\n";
        ret = ReadToken(tokenRead, len);
        ASSERT_EQ(0, ret) << "Failed to read token" << "\n";
        tokenRead[len] = 0;
        ASSERT_STREQ(TEST_TOKEN2, tokenRead) << "token not match" << "\n";
    } else {
        int ret = ReadToken(tokenRead, len);
        ASSERT_EQ(0, ret) << "Failed to read token" << "\n";
        tokenRead[len] = 0;
        ASSERT_STREQ(TEST_TOKEN2, tokenRead) << "token not match" << "\n";
    }
    free(tokenRead);
}
/**
 * @tc.name: TestGetAckeyApi
 * @tc.desc: Check whether the Ackey can be obtained.
 */
HWTEST_F(OEMAPITEST, TestGetAckeyApi, TestSize.Level0)
{
    LOG("----- test get ackey start -----");
    char acKey[ACKEY_LEN];
    unsigned int len = sizeof(acKey);
    int ret = GetAcKey(acKey, len);
    ASSERT_EQ(0, ret) << "Failed to get the ackey" << "\n";
}

/**
 * @tc.name: TestGetProdIdApi
 * @tc.desc: Check whether the ProdId can be obtained.
 */
HWTEST_F(OEMAPITEST, TestGetProdIdApi, TestSize.Level0)
{
    LOG("----- test getprodid start -----");
    char productId[PRODUCT_ID_LEN];
    int ret = GetProdId(productId, sizeof(productId));
    ASSERT_EQ(0, ret) << "Failed to get the productid" << "\n";
}

/**
 * @tc.name: TestGetProdKeyApi
 * @tc.desc: Check whether the Prodkey can be obtained.
 */
HWTEST_F(OEMAPITEST, TestGetProdKeyApi, TestSize.Level0)
{
    LOG("----- test getproductkey start -----");
    char productKey[PRODUCT_KEY_LEN];
    int ret = GetProdKey(productKey, sizeof(productKey));
    ASSERT_EQ(0, ret) << "Failed to get the productkey" << "\n";
}

#else  // defined TOKEN_PERSIST_TEST

/**
 * @tc.name: TestIfTokenPersist
 * @tc.desc: test case of the token related api: read, write
 */
HWTEST_F(OEMAPITEST, TestIfTokenPersist, TestSize.Level0){
    LOG("----- token api test start -----");
    uint32_t len = strlen(TEST_TOKEN2);
    char* tokenRead = (char*)malloc(len + 1);
    if (tokenRead == NULL) {
        LOG("malloc fail, can't do test!");
        GTEST_FAIL();
        return;
    }
    int ret = ReadToken(tokenRead, len);
    ASSERT_EQ(0, ret) << "Failed to read token" << "\n";
    tokenRead[len] = 0;
    LOG("tokenRead:%s", tokenRead);
    ASSERT_STREQ(TEST_TOKEN2, tokenRead) << "token not match" << "\n";

    free(tokenRead);
}
#endif