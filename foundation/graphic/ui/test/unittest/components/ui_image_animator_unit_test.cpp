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

#include "components/ui_image_animator.h"

#include <climits>
#include <gtest/gtest.h>
#include "gfx_utils/file.h"
#include "test_resource_config.h"

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = { 50, 50 };
    const int16_t INIT_WIDTH = 94;
    const int16_t INIT_HEIGHT = 94;
    const uint8_t IMAGE_SIZE = 3;
}

class UIImageAnimatorViewTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIImageAnimatorView* imageAnimator_;
};

UIImageAnimatorView* UIImageAnimatorViewTest::imageAnimator_ = nullptr;

void UIImageAnimatorViewTest::SetUpTestCase(void)
{
    if (imageAnimator_ == nullptr) {
        imageAnimator_ = new UIImageAnimatorView();
    }
}

void UIImageAnimatorViewTest::TearDownTestCase(void)
{
    if (imageAnimator_ != nullptr) {
        delete imageAnimator_;
        imageAnimator_ = nullptr;
    }
}

static ImageAnimatorInfo g_imageAnimatorInfo[IMAGE_SIZE] = {
    { BLUE_RGB888_IMAGE_PATH, INIT_POS, INIT_WIDTH, INIT_HEIGHT, IMG_SRC_FILE_PATH },
    { BLUE_ARGB8888_IMAGE_PATH, INIT_POS, INIT_WIDTH, INIT_HEIGHT, IMG_SRC_FILE_PATH },
    { BLUE_RGB565_IMAGE_PATH, INIT_POS, INIT_WIDTH, INIT_HEIGHT, IMG_SRC_FILE_PATH },
};

/**
 * @tc.name: UIImageAnimatorViewGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQL
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewGetViewType_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(imageAnimator_->GetViewType(), UI_IMAGE_ANIMATOR_VIEW);
}

/**
 * @tc.name: UIImageAnimatorViewSetTimeOfPause_001
 * @tc.desc: Verify SetTimeOfPause function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQL
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetTimeOfPause_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint16_t timeOfPause = 1;

    imageAnimator_->SetTimeOfPause(timeOfPause);
    EXPECT_EQ(imageAnimator_->GetTimeOfPause(), timeOfPause);
}

/**
 * @tc.name: UIImageAnimatorViewSetTimeOfUpdate_001
 * @tc.desc: Verify SetTimeOfUpdate function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQL
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetTimeOfUpdate_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint16_t animatorTime = 300;

    imageAnimator_->SetTimeOfUpdate(animatorTime);
    EXPECT_EQ(imageAnimator_->GetTimeOfUpdate(), animatorTime);
}

/**
 * @tc.name: UIImageAnimatorViewSetImageAnimatorSrc_001
 * @tc.desc: Verify SetImageAnimatorSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQL
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetImageAnimatorSrc_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint8_t imageSize = IMAGE_SIZE;

    imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, imageSize);
    EXPECT_EQ(imageAnimator_->GetImageAnimatorImageNum(), imageSize);
}

/**
 * @tc.name: UIImageAnimatorViewSetImageAnimatorSrc_002
 * @tc.desc: Verify SetImageAnimatorSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetImageAnimatorSrc_002, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ImageAnimatorInfo* info = &g_imageAnimatorInfo[0];
    imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, IMAGE_SIZE);

    EXPECT_EQ(imageAnimator_->GetImageAnimatorSrc(), info);
}

/**
 * @tc.name: UIImageAnimatorViewSetSizeFixed_001
 * @tc.desc: Verify SetSizeFixed function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetSizeFixed_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool fixed = true;

    imageAnimator_->SetSizeFixed(fixed);
    EXPECT_EQ(imageAnimator_->IsSizeFixed(), fixed);
}

/**
 * @tc.name: UIImageAnimatorViewSetRepeat_001
 * @tc.desc: Verify SetRepeat function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetRepeat_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool repeat = true;

    imageAnimator_->SetRepeat(repeat);
    EXPECT_EQ(imageAnimator_->IsRepeat(), repeat);
}

/**
 * @tc.name: UIImageAnimatorViewSetReverse_001
 * @tc.desc: Verify SetReverse function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetReverse_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool reverse = true;

    imageAnimator_->SetReverse(reverse);
    EXPECT_EQ(imageAnimator_->IsReverse(), reverse);
}

/**
 * @tc.name: UIImageAnimatorViewSetStartPosition_001
 * @tc.desc: Verify SetStartPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetStartPosition_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint8_t state = Animator::STOP;

    EXPECT_EQ(imageAnimator_->GetState(), state);
}

/**
 * @tc.name: UIImageAnimatorViewStart_010
 * @tc.desc: Verify Start function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewStart_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageAnimator_->Start();
    EXPECT_EQ(imageAnimator_->GetState(), Animator::START);
}

/**
 * @tc.name: UIImageAnimatorViewStop_001
 * @tc.desc: Verify Stop function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewStop_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageAnimator_->Stop();
    EXPECT_EQ(imageAnimator_->GetState(), Animator::STOP);
}

/**
 * @tc.name: UIImageAnimatorViewPause_001
 * @tc.desc: Verify Pause function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewPause_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageAnimator_->Pause();
    EXPECT_EQ(imageAnimator_->GetState(), Animator::PAUSE);
}

/**
 * @tc.name: UIImageAnimatorViewResume_001
 * @tc.desc: Verify Resume function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewResume_001, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageAnimator_->Resume();
    EXPECT_EQ(imageAnimator_->GetState(), Animator::START);
}

/**
 * @tc.name: UIImageAnimatorViewSetFillMode_001
 * @tc.desc: Verify SetFillMode function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEF
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetFillMode_001, TestSize.Level0)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, IMAGE_SIZE);
    imageAnimator_->SetFillMode(true);
    imageAnimator_->SetReverse(false);
    imageAnimator_->Start();
    imageAnimator_->Stop();
    EXPECT_STREQ(imageAnimator_->GetPath(), g_imageAnimatorInfo[2].imagePath); // 2: the last image
}

/**
 * @tc.name: UIImageAnimatorViewSetFillMode_002
 * @tc.desc: Verify SetFillMode function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5J
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetFillMode_002, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, IMAGE_SIZE);
    imageAnimator_->SetFillMode(true);
    imageAnimator_->SetReverse(true);
    imageAnimator_->Start();
    imageAnimator_->Stop();
    EXPECT_STREQ(imageAnimator_->GetPath(), g_imageAnimatorInfo[0].imagePath);
}

/**
 * @tc.name: UIImageAnimatorViewSetFillMode_003
 * @tc.desc: Verify SetFillMode function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5J
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetFillMode_003, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, IMAGE_SIZE);
    imageAnimator_->SetFillMode(false);
    imageAnimator_->SetReverse(false);
    imageAnimator_->Start();
    imageAnimator_->Stop();
    EXPECT_STREQ(imageAnimator_->GetPath(), g_imageAnimatorInfo[0].imagePath);
}

/**
 * @tc.name: UIImageAnimatorViewSetFillMode_004
 * @tc.desc: Verify SetFillMode function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5J
 */
HWTEST_F(UIImageAnimatorViewTest, UIImageAnimatorViewSetFillMode_004, TestSize.Level1)
{
    if (imageAnimator_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, IMAGE_SIZE);
    imageAnimator_->SetFillMode(false);
    imageAnimator_->SetReverse(true);
    imageAnimator_->Start();
    imageAnimator_->Stop();
    EXPECT_STREQ(imageAnimator_->GetPath(), g_imageAnimatorInfo[2].imagePath); // 2: the last image
}
} // namespace OHOS
