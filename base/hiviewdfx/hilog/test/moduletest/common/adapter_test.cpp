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

#include <array>
#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include <properties.h>
#include <hilog/log.h>

using namespace testing::ext;
using namespace std::chrono_literals;

namespace {
class PropertiesTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};


HWTEST_F(PropertiesTest, PropertiesTest1, TestSize.Level1)
{
    std::string key = "my_property";
    std::string value = "1234";
    PropertySet(key, value.data());

    std::array<char, HILOG_PROP_VALUE_MAX - 1> prop = {0};
    PropertyGet(key, prop.data(), prop.size());

    EXPECT_EQ(std::string(prop.data()), value);
}

HWTEST_F(PropertiesTest, PropertiesTest2, TestSize.Level1)
{
    std::string key = "my_property";
    for (int i = 0; i < 10; ++i) {
        std::string value = std::to_string(i);
        PropertySet(key, value.data());

        std::array<char, HILOG_PROP_VALUE_MAX - 1> prop = {0};
        PropertyGet(key, prop.data(), prop.size());

        EXPECT_EQ(std::string(prop.data()), value);
    }
}

HWTEST_F(PropertiesTest, PropertiesInvalidParamsTest, TestSize.Level1)
{
    std::string key = "my_property";
    std::string value = "5678";
    PropertySet(key, value.data());

    PropertyGet(key, nullptr, 0);
    PropertySet(key, nullptr);

    std::array<char, HILOG_PROP_VALUE_MAX - 1> prop = {0};
    PropertyGet(key, prop.data(), prop.size());

    EXPECT_EQ(std::string(prop.data()), value);
}

HWTEST_F(PropertiesTest, PropertiesTooLongBufferTest, TestSize.Level1)
{
    std::string key = "my_property";
    std::string value = "5678";
    PropertySet(key, value.data());

    std::array<char, HILOG_PROP_VALUE_MAX> prop1 = {0};
    PropertyGet(key, prop1.data(), prop1.size());

    std::string tooLongText = "0123456789";
    while (tooLongText.length() < HILOG_PROP_VALUE_MAX) {
        tooLongText += tooLongText;
    }
    PropertySet(key, tooLongText.data());

    std::array<char, HILOG_PROP_VALUE_MAX-1> prop = {0};
    PropertyGet(key, prop.data(), prop.size());
    std::string currentValue = prop.data();
    EXPECT_EQ(currentValue, value);
    EXPECT_NE(tooLongText, value);
}

HWTEST_F(PropertiesTest, PropertiesNoKeyTest, TestSize.Level1)
{
    std::string key = "PropertiesNoKeyTest";

    std::array<char, HILOG_PROP_VALUE_MAX - 1> prop = {0};
    PropertyGet(key, prop.data(), prop.size());

    EXPECT_TRUE(std::string(prop.data()).empty());
}

HWTEST_F(PropertiesTest, SwitchTest, TestSize.Level1)
{
    PropertySet(GetPropertyName(PROP_PRIVATE), "true");
    PropertySet(GetPropertyName(PROP_PROCESS_FLOWCTRL), "true");
    PropertySet(GetPropertyName(PROP_DOMAIN_FLOWCTRL), "true");
    PropertySet(GetPropertyName(PROP_SINGLE_DEBUG), "true");
    PropertySet(GetPropertyName(PROP_KMSG), "true");
    PropertySet(GetPropertyName(PROP_PERSIST_DEBUG), "true");

    EXPECT_TRUE(IsDebugOn());
    EXPECT_TRUE(IsSingleDebugOn());
    EXPECT_TRUE(IsPersistDebugOn());
    EXPECT_TRUE(IsPrivateSwitchOn());
    EXPECT_TRUE(IsProcessSwitchOn());
    EXPECT_TRUE(IsDomainSwitchOn());
    EXPECT_TRUE(IsKmsgSwitchOn());

    PropertySet(GetPropertyName(PROP_PRIVATE), "false");
    PropertySet(GetPropertyName(PROP_PROCESS_FLOWCTRL), "false");
    PropertySet(GetPropertyName(PROP_DOMAIN_FLOWCTRL), "false");
    PropertySet(GetPropertyName(PROP_SINGLE_DEBUG), "false");
    PropertySet(GetPropertyName(PROP_KMSG), "false");
    PropertySet(GetPropertyName(PROP_PERSIST_DEBUG), "false");

    EXPECT_FALSE(IsDebugOn());
    EXPECT_FALSE(IsSingleDebugOn());
    EXPECT_FALSE(IsPersistDebugOn());
    EXPECT_FALSE(IsPrivateSwitchOn());
    EXPECT_FALSE(IsProcessSwitchOn());
    EXPECT_FALSE(IsDomainSwitchOn());
    EXPECT_FALSE(IsKmsgSwitchOn());

    PropertySet(GetPropertyName(PROP_SINGLE_DEBUG), "true");
    PropertySet(GetPropertyName(PROP_PERSIST_DEBUG), "false");
    EXPECT_TRUE(IsDebugOn());

    PropertySet(GetPropertyName(PROP_SINGLE_DEBUG), "false");
    PropertySet(GetPropertyName(PROP_PERSIST_DEBUG), "true");
    EXPECT_TRUE(IsDebugOn());
}

HWTEST_F(PropertiesTest, LevelTest, TestSize.Level1)
{
    static const std::array<const char*, 10> charLevels = {"d", "D", "f", "F", "e", "E", "w", "W", "i", "I"};
    static const std::array<uint16_t, 10> expected = {
        LOG_DEBUG, LOG_DEBUG,
        LOG_FATAL, LOG_FATAL,
        LOG_ERROR, LOG_ERROR,
        LOG_WARN, LOG_WARN,
        LOG_INFO, LOG_INFO,
    };

    for (size_t i = 0; i < charLevels.size(); ++i) {
        PropertySet(GetPropertyName(PROP_GLOBAL_LOG_LEVEL), charLevels[i]);
        EXPECT_EQ(GetGlobalLevel(), expected[i]);
    }
    PropertySet(GetPropertyName(PROP_GLOBAL_LOG_LEVEL), "z");
    EXPECT_EQ(GetGlobalLevel(), LOG_LEVEL_MIN);

    uint32_t domain = 12345;
    for (size_t i = 0; i < charLevels.size(); ++i) {
        PropertySet(GetPropertyName(PROP_DOMAIN_LOG_LEVEL) + std::to_string(domain), charLevels[i]);
        EXPECT_EQ(GetDomainLevel(domain), expected[i]);
    }
    PropertySet(GetPropertyName(PROP_DOMAIN_LOG_LEVEL) + std::to_string(domain), "z");
    EXPECT_EQ(GetDomainLevel(domain), LOG_LEVEL_MIN);

    std::string tag = "test_tag";
    for (size_t i = 0; i < charLevels.size(); ++i) {
        PropertySet(GetPropertyName(PROP_TAG_LOG_LEVEL) + tag, charLevels[i]);
        EXPECT_EQ(GetTagLevel(tag), expected[i]);
    }
    PropertySet(GetPropertyName(PROP_TAG_LOG_LEVEL) + tag, "z");
    EXPECT_EQ(GetTagLevel(tag), LOG_LEVEL_MIN);
}
} // namespace
