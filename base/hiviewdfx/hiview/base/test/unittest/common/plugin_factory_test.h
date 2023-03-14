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
#ifndef HIVIEW_PLATFORM_PLUGIN_FACTORY_TEST_H
#define HIVIEW_PLATFORM_PLUGIN_FACTORY_TEST_H
#include <gtest/gtest.h>

#include "plugin.h"
#include "plugin_example.h"

class PluginFactoryTest : public testing::Test {
public:
    void SetUp() {};
    void TearDown() {};
};

class RegisterPluginExample2 {
public:
    static std::shared_ptr<OHOS::HiviewDFX::Plugin> GetObject()
    {
        return std::make_shared<OHOS::HiviewDFX::PluginExample>();
    }
};
#endif // HIVIEW_PLATFORM_PLUGIN_FACTORY_TEST_H