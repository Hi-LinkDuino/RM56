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

#include "resource_manager_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>
#define private public

#include "res_config.h"
#include "resource_manager.h"
#include "resource_manager_impl.h"
#include "test_common.h"
#include "utils/errors.h"
#include "utils/string_utils.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

static const int NON_EXIST_ID = 1111;

static const char *g_nonExistName = "non_existent_name";
class ResourceManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerTest() : rm(nullptr)
    {}

    ~ResourceManagerTest()
    {}

public:
    ResourceManager *rm;

    int GetResId(std::string name, ResType resType) const;

    void TestStringByName(const char *name, const char *cmp) const;

    void TestStringById(const char *name, const char *cmp) const;

    void TestPluralStringById(int quantity, const char *cmp, bool format = false) const;

    void TestPluralStringByName(int quantity, const char *cmp, bool format = false) const;

    void AddResource(const char *language, const char *script, const char *region);
};

int ResourceManagerTest::GetResId(std::string name, ResType resType) const
{
    auto idv = ((ResourceManagerImpl *)rm)->hapManager_->GetResourceListByName(name.c_str(), resType);
    if (idv == nullptr) {
        return -1;
    }

    PrintIdValues(idv);
    if (idv->GetLimitPathsConst().size() > 0) {
        return idv->GetLimitPathsConst()[0]->GetIdItem()->id_;
    }
    return OBJ_NOT_FOUND;
}

void ResourceManagerTest::TestStringByName(const char *name, const char *cmp) const
{
    RState rState;
    std::string outValue;
    rState = rm->GetStringByName(name, outValue);
    ASSERT_EQ(SUCCESS, rState);
    HILOG_DEBUG("%s : %s", name, outValue.c_str());
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTest::TestStringById(const char *name, const char *cmp) const
{
    RState rState;
    std::string outValue;
    int id = GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTest::AddResource(const char *language, const char *script, const char *region)
{
    if (language != nullptr || region != nullptr) {
        auto rc = CreateResConfig();
        if (rc == nullptr) {
            EXPECT_TRUE(false);
            return;
        }
        rc->SetLocaleInfo(language, script, region);
        rm->UpdateResConfig(*rc);
        delete rc;
    }
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
}

void ResourceManagerTest::TestPluralStringById(int quantity, const char *cmp, bool format) const
{
    RState ret;
    std::string outValue;
    int id = GetResId("eat_apple", ResType::PLURALS);
    if (format) {
        ret = rm->GetPluralStringByIdFormat(outValue, id, quantity, quantity);
    } else {
        ret = rm->GetPluralStringById(id, quantity, outValue);
    }

    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTest::TestPluralStringByName(int quantity, const char *cmp, bool format) const
{
    RState ret;
    std::string outValue;
    const char *name = "eat_apple";
    if (format) {
        ret = rm->GetPluralStringByNameFormat(outValue, name, quantity, quantity);
    } else {
        ret = rm->GetPluralStringByName(name, quantity, outValue);
    }

    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResourceManagerTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerTest::SetUp(void)
{
    this->rm = CreateResourceManager();
}

void ResourceManagerTest::TearDown(void)
{
    delete this->rm;
}

/*
 * @tc.name: ResourceManagerAddResourceTest001
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest001, TestSize.Level1)
{
    // success cases
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
};

/*
 * @tc.name: ResourceManagerAddResourceTest002
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest002, TestSize.Level1)
{
    // error cases
    // file not exist
    bool ret = rm->AddResource("/data/test/do_not_exist.resources");
    ASSERT_TRUE(!ret);
}

/*
 * @tc.name: ResourceManagerAddResourceTest003
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest003, TestSize.Level1)
{
    // error cases
    // reload the same path
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(!ret);
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest001
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest001, TestSize.Level1)
{
    // success cases
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_CAR);
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(SUCCESS, state);
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest002
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest002, TestSize.Level1)
{
    // error cases
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(LOCALEINFO_IS_NULL, state);
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest003
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest003, TestSize.Level1)
{
    // error cases
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo(nullptr, nullptr, "US");
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(LOCALEINFO_IS_NULL, state);
}

/*
 * load a hap, defaultConfig set to en, then switch to zh
 * @tc.name: ResourceManagerUpdateResConfigTest004
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest004, TestSize.Level1)
{
    // success case
    bool ret = true;
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    state = rm->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state);
    ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    if (!ret) {
        EXPECT_TRUE(false);
        delete rc;
        return;
    }
    // update to another language, will trigger reload
    // before reload:
    TestStringByName("app_name", "App Name");

    rc->SetLocaleInfo("zh", nullptr, nullptr);
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(SUCCESS, state);
    // after reload:
    TestStringByName("app_name", "应用名称");
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest005
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest005, TestSize.Level1)
{
    // error case
    AddResource("zh", nullptr, nullptr);

    // make a fake hapResource, then reload will fail
    HapResource *hapResource = new HapResource("/data/test/non_exist", 0, nullptr, nullptr);
    ((ResourceManagerImpl *)rm)->hapManager_->hapResources_.push_back(hapResource);
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(HAP_INIT_FAILED, state);
}

/*
 * @tc.name: ResourceManagerGetResConfigTest001
 * @tc.desc: Test GetResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetResConfigTest001, TestSize.Level1)
{
    // success cases
    ResConfigImpl rc;
    rm->GetResConfig(rc);
    EXPECT_EQ(nullptr, rc.GetLocaleInfo());
    EXPECT_EQ(DIRECTION_NOT_SET, rc.GetDirection());
    EXPECT_EQ(SCREEN_DENSITY_NOT_SET, rc.GetScreenDensity());
    EXPECT_EQ(DEVICE_NOT_SET, rc.GetDeviceType());
}

/*
 * @tc.name: ResourceManagerGetResConfigTest002
 * @tc.desc: Test GetResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetResConfigTest002, TestSize.Level1)
{
    // success cases
    RState state;
    {
        ResConfig *rc = CreateResConfig();
        if (rc == nullptr) {
            EXPECT_TRUE(false);
            return;
        }
        rc->SetLocaleInfo("en", nullptr, "US");
        rc->SetDeviceType(DeviceType::DEVICE_CAR);
        state = rm->UpdateResConfig(*rc);
        delete rc;
        EXPECT_EQ(SUCCESS, state);
    }

    ResConfigImpl rc;
    rm->GetResConfig(rc);
    EXPECT_EQ("en", std::string(rc.GetLocaleInfo()->GetLanguage()));
    EXPECT_EQ(DEVICE_CAR, rc.GetDeviceType());
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest001
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdTest001, TestSize.Level1)
{
    AddResource("en", nullptr, nullptr);

    TestStringById("app_name", "App Name");

    TestStringById("copyright_text", "XXXXXX All rights reserved. ©2011-2019");

    TestStringById("string_ref", "XXXXXX All rights reserved. ©2011-2019");

    TestStringById("string_ref2", "XXXXXX All rights reserved. ©2011-2019");
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest002
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    TestStringById("app_name", "应用名称");

    TestStringById("copyright_text", "版权所有 ©2011-2019 XXXX有限公司保留一切权利");

    TestStringById("string_ref", "$aaaaa");

    TestStringById("string_ref2", "$aaaaa");
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest003
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdTest003, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest001
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameTest001, TestSize.Level1)
{
    AddResource("en", nullptr, nullptr);

    TestStringByName("app_name", "App Name");

    TestStringByName("copyright_text", "XXXXXX All rights reserved. ©2011-2019");

    TestStringByName("string_ref", "XXXXXX All rights reserved. ©2011-2019");

    TestStringByName("string_ref2", "XXXXXX All rights reserved. ©2011-2019");
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest002
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    TestStringByName("app_name", "应用名称");

    TestStringByName("copyright_text", "版权所有 ©2011-2019 XXXX有限公司保留一切权利");

    TestStringByName("string_ref", "$aaaaa");

    TestStringByName("string_ref2", "$aaaaa");
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest003
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameTest003, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest001
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringFormatByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    const char *name = "app_name";
    int id = GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    RState state = rm->GetStringFormatById(outValue, id, 101);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ("应用名称", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest002
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringFormatByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringFormatById(outValue, NON_EXIST_ID, 101);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest001
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringFormatByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    const char *name = "app_name";
    std::string outValue;
    RState state = rm->GetStringFormatByName(outValue, name, 101);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ("应用名称", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest002
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringFormatByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringFormatByName(outValue, g_nonExistName, 101);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringArrayByIdTest001
 * @tc.desc: Test GetStringArrayById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringArrayByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    int id;
    std::vector<std::string> outValue;
    RState state;

    id = GetResId("size", ResType::STRINGARRAY);
    state = rm->GetStringArrayById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintVectorString(outValue);

    // by name
    state = rm->GetStringArrayByName("size", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintVectorString(outValue);
}

/*
 * @tc.name: ResourceManagerGetStringArrayByIdTest002
 * @tc.desc: Test GetStringArrayById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringArrayByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    RState state;
    // error case
    // not found case
    std::vector<std::string> outValue;
    state = rm->GetStringArrayById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringArrayByNameTest001
 * @tc.desc: Test GetStringArrayByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringArrayByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::vector<std::string> outValue;
    RState state;

    // by name
    state = rm->GetStringArrayByName("size", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintVectorString(outValue);
}

/*
 * @tc.name: ResourceManagerGetStringArrayByNameTest002
 * @tc.desc: Test GetStringArrayByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringArrayByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    RState state;
    // error case
    // not found case
    std::vector<std::string> outValue;
    state = rm->GetStringArrayByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest001
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    int id;
    std::map<std::string, std::string> outValue;
    RState state;

    id = GetResId("base", ResType::PATTERN);
    state = rm->GetPatternById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(3), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest002
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    int id;
    std::map<std::string, std::string> outValue;
    RState state;

    id = GetResId("child", ResType::PATTERN);
    state = rm->GetPatternById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest003
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest003, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    int id;
    std::map<std::string, std::string> outValue;
    RState state;

    id = GetResId("ccchild", ResType::PATTERN);
    state = rm->GetPatternById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(5), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest004
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest004, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    // not found case
    state = rm->GetPatternById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest001
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    state = rm->GetPatternByName("base", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(3), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest002
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    state = rm->GetPatternByName("child", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest003
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest003, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    state = rm->GetPatternByName("ccchild", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(5), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest004
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest004, TestSize.Level1)
{
    AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    // not found case
    state = rm->GetPatternByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest001
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdTest001, TestSize.Level1)
{
    AddResource("en", nullptr, "US");

    int quantity = 1;
    TestPluralStringById(quantity, "%d apple", false);

    quantity = 101;
    TestPluralStringById(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest002
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int quantity = 1;
    TestPluralStringById(quantity, "%d apples", false);

    quantity = 101;
    TestPluralStringById(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest003
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdTest003, TestSize.Level1)
{
    AddResource("pl", nullptr, "PL");

    int quantity = 1;
    TestPluralStringById(quantity, "1 jabłko");

    quantity = 2;
    TestPluralStringById(quantity, "%d jabłka");

    quantity = 23;
    TestPluralStringById(quantity, "%d jabłka");

    quantity = 12;
    TestPluralStringById(quantity, "%d jabłek");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest004
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdTest004, TestSize.Level1)
{
    AddResource("ar", nullptr, "SA");

    int quantity = 0;
    TestPluralStringById(quantity, "zero-0");
    quantity = 1;
    TestPluralStringById(quantity, "one-1");
    quantity = 2;
    TestPluralStringById(quantity, "two-2");
    quantity = 5;
    TestPluralStringById(quantity, "few-%d");
    quantity = 12;
    TestPluralStringById(quantity, "many-%d");
    quantity = 500;
    TestPluralStringById(quantity, "other-%d");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest005
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdTest005, TestSize.Level1)
{
    AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringById(NON_EXIST_ID, 1, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameTest001
 * @tc.desc: Test GetPluralStringByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByNameTest001, TestSize.Level1)
{
    AddResource("en", nullptr, "US");

    int quantity = 1;
    TestPluralStringByName(quantity, "%d apple", false);

    quantity = 101;
    TestPluralStringByName(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameTest002
 * @tc.desc: Test GetPluralStringByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByNameTest002, TestSize.Level1)
{
    AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringByName(g_nonExistName, 1, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatTest001
 * @tc.desc: Test GetPluralStringByIdFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdFormatTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int quantity = 1;
    TestPluralStringById(quantity, "1 apples", true);

    quantity = 101;
    TestPluralStringById(quantity, "101 apples", true);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatTest002
 * @tc.desc: Test GetPluralStringByIdFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByIdFormatTest002, TestSize.Level1)
{
    AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringByIdFormat(outValue, NON_EXIST_ID, 1, 1);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatTest001
 * @tc.desc: Test GetPluralStringByNameFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByNameFormatTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int quantity = 1;
    TestPluralStringByName(quantity, "1 apples", true);

    quantity = 101;
    TestPluralStringByName(quantity, "101 apples", true);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatTest002
 * @tc.desc: Test GetPluralStringByNameFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPluralStringByNameFormatTest002, TestSize.Level1)
{
    AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringByNameFormat(outValue, g_nonExistName, 1, 1);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetThemeByIdTest001
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::map<std::string, std::string> outValue;
    RState state;
    int id = GetResId("app_theme", ResType::THEME);
    ASSERT_TRUE(id > 0);
    state = rm->GetThemeById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetThemeByIdTest002
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::map<std::string, std::string> outValue;
    RState state;
    state = rm->GetThemeById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetThemeByNameTest001
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::map<std::string, std::string> outValue;
    RState state;
    state = rm->GetThemeByName("app_theme", outValue);
    ASSERT_EQ(SUCCESS, state);
    PrintMapString(outValue);

    state = rm->GetThemeByName("activity_theme", outValue);
    ASSERT_EQ(SUCCESS, state);
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetThemeByNameTest002
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::map<std::string, std::string> outValue;
    RState state;
    state = rm->GetThemeByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetBooleanByIdTest001
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    bool outValue = true;
    RState state;
    int id = GetResId("boolean_1", ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);
    state = rm->GetBooleanById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);

    id = GetResId("boolean_ref", ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);
    state = rm->GetBooleanById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);
}

/*
 * @tc.name: ResourceManagerGetBooleanByIdTest002
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    bool outValue = true;
    RState state;
    state = rm->GetBooleanById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameTest001
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    bool outValue = true;
    RState state;
    state = rm->GetBooleanByName("boolean_1", outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);

    state = rm->GetBooleanByName("boolean_ref", outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameTest002
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    bool outValue = true;
    RState state;
    state = rm->GetBooleanByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntegerByIdTest001
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int outValue;
    RState state;
    int id = GetResId("integer_1", ResType::INTEGER);
    ASSERT_TRUE(id > 0);
    state = rm->GetIntegerById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue);

    id = GetResId("integer_ref", ResType::INTEGER);
    ASSERT_TRUE(id > 0);
    state = rm->GetIntegerById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByIdTest002
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int outValue;
    RState state;
    state = rm->GetIntegerById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameTest001
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int outValue;
    RState state;
    state = rm->GetIntegerByName("integer_1", outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue);

    state = rm->GetIntegerByName("integer_ref", outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameTest002
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    int outValue;
    RState state;
    state = rm->GetIntegerByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetFloatByIdTest001
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    float outValue;
    RState state;
    int id = GetResId("width_appBar_backButton_touchTarget", ResType::FLOAT);
    ASSERT_TRUE(id > 0);
    state = rm->GetFloatById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(48, outValue); // 50vp

    id = GetResId("float_ref", ResType::FLOAT);
    ASSERT_TRUE(id > 0);
    state = rm->GetFloatById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(707, outValue); // 48vp
}

/*
 * @tc.name: ResourceManagerGetFloatByIdTest002
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    float outValue;
    RState state;
    state = rm->GetFloatById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetFloatByNameTest001
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    float outValue;
    RState state;
    state = rm->GetFloatByName("width_appBar_backButton_touchTarget", outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(48, outValue); // 50vp

    state = rm->GetFloatByName("float_ref", outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(707, outValue); // 48vp
}

/*
 * @tc.name: ResourceManagerGetFloatByNameTest002
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    float outValue;
    RState state;
    state = rm->GetFloatByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntArrayByIdTest001
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::vector<int> outValue;
    RState state;
    int id = GetResId("intarray_1", ResType::INTARRAY);
    EXPECT_TRUE(id > 0);
    state = rm->GetIntArrayById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(3), outValue.size());
    EXPECT_EQ(100, outValue[0]);
    EXPECT_EQ(200, outValue[1]);
    EXPECT_EQ(101, outValue[2]);
}

/*
 * @tc.name: ResourceManagerGetIntArrayByIdTest002
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::vector<int> outValue;
    RState state;
    state = rm->GetIntArrayById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntArrayByNameTest001
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::vector<int> outValue;
    RState state;
    state = rm->GetIntArrayByName("intarray_1", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(3), outValue.size());
    EXPECT_EQ(100, outValue[0]);
    EXPECT_EQ(200, outValue[1]);
    EXPECT_EQ(101, outValue[2]);
}

/*
 * @tc.name: ResourceManagerGetIntArrayByNameTest002
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::vector<int> outValue;
    RState state;
    state = rm->GetIntArrayByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetColorByIdTest001
 * @tc.desc: Test GetColorById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    int id = GetResId("divider_color", ResType::COLOR);
    EXPECT_TRUE(id > 0);
    state = rm->GetColorById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(268435456), outValue); // #10000000

    id = GetResId("color_aboutPage_title_primary", ResType::COLOR);
    EXPECT_TRUE(id > 0);
    state = rm->GetColorById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(4279834905, outValue); // #191919
}

/*
 * @tc.name: ResourceManagerGetColorByIdTest002
 * @tc.desc: Test GetColorById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    state = rm->GetColorById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest001
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("divider_color", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(268435456), outValue); // #10000000

    state = rm->GetColorByName("color_aboutPage_title_primary", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(4279834905, outValue); // #191919
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest002
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetProfileByIdTest001
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr, nullptr);
    tmp->Init();
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/profile/test_common.h");

    std::string outValue;
    RState state;
    int id = GetResId("test_common", ResType::PROF);
    EXPECT_TRUE(id > 0);
    state = rm->GetProfileById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetProfileByIdTest002
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetProfileById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetProfileByNameTest001
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr, nullptr);
    tmp->Init();
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/profile/test_common.h");

    std::string outValue;
    RState state;
    state = rm->GetProfileByName("test_common", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetProfileByNameTest002
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetProfileByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest001
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByIdTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr, nullptr);
    tmp->Init();
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/media/icon.png");

    std::string outValue;
    RState state;
    int id = GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest002
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByIdTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetMediaById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest001
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByNameTest001, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr, nullptr);
    tmp->Init();
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/media/icon.png");

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest002
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByNameTest002, TestSize.Level1)
{
    AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetMediaByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerResolveReferenceTest001
 * @tc.desc: Test ResolveReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerResolveReferenceTest001, TestSize.Level1)
{
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rm->UpdateResConfig(*rc);

    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    int id = GetResId("integer_1", ResType::INTEGER);
    std::string value(FormatString("$integer:%d", id));
    std::string outValue;
    RState ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("101"), outValue);

    std::string copyright("XXXXXX All rights reserved. ©2011-2019");
    id = GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(copyright, outValue);

    id = GetResId("string_ref", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(copyright, outValue);

    id = GetResId("boolean_1", ResType::BOOLEAN);
    value.assign(FormatString("$boolean:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("true"), outValue);

    id = GetResId("grey_background", ResType::COLOR);
    value.assign(FormatString("$color:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("#F5F5F5"), outValue);

    id = GetResId("aboutPage_minHeight", ResType::FLOAT);
    value.assign(FormatString("$float:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("707vp"), outValue);

    id = GetResId("base", ResType::PATTERN);
    value.assign(FormatString("$pattern:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(ERROR, ret);

    // reload
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rm->UpdateResConfig(*rc);
    delete rc;

    id = GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string("版权所有 ©2011-2019 XXXX有限公司保留一切权利"), outValue.c_str());

    id = GetResId("string_ref", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string("$aaaaa"), outValue.c_str());

    // error case
    // wrong id
    value.assign(FormatString("$boolean:%d", NON_EXIST_ID));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(ERROR, ret);
    // wrong type
    id = GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$boolean:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(ERROR, ret);
}

/*
 * @tc.name: ResourceManagerResolveParentReferenceTest001
 * @tc.desc: Test ResolveParentReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerResolveParentReferenceTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    int id;
    std::map<std::string, std::string> outValue;
    const IdItem *idItem;
    RState ret;

    id = GetResId("base", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    HILOG_DEBUG("=====");
    id = GetResId("child", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    HILOG_DEBUG("=====");
    id = GetResId("ccchild", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    // error case
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(nullptr, outValue);
    ASSERT_EQ(ERROR, ret);
    // wrong resType
    IdItem *item = new IdItem;
    for (int i = 0; i < ResType::MAX_RES_TYPE; ++i) {
        if (i == ResType::THEME || i == ResType::PATTERN) {
            continue;
        }
        item->resType_ = (ResType) i;
        ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(item, outValue);
        EXPECT_EQ(ERROR, ret);
    }
    delete item;
}

/*
 * test res with same name in different resType
 * @tc.name: ResourceManagerSameNameTest001
 * @tc.desc: Test GetStringByName & GetBooleanByName & GetIntegerByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerSameNameTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    std::string outValue;
    std::string name;
    RState state;

    state = rm->GetStringByName("same_name", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(std::string("StringSameName"), outValue);

    bool outValueB = true;
    state = rm->GetBooleanByName("same_name", outValueB);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(false, outValueB);

    int outValueI;
    state = rm->GetIntegerByName("same_name", outValueI);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(999, outValueI);
}
