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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/shadow.h"
#include "core/components/test/json/shadow_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Color COLOR_DEFAULT = Color(0xff000000);
constexpr double BLUR_RADIUS_DEFAULT = 0.0;
const Offset OFFSET_DEFAULT = Offset(0.0, 0.0);
const Color COLOR_VALUE = Color(0xffff0000);
constexpr double BLUR_RADIUS_VALUE = 10.0;
constexpr double SPREAD_RADIUS_VALUE = 20.0;
constexpr float ELEVATION = 30.0f;
const Offset OFFSET_VALUE = Offset(10.0, 10.0);

} // namespace

class ShadowCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    Shadow CreateFromDsl(const std::string content) const;
};

void ShadowCreatorTest::SetUpTestCase() {}
void ShadowCreatorTest::TearDownTestCase() {}
void ShadowCreatorTest::SetUp() {}
void ShadowCreatorTest::TearDown() {}

Shadow ShadowCreatorTest::CreateFromDsl(const std::string content) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> shadowJson = JsonUtil::ParseJsonString(jsonStr);
    return ShadowCreator::CreateFromJson(*shadowJson);
}

/**
 * @tc.name: ShadowCreator001
 * @tc.desc: Verify ShadowCreator can create a shadow with value.
 * @tc.type: FUNC
 * @tc.require: AR000DAQU7
 * @tc.author: chenlien
 */
HWTEST_F(ShadowCreatorTest, ShadowCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of shadow with all properties.
     */
    const std::string content = ""
                                "{                                   "
                                "    \"color\": {                    "
                                "      \"className\": \"Color\",     "
                                "      \"alpha\": 255,               "
                                "      \"red\": 255,                 "
                                "      \"green\": 0,                 "
                                "      \"blue\": 0                   "
                                "    },                              "
                                "    \"blurRadius\": 10,             "
                                "    \"offset\":{                    "
                                "        \"dx\":10,                  "
                                "        \"dy\":10                   "
                                "    }                               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get shadow.
     * @tc.expected: step2. shadow properties are set correctly.
     */
    Shadow shadow = CreateFromDsl(content);
    ASSERT_TRUE(shadow.GetColor() == COLOR_VALUE);
    ASSERT_TRUE(shadow.GetOffset() == OFFSET_VALUE);
    ASSERT_TRUE(shadow.GetBlurRadius() == BLUR_RADIUS_VALUE);
}

/**
 * @tc.name: ShadowCreator002
 * @tc.desc: Verify ShadowCreator can create a shadow with value.
 * @tc.type: FUNC
 * @tc.require: AR000DAQU7
 * @tc.author: chenlien
 */
HWTEST_F(ShadowCreatorTest, ShadowCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of shadow with none properties.
     */
    const std::string content = ""
                                "{}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get shadow.
     * @tc.expected: step2. shadow properties are set as default.
     */
    Shadow shadow = CreateFromDsl(content);
    ASSERT_TRUE(shadow.GetColor() == COLOR_DEFAULT);
    ASSERT_TRUE(shadow.GetOffset() == OFFSET_DEFAULT);
    ASSERT_TRUE(shadow.GetBlurRadius() == BLUR_RADIUS_DEFAULT);
}

/**
 * @tc.name: ShadowCreator003
 * @tc.desc: Verify ShadowCreator can create a shadow with spreadRadius.
 * @tc.type: FUNC
 * @tc.require: AR000DAQU7
 * @tc.author: jiangdayuan
 */
HWTEST_F(ShadowCreatorTest, ShadowCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of shadow with correct properties.
     */
    const std::string content = ""
                                "{"
                                "    \"blurRadius\": 10,             "
                                "    \"spreadRadius\": 20,           "
                                "    \"offset\":{                    "
                                "        \"dx\":10,                  "
                                "        \"dy\":10                   "
                                "    }                               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get shadow.
     * @tc.expected: step2. shadow properties are set correctly.
     */
    Shadow shadow = CreateFromDsl(content);
    ASSERT_TRUE(shadow.GetBlurRadius() == BLUR_RADIUS_VALUE);
    ASSERT_TRUE(shadow.GetOffset() == OFFSET_VALUE);
    ASSERT_TRUE(shadow.GetSpreadRadius() == SPREAD_RADIUS_VALUE);
    ASSERT_FALSE(shadow.GetHardwareAcceleration());
}

/**
 * @tc.name: ShadowCreator004
 * @tc.desc: Verify ShadowCreator can create a shadow with elevation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQU7
 * @tc.author: jiangdayuan
 */
HWTEST_F(ShadowCreatorTest, ShadowCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of shadow with elevation properties.
     */
    const std::string content = ""
                                "{"
                                "    \"elevation\": 30,              "
                                "    \"offset\":{                    "
                                "        \"dx\":10,                  "
                                "        \"dy\":10                   "
                                "    }                               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get shadow.
     * @tc.expected: step2. shadow properties are set correctly.
     */
    Shadow shadow = CreateFromDsl(content);
    ASSERT_TRUE(shadow.GetElevation() == ELEVATION);
    ASSERT_TRUE(shadow.GetOffset() == OFFSET_VALUE);
    ASSERT_TRUE(shadow.IsValid());
    // set elevation for hardrware acceleration.
    ASSERT_TRUE(shadow.GetHardwareAcceleration());
}

/**
 * @tc.name: ShadowCreator005
 * @tc.desc: Verify error shadow properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAQU7
 * @tc.author: jiangdayuan
 */
HWTEST_F(ShadowCreatorTest, ShadowCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of shadow with error properties.
     */
    const std::string content = ""
                                "{"
                                "    \"elevation\": -10,             "
                                "    \"blurRadius\": -10,             "
                                "    \"spreadRadius\": 20            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get shadow.
     * @tc.expected: step2. shadow properties should be set default value.
     */
    Shadow shadow = CreateFromDsl(content);
    ASSERT_TRUE(NearZero(shadow.GetElevation()));
    ASSERT_TRUE(NearEqual(shadow.GetSpreadRadius(), SPREAD_RADIUS_VALUE));
    ASSERT_TRUE(NearZero(shadow.GetBlurRadius()));
    ASSERT_TRUE(shadow.GetColor() == Color::BLACK);
    ASSERT_TRUE(shadow.IsValid());
    ASSERT_FALSE(shadow.GetHardwareAcceleration());
}
} // namespace OHOS::Ace