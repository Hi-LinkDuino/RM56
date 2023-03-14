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

#include "components/ui_image_view.h"
#include <climits>
#include <gtest/gtest.h>
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "draw/draw_utils.h"
#include "test_resource_config.h"

using namespace testing::ext;

namespace OHOS {
class UIImageViewTest : public testing::Test {
public:
    UIImageViewTest() : imageView_(nullptr) {}
    ~UIImageViewTest() {}
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp(void);
    void TearDown(void);
    UIImageView* imageView_;
};

void UIImageViewTest::SetUp(void)
{
    if (imageView_ == nullptr) {
        imageView_ = new UIImageView();
    }
}

void UIImageViewTest::TearDown(void)
{
    if (imageView_ != nullptr) {
        delete imageView_;
        imageView_ = nullptr;
    }
}

/**
 * @tc.name: UIImageViewGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewGetViewType_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(imageView_->GetViewType(), UI_IMAGE_VIEW);
}

/**
 * @tc.name: UIImageViewSetPosition_001
 * @tc.desc: Verify SetPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetPosition_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t initPosX = 10;
    const int16_t initPosY = 10;
    const int16_t initWidth = 100;
    const int16_t initHeight = 150;

    imageView_->SetPosition(initPosX, initPosY);
    imageView_->SetHeight(initWidth);
    imageView_->SetWidth(initHeight);

    EXPECT_EQ(imageView_->GetX(), initPosX);
    EXPECT_EQ(imageView_->GetY(), initPosY);
    EXPECT_EQ(imageView_->GetHeight(), initWidth);
    EXPECT_EQ(imageView_->GetWidth(), initHeight);

    imageView_->SetPosition(0, 0, 0, 0);
    EXPECT_EQ(imageView_->GetX(), 0);
    EXPECT_EQ(imageView_->GetY(), 0);
    EXPECT_EQ(imageView_->GetWidth(), 0);
    EXPECT_EQ(imageView_->GetHeight(), 0);
}

#if (ENABLE_GIF == 1)
/**
 * @tc.name: UIImageViewSetSrc_001
 * @tc.desc: Verify SetSrc function, correct gif path, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEO
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const char* strPath1 = static_cast<const char*>(GIF_IMAGE_PATH1);
    imageView_->SetSrc(strPath1);
    EXPECT_EQ(imageView_->GetPath(), nullptr);
}

/**
 * @tc.name: UIImageViewSetSrc_002
 * @tc.desc: Verify SetSrc function, error gif path, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F3R70
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc_002, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const char* strPathError = static_cast<const char*>(GIF_IMAGE_PATH_ERROR);
    imageView_->SetSrc(strPathError);
    EXPECT_EQ(imageView_->GetPath(), nullptr);
}

/**
 * @tc.name: UIImageViewSetSrc_003
 * @tc.desc: Verify SetSrc function, tif image format path, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F3R70
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc_003, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const char* strPath = static_cast<const char*>(TIF_IMAGE_PATH);
    imageView_->SetSrc(strPath);
    EXPECT_STREQ(imageView_->GetPath(), strPath);
}
#endif

/**
 * @tc.name: UIImageViewSetSrc_004
 * @tc.desc: Verify SetSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc_004, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    char* srcPath = nullptr;
    imageView_->SetSrc(srcPath);
    EXPECT_EQ(imageView_->GetPath(), srcPath);
    const char* strPath2 = static_cast<const char*>(BLUE_RGB888_IMAGE_PATH);
    imageView_->SetSrc(strPath2);
    EXPECT_STREQ(imageView_->GetPath(), strPath2);
    ImageInfo* srcPath2 = nullptr;
    imageView_->SetSrc(srcPath2);
    EXPECT_EQ(imageView_->GetImageInfo(), srcPath2);
}

/**
 * @tc.name: UIImageViewSetSrc001
 * @tc.desc: Verify SetSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000FQNFC
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const ImageInfo* imageInfo = nullptr;
    imageView_->SetSrc(imageInfo);
    EXPECT_EQ(imageView_->GetImageInfo(), nullptr);
}

/**
 * @tc.name: UIImageViewSetSrc002
 * @tc.desc: Verify SetSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFD
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc002, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const ImageInfo imageInfo {0};
    imageView_->SetSrc(&imageInfo);
    EXPECT_EQ(imageView_->GetImageInfo()->dataSize, 0);
}

/**
 * @tc.name: UIImageViewSetSrc003
 * @tc.desc: Verify SetSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFD
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc003, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ImageHeader header {A8, 0, 0, 0, 0, 0};
    const ImageInfo imageInfo {header, 0, nullptr, nullptr};
    imageView_->SetSrc(&imageInfo);
    EXPECT_EQ(imageView_->GetImageInfo()->dataSize, 0);
}

/**
 * @tc.name: UIImageViewSetSrc004
 * @tc.desc: Verify SetSrc function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFD
 */
HWTEST_F(UIImageViewTest, UIImageViewSetSrc004, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ImageHeader header {A8, 0, 0, 0, 0, 0};
    const ImageInfo imageInfo {header, 0, nullptr, nullptr};
    imageView_->SetSrc(&imageInfo);
    ColorMode colorMode = static_cast<ColorMode>(imageView_->GetImageInfo()->header.colorMode);
    uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(colorMode);
    EXPECT_EQ(pxSize, 8); // 8 bits
}

/**
 * @tc.name: UIImageViewSetAutoEnable_001
 * @tc.desc: Verify SetAutoEnable function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetAutoEnable_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetAutoEnable(true);
    EXPECT_EQ(imageView_->GetAutoEnable(), true);
}

/**
 * @tc.name: UIImageViewSetParent_001
 * @tc.desc: Verify SetParent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetParent_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView uiView;
    imageView_->SetParent(nullptr);
    EXPECT_EQ(imageView_->GetParent(), nullptr);
    imageView_->SetParent(&uiView);
    EXPECT_NE(imageView_->GetParent(), nullptr);
}

/**
 * @tc.name: UIImageViewSetNextSibling_001
 * @tc.desc: Verify SetNextSibling function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetNextSibling_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView uiView;
    imageView_->SetNextSibling(nullptr);
    EXPECT_EQ(imageView_->GetNextSibling(), nullptr);
    imageView_->SetNextSibling(&uiView);
    EXPECT_NE(imageView_->GetNextSibling(), nullptr);
}

/**
 * @tc.name: UIImageViewSetVisible_001
 * @tc.desc: Verify SetVisible function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetVisible_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetVisible(true);
    EXPECT_EQ(imageView_->IsVisible(), true);
    imageView_->SetVisible(false);
    EXPECT_EQ(imageView_->IsVisible(), false);
}

/**
 * @tc.name: UIImageViewSetTouchable_001
 * @tc.desc: Verify SetTouchable function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetTouchable_001, TestSize.Level1)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetTouchable(true);
    EXPECT_EQ(imageView_->IsTouchable(), true);
    imageView_->SetTouchable(false);
    EXPECT_EQ(imageView_->IsTouchable(), false);
}

/**
 * @tc.name: UIImageViewSetDraggable_001
 * @tc.desc: Verify SetDraggable function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetDraggable_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetDraggable(true);
    EXPECT_EQ(imageView_->IsDraggable(), true);
    imageView_->SetDraggable(false);
    EXPECT_EQ(imageView_->IsDraggable(), false);
}

/**
 * @tc.name: UIImageViewSetDragParentInstead_001
 * @tc.desc: Verify SetDragParentInstead function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetDragParentInstead_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetDragParentInstead(true);
    EXPECT_EQ(imageView_->IsDragParentInstead(), true);
    imageView_->SetDragParentInstead(false);
    EXPECT_EQ(imageView_->IsDragParentInstead(), false);
}

/**
 * @tc.name: UIImageViewResize_001
 * @tc.desc: Verify Resize function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewResize_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t currPosX = 100;
    const int16_t currPosY = 300;

    imageView_->Resize(currPosX, currPosY);
    EXPECT_EQ(imageView_->GetWidth(), currPosX);
    EXPECT_EQ(imageView_->GetHeight(), currPosY);
}

/**
 * @tc.name: UIImageViewSetListener_001
 * @tc.desc: Verify SetListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetListener_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetOnDragListener(nullptr);
    imageView_->SetOnClickListener(nullptr);
    imageView_->SetOnLongPressListener(nullptr);

    EXPECT_EQ(imageView_->GetOnDragListener(), nullptr);
    EXPECT_EQ(imageView_->GetOnClickListener(), nullptr);
    EXPECT_EQ(imageView_->GetOnLongPressListener(), nullptr);
}

/**
 * @tc.name: UIImageViewSetViewId_001
 * @tc.desc: Verify SetViewId function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetViewId_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    imageView_->SetViewId(nullptr);
    EXPECT_EQ(imageView_->GetViewId(), nullptr);
}

/**
 * @tc.name: UIImageViewSetViewIndex_001
 * @tc.desc: Verify SetViewIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetViewIndex_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t index = 101;
    imageView_->SetViewIndex(index);
    EXPECT_EQ(imageView_->GetViewIndex(), index);
}

/**
 * @tc.name: UIImageViewSetBlurLevel_001
 * @tc.desc: Verify SetBlurLevel function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetBlurLevel_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    BlurLevel level = LEVEL0;
    imageView_->SetBlurLevel(level);
    EXPECT_EQ(imageView_->GetBlurLevel(), level);
}

/**
 * @tc.name: UIImageViewSetTransformAlgorithm_001
 * @tc.desc: Verify SetTransformAlgorithm function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIImageViewTest, UIImageViewSetTransformAlgorithm_001, TestSize.Level0)
{
    if (imageView_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    TransformAlgorithm algorithm = NEAREST_NEIGHBOR;
    imageView_->SetTransformAlgorithm(algorithm);
    EXPECT_EQ(imageView_->GetTransformAlgorithm(), algorithm);
}
} // namespace OHOS
