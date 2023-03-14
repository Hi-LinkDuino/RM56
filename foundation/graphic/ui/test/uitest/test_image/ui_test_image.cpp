/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "ui_test_image.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "default_resource/check_box_res.h"
#if (ENABLE_GIF == 1)
#include "components/ui_image_view.h"
#include "test_resource_config.h"
#endif
#include "imgdecode/image_load.h"
#include "test_resource_config.h"
namespace OHOS {
namespace {
int16_t g_height = 0;
}

void UITestImage::SetUp()
{
    g_height = 0;
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestImage::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
#if (ENABLE_GIF == 1)
    gifImageView_ = nullptr;
    gifToGif_ = nullptr;
    gifToJpeg_ = nullptr;
    gifToPng_ = nullptr;
    gifToBin01_ = nullptr;
    gifToBin02_ = nullptr;
    gifToBin03_ = nullptr;
    gifToBin04_ = nullptr;
#endif
}

const UIView* UITestImage::GetTestView()
{
    UIKit_UIImage_Test_SetImage_001();
    UIKit_UIImage_Test_SetImage_002();
    UIKit_UIImage_Test_SetImage_004();
    UIKit_UIImage_Test_SetImage_005();
    UIKit_UIImage_Test_SetImage_006();
    UIKit_UIImage_Test_SetImage_007();
    UIKit_UIImage_Test_SetImage_008();
    UIKit_UIImage_Test_SetImage_009();
    UIKit_UIImage_Test_SetImage_010();
    UIKit_UIImage_Test_SetImage_011();
    UIKit_UIImage_Test_SetImage_012();
    UIKit_UIImage_Test_SetImage_013();
    UIKit_UIImage_Test_SetImage_014();
#if (ENABLE_GIF == 1)
    UIKit_UIImage_Test_SetImage_015();
    UIKit_UIImage_Test_SetImage_016();
#endif
    UIKit_UIImage_Test_Uncompress_001();
    UIKit_UIImage_Test_Resize_001();
    return container_;
}

const char* UITestImage::GetCharByImageSrcType(ImageSrcType srcType) const
{
    if (srcType == IMG_SRC_VARIABLE) {
        return "the src of image is variable type";
    } else if (srcType == IMG_SRC_FILE) {
        return "the src of image is file type";
    } else {
        return "Null";
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_001()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height + 5, // 5: increase y-coordinate
                           Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("rgb888 格式图片测试 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 40); // 40 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(BLUE_RGB888_IMAGE_PATH);

        UILabel* label2 = new UILabel();
        // 160 : offset 65 : offset 300 : offset
        label2->SetPosition(160, g_height + 65, 300, TITLE_LABEL_DEFAULT_HEIGHT);
        label2->SetText(GetCharByImageSrcType(imageView->GetSrcType()));
        label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        label2->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
        label2->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
        label2->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

        container_->Add(label2);
        container_->Add(imageView);
        g_height += 130; // offset=130
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_002()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示ARGB8888bin文件图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30: increase offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(BLUE_ARGB8888_IMAGE_PATH);

        container_->Add(imageView);
        g_height += 120; // offset=120
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_004()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height + 5,                      // 5: increase offset
                           Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT); // 5: increase offset
        label->SetText("显示RGB565bin文件图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 35); //  35 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(BLUE_RGB565_IMAGE_PATH);
        imageView->GetHeight();

        container_->Add(imageView);
        g_height += 140; // offset=140
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_005()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示ARGB8888 数组型图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 40); // 40 : increase offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(GetCheckBoxOnInfo());
        imageView->GetHeight();

        UIImageView* imageView2 = new UIImageView();
        imageView2->SetPosition(110, g_height + 40); // 110 : offset 40 : increase offset
        imageView2->SetWidth(50);                    // 50 : size
        imageView2->SetHeight(50);                   // 50 : size
        imageView2->SetSrc(GetCheckBoxOffInfo());
        imageView2->GetHeight();

        container_->Add(imageView);
        container_->Add(imageView2);
        g_height += 100; // offset=100
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_006()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示24位真彩色jpg图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(JPEG_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 190; // offset=190
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_007()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深1索引png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_1_PALETTE_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 90; // offset=90
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_008()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深2索引png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_2_PALETTE_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 90; // offset=90
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_009()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深4索引png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_4_PALETTE_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 100; // offset=100
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_010()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深8索引png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_8_PALETTE_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 100; // offset=100
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_011()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深24真彩色png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_24_RGB_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 100; // offset=100
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_012()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深32真彩色带Alpha通道png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_32_RGB_ALPHA_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 100; // offset=100
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_013()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深8灰度png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_8_GRAY_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 100; // offset=100
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_014()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示位深16灰度png图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30 : offset
        imageView->SetWidth(50);                                           // 50 : size
        imageView->SetHeight(50);                                          // 50 : size
        imageView->SetSrc(PNG_16_GRAY_IMAGE_PATH);
        imageView->GetHeight();
        container_->Add(imageView);
        g_height += 100; // offset=100
    }
}

#if (ENABLE_GIF == 1)
void UITestImage::UIKit_UIImage_Test_SetImage_015()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("显示gif文件图片 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 30); // 30: Y-coordinate
        imageView->SetSrc(GIF_IMAGE_PATH1);
        container_->Add(imageView);

        UIImageView* imageView2 = new UIImageView();
        imageView2->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, g_height + 120); // 120: Y-coordinate
        imageView2->SetSrc(GIF_IMAGE_PATH2);
        container_->Add(imageView2);
        g_height += 180; // offset=180
    }
}

void UITestImage::UIKit_UIImage_Test_SetImage_016()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("不同类型图片切换 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        gifImageView_ = new UIImageView();
        gifImageView_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, 1790); // 1790: Y-coordinate
        gifImageView_->SetSrc(GIF_IMAGE_PATH1);
        container_->Add(gifImageView_);
        gifToGif_ = new UILabelButton();
        SetUpButton(gifToGif_, "切换GIF");
        uint16_t gifHeight = g_height + 30; // 30: offset
        gifToGif_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, gifHeight);
        gifToJpeg_ = new UILabelButton();
        SetUpButton(gifToJpeg_, "切换JPEG");
        gifToJpeg_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 160, gifHeight); // 160: offset
        gifToPng_ = new UILabelButton();
        SetUpButton(gifToPng_, "切换PNG");
        gifToPng_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 320, gifHeight); // 320: offset
        gifToBin01_ = new UILabelButton();
        SetUpButton(gifToBin01_, "切换rgb888");
        gifToBin01_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 480, gifHeight); // 480: offset
        gifToBin02_ = new UILabelButton();
        SetUpButton(gifToBin02_, "ARGB8888bin");
        gifHeight += 50; // 50: offset
        gifToBin02_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, gifHeight);
        gifToBin03_ = new UILabelButton();
        SetUpButton(gifToBin03_, "RGB565bin");
        gifToBin03_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 160, gifHeight); // 160: offset
        gifToBin04_ = new UILabelButton();
        SetUpButton(gifToBin04_, "ARGB8888数组");
        gifToBin04_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 320, gifHeight); // 320: offset
        g_height += 300;                                                       // offset=300
    }
}

void UITestImage::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->Resize(BUTTON_WIDHT3, BUTTON_HEIGHT3);
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

bool UITestImage::OnClick(UIView &view, const ClickEvent& event)
{
    if (gifImageView_ == nullptr) {
        return false;
    }
    if (&view == gifToGif_) {
        gifImageView_->SetSrc(GIF_IMAGE_PATH2);
    } else if (&view == gifToJpeg_) {
        gifImageView_->SetSrc(JPEG_IMAGE_PATH);
    } else if (&view == gifToPng_) {
        gifImageView_->SetSrc(BLUE_IMAGE_PATH);
    } else if (&view == gifToBin01_) {
        gifImageView_->SetSrc(BLUE_RGB888_IMAGE_PATH);
    } else if (&view == gifToBin02_) {
        gifImageView_->SetSrc(BLUE_ARGB8888_IMAGE_PATH);
    } else if (&view == gifToBin03_) {
        gifImageView_->SetSrc(BLUE_RGB565_IMAGE_PATH);
    } else if (&view == gifToBin04_) {
        gifImageView_->SetSrc(GetCheckBoxOnInfo());
    }
    return true;
}
#endif

void UITestImage::UIKit_UIImage_Test_Uncompress_001()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("图片解压测试");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* imageView1 = new UIImageView();
        imageView1->SetPosition(48, g_height + 30); // 48 : offset; 30 : offset
        imageView1->SetSrc(IMAGE_DIR "blue_compress.bin");

        UIImageView* imageView2 = new UIImageView();
        imageView2->SetPosition(158, g_height + 30); // 158 : offset 30 : offset
        imageView2->SetSrc(IMAGE_DIR "red_compress.bin");

        UIImageView* imageView3 = new UIImageView();
        imageView3->SetPosition(268, g_height + 30); // 268 : offset 30 : offset
        imageView3->SetSrc(IMAGE_DIR "FaultInjection.bin");

        container_->Add(imageView1);
        container_->Add(imageView2);
        container_->Add(imageView3);
        g_height += 150; // offset=150
    }
}

UILabel* UITestImage::AddLable(int16_t x, int16_t y, const char* data)
{
    UILabel* label = new UILabel();
    container_->Add(label);
    label->SetPosition(x, y, Screen::GetInstance().GetWidth(),
        TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(data);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

UIImageView* UITestImage::AddImageView(const Rect rect, const char* src, bool autoEnable,
    UIImageView::ImageResizeMode mode)
{
    UIImageView* imageView = new UIImageView();
    imageView->SetAutoEnable(autoEnable);
    imageView->SetPosition(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
    imageView->SetStyle(STYLE_BORDER_COLOR, Color::Blue().full);
    imageView->SetStyle(STYLE_BORDER_WIDTH, 1);
    imageView->SetSrc(src);
    imageView->SetResizeMode(mode);
    container_->Add(imageView);
    return imageView;
}

void UITestImage::UIKit_UIImage_Test_Resize_001()
{
    if (container_ != nullptr) {
        AddLable(TEXT_DISTANCE_TO_LEFT_SIDE, g_height, "图片缩放模式测试");
        constexpr uint16_t STEP = 110; // 110: increase y-coordinate per step
        const uint16_t LABLE_Y = g_height + 30;
        const uint16_t IMAGE_Y = g_height + 70;
        uint16_t x = 40; // 40: the orign x-coordinate

        AddLable(x, LABLE_Y, "Auto");
        // 100: width and height
        AddImageView(GetRect(x, IMAGE_Y, 100, 100), IMAGE_RESIZEMODE_PATH, true, UIImageView::ImageResizeMode::NONE);

        x +=  STEP;
        AddLable(x, LABLE_Y, "Tiling");
        // 100: width and height
        AddImageView(GetRect(x, IMAGE_Y, 100, 100), IMAGE_RESIZEMODE_PATH, false, UIImageView::ImageResizeMode::NONE);

        x +=  STEP;
        AddLable(x, LABLE_Y, "Fill");
        // 100: width and height
        AddImageView(GetRect(x, IMAGE_Y, 100, 100), IMAGE_RESIZEMODE_PATH, false, UIImageView::ImageResizeMode::FILL);

        x +=  STEP;
        AddLable(x, LABLE_Y, "Contain");
        // 100: width; 50: height
        AddImageView(GetRect(x, IMAGE_Y, 100, 50), IMAGE_RESIZEMODE_PATH,
            false, UIImageView::ImageResizeMode::CONTAIN);

        x +=  STEP;
        AddLable(x, LABLE_Y, "Cover");
        // 100: width and height
        AddImageView(GetRect(x, IMAGE_Y, 100, 100), IMAGE_RESIZEMODE_PATH, false,
            UIImageView::ImageResizeMode::COVER);

        x +=  STEP;
        AddLable(x, LABLE_Y, "Center");
        // 100: width and height
        AddImageView(GetRect(x, IMAGE_Y, 100, 100), IMAGE_RESIZEMODE_PATH, false,
            UIImageView::ImageResizeMode::CENTER);

        x +=  STEP;
        AddLable(x, LABLE_Y, "Scale Down");
        // 100: width and height
        AddImageView(GetRect(x, IMAGE_Y, 100, 100), IMAGE_RESIZEMODE_PATH, false,
            UIImageView::ImageResizeMode::SCALE_DOWN);
        g_height += 200; // offset 200
    }
}
} // namespace OHOS
