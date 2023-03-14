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

#include "res_desc_test.h"

#include <gtest/gtest.h>

#include "res_desc.h"
#include "test_common.h"
#include "utils/string_utils.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class ResDescTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResDescTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResDescTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResDescTest::SetUp()
{
    // step 3: input testcase setup step
    HILOG_DEBUG("ResDescTest setup");
}

void ResDescTest::TearDown()
{
    // step 4: input testcase teardown step
    HILOG_DEBUG("ResDescTest teardown");
}

/*
 * @tc.name: ResDescFuncTest001
 * @tc.desc: Test IsRef function, non file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResDescTest, ResDescFuncTest001, TestSize.Level1)
{
    std::string str;
    int id;
    ResType resType;
    str.assign("abc");
    EXPECT_TRUE(!IdItem::IsRef(str, resType, id));

    str.assign("$abc");
    EXPECT_TRUE(!IdItem::IsRef(str, resType, id));

    str.assign("$abc:");
    EXPECT_TRUE(!IdItem::IsRef(str, resType, id));

    str.assign("$abc:abc");
    EXPECT_TRUE(!IdItem::IsRef(str, resType, id));

    str.assign("$abc:123456");
    EXPECT_TRUE(!IdItem::IsRef(str, resType, id));

    str.assign("$string:123456");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::STRING, resType);
    EXPECT_EQ(123456, id);
    str.assign("$boolean:100001");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::BOOLEAN, resType);
    EXPECT_EQ(100001, id);
    str.assign("$color:66666");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::COLOR, resType);
    EXPECT_EQ(66666, id);
    str.assign("$float:100002");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::FLOAT, resType);
    EXPECT_EQ(100002, id);
    str.assign("$integer:2008168");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::INTEGER, resType);
    EXPECT_EQ(2008168, id);
    str.assign("$pattern:654321");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::PATTERN, resType);
    EXPECT_EQ(654321, id);
    str.assign("$theme:99999");
    EXPECT_TRUE(IdItem::IsRef(str, resType, id));
    EXPECT_EQ(ResType::THEME, resType);
    EXPECT_EQ(99999, id);
}

void TestKeyParam(KeyType keyType, int value, std::string expectStr)
{
    KeyParam keyParam;
    keyParam.type_ = keyType;
    keyParam.value_ = value;
    keyParam.InitStr();
    EXPECT_EQ(expectStr, keyParam.str_);
}

/*
 * @tc.name: ResDescFuncTest002
 * @tc.desc: Test IsRef function, non file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResDescTest, ResDescFuncTest002, TestSize.Level1)
{
    TestKeyParam(KeyType::LANGUAGES, 25966, "en");
    TestKeyParam(KeyType::LANGUAGES, 31336, "zh");

    TestKeyParam(KeyType::REGION, 21843, "US");
    TestKeyParam(KeyType::REGION, 17230, "CN");

    TestKeyParam(KeyType::DIRECTION, 0, VERTICAL);
    TestKeyParam(KeyType::DIRECTION, 1, HORIZONTAL);

    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_PHONE, PHONE_STR);
    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_TABLET, TABLET_STR);
    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_CAR, CAR_STR);
    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_PC, PC_STR);
    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_TV, TV_STR);
    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_WEARABLE, WEARABLE_STR);
    TestKeyParam(KeyType::DEVICETYPE, DeviceType::DEVICE_NOT_SET, "not_device_type");

    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_SDPI, RE_120_STR);
    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_MDPI, RE_160_STR);
    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_LDPI, RE_240_STR);
    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_XLDPI, RE_320_STR);
    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_XXLDPI, RE_480_STR);
    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_XXXLDPI, RE_640_STR);
    TestKeyParam(KeyType::SCREEN_DENSITY, ScreenDensity::SCREEN_DENSITY_NOT_SET, "not_screen_density");
}