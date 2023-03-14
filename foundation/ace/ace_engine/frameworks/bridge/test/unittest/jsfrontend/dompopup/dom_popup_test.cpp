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

#include "core/components/common/layout/constants.h"
#include "core/components/popup/popup_component.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_popup.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const Placement PLACEMENT_VALUE = Placement::LEFT;
const std::string MASK_COLOR_DEFAULT = "#0x0000ff";

} // namespace

class DomPopupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomPopupTest::SetUpTestCase() {}
void DomPopupTest::TearDownTestCase() {}
void DomPopupTest::SetUp() {}
void DomPopupTest::TearDown() {}

/**
 * @tc.name: DomPopupCreatorTest001
 * @tc.desc: Test create popup node successfully and popupcomponent create as desire.
 * @tc.type: FUNC
 */
HWTEST_F(DomPopupTest, DomPopupCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of popup component with tag only.
     */
    const std::string jsonPopupStr = ""
                                    "{                                                      "
                                    "  \"tag\": \"popup\"                                   "
                                    "}";
    /**
     * @tc.steps: step2. Verify whether the DomPopup.
     * @tc.expected: step3. DomPopup is not null.
     */
    auto popup = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonPopupStr);
    EXPECT_TRUE(popup);
}

/**
 * @tc.name: DomPopupTest002
 * @tc.desc: Verify that DomPopup can be set attributes and styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomPopupTest, DomPopupTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomPopup with attributes and styles.
     */
    const std::string jsonPopupStr = ""
                                    "{                                                         "
                                    "  \"tag\": \"popup\",                                     "
                                    "  \"attr\": [                                             "
                                    "              {"
                                    "                \"placement\" : \"left\"                  "
                                    "              }],                                         "
                                    "  \"style\":  [{ "
                                    "                \"maskColor\" : \"#0x0000ff\"             "
                                    "              }]"
                                    "}";

    /**
     * @tc.steps: step2. call jsonPopupStr interface, create DomPopup and set its style.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonPopupStr);
    auto popupChild = AceType::DynamicCast<PopupComponent>(domNodeRoot->GetSpecializedComponent());
    ACE_DCHECK(popupChild);

    /**
     * @tc.steps: step3. Check all the attributes and styles matched.
     * @tc.expected: step3. All the attributes and styles are matched.
     */
    EXPECT_TRUE(popupChild);
    EXPECT_EQ(popupChild->GetPopupParam()->GetPlacement(), PLACEMENT_VALUE);
    EXPECT_EQ(popupChild->GetPopupParam()->GetMaskColor(), Color::FromString(MASK_COLOR_DEFAULT));
}

/**
 * @tc.name: DomPopupTest003
 * @tc.desc: Test add event to popup component successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomPopupTest, DomPopupTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create popup node with it.
     * @tc.expected: step1. Popup node and is created successfully.
     */
    const std::string jsonPopupStr = ""
                                        "{                                      "
                                        "  \"tag\": \"popup\",                  "
                                        "  \"event\": [ \"visibilitychange\" ]  "
                                        "}";

    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonPopupStr);
    auto popupChild = AceType::DynamicCast<PopupComponent>(domNodeRoot->GetSpecializedComponent());
    ACE_DCHECK(popupChild);

    /**
     * @tc.steps: step2. Check eventId of created popup component.
     * @tc.expected: step2. The eventId value of popup component is as expected.
     */
    EXPECT_TRUE(popupChild);
    EXPECT_EQ(popupChild->GetPopupParam()->GetOnVisibilityChange(), std::to_string(domNodeRoot->GetNodeId()));
}

} // namespace OHOS::Ace::Framework
