/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/card_frontend/js_card_parser.h"
#include "frameworks/bridge/common/utils/utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t COMMAND_SIZE = 1;

} // namespace

class CardFrontendTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CardFrontendTest::SetUpTestCase() {}
void CardFrontendTest::TearDownTestCase() {}
void CardFrontendTest::SetUp() {}
void CardFrontendTest::TearDown() {}

/**
 * @tc.name: CardFrontendTest001
 * @tc.desc: Test data binding.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{variable}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"shown\": \"{{show}}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"show\": false,\n"
                                 "\t\t\"variable\": \"hello!\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    auto shown = rootTemplate->GetValue("shown");
    auto shownValue = shown->GetString();
    int32_t nodeId = 1;

    /**
     * @tc.steps: step2. Get the value of the shown attribute.
     * @tc.expected: step2. shown value is false.
     */
    jsCardParser->GetShownValue(shownValue);
    ASSERT_TRUE(shownValue == "false");

    /**
     * @tc.steps: step3. Get the value of the attribute.
     * @tc.expected: step3. Property value is correct.
     */
    std::vector<std::pair<std::string, std::string>> attrs;
    auto type = rootTemplate->GetValue("type")->GetString();
    RefPtr<Framework::JsCommandDomElementCreator> command =
        Referenced::MakeRefPtr<Framework::JsCommandAddDomElement>(type, nodeId, 0);
    auto ptr = Referenced::RawPtr(command);
    jsCardParser->ParseAttributes(rootTemplate, nodeId, attrs, (Framework::JsCommandDomElementOperator*)ptr);
    ASSERT_TRUE(attrs.size() == COMMAND_SIZE);
    ASSERT_TRUE(attrs[0].first == "value" && attrs[0].second == "hello!");
}

/**
 * @tc.name: CardFrontendTest002
 * @tc.desc: Test data binding.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"shown\": \"{{}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the shown attribute.
     * @tc.expected: step2. shown value is not true.
     */
    auto shown = rootTemplate->GetValue("shown");
    auto shownValue = shown->GetString();
    int32_t nodeId = 1;
    jsCardParser->GetShownValue(shownValue);
    ASSERT_TRUE(shownValue != "true");

    /**
     * @tc.steps: step3. Get the value of the attribute.
     * @tc.expected: step3. Property value is "{{}}".
     */
    std::vector<std::pair<std::string, std::string>> attrs;
    auto type = rootTemplate->GetValue("type")->GetString();
    RefPtr<Framework::JsCommandDomElementCreator> command =
        Referenced::MakeRefPtr<Framework::JsCommandAddDomElement>(type, nodeId, 0);
    auto ptr = Referenced::RawPtr(command);
    jsCardParser->ParseAttributes(rootTemplate, nodeId, attrs, (Framework::JsCommandDomElementOperator*)ptr);
    ASSERT_TRUE(attrs.size() == COMMAND_SIZE);
    ASSERT_TRUE(attrs[0].first == "value" && attrs[0].second == "{{}}");
}

/**
 * @tc.name: CardFrontendTest003
 * @tc.desc: Test data binding.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"style\": {\n"
                                 "\t\t\t\"color\": \"{{{variable}}\",\n"
                                 "\t\t\t\"strokeColor\": \"{{variable}}\"\n"
                                 "\t\t}\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"variable\": \"0xffffff\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the style.
     * @tc.expected: step2. The style value meets expectations.
     */
    int32_t nodeId = 1;
    std::vector<std::pair<std::string, std::string>> styles;
    auto type = rootTemplate->GetValue("type")->GetString();
    jsCardParser->ParseStyles(rootTemplate, nodeId, styles);
    ASSERT_TRUE(styles.size() == 2);
    ASSERT_TRUE(styles[0].first == "color" && styles[0].second == "{{{variable}}");
    ASSERT_TRUE(styles[1].first == "strokeColor" && styles[1].second == "0xffffff");
}

/**
 * @tc.name: CardFrontendTest004
 * @tc.desc: Test condition rendering.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"shown\": \"{{show}} && {{display}}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"display\": true,\n"
                                 "\t\t\"show\": false\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the shown.
     * @tc.expected: step2. The shown value meets expectations.
     */
    auto shown = rootTemplate->GetValue("shown");
    auto shownValue = shown->GetString();
    jsCardParser->GetShownValue(shownValue);
    ASSERT_TRUE(shownValue == "false");
}

/**
 * @tc.name: CardFrontendTest005
 * @tc.desc: Test condition rendering.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"shown\": \"{{show}} && !{{display}}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"display\": false,\n"
                                 "\t\t\"show\": false\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the shown.
     * @tc.expected: step2. The shown value meets expectations.
     */
    auto shown = rootTemplate->GetValue("shown");
    auto shownValue = shown->GetString();
    jsCardParser->GetShownValue(shownValue);
    ASSERT_TRUE(shownValue == "false");
}

/**
 * @tc.name: CardFrontendTest006
 * @tc.desc: Test condition rendering.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"shown\": \"!{{show}} && !{{display}}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"display\": false,\n"
                                 "\t\t\"show\": false\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the shown.
     * @tc.expected: step2. The shown value meets expectations.
     */
    auto shown = rootTemplate->GetValue("shown");
    auto shownValue = shown->GetString();
    jsCardParser->GetShownValue(shownValue);
    ASSERT_TRUE(shownValue == "true");
}

/**
 * @tc.name: CardFrontendTest007
 * @tc.desc: Test condition rendering.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"shown\": \"{{show}} && {{xdisplay}}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"display\": true,\n"
                                 "\t\t\"show\": true\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the shown.
     * @tc.expected: step2. The shown value meets expectations.
     */
    auto shown = rootTemplate->GetValue("shown");
    auto shownValue = shown->GetString();
    jsCardParser->GetShownValue(shownValue);
    ASSERT_TRUE(shownValue != "true");
}

/**
 * @tc.name: CardFrontendTest008
 * @tc.desc: Test "repeat" attr: for = "{{list}}".
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"repeat\": \"{{list}}\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"list\": [\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"aa\",\n"
                                 "\t\t\t\t\"idx\": 1\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"bb\",\n"
                                 "\t\t\t\t\"idx\": 2\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"cc\",\n"
                                 "\t\t\t\t\"idx\": 1\n"
                                 "\t\t\t}\n"
                                 "\t\t]\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    /**
     * @tc.steps: step2. Get the value of the repeat.
     * @tc.expected: step2. The repeat value meets expectations.
     */
    auto repeatValue = rootTemplate->GetValue("repeat");
    std::string key;
    jsCardParser->GetRepeatData(repeatValue, key);

    ASSERT_EQ(repeatValue->GetArraySize(), 3);
    auto item = repeatValue->GetChild();
    auto itemName = item->GetValue("name")->GetString();
    auto itemIndex = item->GetValue("idx")->GetInt();
    ASSERT_EQ(itemName, "aa");
    ASSERT_EQ(itemIndex, 1);
}

/**
 * @tc.name: CardFrontendTest009
 * @tc.desc: Test "repeat" attr: for = "value in list".
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{item.name}}\",\n"
                                 "\t\t\t\"index\": \"{{item.idx}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"repeat\": {\n"
                                 "\t\t\t\"exp\": \"{{list}}\",\n"
                                 "\t\t\t\"value\": \"item\"\n"
                                 "\t\t}\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"list\": [\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"aa\",\n"
                                 "\t\t\t\t\"idx\": 1\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"bb\",\n"
                                 "\t\t\t\t\"idx\": 2\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"cc\",\n"
                                 "\t\t\t\t\"idx\": 1\n"
                                 "\t\t\t}\n"
                                 "\t\t]\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    jsCardParser->SetIsRepeat(true);

    /**
     * @tc.steps: step2. Get the value of the repeat and parse data.
     */
    auto repeatValue = rootTemplate->GetValue("repeat");
    jsCardParser->ParseRepeatIndexItem(repeatValue);
    auto expValue = repeatValue->GetValue("exp");
    std::string key;
    jsCardParser->GetRepeatData(expValue, key);
    jsCardParser->SetRepeatItemValue(0, expValue, true);

    /**
     * @tc.steps: step3. Parse repeat attr and assert value of repeat.
     * @tc.expected: step3. The repeat value meets expectations.
     */
    std::vector<std::pair<std::string, std::string>> attrs;
    auto type = rootTemplate->GetValue("type")->GetString();
    RefPtr<Framework::JsCommandDomElementCreator> command =
        Referenced::MakeRefPtr<Framework::JsCommandAddDomElement>(type, 1, 0);
    auto ptr = Referenced::RawPtr(command);
    jsCardParser->ParseAttributes(rootTemplate, 1, attrs, (Framework::JsCommandDomElementOperator*)ptr);

    ASSERT_EQ(attrs.begin()->first, "value");
    ASSERT_EQ(attrs.begin()->second, "aa");

    ASSERT_EQ(attrs.rbegin()->first, "index");
    ASSERT_EQ(attrs.rbegin()->second, "1");

    jsCardParser->SetRepeatItemValue(1, expValue, true);
    attrs.clear();
    jsCardParser->ParseAttributes(rootTemplate, 1, attrs, (Framework::JsCommandDomElementOperator*)ptr);
    ASSERT_EQ(attrs.begin()->first, "value");
    ASSERT_EQ(attrs.begin()->second, "bb");

    ASSERT_EQ(attrs.rbegin()->first, "index");
    ASSERT_EQ(attrs.rbegin()->second, "2");
}

/**
 * @tc.name: CardFrontendTest010
 * @tc.desc: Test "repeat" attr: for = "(index, item) in list".
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct json string.
     */
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{$item.name}}\",\n"
                                 "\t\t\t\"index\": \"{{$idx}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\",\n"
                                 "\t\t\"repeat\": {\n"
                                 "\t\t\t\"exp\": \"{{list}}\",\n"
                                 "\t\t\t\"key\": \"index\",\n"
                                 "\t\t\t\"value\": \"item\"\n"
                                 "\t\t}\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"list\": [\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"aa\",\n"
                                 "\t\t\t\t\"idx\": 1\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"bb\",\n"
                                 "\t\t\t\t\"idx\": 2\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"name\": \"cc\",\n"
                                 "\t\t\t\t\"idx\": 3\n"
                                 "\t\t\t}\n"
                                 "\t\t]\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    jsCardParser->SetIsRepeat(true);

    /**
     * @tc.steps: step2. Get the value of the repeat and parse data.
     */
    auto repeatValue = rootTemplate->GetValue("repeat");
    jsCardParser->ParseRepeatIndexItem(repeatValue);
    auto expValue = repeatValue->GetValue("exp");
    std::string key;
    jsCardParser->GetRepeatData(expValue, key);
    jsCardParser->SetRepeatItemValue(2, expValue, true);

    /**
     * @tc.steps: step3. Parse repeat attr and assert value of repeat.
     * @tc.expected: step3. The repeat value meets expectations.
     */
    std::vector<std::pair<std::string, std::string>> attrs;
    auto type = rootTemplate->GetValue("type")->GetString();
    RefPtr<Framework::JsCommandDomElementCreator> command =
        Referenced::MakeRefPtr<Framework::JsCommandAddDomElement>(type, 1, 0);
    auto ptr = Referenced::RawPtr(command);
    jsCardParser->ParseAttributes(rootTemplate, 1, attrs, (Framework::JsCommandDomElementOperator*)ptr);

    ASSERT_EQ(attrs.begin()->first, "value");
    ASSERT_EQ(attrs.begin()->second, "cc");

    ASSERT_EQ(attrs.rbegin()->first, "index");
    ASSERT_EQ(attrs.rbegin()->second, "2");

    jsCardParser->SetRepeatItemValue(1, expValue, true);
    attrs.clear();
    jsCardParser->ParseAttributes(rootTemplate, 1, attrs, (Framework::JsCommandDomElementOperator*)ptr);
    ASSERT_EQ(attrs.begin()->first, "value");
    ASSERT_EQ(attrs.begin()->second, "bb");

    ASSERT_EQ(attrs.rbegin()->first, "index");
    ASSERT_EQ(attrs.rbegin()->second, "1");
}

/**
 * @tc.name: CardFrontendTest011
 * @tc.desc: Test point operation.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest011, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"number\": {\n"
                                 "\t\t\t\"key\": 1\n"
                                 "\t\t}\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    std::string value = "{{number.key}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "1");
}

/**
 * @tc.name: CardFrontendTest012
 * @tc.desc: Test array operations.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest012, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"index\": [\n"
                                 "\t\t\t0,\n"
                                 "\t\t\t3,\n"
                                 "\t\t\t4\n"
                                 "\t\t]\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    std::string value = "{{index[0]}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "0");
    value = "{{index[1]}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "3");
    value = "{{index[2]}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "4");
}

/**
 * @tc.name: CardFrontendTest013
 * @tc.desc: Test complex expressions.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest013, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"hours\": [\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"time\": [\n"
                                 "\t\t\t\t\t8,\n"
                                 "\t\t\t\t\t2,\n"
                                 "\t\t\t\t\t3\n"
                                 "\t\t\t\t],\n"
                                 "\t\t\t\t\"src\": \"src/XXX.png\",\n"
                                 "\t\t\t\t\"prop\": 10,\n"
                                 "\t\t\t\t\"isPast\": false,\n"
                                 "\t\t\t\t\"temperature\": \"22°\",\n"
                                 "\t\t\t\t\"description\": \"\"\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"time\": \"3:00\",\n"
                                 "\t\t\t\t\"src\": \"src/XXX.png\",\n"
                                 "\t\t\t\t\"prop\": 11,\n"
                                 "\t\t\t\t\"isPast\": false,\n"
                                 "\t\t\t\t\"temperature\": \"22°\",\n"
                                 "\t\t\t\t\"description\": \"\"\n"
                                 "\t\t\t}\n"
                                 "\t\t],\n"
                                 "\t\t\"number\": {\n"
                                 "\t\t\t\"key\": 1\n"
                                 "\t\t}\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    std::string value = "{{hours[0].time[0]}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "8");
    value = "{{hours[number.key].time}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "3:00");
    value = "{{hours[0].time}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "[8,2,3]");
}

/**
 * @tc.name: CardFrontendTest014
 * @tc.desc: Test error expression.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendTest014, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"hours\": [\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"time\": [\n"
                                 "\t\t\t\t\t8,\n"
                                 "\t\t\t\t\t2,\n"
                                 "\t\t\t\t\t3\n"
                                 "\t\t\t\t],\n"
                                 "\t\t\t\t\"src\": \"src/XXX.png\",\n"
                                 "\t\t\t\t\"prop\": 10,\n"
                                 "\t\t\t\t\"isPast\": false,\n"
                                 "\t\t\t\t\"temperature\": \"22°\",\n"
                                 "\t\t\t\t\"description\": \"\"\n"
                                 "\t\t\t},\n"
                                 "\t\t\t{\n"
                                 "\t\t\t\t\"time\": \"3:00\",\n"
                                 "\t\t\t\t\"src\": \"src/XXX.png\",\n"
                                 "\t\t\t\t\"prop\": 11,\n"
                                 "\t\t\t\t\"isPast\": false,\n"
                                 "\t\t\t\t\"temperature\": \"22°\",\n"
                                 "\t\t\t\t\"description\": \"\"\n"
                                 "\t\t\t}\n"
                                 "\t\t],\n"
                                 "\t\t\"number\": {\n"
                                 "\t\t\t\"key\": 1\n"
                                 "\t\t}\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();
    std::string value = "{{hours[key].time}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{hours[key].time}}");
    value = "{{key.number}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{key.number}}");
    value = "{{number.key}}}";
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{number.key}}}");
}

/**
 * @tc.name: CardFrontendDataBindingTest001
 * @tc.desc: Test data binding for {{key1}}.{{key2}
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest001, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f({{aaa}}.{{ddd}})\",\n"
                                 "\t\t\t\"index\": \"$f({{bbb}}.{{ccc}})\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // {{aaa}}.{{ddd}} == abc.123
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "abc.123");

    // {{bbb}}.{{ccc}} = true.false
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "true.false");
}

/**
 * @tc.name: CardFrontendDataBindingTest002
 * @tc.desc: Test data binding for {{key1}}.{{key2}.{{key3}}
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest002, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f({{aaa}}.{{bbb}}.{{ccc}})\",\n"
                                 "\t\t\t\"index\": \"$f({{aaa}}.{{bbb}}.{{ccc}}.{{ddd}})\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // {{aaa}}.{{bbb}}.{{ccc}} == abc.true.false
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "abc.true.false");

    // {{aaa}}.{{bbb}}.{{ccc}}.{{ddd}} = abc.true.false.123
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "abc.true.false.123");
}

/**
 * @tc.name: CardFrontendDataBindingTest003
 * @tc.desc: Test data binding for {{flag1 ? key1 : key2}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest003, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{bbb ? aaa : bbb}}\",\n"
                                 "\t\t\t\"index\": \"{{ccc ? aaa : ddd}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // bbb = true, value = {{aaa}}
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "abc");

    // ccc = false, value = {{ddd}}
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "123");
}

/**
 * @tc.name: CardFrontendDataBindingTest004
 * @tc.desc: Test data binding for illegal {{flag1 ? key1 : key2}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest004, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{aaa ? bbb : ccc}}\",\n"
                                 "\t\t\t\"index\": \"{{ccc ? aaa : eee}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // aaa if not a bool value, set aaa = false, and value = {{ccc}}
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "false");

    // ccc = false, value = {{eee}}, eee does not belong to the data domain.
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "eee");
}

/**
 * @tc.name: CardFrontendDataBindingTest005
 * @tc.desc: Test data binding for {{flag1 && flag2}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest005, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{bbb && ccc}}\",\n"
                                 "\t\t\t\"index\": \"{{bbb && bbb}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // bbb = true, ccc = false, value = (bbb && ccc) = false.
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "false");

    // bbb = true, value = (bbb && bbb) = true.
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "true");
}

/**
 * @tc.name: CardFrontendDataBindingTest006
 * @tc.desc: Test data binding for {{flag1 || flag2}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest006, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{bbb || ccc}}\",\n"
                                 "\t\t\t\"index\": \"{{ccc || ccc}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // bbb = true, ccc = false, value = (bbb || ccc) = true.
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "true");

    // ccc = false, value = (ccc || ccc) = false.
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "false");
}

/**
 * @tc.name: CardFrontendDataBindingTest007
 * @tc.desc: Test data binding for {{!flag1}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest007, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{!bbb}}\",\n"
                                 "\t\t\t\"index\": \"{{!ccc}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // bbb = true, !bbb =  false.
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "false");

    // ccc = false, !ccc = true.
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "true");
}

/**
 * @tc.name: CardFrontendDataBindingTest008
 * @tc.desc: Test data binding for {{flag1 && flag2 && flag3}} (not support now).
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest008, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{bbb && ccc && aaa}}\",\n"
                                 "\t\t\t\"index\": \"{{bbb && bbb && ccc}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // When more than two variables want to &&, return itself.
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{bbb && ccc && aaa}}");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{bbb && bbb && ccc}}");
}

/**
 * @tc.name: CardFrontendDataBindingTest009
 * @tc.desc: Test data binding for {{flag1 || flag2 || flag3}} (not support now).
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest009, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{bbb || ccc || aaa}}\",\n"
                                 "\t\t\t\"index\": \"{{bbb || bbb || ccc}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // When more than two variables want to ||, return itself.
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{bbb || ccc || aaa}}");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{bbb || bbb || ccc}}");
}

/**
 * @tc.name: CardFrontendDataBindingTest010
 * @tc.desc: Test data binding for {{flag1 && flag2 || flag3}} (not support now).
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest010, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{bbb && ccc || aaa}}\",\n"
                                 "\t\t\t\"index\": \"{{bbb || bbb && ccc}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // When more than two variables want to || or &&, return itself.
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{bbb && ccc || aaa}}");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{bbb || bbb && ccc}}");
}

/**
 * @tc.name: CardFrontendDataBindingTest011
 * @tc.desc: Test data binding for {{!flag1 && flag2}} (not support now).
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest011, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"{{!bbb && ccc}}\",\n"
                                 "\t\t\t\"index\": \"{{!ccc || bbb}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{!bbb && ccc}}");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "{{!ccc || bbb}}");
}

/**
 * @tc.name: CardFrontendDataBindingTest012
 * @tc.desc: Test data binding for {{!flag1}}.{{!flag2}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest012, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f({{!bbb}}.{{!ccc}})\",\n"
                                 "\t\t\t\"index\": \"$f({{!aaa}}.{{!ccc}})\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // !bbb = false, !ccc = true.
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "false.true");

    // aaa is not bool value, so aaa = false, !aaa = true.
    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "true.true");
}

/**
 * @tc.name: CardFrontendDataBindingTest013
 * @tc.desc: Test data binding for {{key1}}.{{!flag1}}.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest013, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f({{bbb}}.{{!ccc}})\",\n"
                                 "\t\t\t\"index\": \"$f({{aaa}}.{{!ccc}})\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "true.true");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "abc.true");
}

/**
 * @tc.name: CardFrontendDataBindingTest014
 * @tc.desc: Test data binding for constant + variable.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest014, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f(value1 is : {{aaa}})\",\n"
                                 "\t\t\t\"index\": \"$f(ccc is: {{ccc}}, bbb is {{bbb}}.)\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "value1 is : abc");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "ccc is: false, bbb is true.");
}

/**
 * @tc.name: CardFrontendDataBindingTest015
 * @tc.desc: Test data binding for {{!flag1}}.{{flag2 && flag3}}
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest015, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f({{!bbb}}.{{bbb && ccc}})\",\n"
                                 "\t\t\t\"index\": \"$f({{aaa}}.{{bbb || ccc}})\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "false.false");

    value = attrValue->GetValue("index")->GetString();
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "abc.true");
}

/**
 * @tc.name: CardFrontendDataBindingTest016
 * @tc.desc: Test data binding for error expression.
 * @tc.type: FUNC
 */
HWTEST_F(CardFrontendTest, CardFrontendDataBindingTest016, TestSize.Level1)
{
    const std::string rootJson = "{\n"
                                 "\t\"template\": {\n"
                                 "\t\t\"attr\": {\n"
                                 "\t\t\t\"value\": \"$f({{aaa}}.{{eee}})\",\n"
                                 "\t\t\t\"value1\": \"{{aaa ? bbb : ddd}}\",\n"
                                 "\t\t\t\"value2\": \"{{aaa && ddd}}\",\n"
                                 "\t\t\t\"value3\": \"{{aaa || ddd}}\",\n"
                                 "\t\t\t\"value4\": \"{{!ddd}}\"\n"
                                 "\t\t},\n"
                                 "\t\t\"type\": \"text\"\n"
                                 "\t},\n"
                                 "\t\"styles\": {},\n"
                                 "\t\"actions\": {},\n"
                                 "\t\"data\": {\n"
                                 "\t\t\"aaa\": \"abc\",\n"
                                 "\t\t\"bbb\": \"true\",\n"
                                 "\t\t\"ccc\": \"false\",\n"
                                 "\t\t\"ddd\": \"123\"\n"
                                 "\t}\n"
                                 "}";
    auto rootBody = JsonUtil::ParseJsonString(rootJson);
    auto rootTemplate = rootBody->GetValue("template");
    auto jsCardParser = AceType::MakeRefPtr<JsCardParser>(nullptr, nullptr, std::move(rootBody));
    jsCardParser->Initialize();

    auto attrValue = rootTemplate->GetValue("attr");
    auto value = attrValue->GetValue("value")->GetString();

    // {{aaa}}.{{eee}}, eee does not belong to the data field.
    jsCardParser->ParseMultiVariable(value);
    ASSERT_EQ(value, "abc.{{eee}}");

    // aaa ? bbb : ddd, aaa is not bool variable, the default setting is false.
    value = attrValue->GetValue("value1")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "123");

    // aaa && ddd, aaa and ddd are not bool variables, the default setting is false.
    value = attrValue->GetValue("value2")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "false");

    // aaa || ddd, aaa and ddd are not bool variables, the default setting is false.
    value = attrValue->GetValue("value3")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "false");

    // ddd is not a bool variable, the default setting is false.
    value = attrValue->GetValue("value4")->GetString();
    jsCardParser->ParseVariable(value);
    ASSERT_EQ(value, "true");
}

} // namespace OHOS::Ace::Framework