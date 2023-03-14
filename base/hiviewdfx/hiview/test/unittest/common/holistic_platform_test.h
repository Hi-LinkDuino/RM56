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
#ifndef HIVIEW_HOLIETIC_PLUGIN_PLATFORM_TEST_H
#define HIVIEW_HOLIETIC_PLUGIN_PLATFORM_TEST_H
#include <gtest/gtest.h>

#include "hiview_platform.h"
class HolisticPlatformTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
    static OHOS::HiviewDFX::HiviewPlatform platform_;
    static std::set<std::string>* count1_;
    static std::set<std::string>* count2_;

    void LoadingPlugins002Check();
    void Run001Check01();
    void Run001Check02();
    void Run002Check01();
    void Run002Check02();
};
#endif // HIVIEW_PLUGIN_PLATFORM_TEST_H