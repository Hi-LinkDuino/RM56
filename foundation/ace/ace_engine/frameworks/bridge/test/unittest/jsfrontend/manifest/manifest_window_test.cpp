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

#include <limits>

#include "gtest/gtest.h"

#include "frameworks/bridge/common/manifest/manifest_window.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

#ifndef WEARABLE_PRODUCT
constexpr int32_t DESIGN_WIDTH_DEFAULT = 720;
#else
constexpr int32_t DESIGN_WIDTH_DEFAULT = 454;
#endif
constexpr int32_t DESIGN_WIDTH_CUSTOM = 800;
constexpr int32_t DESIGN_WIDTH_MAX = std::numeric_limits<int32_t>::max();

const std::string MANIFEST_DESIGN_WIDTH_CUSTOM = ""
                                                 "{                                                          "
                                                 "  \"window\": {                                            "
                                                 "                \"designWidth\" : 800,                     "
                                                 "                \"autoDesignWidth\" : false                "
                                                 "              }                                            "
                                                 "}";

const std::string MANIFEST_DESIGN_WIDTH_NEGATIVE = ""
                                                   "{                                                          "
                                                   "  \"window\": {                                            "
                                                   "                \"designWidth\" : -1,                      "
                                                   "                \"autoDesignWidth\" : false                "
                                                   "              }                                            "
                                                   "}";

const std::string MANIFEST_DESIGN_WIDTH_ZERO = ""
                                               "{                                                          "
                                               "  \"window\": {                                            "
                                               "                \"designWidth\" : 0,                       "
                                               "                \"autoDesignWidth\" : false                "
                                               "              }                                            "
                                               "}";

const std::string MANIFEST_DESIGN_WIDTH_OVER_LIMIT = ""
                                                     "{                                                          "
                                                     "  \"window\": {                                            "
                                                     "                \"designWidth\" : 2147483648,              "
                                                     "                \"autoDesignWidth\" : false                "
                                                     "              }                                            "
                                                     "}";

const std::string MANIFEST_DESIGN_WIDTH_FORMAT_ERR = ""
                                                     "{                                                          "
                                                     "  \"window\": {                                            "
                                                     "                \"designWidth\" : \"800\",                 "
                                                     "                \"autoDesignWidth\" : false                "
                                                     "              }                                            "
                                                     "}";

const std::string MANIFEST_DESIGN_WIDTH_NONE = ""
                                               "{                                                          "
                                               "  \"window\": {                                            "
                                               "                \"autoDesignWidth\" : false                "
                                               "              }                                            "
                                               "}";

const std::string MANIFEST_AUTO_DESIGN_WIDTH_TRUE = ""
                                                    "{                                                          "
                                                    "  \"window\": {                                            "
                                                    "                \"autoDesignWidth\" : true                 "
                                                    "              }                                            "
                                                    "}";

const std::string MANIFEST_AUTO_DESIGN_WIDTH_FALSE = ""
                                                     "{                                                          "
                                                     "  \"window\": {                                            "
                                                     "                \"autoDesignWidth\" : false                "
                                                     "              }                                            "
                                                     "}";

const std::string MANIFEST_AUTO_DESIGN_WIDTH_FORMAT_ERR = ""
                                                          "{                                                          "
                                                          "  \"window\": {                                            "
                                                          "                \"autoDesignWidth\" : \"true\"             "
                                                          "              }                                            "
                                                          "}";

const std::string MANIFEST_AUTO_DESIGN_WIDTH_NONE = ""
                                                    "{                                                          "
                                                    "  \"window\": {                                            "
                                                    "              }                                            "
                                                    "}";

void AssertParseMenifest(const std::string& json, int32_t expectDesignWidth, bool expectAutoDesignWidth)
{
    auto manifestWindow = AceType::MakeRefPtr<ManifestWindow>();
    auto rootJson = JsonUtil::ParseJsonString(json);
    manifestWindow->WindowParse(rootJson);
    auto& config = manifestWindow->GetWindowConfig();
    ASSERT_EQ(config.designWidth, expectDesignWidth);
    ASSERT_EQ(config.autoDesignWidth, expectAutoDesignWidth);
}

} // namespace

class ManifestWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ManifestWindowTest::SetUpTestCase() {}
void ManifestWindowTest::TearDownTestCase() {}
void ManifestWindowTest::SetUp() {}
void ManifestWindowTest::TearDown() {}

/**
 * @tc.name: ManifestParseDesignWidth001
 * @tc.desc: Design width in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseDesignWidth001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with normal value.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_DESIGN_WIDTH_CUSTOM, DESIGN_WIDTH_CUSTOM, false);
}

/**
 * @tc.name: ManifestParseDesignWidth002
 * @tc.desc: Design width of negative value in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseDesignWidth002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with negative value.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_DESIGN_WIDTH_NEGATIVE, DESIGN_WIDTH_DEFAULT, false);
}

/**
 * @tc.name: ManifestParseDesignWidth003
 * @tc.desc: Design width of zero value in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseDesignWidth003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with zero value.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_DESIGN_WIDTH_ZERO, DESIGN_WIDTH_DEFAULT, false);
}

/**
 * @tc.name: ManifestParseDesignWidth004
 * @tc.desc: Design width of over limit value in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseDesignWidth004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with over limit value.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_DESIGN_WIDTH_OVER_LIMIT, DESIGN_WIDTH_MAX, false);
}

/**
 * @tc.name: ManifestParseDesignWidth005
 * @tc.desc: Design width of value not number in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseDesignWidth005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with value not number.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_DESIGN_WIDTH_FORMAT_ERR, DESIGN_WIDTH_DEFAULT, false);
}

/**
 * @tc.name: ManifestParseDesignWidth006
 * @tc.desc: No design width config in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseDesignWidth006, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with no design width.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_DESIGN_WIDTH_NONE, DESIGN_WIDTH_DEFAULT, false);
}

/**
 * @tc.name: ManifestParseAutoDesignWidth001
 * @tc.desc: Auto design width in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseAutoDesignWidth001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with autoDesignWidth = true.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_AUTO_DESIGN_WIDTH_TRUE, DESIGN_WIDTH_DEFAULT, true);
}

/**
 * @tc.name: ManifestParseAutoDesignWidth002
 * @tc.desc: Auto design width of "false" in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseAutoDesignWidth002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with autoDesignWidth = false.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_AUTO_DESIGN_WIDTH_FALSE, DESIGN_WIDTH_DEFAULT, false);
}

/**
 * @tc.name: ManifestParseAutoDesignWidth003
 * @tc.desc: Auto design width with wrong format in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseAutoDesignWidth003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with autoDesignWidth of wrong format.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_AUTO_DESIGN_WIDTH_FORMAT_ERR, DESIGN_WIDTH_DEFAULT, false);
}

/**
 * @tc.name: ManifestParseAutoDesignWidth004
 * @tc.desc: Auto design width not config in manifest can be parsed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ManifestWindowTest, ManifestParseAutoDesignWidth004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Construct ManifestWindow and parse json with autoDesignWidth not config.
     * @tc.expected: step1. "designWidth" and "autoDesignWidth" can be parsed correctly.
     */
    AssertParseMenifest(MANIFEST_AUTO_DESIGN_WIDTH_NONE, DESIGN_WIDTH_DEFAULT, false);
}

} // namespace OHOS::Ace::Framework
