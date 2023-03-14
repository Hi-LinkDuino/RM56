/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_qrcode.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t WIDTH = 100;
    const int16_t HEIGHT = 100;
}
class UIQrcodeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIQrcode* qrcode_;
};

UIQrcode* UIQrcodeTest::qrcode_ = nullptr;

void UIQrcodeTest::SetUpTestCase()
{
    if (qrcode_ == nullptr) {
        qrcode_ = new UIQrcode();
    }
}

void UIQrcodeTest::TearDownTestCase()
{
    if (qrcode_ != nullptr) {
        delete qrcode_;
        qrcode_ = nullptr;
    }
}

/**
 * @tc.name: Graphic_UIQrcodeTest_Test_GetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5M
 */
HWTEST_F(UIQrcodeTest, Graphic_UIQrcodeTest_Test_GetViewType_001, TestSize.Level1)
{
    if (qrcode_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(qrcode_->GetViewType(), UI_QRCODE);
}

/**
 * @tc.name: Graphic_UIQrcodeTest_Test_SetWidth_001
 * @tc.desc: Verify SetWidth function.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5M
 */
HWTEST_F(UIQrcodeTest, Graphic_UIQrcodeTest_Test_SetWidth_001, TestSize.Level0)
{
    if (qrcode_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    qrcode_->SetWidth(WIDTH);
    EXPECT_EQ(qrcode_->GetWidth(), WIDTH);
}

/**
 * @tc.name: Graphic_UIQrcodeTest_Test_SetHeight_001
 * @tc.desc: Verify SetHeight function.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEF
 */
HWTEST_F(UIQrcodeTest, Graphic_UIQrcodeTest_Test_SetHeight_001, TestSize.Level0)
{
    if (qrcode_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    qrcode_->SetHeight(HEIGHT);
    EXPECT_EQ(qrcode_->GetHeight(), HEIGHT);
}
} // namespace OHOS
