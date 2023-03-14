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

#include "base/utils/system_properties.h"
#include "core/components/piece/piece_theme.h"
#include "core/pipeline/base/constants.h"
#include "frameworks/bridge/common/dom/dom_piece.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string PIECE_CONTENT_VALUE = "Chips";
const std::string PIECE_ICON_SRC = "test.svg";

} // namespace

class DomPieceTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: CreateDOMNodeFromDsl001
 * @tc.desc: Verify that DomPiece can create PieceComponent correctly with all attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomPieceTest, CreateDOMNodeFromDsl001, TestSize.Level1)
{
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        /**
         * @tc.steps: step1. the json string of DomPiece with all attributes set.
         */
        const std::string jsonPieceStr = ""
                                         "{                                                                "
                                         "  \"tag\": \"piece\",                                            "
                                         "  \"attr\": [{                                                   "
                                         "                \"content\" : \"Chips\"                          "
                                         "              },                                                 "
                                         "              {                                                  "
                                         "                \"closable\" : \"true\"                          "
                                         "              },                                                 "
                                         "              {                                                  "
                                         "                \"icon\" : \"test.svg\"                          "
                                         "              }],                                                "
                                         "  \"event\": [ \"close\" ]                                       "
                                         "}";

        /**
         * @tc.steps: step2. call JsonUtil interface and create DomPiece.
         */
        auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonPieceStr);
        auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
        auto pieceComponent = AceType::DynamicCast<PieceComponent>(boxChild->GetChild());

        /**
         * @tc.steps: step3. Verify whether all attributes of PieceComponent are as excepted.
         * @tc.expected: step3. All attributes of PieceComponent are as excepted.
         */
        EXPECT_TRUE(pieceComponent);
        EXPECT_TRUE(pieceComponent->ShowDelete());
        EXPECT_TRUE(pieceComponent->GetContent() == PIECE_CONTENT_VALUE);
        EXPECT_TRUE(pieceComponent->GetIcon() == PIECE_ICON_SRC);
        EXPECT_TRUE(pieceComponent->GetOnDelete() == std::to_string(domNodeRoot->GetNodeId()));
    }
}

/**
 * @tc.name: CreateDOMNodeFromDsl002
 * @tc.desc: Verify that DomPiece can create PieceComponent correctly with no attribute set.
 * @tc.type: FUNC
 */
HWTEST_F(DomPieceTest, CreateDOMNodeFromDsl002, TestSize.Level1)
{
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        /**
         * @tc.steps: step1. the json string of DomPiece with no attribute set.
         */
        const std::string jsonPieceStr = ""
                                         "{                     "
                                         "  \"tag\": \"piece\" "
                                         "}";

        /**
         * @tc.steps: step2. call JsonUtil interface and create DomPiece.
         */
        auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonPieceStr);
        auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
        auto pieceComponent = AceType::DynamicCast<PieceComponent>(boxChild->GetChild());

        /**
         * @tc.steps: step3. Verify whether all attributes of PieceComponent are as excepted.
         * @tc.expected: step3. All attributes of PieceComponent are as excepted.
         */
        EXPECT_TRUE(pieceComponent);
        EXPECT_TRUE(!pieceComponent->ShowDelete());
        EXPECT_TRUE(pieceComponent->GetContent().empty());
        EXPECT_TRUE(pieceComponent->GetIcon().empty());
    }
}

} // namespace OHOS::Ace::Framework