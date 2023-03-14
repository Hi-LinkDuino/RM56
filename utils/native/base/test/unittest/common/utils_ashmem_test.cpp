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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <gtest/gtest.h>
#include "directory_ex.h"
#include "securec.h"
#include "hilog/log.h"
#include "parcel.h"
#include "refbase.h"
#include "ashmem.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;

const int MAX_PARCEL_SIZE = 1000;
char g_data[MAX_PARCEL_SIZE];
const int32_t MEMORY_SIZE = 1024;
const std::string MEMORY_CONTENT = "HelloWorld2020\0";
const std::string MEMORY_NAME = "Test SharedMemory\0";
static constexpr HiviewDFX::HiLogLabel label = { LOG_CORE, 0xD003D00, "UtilsAshmemTest" };
#define UTILS_LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(label, __VA_ARGS__)
#define UTILS_LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(label, __VA_ARGS__)
#define UTILS_LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(label, __VA_ARGS__)
#define UTILS_LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(label, __VA_ARGS__)
#define UTILS_LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(label, __VA_ARGS__)

class UtilsAshmemTest : public testing::Test {
public:
    static void TearDownTestCase(void);
};

void UtilsAshmemTest::TearDownTestCase(void)
{
    for (int i = 0; i < MAX_PARCEL_SIZE; i++) {
        g_data[i] = 0;
    }
}

/**
 * @tc.name: test_ashmem_WriteAndRead_001
 * @tc.desc: create and map ashmem
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_001, TestSize.Level0)
{
    UTILS_LOGI("test_ashmem_WriteAndRead_001");

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);
    ASSERT_TRUE(ashmem->GetAshmemSize() == MEMORY_SIZE);

    bool ret = ashmem->MapAshmem(PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ret);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_WriteAndRead_002
 * @tc.desc: write to and read from ashmem
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_002, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), sizeof(MEMORY_CONTENT));
    ASSERT_TRUE(ret);

    auto readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(readData != nullptr);

    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), sizeof(MEMORY_CONTENT));
    ASSERT_TRUE(readData != nullptr);

    readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_WriteAndRead_003
 * @tc.desc: test read-only ashmem
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_003, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(ret);

    ashmem->UnmapAshmem();

    ret = ashmem->MapReadOnlyAshmem();
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), sizeof(MEMORY_CONTENT));
    ASSERT_FALSE(ret);

    auto readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(readData != nullptr);

    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_WriteAndRead_004
 * @tc.desc: set read-only protection and map again
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_004, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(ret);

    ashmem->UnmapAshmem();

    ret = ashmem->SetProtection(PROT_READ);
    ASSERT_TRUE(ret);

    ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_FALSE(ret);

    ret = ashmem->MapReadOnlyAshmem();
    ASSERT_TRUE(ret);

    auto readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(readData != nullptr);

    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_WriteAndRead_005
 * @tc.desc: set read-only protection without mapping again
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_005, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(ret);

    ret = ashmem->SetProtection(PROT_READ);
    ASSERT_TRUE(ret);

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_FALSE(ret);

    auto readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(readData != nullptr);

    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_InvalidOperation_001
 * @tc.desc: create invalid-size ashmem or set invalid protection type
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_001, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), -1);
    ASSERT_TRUE(ashmem == nullptr);

    ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->SetProtection(-1);
    ASSERT_FALSE(ret);

    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_InvalidOperation_002
 * @tc.desc: map after closing ashmem
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_002, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    ashmem->CloseAshmem();

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: test_ashmem_InvalidOperation_003
 * @tc.desc: write or read after closing ashmem
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_003, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_TRUE(ret);

    ashmem->CloseAshmem();

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_FALSE(ret);

    auto readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(readData == nullptr);
}

/**
 * @tc.name: test_ashmem_InvalidOperation_004
 * @tc.desc: write or read after unmapping ashmem
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_004, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->MapReadAndWriteAshmem();
    ASSERT_TRUE(ret);

    ashmem->UnmapAshmem();

    ret = ashmem->WriteToAshmem(MEMORY_CONTENT.c_str(), sizeof(MEMORY_CONTENT), 0);
    ASSERT_FALSE(ret);

    auto readData = ashmem->ReadFromAshmem(sizeof(MEMORY_CONTENT), 0);
    ASSERT_TRUE(readData == nullptr);

    ashmem->CloseAshmem();
}

/**
 * @tc.name: test_ashmem_InvalidOperation_005
 * @tc.desc: expand protection type
 * @tc.type: FUNC
 */
HWTEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_005, TestSize.Level0)
{
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(ashmem != nullptr);

    bool ret = ashmem->SetProtection(PROT_WRITE);
    ASSERT_TRUE(ret);

    ret = ashmem->SetProtection(PROT_READ);
    ASSERT_FALSE(ret);

    ret = ashmem->SetProtection(PROT_READ | PROT_WRITE);
    ASSERT_FALSE(ret);

    ret = ashmem->SetProtection(PROT_NONE);
    ASSERT_TRUE(ret);

    ret = ashmem->SetProtection(PROT_READ);
    ASSERT_FALSE(ret);

    ashmem->CloseAshmem();
}
