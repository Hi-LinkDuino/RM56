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

#include <gtest/gtest.h>

#include "session.h"
#include "softbus_errcode.h"

using namespace testing::ext;

namespace OHOS {
class TransSdkStreamTest : public testing::Test {
public:
    TransSdkStreamTest()
    {}
    ~TransSdkStreamTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override
    {}
    void TearDown() override
    {}
};

void TransSdkStreamTest::SetUpTestCase(void)
{}

void TransSdkStreamTest::TearDownTestCase(void)
{}

/**
 * @tc.name: SendStreamTest001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransSdkStreamTest, SendStreamTest001, TestSize.Level0)
{
    int ret;
    int sessionId = 1;
    const StreamData streamData = {0};
    const StreamData ext = {0};
    const StreamFrameInfo param = {0};

    ret = SendStream(-1, &streamData, &ext, &param);
    EXPECT_EQ(SOFTBUS_TRANS_INVALID_SESSION_ID, ret);

    ret = SendStream(sessionId, NULL, &ext, &param);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SendStream(sessionId, &streamData, NULL, &param);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SendStream(sessionId, &streamData, &ext, NULL);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}
}