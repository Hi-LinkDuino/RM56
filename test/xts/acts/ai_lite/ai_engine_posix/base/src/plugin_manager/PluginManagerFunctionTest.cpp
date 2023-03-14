/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <cstring>
#include <unistd.h>

#include "plugin_manager/include/aie_plugin_info.h"
#include "plugin_manager/include/i_plugin_manager.h"
#include "utils/aie_client_const.h"
#include "utils/log/aie_log.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

namespace {
    std::string AID_DEMO_PLUGIN_SYNC = "sample_plugin_1"; // defined by ALGORITHM_ID_SAMPLE_1 in aie_plugin_info.h
    std::string AID_DEMO_PLUGIN_ASYNC = "sample_plugin_2"; // defined by ALGORITHM_ID_SAMPLE_2 in aie_plugin_info.h
    std::string AID_PLUGIN_INVALID = "invalid_plugin";
}

class PluginManagerFunctionTest : public testing::Test {};

static void TestPluginManager(std::string aid, bool isExceptedLoadPluginSuccess)
{
    // Step 1: Create an instance.
    IPluginManager *pluginManager = IPluginManager::GetPluginManager();

    // Step 2: Invoke pluginManager->GetPlugin.
    long long version = ALGORITHM_VERSION_VALID;
    std::shared_ptr<Plugin> plugin = nullptr;
    int ret = pluginManager->GetPlugin(aid, version, plugin);
    HILOGE("[Test]ret is %d.", ret);
    EXPECT_EQ(isExceptedLoadPluginSuccess, plugin != nullptr) << "pluginManager->GetPlugin test failed.";

    if (plugin != nullptr) {
        const char *name = plugin->GetPluginAlgorithm()->GetName();
        HILOGD("[Test]The plugin name [%s].", name);
        EXPECT_EQ(isExceptedLoadPluginSuccess, name != nullptr) << "Get plugin name failed.";
    }

    // Step 3: Unload the plugin.
    pluginManager->UnloadPlugin(aid, version);

    // Step 4: Destroy the plugin.
    pluginManager->Destroy();
}

static void TestPluginManagerUnloadPlugin(std::string aid)
{
    bool isPass = false;
    long long version = ALGORITHM_VERSION_VALID;
    IPluginManager *pluginManager2 = IPluginManager::GetPluginManager();
    pluginManager2->UnloadPlugin(aid, version);
    isPass = true;
    EXPECT_EQ(true, isPass) << "UnloadPlugin test failed.";
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiSupportPlugin_1100
 * @tc.name   : 011. 系统中已预置算法插件（同步），加载、卸载插件成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(PluginManagerFunctionTest, testPluginManagerFunction0101, Function | MediumTest | Level3)
{
    HILOGI("[Test]testPluginManagerFunction0101.");
    TestPluginManager(AID_DEMO_PLUGIN_SYNC, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiSupportPlugin_1200
 * @tc.name   : 012. 系统中已预置算法插件（异步），加载、卸载插件成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(PluginManagerFunctionTest, testPluginManagerFunction0102, Function | MediumTest | Level3)
{
    HILOGI("[Test]testPluginManagerFunction0102.");
    TestPluginManager(AID_DEMO_PLUGIN_ASYNC, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiSupportPlugin_1300
 * @tc.name   : 013. 系统中未预置插件，加载插件失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(PluginManagerFunctionTest, testPluginManagerFunction0103, Function | MediumTest | Level3)
{
    HILOGI("[Test]testPluginManagerFunction0103.");
    TestPluginManager(AID_PLUGIN_INVALID, false);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiSupportPlugin_2100
 * @tc.name   : 021. 系统中已预置算法插件（同步），不加载插件，直接卸载插件，不发生crash
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(PluginManagerFunctionTest, testPluginManagerFunction0201, Function | MediumTest | Level3)
{
    HILOGI("[Test]testPluginManagerFunction0201.");
    TestPluginManagerUnloadPlugin(AID_DEMO_PLUGIN_SYNC);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiSupportPlugin_2200
 * @tc.name   : 022. 系统中已预置算法插件（异步），不加载插件，直接卸载插件，不发生crash
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(PluginManagerFunctionTest, testPluginManagerFunction0202, Function | MediumTest | Level3)
{
    HILOGI("[Test]testPluginManagerFunction0202.");
    TestPluginManagerUnloadPlugin(AID_DEMO_PLUGIN_ASYNC);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiSupportPlugin_2300
 * @tc.name   : 023. 系统中未预置插件，不加载插件，直接卸载插件，不发生crash
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(PluginManagerFunctionTest, testPluginManagerFunction0203, Function | MediumTest | Level3)
{
    HILOGI("[Test]testPluginManagerFunction0203.");
    TestPluginManagerUnloadPlugin(AID_PLUGIN_INVALID);
}