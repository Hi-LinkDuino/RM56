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

#include "resource_manager_performance_test.h"

#include <chrono>
#include <climits>
#include <cstring>
#include <ctime>
#include <gtest/gtest.h>

#define private public

#include "resource_manager.h"
#include "resource_manager_impl.h"
#include "test_common.h"
#include "utils/errors.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
using namespace std;

class ResourceManagerPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerPerformanceTest() : rm(nullptr)
    {}

    ~ResourceManagerPerformanceTest()
    {}

public:
    ResourceManager *rm;

    int GetResId(std::string name, ResType resType) const;
};

int ResourceManagerPerformanceTest::GetResId(std::string name, ResType resType) const
{
    auto idv = ((ResourceManagerImpl *)rm)->hapManager_->GetResourceListByName(name.c_str(), resType);
    if (idv == nullptr) {
        return -1;
    }

    if (idv->GetLimitPathsConst().size() > 0) {
        return idv->GetLimitPathsConst()[0]->GetIdItem()->id_;
    }
    return OBJ_NOT_FOUND;
}

void ResourceManagerPerformanceTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void ResourceManagerPerformanceTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerPerformanceTest::SetUp(void)
{
    // PerformanceTest need higher log level
    g_logLevel = LOG_INFO;
    this->rm = CreateResourceManager();
    if (rm == nullptr) {
        return;
    }
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, nullptr);
    rm->UpdateResConfig(*rc);
    delete rc;
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    if (!ret) {
        HILOG_ERROR("AddResource failed. test would fail.");
    }
}

void ResourceManagerPerformanceTest::TearDown(void)
{
    if (this->rm != nullptr) {
        delete this->rm;
        this->rm = nullptr;
    }
}

/*
 * @tc.name: ResourceManagerPerformanceFuncTest001
 * @tc.desc: Test AddResource
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest001, TestSize.Level1)
{
    unsigned long long total = 0;
    double average = 0;
    for (int k = 0; k < 1000; ++k) {
        auto tmpRm = CreateResourceManager();
        if (tmpRm == nullptr) {
            EXPECT_TRUE(false);
            return;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        tmpRm->AddResource(FormatFullPath(g_resFilePath).c_str());
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        delete tmpRm;
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 001: %f us", average);
    EXPECT_LT(average, 9000);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest002
 * @tc.desc: Test UpdateResConfig
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest002, TestSize.Level1)
{
    unsigned long long total = 0;
    double average = 0;
    auto tmpRm = CreateResourceManager();
    if (tmpRm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        delete tmpRm;
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_CAR);
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        tmpRm->UpdateResConfig(*rc);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    delete tmpRm;
    delete rc;
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 002: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest003
 * @tc.desc: Test GetResConfig
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest003, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_CAR);
    ResConfigImpl rci;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetResConfig(rci);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    delete rc;
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 003: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest004
 * @tc.desc: Test GetStringByID
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest004, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    vector<uint32_t> ids;
    int count = 2;
    for (int i = 0; i < count; ++i) {
        int id = GetResId(name[i], ResType::STRING);
        ASSERT_TRUE(id > 0);
        ids.push_back(static_cast<uint32_t>(id));
    }

    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringById(ids[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 004: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest005
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest005, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    int count = 2;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringByName(name[i].c_str(), outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 005: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest006
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest006, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"string_ref", "string_ref2"};
    int count = 2;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringByName(name[i].c_str(), outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 006: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest007
 * @tc.desc: Test GetStringFormatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest007, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    vector<uint32_t> ids;
    int count = 2;
    for (int i = 0; i < count; ++i) {
        int id = GetResId(name[i], ResType::STRING);
        ASSERT_TRUE(id > 0);
        ids.push_back(static_cast<uint32_t>(id));
    }

    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringFormatById(outValue, ids[i], 12);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 007: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest008
 * @tc.desc: Test GetStringFormatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest008, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    int count = 2;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringFormatByName(outValue, name[i].c_str(), 123);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 008: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest009
 * @tc.desc: Test GetStringArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest009, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("size", ResType::STRINGARRAY);
    ASSERT_TRUE(id > 0);

    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetStringArrayById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 009: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest010
 * @tc.desc: Test GetStringArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest010, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetStringArrayByName("size", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 010: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest011
 * @tc.desc: Test GetPatternById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest011, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"base", "child"};
    vector<uint32_t> ids;
    int count = 2;
    for (int i = 0; i < count; ++i) {
        int id = GetResId(name[i], ResType::PATTERN);
        ASSERT_TRUE(id > 0);
        ids.push_back(static_cast<uint32_t>(id));
    }
    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPatternById(ids[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 011: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest012
 * @tc.desc: Test GetPatternByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest012, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"base", "child"};
    int count = 2;
    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPatternByName(name[i].c_str(), outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 012: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest013
 * @tc.desc: Test GetPluralStringById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest013, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    int id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringById(id, quantity[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 013: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest014
 * @tc.desc: Test GetPluralStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest014, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringByName("eat_apple", quantity[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 014: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest015
 * @tc.desc: Test GetPluralStringByIdFormat
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest015, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    int id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringByIdFormat(outValue, id, quantity[i], quantity[i]);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 015: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest016
 * @tc.desc: Test GetPluralStringByNameFormat
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest016, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringByNameFormat(outValue, "eat_apple", quantity[i], quantity[i]);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 016: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest017
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest017, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("app_theme", ResType::THEME);
    ASSERT_TRUE(id > 0);

    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetThemeById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 017: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest018
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest018, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetThemeByName("app_theme", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 018: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest019
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest019, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("boolean_1", ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);

    bool outValue = true;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetBooleanById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 019: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest020
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest020, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    bool outValue = true;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetBooleanByName("boolean_1", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 020: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest021
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest021, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("integer_1", ResType::INTEGER);
    ASSERT_TRUE(id > 0);

    int outValue = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntegerById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 021: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest022
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest022, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int outValue = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntegerByName("integer_ref", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 022: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest023
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest023, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("width_appBar_backButton_touchTarget", ResType::FLOAT);
    ASSERT_TRUE(id > 0);

    float outValue = 0.0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetFloatById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 023: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest024
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest024, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    float outValue = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetFloatByName("width_appBar_backButton_touchTarget", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 024: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest025
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest025, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("intarray_1", ResType::INTARRAY);
    ASSERT_TRUE(id > 0);

    std::vector<int> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntArrayById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 025: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest026
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest026, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    std::vector<int> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntArrayByName("intarray_1", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 026: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest027
 * @tc.desc: Test GetColorById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest027, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("divider_color", ResType::COLOR);
    ASSERT_TRUE(id > 0);

    uint32_t outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetColorById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 027: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest028
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest028, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    uint32_t outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetColorByName("divider_color", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 028: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest029
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest029, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("test_common", ResType::PROF);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetProfileById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 029: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest030
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest030, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetProfileByName("test_common", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 030: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest031
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest031, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 031: %f us", average);
    EXPECT_LT(average, 500);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest032
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest032, TestSize.Level1)
{
    if (rm == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    unsigned long long total = 0;
    double average = 0;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaByName("icon", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    HILOG_DEBUG("avg cost 032: %f us", average);
    EXPECT_LT(average, 500);
};
