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

#include "gtest/gtest.h"
#include "utils/DMSTestBase.h"
#include "utils/dms_packet.h"
#include "distributed_service_interface.h"

using namespace testing::ext;

static const int MAX_LENGTH = 1024;
static BOOL handleFlag = FALSE;

static int8_t RunTest(const uint8_t *buffer, uint16_t bufferLen, const TlvParseCallback onTlvParseDone,
                      const StartAbilityCallback onStartAbilityDone)
{
    IDmsFeatureCallback dmsFeatureCallback = {
        .onTlvParseDone = onTlvParseDone,
        .onStartAbilityDone = onStartAbilityDone
    };

    CommuInterInfo interInfo;
    interInfo.payloadLength = bufferLen;
    interInfo.payload = buffer;

    return DmsLiteProcessCommuMsg(&interInfo, &dmsFeatureCallback);
}

class MsgParserFuncTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        SystemInitProxy();
        InstallHap();
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
    }
    // Testcase setup
    virtual void SetUp()
    {
    }
    // Testcase teardown
    virtual void TearDown()
    {
    }
};

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0010
 * @tc.name      : Normal package with bundle name and ability name can be parsed correctly
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0010, Function | MediumTest | Level1) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x6f,
        0x68, 0x6f, 0x73, 0x2e, 0x6c, 0x61, 0x75, 0x6e, 0x63, 0x68,
        0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41,
        0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00, 0x04, 0x0a,
        0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };

    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        const TlvDmsMsgInfo *msg = reinterpret_cast<const TlvDmsMsgInfo *>(dmsMsg);
        EXPECT_EQ(errCode, DMS_TLV_SUCCESS);
        EXPECT_EQ(msg->commandId, 1);
        EXPECT_EQ(string(msg->calleeBundleName), "com.ohos.launcher");
        EXPECT_EQ(string(msg->calleeAbilityName), "MainActivity");
        EXPECT_EQ(string(msg->callerSignature), "publickey");
    };

    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0020
 * @tc.name      : When parse abnormal package with null payload user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0020, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65,
        0x79, 0x00
    };

    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        printf("[hcpptest]result code : %d \n", errCode);
    };

    int8_t resultCode = RunTest(nullptr, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_FAILURE);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0030
 * @tc.name      : When parse abnormal package that size less than min user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0030, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01
    };
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        printf("[hcpptest]result code:%d \n", errCode);
    };

    // the length range: [2,1024]
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0040
 * @tc.name      : When parse abnormal package longer than max length user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0040, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65,
        0x79, 0x00
    };
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        printf("[hcpptest]result code:%d \n", errCode);
    };

    // the length range: [2,1024]
    int8_t resultCode = RunTest(buffer, MAX_LENGTH + 1, onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0050
 * @tc.name      : When parse abnormal package lack of COMMAND_ID node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0050, Function | MediumTest | Level2) {
    uint8_t buffer[] = { };

    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_LEN);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0060
 * @tc.name      : When parse abnormal package lack of BUNDLE_NAME node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0060, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_BAD_NODE_NUM);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0070
 * @tc.name      : When parse abnormal package lack of ABILITY_NAME node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0070, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_BAD_NODE_NUM);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0080
 * @tc.name      : When parse abnormal package lack of SIGNATURE node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0080, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_BAD_NODE_NUM);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0090
 * @tc.name      : When parse abnormal package with an additional node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0090, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65,
        0x79, 0x00, 0x05, 0x01, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_BAD_NODE_NUM);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0100
 * @tc.name      : When parse abnormal package with node type sequence in non-continuous order user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0100, Function | MediumTest | Level2) {
    // this sequence is: node1 node2 node4
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x04, 0x00, 0x04, 0x6d, 0x80, 0xff,
        0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_OUT_OF_ORDER);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0110
 * @tc.name      : When parse abnormal package with node type sequence in disorder user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0110, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77,
        0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e, 0x63, 0x68, 0x65,
        0x72, 0x01, 0x01, 0x00, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_OUT_OF_ORDER);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0120
 * @tc.name      : When parse abnormal package with duplicated COMMAND_ID node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0120, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x02, 0x14, 0x63, 0x6f,
        0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c,
        0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d,
        0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69,
        0x74, 0x79, 0x00, 0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69,
        0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_OUT_OF_ORDER);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0130
 * @tc.name      : When parse abnormal package with duplicated BUNDLE_NAME node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0130, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c,
              0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00, 
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c,
              0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69,
              0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_OUT_OF_ORDER);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0140
 * @tc.name      : When parse abnormal package with duplicated ABILITY_NAME node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0140, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c,
              0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69,
              0x76, 0x69, 0x74, 0x79, 0x00,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69,
              0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_OUT_OF_ORDER);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0150
 * @tc.name      : When parse abnormal package with duplicated SIGNATURE node user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0150, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c,
              0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69,
              0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_BAD_NODE_NUM);    // catched by node number validation
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0160
 * @tc.name      : When parse abnormal package which BUNDLE_NAME lack of length attribute user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0160, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_LEN);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0170
 * @tc.name      : When parse abnormal package which ABILITY_NAME lack of length attribute user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0170, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_LEN);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0180
 * @tc.name      : When parse abnormal package which CALLER_SIGNATURE lack of length attribute user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0180, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(errCode, DMS_TLV_ERR_LEN);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0190
 * @tc.name      : When parse abnormal package with COMMAND_ID length 0 user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0190, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x00, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65,
        0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]testMsgParserFunc0190:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0200
 * @tc.name      : When parse abnormal package with BUNDLE_NAME length 0 user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0200, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x00, 
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]testMsgParserFunc0200:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0210
 * @tc.name      : When parse abnormal package with ABILITY_NAME length 0 user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0210, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65,
              0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00,
        0x03, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]testMsgParserFunc0210:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0220
 * @tc.name      : When parse abnormal package with SIGNATURE length 0 user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0220, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]testMsgParserFunc0220:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0230
 * @tc.name      : When parse abnormal package that COMMAND_ID lake of value user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0230, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01
    };
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        printf("[hcpptest]testMsgParserFunc0230:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0240
 * @tc.name      : When parse abnormal package that BUNDLE_NAME lake of value user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0240, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]errCode:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0250
 * @tc.name      : When parse abnormal package that ABILITY_NAME lake of value user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0250, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]errCode:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0260
 * @tc.name      : When parse abnormal package that CALLER_SIGNATURE lake of value user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0260, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68,
        0x75, 0x61, 0x77, 0x65, 0x69, 0x2e, 0x6c, 0x61, 0x75, 0x6e,
        0x63, 0x68, 0x65, 0x72, 0x00, 0x03, 0x0d, 0x4d, 0x61, 0x69,
        0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        printf("[hcpptest]errCode:%d \n", errCode);
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    int8_t resultCode = RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(resultCode, DMS_EC_PARSE_TLV_FAILURE);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0270
 * @tc.name      : When parse abnormal package which BUNDLE_NAME value is not string user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0270, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69,
              0x2e, 0x6c, 0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x01,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69,
              0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0280
 * @tc.name      : When parse abnormal package which ABILITY_NAME value is not string user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0280, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69,
              0x2e, 0x6c, 0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69,
              0x74, 0x79, 0x01,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x00
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0290
 * @tc.name      : When parse abnormal package which SIGNATURE value is not string user get error code
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0290, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01,
        0x02, 0x14, 0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69,
              0x2e, 0x6c, 0x61, 0x75, 0x6e, 0x63, 0x68, 0x65, 0x72, 0x00,
        0x03, 0x0d, 0x4d, 0x61, 0x69, 0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69,
              0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79, 0x01
    };
    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        EXPECT_EQ(TRUE, errCode != DMS_TLV_SUCCESS);
    };
    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserFunc_0300
 * @tc.name      : Normal package with 255 bytes long bundle name can be parsed
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserFuncTest, testMsgParserFunc0300, Function | MediumTest | Level2) {
    uint8_t buffer[] = {
        0x01, 0x01, 0x01, 0x02, 0x82, 0x00, 0x63, 0x6f, 0x6d, 0x2e, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c,
        0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53,
        0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x61, 0x62, 0x63, 0x64,
        0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
        0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
        0x57, 0x58, 0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
        0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e,
        0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
        0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
        0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51,
        0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x61, 0x62,
        0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
        0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x2e, 0x6f, 0x68, 0x6f, 0x73, 0x00, 0x03, 0x0d, 0x4d, 0x61,
        0x69, 0x6e, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79, 0x00,
        0x04, 0x0a, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x6B, 0x65, 0x79,
        0x00
    };

    handleFlag = FALSE;
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        handleFlag = TRUE;
        const TlvDmsMsgInfo *msg = reinterpret_cast<const TlvDmsMsgInfo *>(dmsMsg);
        EXPECT_EQ(errCode, DMS_TLV_SUCCESS);
        std::stringstream ss;
        ss << "com.";
        for (int8_t i = 0; i < 4; i++) {
            ss << "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        }
        ss << "abcdefghijklmnopqrstuvwxyzABCDEFGHIJ";
        ss << ".ohos";
        EXPECT_EQ(msg->commandId, 1);
        EXPECT_EQ(string(msg->calleeBundleName), ss.str());
        EXPECT_EQ(string(msg->calleeAbilityName), "MainActivity");
        EXPECT_EQ(string(msg->callerSignature), "publickey");
    };

    RunTest(buffer, sizeof(buffer), onTlvParseDone, nullptr);
    ASSERT_EQ(TRUE, handleFlag);
}
