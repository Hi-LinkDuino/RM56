/*
 * Copyright (c) 20212022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/common/media_query/media_queryer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class MediaQueryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MediaQueryTest::SetUpTestCase() {}
void MediaQueryTest::TearDownTestCase() {}
void MediaQueryTest::SetUp() {}
void MediaQueryTest::TearDown() {}

/**
 * @tc.name: MediaQueryTest001
 * @tc.desc: Verify that media query condition can match right result.
 * @tc.type: FUNC
 */
HWTEST_F(MediaQueryTest, MediaQueryTest001, TestSize.Level1)
{
    OHOS::Ace::Framework::MediaFeature mediaFeature = JsonUtil::Create(true);
    mediaFeature->Put("width", 1500);
    mediaFeature->Put("device-type", "tv");
    mediaFeature->Put("round-screen", true);

    struct ConditionTestCase {
        std::string condition;
        bool result;
    };
    std::vector<ConditionTestCase> conditions = {
        { "(max-width: )", false },
        { "(min-width: 1000)", true },
        { "(width < 2000)", true },
        { "(width << 2000)", false },
        { "(1000 < width)", true },
        { "(1000 < width < 2000)", true },
        { "(round-screen: true) (device-type:tv)", false },
        { "screen and (round-screen: true) and (device-type:tv)", true },
        { "not screen and (round-screen: true) and (device-type:tv)", false },
        { "not screen and (round-screen: false) and (device-type:tv)", true },
        { "not screen and (round-screen: true) and (device-type:phone)", true },
        { "screen and (round-screen: false), (device-type:tv)", true },
        { "only screen and (1000 < width < 2000), (device-type:phone)", true },
        { "(device-height < 2000) and (device-width < 2000) and (round-screen: true) and (device-type:phone)", false },
        { "(device-height > 2000) or (device-width > 2000) or (round-screen: false) or (device-type:tv)", true },
        { "(round-screen: true) or (device-type:phone)", true },
        { "(round-screen: true), screen and (1000 < width < 2000), (device-type:phone)", false },
    };

    MediaQueryer mediaQueryer;
    for (const auto& item : conditions) {
        bool result = mediaQueryer.MatchCondition(item.condition, mediaFeature);
        ASSERT_EQ(result, item.result) << "condition = " << item.condition;
    }
}

} // namespace OHOS::Ace::Framework
