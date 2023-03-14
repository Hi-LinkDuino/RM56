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

#include <dlfcn.h>
#include <hwext/gtest-ext.h>
#include <hwext/gtest-tag.h>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "logging.h"
#include "memory_plugin_config.pb.h"
#include "memory_plugin_result.pb.h"
#include "plugin_module_api.h"

using namespace testing::ext;

#if defined(__i386__) || defined(__x86_64__)
const std::string LIB_PATH = const_cast<char*>("./hos/out/hos-arm/clang_x64/devtools/devtools/libmemdataplugin.z.so");
#else
const std::string LIB_PATH = const_cast<char*>("/system/lib/libmemdataplugin.z.so");
#endif

namespace {
enum NumType {
    BIT_WIDTH = 35,
    MS_S = 1000000,
};

class PluginModuleApiTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}
    void TearDown() override {}

    bool MatchTail(const std::string& name, const char* str)
    {
        int index = name.size() - strlen(str);
        if (index < 0) {
            return false;
        }
        return strncmp(name.c_str() + index, str, strlen(str)) == 0;
    }

    bool MemoryPluginTest(MemoryConfig& protoConfig, const std::string libPath)
    {
        MemoryData memoryData;
        PluginModuleStruct* memplugin;
        void* handle;
        int cnt = 1;
        uint8_t* dataBuffer;
        clock_t clockstart, clockend;
        struct timeval start, end;
        int timeuse;

        if (!MatchTail(libPath, ".so")) {
            printf("libPath=%s\r\n", libPath.c_str());
            return false;
        }

        handle = dlopen(libPath.c_str(), RTLD_LAZY);
        if (handle == nullptr) {
            HILOG_DEBUG(LOG_CORE, "test：dlopen err:%s.", dlerror());
            return false;
        }

        memplugin = (PluginModuleStruct*)dlsym(handle, "g_pluginModule");
        dataBuffer = (uint8_t*)malloc(memplugin->resultBufferSizeHint);

        int configlength = protoConfig.ByteSizeLong();
        std::vector<uint8_t> config(configlength);
        protoConfig.SerializeToArray(config.data(), config.size());

        if (memplugin->callbacks->onPluginSessionStart(config.data(), config.size()) < 0) {
            HILOG_DEBUG(LOG_CORE, "start failed");
            dlclose(handle);
            free(dataBuffer);
            return false;
        }

        clockstart = clock();
        gettimeofday(&start, nullptr);

        while (cnt--) {
            int len = memplugin->callbacks->onPluginReportResult(dataBuffer, memplugin->resultBufferSizeHint);
            if (len > 0) {
                memoryData.ParseFromArray(dataBuffer, len);
            }
        }

        gettimeofday(&end, nullptr);
        clockend = clock();
        timeuse = MS_S * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
        HILOG_DEBUG(LOG_CORE, "clock time=%.3fs, timeofday=%.3fs", (double)(clockend - clockstart) / CLOCKS_PER_SEC,
              (double)timeuse / MS_S);

        memplugin->callbacks->onPluginSessionStop();

        memplugin->callbacks->onRegisterWriterStruct(nullptr);

        dlclose(handle);
        free(dataBuffer);
        return true;
    }
};

HWTEST_F(PluginModuleApiTest, ProtoConfigNullAndInvalidSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    EXPECT_FALSE(PluginModuleApiTest::MemoryPluginTest(protoConfig, "1111"));
}

HWTEST_F(PluginModuleApiTest, ProtoConfigNullAndEffectiveSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    EXPECT_TRUE(PluginModuleApiTest::MemoryPluginTest(protoConfig, LIB_PATH));
}

HWTEST_F(PluginModuleApiTest, ProtoConfigMemAndInvalidSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    protoConfig.set_report_process_mem_info(true);
    EXPECT_FALSE(PluginModuleApiTest::MemoryPluginTest(protoConfig, "1111"));
}

HWTEST_F(PluginModuleApiTest, ProtoConfigMemAndEffectiveSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    protoConfig.set_report_process_mem_info(true);
    EXPECT_TRUE(PluginModuleApiTest::MemoryPluginTest(protoConfig, LIB_PATH));
}

HWTEST_F(PluginModuleApiTest, ProtoConfigPidAndInvalidSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    protoConfig.set_report_app_mem_info(true);
    protoConfig.add_pid(1);
    EXPECT_FALSE(PluginModuleApiTest::MemoryPluginTest(protoConfig, "1111"));
}

HWTEST_F(PluginModuleApiTest, ProtoConfigPidAndEffectiveSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    protoConfig.set_report_app_mem_info(true);
    protoConfig.add_pid(1);
    EXPECT_TRUE(PluginModuleApiTest::MemoryPluginTest(protoConfig, LIB_PATH));
}

HWTEST_F(PluginModuleApiTest, MemoryPluginTreeAndInvalidSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    protoConfig.set_report_process_tree(true);
    EXPECT_FALSE(PluginModuleApiTest::MemoryPluginTest(protoConfig, "1111"));
}

HWTEST_F(PluginModuleApiTest, MemoryPluginTreeAndEffectiveSo, TestSize.Level1)
{
    MemoryConfig protoConfig;
    protoConfig.set_report_process_tree(true);
    EXPECT_TRUE(PluginModuleApiTest::MemoryPluginTest(protoConfig, LIB_PATH));
}

HWTEST_F(PluginModuleApiTest, ApiCallInvalidSoAndInvalidStruct, TestSize.Level1)
{
    PluginModuleStruct memplugin;
    ASSERT_EQ(memplugin.callbacks->onPluginSessionStart(nullptr, 0), 0);
    ASSERT_EQ(memplugin.callbacks->onPluginReportResult(nullptr, 0), 0);
    ASSERT_EQ(memplugin.callbacks->onPluginSessionStop(), 0);
    ASSERT_EQ(memplugin.callbacks->onRegisterWriterStruct(nullptr), 0);
}

HWTEST_F(PluginModuleApiTest, ApiCallEffectiveSoAndInvalidStruct, TestSize.Level1)
{
    PluginModuleStruct* memplugin;
    void* handle = dlopen(LIB_PATH.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        HILOG_DEBUG(LOG_CORE, "test：dlopen err:%s.", dlerror());
    }
    memplugin = (PluginModuleStruct*)dlsym(handle, "g_pluginModule");
    ASSERT_EQ(memplugin->callbacks->onPluginSessionStart(nullptr, 0), 0);
    ASSERT_EQ(memplugin->callbacks->onPluginReportResult(nullptr, 0), 0);
    ASSERT_EQ(memplugin->callbacks->onPluginSessionStop(), 0);
    ASSERT_EQ(memplugin->callbacks->onRegisterWriterStruct(nullptr), 0);
}

HWTEST_F(PluginModuleApiTest, ApiCallInvalidSoAndEffectiveStruct, TestSize.Level1)
{
    PluginModuleStruct memplugin;
    WriterStruct writer;

    ASSERT_EQ(memplugin.callbacks->onPluginSessionStart(nullptr, 0), 0);
    ASSERT_EQ(memplugin.callbacks->onPluginSessionStop(), 0);
    ASSERT_EQ(memplugin.callbacks->onRegisterWriterStruct(&writer), 0);
}

HWTEST_F(PluginModuleApiTest, ApiCallEffectiveSoAndEffectiveStruct, TestSize.Level1)
{
    PluginModuleStruct* memplugin;
    WriterStruct writer;

    void* handle = dlopen(LIB_PATH.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        HILOG_DEBUG(LOG_CORE, "test：dlopen err:%s.", dlerror());
    }
    memplugin = (PluginModuleStruct*)dlsym(handle, "g_pluginModule");
    ASSERT_EQ(memplugin->callbacks->onPluginSessionStart(nullptr, 0), 0);
    ASSERT_EQ(memplugin->callbacks->onPluginSessionStop(), 0);
    ASSERT_EQ(memplugin->callbacks->onRegisterWriterStruct(&writer), 0);
}
} // namespace

