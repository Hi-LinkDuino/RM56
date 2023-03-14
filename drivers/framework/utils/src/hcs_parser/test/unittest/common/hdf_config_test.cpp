/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

namespace ConfigTest {
const int8_t HDF_MSG_RESULT_DEFAULT = 3;

// hcs config test case number
enum HdfTestCaseCmd {
    HDF_CREATE_DM_HSL_TO_TREE_001,
    HDF_GET_NODE_BY_ATTR_VALUE_001,
    HDF_GET_NODE_BY_ATTR_VALUE_002,
    HDF_GET_BOOL_ATTR_VALUE_001,
    HDF_GET_BOOL_ATTR_VALUE_002,
    HDF_GET_UINT8_ATTR_VALUE_001,
    HDF_GET_UINT8_ATTR_VALUE_002,
    HDF_GET_UINT8_ARRAY_ELEM_ATTR_VALUE_001,
    HDF_GET_UINT8_ARRAY_ELEM_ATTR_VALUE_002,
    HDF_GET_UINT8_ARRAY_ATTR_VALUE_001,
    HDF_GET_UINT8_ARRAY_ATTR_VALUE_002,
    HDF_GET_UINT16_ATTR_VALUE_001,
    HDF_GET_UINT16_ATTR_VALUE_002,
    HDF_GET_UINT16_ARRAY_ELEM_ATTR_VALUE_001,
    HDF_GET_UINT16_ARRAY_ELEM_ATTR_VALUE_002,
    HDF_GET_UINT16_ARRAY_ATTR_VALUE_001,
    HDF_GET_UINT16_ARRAY_ATTR_VALUE_002,
    HDF_GET_UINT32_ATTR_VALUE_001,
    HDF_GET_UINT32_ATTR_VALUE_002,
    HDF_GET_UINT32_ARRAY_ELEM_ATTR_VALUE_001,
    HDF_GET_UINT32_ARRAY_ELEM_ATTR_VALUE_002,
    HDF_GET_UINT32_ARRAY_ATTR_VALUE_001,
    HDF_GET_UINT32_ARRAY_ATTR_VALUE_002,
    HDF_GET_UINT64_ATTR_VALUE_001,
    HDF_GET_UINT64_ATTR_VALUE_002,
    HDF_GET_UINT64_ARRAY_ELEM_ATTR_VALUE_001,
    HDF_GET_UINT64_ARRAY_ELEM_ATTR_VALUE_002,
    HDF_GET_UINT64_ARRAY_ATTR_VALUE_001,
    HDF_GET_UINT64_ARRAY_ATTR_VALUE_002,
    HDF_GET_ELEM_NUM_VALUE_001,
    HDF_GET_ELEM_NUM_VALUE_002,
    HDF_GET_CHILD_NODE_001,
    HDF_GET_CHILD_NODE_002,
    HDF_TRAVERSE_ATTR_IN_NODE_001,
    HDF_TRAVERSE_ATTR_IN_NODE_002,
    HDF_GET_STRING_ATTR_VALUE_001,
    HDF_GET_STRING_ATTR_VALUE_002,
    HDF_GET_STRING_ARRAY_ELEM_ATTR_VALUE_001,
    HDF_GET_STRING_ARRAY_ELEM_ATTR_VALUE_002,
    HDF_GET_NODE_BY_ATTR_REF_001,
    HDF_GET_NODE_BY_ATTR_REF_002,
    HDF_MACRO_GET_ONE_NODE,
    HDF_MACRO_CHECK_NODE_EXISTS,
    HDF_MACRO_CHECK_NODE_NOT_EXISTS,
    HDF_MACRO_GET_ONE_NODE_PROP,
    HDF_MACRO_CHECK_NODE_PROP_EXISTS,
    HDF_MACRO_CHECK_NODE_PROP_NOT_EXISTS,
    HDF_MACRO_TRAVERSAL_ONE_NODE,
    HDF_MACRO_GET_ARRAY_SIZE,
    HDF_MACRO_GET_ONE_ARRAY_ELEMENT,
    HDF_MACRO_TRAVERSAL_ONE_ARRAY,
    HDF_MACRO_TRAVERSAL_ALL_ARRAY,
    HDF_MACRO_TRAVERSAL_NODE_CHILD,
    HDF_MACRO_TRAVERSAL_NODE_CHILD_VARGS,
    HDF_MACRO_GET_ONE_FILE,
};

class HdfConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfConfigTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfConfigTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfConfigTest::SetUp() {}

void HdfConfigTest::TearDown() {}

/**
 * @tc.name: HslTestCreateDMHslToTree001
 * @tc.desc: Create a configuration tree and start configuration test
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestCreateDMHslToTree001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_CREATE_DM_HSL_TO_TREE_001, HDF_MSG_RESULT_DEFAULT};
    printf("HdfConfigTest enter\n\r");
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetNodeByMatchAttrSuccess001
 * @tc.desc: Obtains the root node of the configuration tree
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetNodeByMatchAttrSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_NODE_BY_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetNodeByMatchAttrFail001
 * @tc.desc: Failed to obtain the root node of the configuration tree
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetNodeByMatchAttrFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_NODE_BY_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetBoolAttrValueSuccess001
 * @tc.desc: Obtains the value of a <b>BOOL</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetBoolAttrValueSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_BOOL_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetBoolAttrValueFail001
 * @tc.desc: Failed to obtain the value of a <b>BOOL</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetBoolAttrValueFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_BOOL_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint8AttrValueSuccess001
 * @tc.desc: Obtains the value of a <b>Uint8</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint8AttrValueSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT8_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint8AttrValueFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint8</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint8AttrValueFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT8_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint8ArrayElemSuccess001
 * @tc.desc: Obtains the value of a <b>Unit8</b> array attribute numbered of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint8ArrayElemSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT8_ARRAY_ELEM_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint8ArrayElemFail001
 * @tc.desc: Failed to obtain the value of a <b>Unit8</b> array attribute numbered of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint8ArrayElemFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT8_ARRAY_ELEM_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint8ArraySuccess001
 * @tc.desc: Obtains the values of a <b>Uint8</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint8ArraySuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT8_ARRAY_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint8ArrayFail001
 * @tc.desc: Failed to obtain the values of a <b>Uint8</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint8ArrayFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT8_ARRAY_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint16AttrValueSuccess001
 * @tc.desc: Obtains the value of a <b>Uint16</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint16AttrValueSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT16_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint16AttrValueFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint16</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint16AttrValueFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT16_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint16ArrayElemSuccess001
 * @tc.desc: Obtains the value of a <b>Uint16</b> array attribute numbered <b>index</b> of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint16ArrayElemSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT16_ARRAY_ELEM_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint16ArrayElemFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint16</b> array attribute numbered <b>index</b> of a
 * configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint16ArrayElemFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT16_ARRAY_ELEM_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint16ArraySuccess001
 * @tc.desc: Obtains the values of a <b>Uint16</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint16ArraySuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT16_ARRAY_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint16ArrayFail001
 * @tc.desc: Failed to obtain the values of a <b>Uint16</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint16ArrayFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT16_ARRAY_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint32AttrValueSuccess001
 * @tc.desc: Obtains the value of a <b>Uint32</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint32AttrValueSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT32_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint32AttrValueFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint32</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint32AttrValueFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT32_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint32ArrayElemSuccess001
 * @tc.desc: Obtains the value of a <b>Uint32</b> array attribute numbered <b>index</b> of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint32ArrayElemSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT32_ARRAY_ELEM_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint32ArrayElemFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint32</b> array attribute numbered <b>index</b> of a
 * configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint32ArrayElemFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT32_ARRAY_ELEM_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint32ArraySuccess001
 * @tc.desc: Obtains the values of a <b>Uint32</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint32ArraySuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT32_ARRAY_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint32ArrayFail001
 * @tc.desc: Failed to obtain the values of a <b>Uint32</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint32ArrayFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT32_ARRAY_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint64AttrValueSuccess001
 * @tc.desc: Obtains the value of a <b>Uint64</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint64AttrValueSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT64_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint64AttrValueFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint64</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint64AttrValueFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT64_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint64ArrayElemSuccess001
 * @tc.desc: Obtains the value of a <b>Uint64</b> array attribute numbered <b>index</b> of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint64ArrayElemSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT64_ARRAY_ELEM_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint64ArrayElemFail001
 * @tc.desc: Failed to obtain the value of a <b>Uint64</b> array attribute numbered <b>index</b> of a
 * configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint64ArrayElemFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT64_ARRAY_ELEM_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint64ArraySuccess001
 * @tc.desc: Obtains the values of a <b>Uint64</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint64ArraySuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT64_ARRAY_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetUint64ArrayFail001
 * @tc.desc: Failed to obtain the values of a <b>Uint64</b> array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetUint64ArrayFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_UINT64_ARRAY_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetElemNumSuccess001
 * @tc.desc: Obtains the number of values for an array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetElemNumSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_ELEM_NUM_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetElemNumFail001
 * @tc.desc: Failed to obtain the number of values for an array attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetElemNumFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_ELEM_NUM_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetChildNodeSuccess001
 * @tc.desc: Obtains the child node with a specified node name from a parent node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetChildNodeSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_CHILD_NODE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetChildNodeFail001
 * @tc.desc: Failed to obtain the child node with a specified node name from a parent node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetChildNodeFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_CHILD_NODE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestTraverseAttrInNodeSuccess001
 * @tc.desc: Traverses the attributes of the current configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestTraverseAttrInNodeSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_TRAVERSE_ATTR_IN_NODE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestTraverseAttrInNodeFail001
 * @tc.desc: Failed to traverse the attributes of the current configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestTraverseAttrInNodeFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_TRAVERSE_ATTR_IN_NODE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetStringSuccess001
 * @tc.desc: Obtains the value of a <b>String</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetStringSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_STRING_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetStringFail001
 * @tc.desc: Failed to obtain the value of a <b>String</b> attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetStringFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_STRING_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetStringArrayElemSuccess001
 * @tc.desc: Obtains the value of a <b>String</b> array attribute numbered <b>index</b> of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetStringArrayElemSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_STRING_ARRAY_ELEM_ATTR_VALUE_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetStringArrayElemFail001
 * @tc.desc: Failed to obtain the value of a <b>String</b> array attribute numbered <b>index</b> of a
 * configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetStringArrayElemFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_STRING_ARRAY_ELEM_ATTR_VALUE_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetNodeAttrRefSuccess001
 * @tc.desc: Obtains the node that is specified by a node-type attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetNodeAttrRefSuccess001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_NODE_BY_ATTR_REF_001, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestGetNodeAttrRefFail001
 * @tc.desc: Failed to obtain the node that is specified by a node-type attribute of a configuration tree node
 * @tc.type: FUNC
 * @tc.require: AR000DQ0TB
 */
HWTEST_F(HdfConfigTest, HslTestGetNodeAttrRefFail001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_GET_NODE_BY_ATTR_REF_002, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_GET_ONE_NODE test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroGetOneNode, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_GET_ONE_NODE, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_CHECK_NODE_EXISTS test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroCheckNodeExists, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_CHECK_NODE_EXISTS, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_CHECK_NODE_NOT_EXISTS test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroCheckNodeNotExists, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_CHECK_NODE_NOT_EXISTS, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_GET_ONE_NODE_PROP test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroGetOneNodeProp, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_GET_ONE_NODE_PROP, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_CHECK_NODE_PROP_EXISTS test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroCheckNodePropExists, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_CHECK_NODE_PROP_EXISTS, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_CHECK_NODE_PROP_NOT_EXISTS test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroCheckNodePropNotExists, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_CHECK_NODE_PROP_NOT_EXISTS, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_TRAVERSAL_ONE_NODE test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroTraversalOneNode, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_TRAVERSAL_ONE_NODE, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_GET_ARRAY_SIZE test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroGetArraySize, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_GET_ARRAY_SIZE, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_GET_ONE_ARRAY_ELEMENT test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroGetOneArrayElevent, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_GET_ONE_ARRAY_ELEMENT, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_TRAVERSAL_ONE_ARRAY test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroTraversalOneArray, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_TRAVERSAL_ONE_ARRAY, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_TRAVERSAL_ALL_ARRAY test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroTraversalAllArray, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_TRAVERSAL_ALL_ARRAY, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_TRAVERSAL_NODE_CHILD test
 * @tc.type: FUNC
 * @tc.require: AR000GGTQK
 */
HWTEST_F(HdfConfigTest, HslTestMacroTraversalNodeChild, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_TRAVERSAL_NODE_CHILD, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_TRAVERSAL_NODE_CHILD_VARGS test
 * @tc.type: FUNC
 * @tc.require: SR000GGTQI
 */
HWTEST_F(HdfConfigTest, HslTestMacroTraversalNodeChildVargs, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_TRAVERSAL_NODE_CHILD_VARGS, HDF_MSG_RESULT_DEFAULT};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: HslTestMacroGetOneNode
 * @tc.desc: HDF_MACRO_GET_ONE_FILE test
 * @tc.type: FUNC
 * @tc.require: SR000GGTQI
 */
HWTEST_F(HdfConfigTest, HslTestMacroTraversalOneFile, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_CONFIG_TYPE, HDF_MACRO_GET_ONE_FILE, HDF_MSG_RESULT_DEFAULT};
    printf("HdfConfigTest last enter\n\r");
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
