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
#include "bundle_parser.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const string RESOURCE_ROOT_PATH = "/data/test/bms_bundle/";
const string INVALID_BUNDLE_SUFFIX = ".rpk";
}  // namespace

class BmsBundleParserModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void CheckBundleParserResult(const std::ostringstream &pathStream) const
    {
        BundleParser bundleParser;
        InnerBundleInfo innerBundleInfo;
        ErrCode result = bundleParser.Parse(pathStream.str(), innerBundleInfo);
        BundleInfo bundleInfo = innerBundleInfo.GetBaseBundleInfo();
        EXPECT_EQ(result, ERR_OK);

        EXPECT_EQ(bundleInfo.name, "com.third.hiworld.example1");
        EXPECT_EQ(bundleInfo.versionName, "1.0");

        uint32_t versionCode = 1;
        EXPECT_EQ(bundleInfo.versionCode, versionCode);
        EXPECT_EQ(bundleInfo.vendor, "example");

        EXPECT_EQ(bundleInfo.maxSdkVersion, -1);
        EXPECT_EQ(bundleInfo.minSdkVersion, -1);
    }

protected:
    std::ostringstream pathStream_;
};

void BmsBundleParserModuleTest::SetUpTestCase()
{}

void BmsBundleParserModuleTest::TearDownTestCase()
{}

void BmsBundleParserModuleTest::SetUp()
{}

void BmsBundleParserModuleTest::TearDown()
{
    pathStream_.clear();
}

/**
 * @tc.number: MultiAbilityParse_0100
 * @tc.name: Test parse the bundle with two abilities success
 * @tc.desc: parse a bundle with two abilities
 */
HWTEST_F(BmsBundleParserModuleTest, MultiAbilityParse_0100, Function | MediumTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH << "bmsThirdBundle2" << Constants::INSTALL_FILE_SUFFIX;
    CheckBundleParserResult(pathStream_);
}

/**
 * @tc.number: NullAbility_0200
 * @tc.name: Test parse the bundle with no ability success
 * @tc.desc: parse a bundle with no ability
 */
HWTEST_F(BmsBundleParserModuleTest, NullAbility_0200, Function | MediumTest | Level2)
{
    pathStream_ << RESOURCE_ROOT_PATH << "bmsThirdBundle1" << Constants::INSTALL_FILE_SUFFIX;
    CheckBundleParserResult(pathStream_);
}

/**
 * @tc.number: InvalidBundleProfileName_0300
 * @tc.name: Test parse the bundle with invalid profile success
 * @tc.desc: 1. parse a bundle with invalid profile suffix
 *           2. parse a bundle with invalid profile format
 */
HWTEST_F(BmsBundleParserModuleTest, InvalidBundleProfileName_0300, Function | MediumTest | Level2)
{
    pathStream_ << RESOURCE_ROOT_PATH << "bmsThirdBundle11" << Constants::INSTALL_FILE_SUFFIX;
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;

    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_NO_PROFILE) << "fail, parser bundle success!";

    pathStream_.clear();

    pathStream_ << RESOURCE_ROOT_PATH << "bmsThirdBundle14" << Constants::INSTALL_FILE_SUFFIX;
    result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED) << "fail, parser bundle success!";
}

/**
 * @tc.number: RepeatBundleParser_0400
 * @tc.name: Test parse a bundle ten times success
 * @tc.desc: parser a bundle ten times
 */
HWTEST_F(BmsBundleParserModuleTest, RepeatBundleParser_0400, Function | MediumTest | Level1)
{
    int count = 10;
    pathStream_ << RESOURCE_ROOT_PATH << "bmsThirdBundle1" << Constants::INSTALL_FILE_SUFFIX;
    for (int i = 0; i < count; i++) {
        CheckBundleParserResult(pathStream_);
    }
}
