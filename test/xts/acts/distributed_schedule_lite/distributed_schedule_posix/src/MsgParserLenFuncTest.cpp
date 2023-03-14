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

static const int BYTE_CAPA = 127;
static const int NODE_MAX_LENGTH = 1009;

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

class MsgParserLenFuncTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        SystemInitProxy();
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
 * @tc.number    : DMSLite_DMS_MsgParserLen_0010
 * @tc.name      : Package which node has 127 bytes value can be parsed correctly
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserLenFuncTest, testMsgParserLen0010, Function | MediumTest | Level2) {
    std::string tLenStr = GetStringByLen(BYTE_CAPA - 1);  // - \0
    std::string bundleName = tLenStr;
    std::string abilityName = tLenStr;
    std::string signature = tLenStr;

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        reinterpret_cast<const TlvDmsMsgInfo *>(dmsMsg);

        printf("[hcpptest]errCode:%d \n", errCode);
        EXPECT_EQ(errCode, DMS_TLV_SUCCESS);
    };
    RunTest((uint8_t *)buffer, dataSize, onTlvParseDone, nullptr);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserLen_0020
 * @tc.name      : Package which node has 128 bytes can be parsed correctly
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserLenFuncTest, testMsgParserLen0020, Function | MediumTest | Level2) {
    // > 127, length need 2 bytes
    std::string tLenStr = GetStringByLen(BYTE_CAPA);
    std::string bundleName = tLenStr;
    std::string abilityName = tLenStr;
    std::string signature = tLenStr;

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed");
    }
    
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        reinterpret_cast<const TlvDmsMsgInfo *>(dmsMsg);

        printf("[hcpptest]errCode:%d \n", errCode);
        EXPECT_EQ(errCode, DMS_TLV_SUCCESS);
    };
    RunTest((uint8_t *)buffer, dataSize, onTlvParseDone, nullptr);
}

/**
 * @tc.number    : DMSLite_DMS_MsgParserLen_0030
 * @tc.name      : Package which node has 1010 bytes can be parsed correctly
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(MsgParserLenFuncTest, testMsgParserLen0030, Function | MediumTest | Level2) {
    std::string bundleName = GetStringByLen(NODE_MAX_LENGTH);
    std::string abilityName = "a";
    std::string signature = "a";

    char buffer[PACKET_DATA_SIZE] = {0};
    uint16_t dataSize = 0;
    DmsPacket dmsPacket {buffer, PACKET_DATA_SIZE};
    DmsMsgInfo dmsMsgInfo = {
        DMSLITE_COMMAND::START_ABILITY_FROM_REMOTE,
        bundleName,
        abilityName,
        signature
    };
    if (!dmsPacket.BuildDmsPacket(dmsMsgInfo, dataSize)) {
        printf("[hcpptest]E BuildDmsPacket failed \n");
    }
    auto onTlvParseDone = [] (int8_t errCode, const void *dmsMsg) {
        reinterpret_cast<const TlvDmsMsgInfo *>(dmsMsg);
        printf("[hcpptest]errCode:%d \n", errCode);
        EXPECT_EQ(errCode, DMS_TLV_SUCCESS);
    };
    RunTest((uint8_t *)buffer, dataSize, onTlvParseDone, nullptr);
}