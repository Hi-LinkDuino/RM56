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
#include "plugin_factory_test.h"

#include <memory>

#include "plugin_example.h"
#include "plugin_factory.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
/**
 * @tc.name: PluginFactoryRegisterTest001
 * @tc.desc: create PluginExample with pre-registed constructor method
 * @tc.type: FUNC
 * @tc.require: AR000DPTT6
 */
HWTEST_F(PluginFactoryTest, PluginFactoryRegisterTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create PluginExample by plugin factory
     */
    printf("PluginFactoryTest.\n");
    auto plugin = PluginFactory::GetGlobalPluginInfo("PluginExample");
    ASSERT_NE(plugin.get(), nullptr);
    auto examplePlugin = std::static_pointer_cast<PluginExample>(plugin->getPluginObject());
    ASSERT_EQ(examplePlugin->isConstructed_, true);
}

/**
 * @tc.name: PluginFactoryRegisterTest002
 * @tc.desc: register plugin dynamically and create instance after registration
 * @tc.type: FUNC
 * @tc.require: AR000DPTT6
 */
HWTEST_F(PluginFactoryTest, PluginFactoryRegisterTest002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create PluginExample by plugin factory
     */
    printf("PluginFactoryTest.\n");
    auto plugin = PluginFactory::GetGlobalPluginInfo("PluginExample2");
    ASSERT_EQ(plugin, nullptr);
    PluginFactory::RegisterPlugin("PluginExample2",
        std::make_shared<PluginRegistInfo>(RegisterPluginExample2::GetObject, false, false));
    plugin = PluginFactory::GetGlobalPluginInfo("PluginExample2");
    ASSERT_NE(plugin, nullptr);
    auto examplePlugin = std::static_pointer_cast<PluginExample>(plugin->getPluginObject());
    ASSERT_EQ(examplePlugin->isConstructed_, true);
}
