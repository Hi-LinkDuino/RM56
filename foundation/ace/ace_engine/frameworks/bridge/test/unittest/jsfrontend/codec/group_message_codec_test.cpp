/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "base/log/log.h"
#include "frameworks/bridge/codec/codec_data.h"
#include "frameworks/bridge/codec/function_call.h"
#include "frameworks/bridge/codec/standard_function_codec.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string FUNCTION_NAME_VALUE = "getInfo";
const std::string FUNCTION_PARA_STRING_VALUE = "onePara";
const int32_t FUNCTION_PARA_INT_VALUE = 100;
const std::vector<int8_t> FUNCTION_PARA_INT8_ARRAY = { 127, 0, 100 };
const std::vector<uint8_t> INT8_ARRAY_ENCODE_RESULT = { 22, 7, 0, 0, 0, 103, 101, 116, 73, 110, 102, 111,
    1, 25, 3, 0, 0, 0, 127, 0, 100 };
const std::vector<int16_t> FUNCTION_PARA_INT16_ARRAY = { 32767, 0, 100 };
const std::vector<uint8_t> INT16_ARRAY_ENCODE_RESULT = { 22, 7, 0, 0, 0, 103, 101, 116, 73, 110, 102, 111,
    1, 26, 3, 0, 0, 0, 255, 127, 0, 0, 100, 0 };
const std::vector<int32_t> FUNCTION_PARA_INT32_ARRAY = { 2147483647, 0, 100 };
const std::vector<uint8_t> INT32_ARRAY_ENCODE_RESULT = { 22, 7, 0, 0, 0, 103, 101, 116, 73, 110, 102, 111,
    1, 27, 3, 0, 0, 0, 255, 255, 255, 127, 0, 0, 0, 0, 100, 0, 0, 0 };
const std::set<std::string> FUNCTION_PARA_SET = { "1", "2", "3" };
const std::vector<uint8_t> SET_ENCODE_RESULT = { 22, 7, 0, 0, 0, 103, 101, 116, 73, 110, 102, 111,
    1, 24, 3, 0, 0, 0, 1, 0, 0, 0, 49, 1, 0, 0, 0, 50, 1, 0, 0, 0, 51 };
const std::map<std::string, std::string> FUNCTION_PARA_MAP = { { "1", "value1" }, { "2", "value2" } };
const std::vector<uint8_t> MAP_ENCODE_RESULT = { 22, 7, 0, 0, 0, 103, 101, 116, 73, 110, 102, 111,
    1, 23, 2, 0, 0, 0, 1, 0, 0, 0, 49, 6, 0, 0, 0, 118, 97, 108, 117, 101, 49, 1, 0, 0, 0, 50, 6,
    0, 0, 0, 118, 97, 108, 117, 101, 50 };

} // namespace

class GroupMessageCodecTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GroupMessageCodecTest::SetUpTestCase() {}
void GroupMessageCodecTest::TearDownTestCase() {}
void GroupMessageCodecTest::SetUp() {}
void GroupMessageCodecTest::TearDown() {}

/**
 * @tc.name: PrimitiveTypeCodecTest001
 * @tc.desc: Encode a function with one null para to binary, then decode and check decode result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data:NULL.
     * @tc.expected: step1. test data is NULL.
     */
    std::vector<CodecData> args;
    CodecData arg1;
    ASSERT_TRUE(arg1.IsNull());
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_TRUE(resultArgs[0].IsNull());
}

/**
 * @tc.name: PrimitiveTypeCodecTest002
 * @tc.desc: Encode a function with one True para to binary, then decode and check decode result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data:True.
     * @tc.expected: step1. test data is True.
     */
    std::vector<CodecData> args;
    CodecData arg1(true);
    ASSERT_TRUE(arg1.IsBool());
    ASSERT_TRUE(arg1.GetBoolValue());
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_TRUE(resultArgs[0].IsBool());
    ASSERT_TRUE(resultArgs[0].GetBoolValue());
}

/**
 * @tc.name: PrimitiveTypeCodecTest003
 * @tc.desc: Encode a function with one False para to binary, then decode and check decode result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data:False.
     * @tc.expected: step1. test data is False.
     */
    std::vector<CodecData> args;
    CodecData arg1(false);
    ASSERT_TRUE(arg1.IsBool());
    ASSERT_FALSE(arg1.GetBoolValue());
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_TRUE(resultArgs[0].IsBool());
    ASSERT_FALSE(resultArgs[0].GetBoolValue());
}

/**
 * @tc.name: PrimitiveTypeCodecTest004
 * @tc.desc: Encode a function with one int para to binary, then decode and check decode result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data 100(int).
     * @tc.expected: step1. test data is 100.
     */
    std::vector<CodecData> args;
    CodecData arg1(FUNCTION_PARA_INT_VALUE);
    ASSERT_TRUE(arg1.IsInt());
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_TRUE(resultArgs[0].IsInt());
    ASSERT_EQ(FUNCTION_PARA_INT_VALUE, resultArgs[0].GetIntValue());
}

/**
 * @tc.name: PrimitiveTypeCodecTest005
 * @tc.desc: Encode a function with one String para to binary, then decode and check decode result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data 'onePara'.
     * @tc.expected: step1. test data is 'onePara'.
     */
    std::vector<CodecData> args;
    CodecData arg1(FUNCTION_PARA_STRING_VALUE);
    ASSERT_TRUE(arg1.IsString());
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_TRUE(resultArgs[0].IsString());
    ASSERT_EQ(FUNCTION_PARA_STRING_VALUE, resultArgs[0].GetStringValue());
}

/**
 * @tc.name: PrimitiveTypeCodecTest006
 * @tc.desc: Encode a function with int8 array para to binary and check the encode result,
 * then decode and check decode result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data .
     */
    CodecData arg1(FUNCTION_PARA_INT8_ARRAY);
    std::vector<CodecData> args;
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     * @tc.expected: step2. encode data equals to prepared binary data.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);
    for (size_t idx = 0; idx < encodeBuf.size(); idx++) {
        ASSERT_EQ(encodeBuf.at(idx), INT8_ARRAY_ENCODE_RESULT.at(idx));
    }

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_EQ(resultArgs[0].GetType(), BufferDataType::TYPE_INT8_ARRAY);

    std::vector<int8_t> data = resultArgs[0].GetInt8ArrayValue();
    for (size_t idx = 0; idx < data.size(); idx++) {
        ASSERT_EQ(data.at(idx), FUNCTION_PARA_INT8_ARRAY[idx]);
    }
}

/**
 * @tc.name: PrimitiveTypeCodecTest007
 * @tc.desc: Encode a function with int8 array para to binary and check the encode result ,then decode and check decode
 * result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data .
     */
    CodecData arg1(FUNCTION_PARA_INT16_ARRAY);

    std::vector<CodecData> args;
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     * @tc.expected: step2. encode data equals to prepared binary data.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);

    for (size_t idx = 0; idx < encodeBuf.size(); idx++) {
        ASSERT_EQ(encodeBuf.at(idx), INT16_ARRAY_ENCODE_RESULT.at(idx));
    }

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_EQ(resultArgs[0].GetType(), BufferDataType::TYPE_INT16_ARRAY);

    std::vector<int16_t> data = resultArgs[0].GetInt16ArrayValue();
    for (size_t idx = 0; idx < data.size(); idx++) {
        ASSERT_EQ(data.at(idx), FUNCTION_PARA_INT16_ARRAY.at(idx));
    }
}

/**
 * @tc.name: PrimitiveTypeCodecTest008
 * @tc.desc: Encode a function with int8 array para to binary and check the encode result ,then decode and check decode
 * result.
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data .
     */
    CodecData arg1(FUNCTION_PARA_INT32_ARRAY);
    std::vector<CodecData> args;
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     * @tc.expected: step2. encode data equals to prepared binary data.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);
    for (size_t idx = 0; idx < encodeBuf.size(); idx++) {
        ASSERT_EQ(encodeBuf.at(idx), INT32_ARRAY_ENCODE_RESULT.at(idx));
    }

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_EQ(resultArgs[0].GetType(), BufferDataType::TYPE_INT32_ARRAY);

    std::vector<int32_t> data = resultArgs[0].GetInt32ArrayValue();
    for (size_t idx = 0; idx < data.size(); idx++) {
        ASSERT_EQ(data.at(idx), FUNCTION_PARA_INT32_ARRAY.at(idx));
    }
}

/**
 * @tc.name: PrimitiveTypeCodecTest009
 * @tc.desc: Encode a function with int8 array para to binary and check the encode result ,then decode and check decode
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data .
     */
    CodecData arg1(FUNCTION_PARA_SET);
    std::vector<CodecData> args;
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     * @tc.expected: step2. encode data equals to prepared binary data.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);
    std::vector<uint8_t>::iterator encodeIter = encodeBuf.begin();
    std::vector<uint8_t>::const_iterator encodeIter1 = SET_ENCODE_RESULT.begin();

    while (encodeIter != encodeBuf.end()) {
        ASSERT_EQ(*encodeIter, *encodeIter1);
        encodeIter++;
        encodeIter1++;
    }

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    std::vector<CodecData> resultArgs = result.GetArgs();
    std::set<std::string> data = resultArgs[0].GetSetValue();
    std::set<std::string>::iterator iter = data.begin();
    std::set<std::string>::iterator iter1 = FUNCTION_PARA_SET.begin();

    while (iter != data.end()) {
        ASSERT_EQ(*iter, *iter1);
        iter++;
        iter1++;
    }
}

/**
 * @tc.name: PrimitiveTypeCodecTest010
 * @tc.desc: Encode a function with int8 array para to binary and check the encode result ,then decode and check decode
 * @tc.type: FUNC
 */
HWTEST_F(GroupMessageCodecTest, PrimitiveTypeCodecTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Prepare FunctionCall instance & test data .
     */
    CodecData arg1(FUNCTION_PARA_MAP);
    std::vector<CodecData> args;
    args.push_back(arg1);
    FunctionCall functionCall(FUNCTION_NAME_VALUE, args);

    /**
     * @tc.steps: step2. Prepare codec instance & encode the function call.
     * @tc.expected: step2. encode data equals to prepared binary data.
     */
    std::vector<uint8_t> encodeBuf;
    StandardFunctionCodec codec;
    codec.EncodeFunctionCall(functionCall, encodeBuf);
    std::vector<uint8_t>::iterator encodeIter = encodeBuf.begin();
    std::vector<uint8_t>::const_iterator encodeIter1 = MAP_ENCODE_RESULT.begin();

    while (encodeIter != encodeBuf.end()) {
        ASSERT_EQ(*encodeIter, *encodeIter1);
        encodeIter++;
        encodeIter1++;
    }

    /**
     * @tc.steps: step3. Decode the binary to FunctionCall, check function Name & para type and value.
     * @tc.expected: step3. check function name & para type and value.
     */
    FunctionCall result;
    codec.DecodeFunctionCall(encodeBuf, result);
    ASSERT_EQ(FUNCTION_NAME_VALUE, result.GetFuncName());
    std::vector<CodecData> resultArgs = result.GetArgs();
    ASSERT_EQ(resultArgs[0].GetType(), BufferDataType::TYPE_MAP);

    std::map<std::string, std::string> data = resultArgs[0].GetMapValue();
    std::map<std::string, std::string>::iterator iter = data.begin();
    std::map<std::string, std::string>::const_iterator iter1 = FUNCTION_PARA_MAP.begin();

    while (iter != data.end()) {
        ASSERT_EQ(iter->first, iter1->first);
        ASSERT_EQ(iter->second, iter1->second);
        iter++;
        iter1++;
    }
}

} // namespace OHOS::Ace::Framework
