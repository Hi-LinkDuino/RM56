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

#include "ui_test_image_animator.h"
#include "common/screen.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
static ImageAnimatorInfo g_imageAnimatorInfo[4] = {
    {IMAGE_ANIMATOR_0_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {IMAGE_ANIMATOR_1_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {IMAGE_ANIMATOR_2_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {IMAGE_ANIMATOR_3_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
};

static ImageAnimatorInfo g_imageAnimatorInfo2[4] = {
    {BLUE_IMAGE_PATH, {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
    {RED_IMAGE_PATH, {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
    {GREEN_IMAGE_PATH, {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
    {YELLOW_IMAGE_PATH, {84, 108}, 94, 94, IMG_SRC_FILE_PATH},
};
} // namespace

void UITestImageAnimator::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);

        UIViewGroup* uiViewGroup = new UIViewGroup();
        // 320: width; 390: height
        uiViewGroup->SetPosition(0, 0, 320, 390);
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: width; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("UIImageAnimator效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        UIViewGroup* uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame_);
        // 288: width; 336: height
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 288, 336);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);

        imageAnimator_ = new UIImageAnimatorView();
        imageAnimator_->SetPosition(50, 50, 200, 200); // 50 : offset 50 : offset 200 : offset 200: offset
        imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo, 4, 100); // 4: the number of images, 100: updating time
        imageAnimator_->Start();
        listener_ = new TestAnimatorStopListener(container_);
        listener_->Init();
        uiViewGroupFrame_->Add(imageAnimator_);
        imageAnimator_->LayoutCenterOfParent();
    }

    if (layout_ == nullptr) {
        layout_ = new GridLayout();
        uint16_t height = Screen::GetInstance().GetHeight();
        layout_->SetPosition(370, 0, 320, height - 50); // 370: x , 320: width, 50: offset
        container_->Add(layout_);
        layout_->SetLayoutDirection(LAYOUT_VER);
        layout_->SetRows(9); // 9 : rows
        layout_->SetCols(2); // 2 : columns
    }

    InitImageInfo();
}

void UITestImageAnimator::InitImageInfo()
{
    const uint16_t pixelByteSize = 4; // 4: bytes per pixel
    const uint16_t width = 100;       // 100: image's width
    const uint16_t height = 100;      // 100: image's height
    uint32_t dataSize = width * height * pixelByteSize;
    uint8_t* srcData1 = static_cast<uint8_t*>(UIMalloc(dataSize));
    if (srcData1 == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < dataSize; i += pixelByteSize) {
        srcData1[i] = 255;            // 255: pixel value
        srcData1[i + 1] = 0;          // 1: set green channel
        srcData1[i + 2] = 0;          // 2: set red channel
        srcData1[i + 3] = OPA_OPAQUE; // 3: set alpha channel
    }
    imageInfo1_ = static_cast<ImageInfo*>(UIMalloc(sizeof(ImageInfo)));
    if (imageInfo1_ == nullptr) {
        UIFree(srcData1);
        return;
    }

    imageInfo1_->header.width = width;
    imageInfo1_->header.height = height;
    imageInfo1_->header.colorMode = ColorMode::ARGB8888;
    imageInfo1_->dataSize = dataSize;
    imageInfo1_->data = srcData1;

    uint8_t* srcData2 = static_cast<uint8_t*>(UIMalloc(dataSize));
    if (srcData2 == nullptr) {
        UIFree(imageInfo1_);
        UIFree(srcData1);
        return;
    }
    for (uint32_t i = 0; i < dataSize; i += pixelByteSize) {
        srcData2[i] = 0;              // set blue channel
        srcData2[i + 1] = 255;        // 1: set green channel 255: pixel value
        srcData2[i + 2] = 0;          // 2: set red channel
        srcData2[i + 3] = OPA_OPAQUE; // 3: set alpha channel
    }

    imageInfo2_ = static_cast<ImageInfo*>(UIMalloc(sizeof(ImageInfo)));
    if (imageInfo2_ == nullptr) {
        UIFree(srcData2);
        UIFree(imageInfo1_);
        UIFree(srcData1);
        return;
    }
    imageInfo2_->header.width = width;
    imageInfo2_->header.height = height;
    imageInfo2_->header.colorMode = ARGB8888;
    imageInfo2_->dataSize = dataSize;
    imageInfo2_->data = srcData2;

    uint8_t* srcData3 = static_cast<uint8_t*>(UIMalloc(dataSize));
    if (srcData3 == nullptr) {
        UIFree(imageInfo2_);
        UIFree(srcData2);
        UIFree(imageInfo1_);
        UIFree(srcData1);
        return;
    }
    for (uint32_t i = 0; i < dataSize; i += pixelByteSize) {
        srcData3[i] = 0;              // set blue channel
        srcData3[i + 1] = 0;          // 1: set green channel
        srcData3[i + 2] = 255;        // 2: set red channel 255: pixel value
        srcData3[i + 3] = OPA_OPAQUE; // 3: set alpha channel
    }

    imageInfo3_ = static_cast<ImageInfo*>(UIMalloc(sizeof(ImageInfo)));
    if (imageInfo3_ == nullptr) {
        UIFree(srcData3);
        UIFree(imageInfo2_);
        UIFree(srcData2);
        UIFree(imageInfo1_);
        UIFree(srcData1);
        return;
    }
    imageInfo3_->header.width = width;
    imageInfo3_->header.height = height;
    imageInfo3_->header.colorMode = ARGB8888;
    imageInfo3_->dataSize = dataSize;
    imageInfo3_->data = srcData3;

    imageAnimatorImageInfo_[0].imageInfo = imageInfo1_;
    imageAnimatorImageInfo_[0].pos = {84, 108};
    imageAnimatorImageInfo_[0].width = 100;  // 100: width value
    imageAnimatorImageInfo_[0].height = 100; // 100: height value
    imageAnimatorImageInfo_[0].imageType = IMG_SRC_IMAGE_INFO;

    imageAnimatorImageInfo_[1].imageInfo = imageInfo2_;
    imageAnimatorImageInfo_[1].pos = {84, 108};
    imageAnimatorImageInfo_[1].width = 100;  // 100: width value
    imageAnimatorImageInfo_[1].height = 100; // 100: height value
    imageAnimatorImageInfo_[1].imageType = IMG_SRC_IMAGE_INFO;

    imageAnimatorImageInfo_[2].imageInfo = imageInfo3_;        // 2: image index
    imageAnimatorImageInfo_[2].pos = {84, 108};                // 2: image index
    imageAnimatorImageInfo_[2].width = 100;                    // 2: image index, 100: width value
    imageAnimatorImageInfo_[2].height = 100;                   // 2: image index, 100: height value
    imageAnimatorImageInfo_[2].imageType = IMG_SRC_IMAGE_INFO; // 2: image index
}

void UITestImageAnimator::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    imageAnimator_ = nullptr;
    layout_ = nullptr;
    delete listener_;
    listener_ = nullptr;

    UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(imageInfo1_->data)));
    imageInfo1_->data = nullptr;
    UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(imageInfo2_->data)));
    imageInfo2_->data = nullptr;
    UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(imageInfo3_->data)));
    imageInfo3_->data = nullptr;
    UIFree(reinterpret_cast<void*>(const_cast<ImageInfo*>(imageInfo1_)));
    imageInfo1_ = nullptr;
    UIFree(reinterpret_cast<void*>(const_cast<ImageInfo*>(imageInfo2_)));
    imageInfo2_ = nullptr;
    UIFree(reinterpret_cast<void*>(const_cast<ImageInfo*>(imageInfo3_)));
    imageInfo3_ = nullptr;
}

const UIView* UITestImageAnimator::GetTestView()
{
    UIKit_ImageAnimator_Test_Start_001();
    UIKit_ImageAnimator_Test_Stop_002();
    UIKit_ImageAnimator_Test_Pause_003();
    UIKit_ImageAnimator_Test_Resume_004();
    UIKit_ImageAnimator_Test_SetImageAnimatorSrc_005();
    UIKit_ImageAnimator_Test_SetTickOfUpdate_006();
    UIKit_ImageAnimator_Test_SetSizeFixed_007();
    UIKit_ImageAnimator_Test_SetRepeat_008();
    UIKit_ImageAnimator_Test_SetReverse_009();
    UIKit_ImageAnimator_Test_SetAnimatorStopListener_010();
    UIKit_ImageAnimator_Test_SetRepeatTimes_011();
    UIKit_ImageAnimator_Test_SetFillMode_True_Forward_012();
    UIKit_ImageAnimator_Test_SetFillMode_True_Backward_013();
    UIKit_ImageAnimator_Test_SetFillMode_False_Forward_014();
    UIKit_ImageAnimator_Test_SetFillMode_False_Backward_015();
    UIKit_ImageAnimator_Test_SetImageInfo_016();

    layout_->LayoutChildren();
    return container_;
}

void UITestImageAnimator::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    layout_->Add(btn);
    btn->Resize(BUTTON_WIDHT2, BUTTON_HEIGHT2);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_Start_001()
{
    startBtn_ = new UILabelButton();
    SetUpButton(startBtn_, "Start");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_Stop_002()
{
    stopBtn_ = new UILabelButton();
    SetUpButton(stopBtn_, "Stop");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_Pause_003()
{
    pauseBtn_ = new UILabelButton();
    SetUpButton(pauseBtn_, "Pause");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_Resume_004()
{
    resumeBtn_ = new UILabelButton();
    SetUpButton(resumeBtn_, "Resume");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetImageAnimatorSrc_005()
{
    setImageBtn_ = new UILabelButton();
    SetUpButton(setImageBtn_, "设置图片");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetTickOfUpdate_006()
{
    setSpeedBtn_ = new UILabelButton();
    SetUpButton(setSpeedBtn_, "设置速度");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetSizeFixed_007()
{
    fixedBtn_ = new UILabelButton();
    SetUpButton(fixedBtn_, "图片大小固定");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetRepeat_008()
{
    repeatBtn_ = new UILabelButton();
    noRepeatBtn_ = new UILabelButton();
    SetUpButton(repeatBtn_, "循环播放");
    SetUpButton(noRepeatBtn_, "播放一次 ");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetReverse_009()
{
    reverseOrderBtn_ = new UILabelButton();
    positiveOrderBtn_ = new UILabelButton();
    SetUpButton(reverseOrderBtn_, "逆序播放");
    SetUpButton(positiveOrderBtn_, "正序播放");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetAnimatorStopListener_010()
{
    listenerBtn_ = new UILabelButton();
    SetUpButton(listenerBtn_, "播放结束监听");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetRepeatTimes_011()
{
    repeatTimesBtn_ = new UILabelButton();
    SetUpButton(repeatTimesBtn_, "播放次数+");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetFillMode_True_Forward_012()
{
    fillModeTrueForwardBtn_ = new UILabelButton();
    SetUpButton(fillModeTrueForwardBtn_, "fillMode true 正播");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetFillMode_True_Backward_013()
{
    fillModeTrueBackwardBtn_ = new UILabelButton();
    SetUpButton(fillModeTrueBackwardBtn_, "fillMode true 逆播");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetFillMode_False_Forward_014()
{
    fillModeFalseForwardBtn_ = new UILabelButton();
    SetUpButton(fillModeFalseForwardBtn_, "fillMode false 正播");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetFillMode_False_Backward_015()
{
    fillModeFalseBackwardBtn_ = new UILabelButton();
    SetUpButton(fillModeFalseBackwardBtn_, "fillMode false 逆播");
}

void UITestImageAnimator::UIKit_ImageAnimator_Test_SetImageInfo_016()
{
    setImageInfoBtn_ = new UILabelButton();
    SetUpButton(setImageInfoBtn_, "设置ImageInfo");
}

bool UITestImageAnimator::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == startBtn_) {
        imageAnimator_->Start();
    } else if (&view == stopBtn_) {
        imageAnimator_->Stop();
    } else if (&view == pauseBtn_) {
        imageAnimator_->Pause();
    } else if (&view == resumeBtn_) {
        imageAnimator_->Resume();
    } else if (&view == setImageBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetImageAnimatorSrc(g_imageAnimatorInfo2, 4); // 4: the number of images
        imageAnimator_->Start();
    } else if (&view == setSpeedBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetTimeOfUpdate(10); // 10: set time of update
        imageAnimator_->Start();
    } else if (&view == fixedBtn_) {
        imageAnimator_->SetSizeFixed(true);
        imageAnimator_->SetPosition(50, 50, 200, 200); // 50 : offset 50 : offset 200 : offset 200 ：offset
    } else if (&view == repeatBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetRepeat(true);
        imageAnimator_->Start();
    } else if (&view == noRepeatBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetRepeat(false);
        imageAnimator_->Start();
    } else if (&view == reverseOrderBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetReverse(true);
        imageAnimator_->Start();
    } else if (&view == positiveOrderBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetReverse(false);
        imageAnimator_->Start();
    } else if (&view == listenerBtn_) {
        imageAnimator_->SetAnimatorStopListener(listener_);
    } else if (&view == repeatTimesBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetRepeatTimes(imageAnimator_->GetRepeatTimes() + 1);
        imageAnimator_->Start();
    } else if (&view == fillModeTrueForwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(true);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(false);
        imageAnimator_->Start();
    } else if (&view == fillModeTrueBackwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(true);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(true);
        imageAnimator_->Start();
    } else if (&view == fillModeFalseForwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(false);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(false);
        imageAnimator_->Start();
    } else if (&view == fillModeFalseBackwardBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetFillMode(false);
        imageAnimator_->SetRepeat(false);
        imageAnimator_->SetReverse(true);
        imageAnimator_->Start();
    } else if (&view == setImageInfoBtn_) {
        imageAnimator_->Stop();
        imageAnimator_->SetImageAnimatorSrc(imageAnimatorImageInfo_, 3); // 3: the number of images
        imageAnimator_->Start();
    }
    return true;
}
} // namespace OHOS
