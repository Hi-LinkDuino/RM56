/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "wifi_log.h"
#include "dhcp_options.h"
#include "dhcp_client.h"
#include "securec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace testing::ext;

namespace OHOS {
class DhcpOptionsTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {}
    virtual void TearDown()
    {}
};

HWTEST_F(DhcpOptionsTest, GetDhcpOptionDataLen_SUCCESS, TestSize.Level1)
{
    EXPECT_GT(GetDhcpOptionDataLen(DHCP_MESSAGE_TYPE_OPTION), 0);
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionDataLen_FAILED, TestSize.Level1)
{
    EXPECT_EQ(GetDhcpOptionDataLen(PAD_OPTION), 0);
    EXPECT_EQ(GetDhcpOptionDataLen(CLIENT_IDENTIFIER_OPTION), 0);
    EXPECT_EQ(GetDhcpOptionDataLen(DOMAIN_NAME_SERVER_OPTION), 0);
}

HWTEST_F(DhcpOptionsTest, GetDhcpOption_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = SUBNET_MASK_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    uint32_t u32Data = 3226272231;
    ASSERT_TRUE(
        memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &u32Data, DHCP_UINT32_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = DHCP_MESSAGE_TYPE_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uint8_t uData = DHCP_DISCOVER;
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    size_t len = 0;
    EXPECT_NE(GetDhcpOption(&packet, DHCP_MESSAGE_TYPE_OPTION, &len), nullptr);
}

HWTEST_F(DhcpOptionsTest, GetDhcpOption_FAILED, TestSize.Level1)
{
    size_t len = 0;
    EXPECT_EQ(GetDhcpOption(nullptr, DHCP_MESSAGE_TYPE_OPTION, &len), nullptr);

    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = SUBNET_MASK_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    uint32_t u32Data = 3226272231;
    ASSERT_TRUE(
        memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &u32Data, DHCP_UINT32_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = OPTION_OVERLOAD_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uint8_t uData = FILE_FIELD | SNAME_FIELD;
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    len = 0;
    EXPECT_EQ(GetDhcpOption(&packet, DHCP_MESSAGE_TYPE_OPTION, &len), nullptr);
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionUint8_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    uint8_t *pOption = packet.options;
    pOption[DHCP_OPT_CODE_INDEX] = DHCP_MESSAGE_TYPE_OPTION;
    pOption[DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uint8_t uData = DHCP_DISCOVER;
    ASSERT_TRUE(memcpy_s(pOption + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);

    int endIndex = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[DHCP_OPT_LEN_INDEX];
    pOption[endIndex] = END_OPTION;

    uint8_t u8Message = 0;
    EXPECT_EQ(true, GetDhcpOptionUint8(&packet, DHCP_MESSAGE_TYPE_OPTION, &u8Message));
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionUint8_FAILED, TestSize.Level1)
{
    uint8_t u8Message = 0;
    EXPECT_EQ(false, GetDhcpOptionUint8(nullptr, DHCP_MESSAGE_TYPE_OPTION, &u8Message));
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionUint32_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = DHCP_MESSAGE_TYPE_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uint8_t uData = DHCP_DISCOVER;
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = IP_ADDRESS_LEASE_TIME_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    uint32_t u32Data = ONE_HOURS_SEC;
    ASSERT_TRUE(
        memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &u32Data, DHCP_UINT32_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    u32Data = 0;
    EXPECT_EQ(true, GetDhcpOptionUint32(&packet, IP_ADDRESS_LEASE_TIME_OPTION, &u32Data));
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionUint32_FAILED, TestSize.Level1)
{
    uint32_t u32Data = 0;
    EXPECT_EQ(false, GetDhcpOptionUint32(nullptr, IP_ADDRESS_LEASE_TIME_OPTION, &u32Data));

    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = DHCP_MESSAGE_TYPE_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uint8_t uData = DHCP_DISCOVER;
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    u32Data = 0;
    EXPECT_EQ(false, GetDhcpOptionUint32(&packet, DHCP_MESSAGE_TYPE_OPTION, &u32Data));

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = OPTION_OVERLOAD_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uData = FILE_FIELD | SNAME_FIELD;
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = IP_ADDRESS_LEASE_TIME_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    u32Data = ONE_HOURS_SEC;
    ASSERT_TRUE(
        memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &u32Data, DHCP_UINT32_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    u32Data = 0;
    EXPECT_EQ(false, GetDhcpOptionUint32(&packet, IP_ADDRESS_LEASE_TIME_OPTION, &u32Data));
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionUint32n_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = ROUTER_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES + DHCP_UINT32_BYTES;
    uint32_t u32Data1 = 3226272231;
    uint32_t u32Data2 = 3226272232;
    ASSERT_TRUE(
        memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &u32Data1, DHCP_UINT32_BYTES) == EOK);
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX + DHCP_UINT32_BYTES,
                    DHCP_UINT32_BYTES,
                    &u32Data2,
                    DHCP_UINT32_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    uint32_t uData1 = 0;
    uint32_t uData2 = 0;
    EXPECT_EQ(true, GetDhcpOptionUint32n(&packet, ROUTER_OPTION, &uData1, &uData2));
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionUint32n_FAILED, TestSize.Level1)
{
    uint32_t uData1 = 0;
    uint32_t uData2 = 0;
    EXPECT_EQ(false, GetDhcpOptionUint32n(nullptr, ROUTER_OPTION, &uData1, &uData2));

    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = DHCP_MESSAGE_TYPE_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT8_BYTES;
    uint8_t uData = DHCP_DISCOVER;
    ASSERT_TRUE(memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT8_BYTES, &uData, DHCP_UINT8_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    EXPECT_EQ(false, GetDhcpOptionUint32n(nullptr, DHCP_MESSAGE_TYPE_OPTION, &uData1, &uData2));
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionString_SUCCESS, TestSize.Level1)
{
    char buf[VENDOR_MAX_LEN - DHCP_OPT_CODE_BYTES - DHCP_OPT_LEN_BYTES] = {0};
    ASSERT_TRUE(snprintf_s(buf,
                    VENDOR_MAX_LEN - DHCP_OPT_DATA_INDEX,
                    VENDOR_MAX_LEN - DHCP_OPT_DATA_INDEX - 1,
                    "%s-%s",
                    DHCPC_NAME,
                    DHCPC_VERSION) >= 0);

    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    uint8_t *pOption = packet.options;
    pOption[DHCP_OPT_CODE_INDEX] = VENDOR_SPECIFIC_INFO_OPTION;
    pOption[DHCP_OPT_LEN_INDEX] = strlen(buf);
    ASSERT_TRUE(memcpy_s(pOption + DHCP_OPT_DATA_INDEX,
                    VENDOR_MAX_LEN - DHCP_OPT_CODE_BYTES - DHCP_OPT_LEN_BYTES,
                    buf,
                    strlen(buf)) == EOK);

    int endIndex = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[DHCP_OPT_LEN_INDEX];
    pOption[endIndex] = END_OPTION;

    char *pVendor = GetDhcpOptionString(&packet, VENDOR_SPECIFIC_INFO_OPTION);
    ASSERT_TRUE(pVendor != NULL);
    free(pVendor);
}

HWTEST_F(DhcpOptionsTest, GetDhcpOptionString_FAILED, TestSize.Level1)
{
    EXPECT_EQ(nullptr, GetDhcpOptionString(nullptr, VENDOR_SPECIFIC_INFO_OPTION));
}

HWTEST_F(DhcpOptionsTest, GetEndOptionIndex_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = PAD_OPTION;
    endIndex += DHCP_OPT_CODE_BYTES;

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = SUBNET_MASK_OPTION;
    pOption[endIndex + DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    uint32_t u32Data = 3226272231;
    ASSERT_TRUE(
        memcpy_s(pOption + endIndex + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &u32Data, DHCP_UINT32_BYTES) == EOK);
    endIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[endIndex + DHCP_OPT_LEN_INDEX];

    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    EXPECT_GE(GetEndOptionIndex(packet.options), 0);
}

HWTEST_F(DhcpOptionsTest, AddOptStrToOpts_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);
    packet.options[DHCP_OPT_CODE_INDEX] = END_OPTION;

    uint8_t option[DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + DHCP_UINT32_BYTES];
    option[DHCP_OPT_CODE_INDEX] = IP_ADDRESS_LEASE_TIME_OPTION;
    option[DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    uint32_t uData = ONE_HOURS_SEC;
    ASSERT_TRUE(memcpy_s(option + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &uData, DHCP_UINT32_BYTES) == EOK);
    int nLen = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + option[DHCP_OPT_LEN_INDEX];

    EXPECT_GT(AddOptStrToOpts(packet.options, option, nLen), 0);
}

HWTEST_F(DhcpOptionsTest, AddOptStrToOpts_FAILED, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);

    int endIndex = 0;
    uint8_t *pOption = packet.options;
    pOption[endIndex + DHCP_OPT_CODE_INDEX] = END_OPTION;

    uint8_t option[DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + DHCP_UINT32_BYTES];
    option[DHCP_OPT_CODE_INDEX] = IP_ADDRESS_LEASE_TIME_OPTION;
    option[DHCP_OPT_LEN_INDEX] = DHCP_UINT32_BYTES;
    uint32_t uData = ONE_HOURS_SEC;
    ASSERT_TRUE(memcpy_s(option + DHCP_OPT_DATA_INDEX, DHCP_UINT32_BYTES, &uData, DHCP_UINT32_BYTES) == EOK);
    EXPECT_EQ(AddOptStrToOpts(pOption, option, DHCP_OPT_CODE_BYTES), 0);

    option[DHCP_OPT_LEN_INDEX] = (uint8_t)(DHCP_OPTIONS_SIZE - DHCP_OPT_CODE_BYTES - DHCP_OPT_LEN_BYTES);
    EXPECT_EQ(AddOptStrToOpts(pOption, option, DHCP_OPTIONS_SIZE), 0);
}

HWTEST_F(DhcpOptionsTest, AddOptValueToOpts_SUCCESS, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);
    packet.options[DHCP_OPT_CODE_INDEX] = END_OPTION;

    uint8_t data1 = DHCP_REQUEST;
    uint16_t data2 = 1;
    uint32_t data3 = ONE_HOURS_SEC;
    EXPECT_GT(AddOptValueToOpts(packet.options, DHCP_MESSAGE_TYPE_OPTION, data1), 0);
    EXPECT_GT(AddOptValueToOpts(packet.options, INTERFACE_MTU_OPTION, data2), 0);
    EXPECT_GT(AddOptValueToOpts(packet.options, IP_ADDRESS_LEASE_TIME_OPTION, data3), 0);
}

HWTEST_F(DhcpOptionsTest, AddOptValueToOpts_FAILED, TestSize.Level1)
{
    struct DhcpPacket packet;
    ASSERT_TRUE(memset_s(&packet, sizeof(struct DhcpPacket), 0, sizeof(struct DhcpPacket)) == EOK);
    packet.options[DHCP_OPT_CODE_INDEX] = END_OPTION;

    EXPECT_EQ(AddOptValueToOpts(packet.options, PAD_OPTION, 0), 0);
}
}  // namespace OHOS