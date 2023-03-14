/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_qrcode.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view.h"
#include "ui_test.h"

#define IMAGE_DIR "/data/img/"
#define GIF_PATH1 (IMAGE_DIR "01.gif")
#define GIF_PATH2 (IMAGE_DIR "02.gif")
#define PNG_PATH (IMAGE_DIR "03.png")
#define JPG_PATH (IMAGE_DIR "04.jpg")

using namespace OHOS;

class ImageDemo : public UIView::OnClickListener
{
public:
    static ImageDemo *GetInstance()
    {
        static ImageDemo inst;
        return &inst;
    }

    void SetUp();
    UIView *GetView();

private:
    ImageDemo() {}
    ~ImageDemo();

    bool OnClick(UIView &view, const ClickEvent &event) override;

    void CreateImageSwitch();

    void SetUpButton(UILabelButton *btn, const char *title);

    UIScrollView *container_ = nullptr;

    UIImageView *gifImageView_ = nullptr;
    UILabelButton *gifToGif_ = nullptr;
    UILabelButton *gifToJpeg_ = nullptr;
    UILabelButton *gifToPng_ = nullptr;

    int16_t g_height = 0;
};

static void DeleteChildren(UIView *view)
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView *tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup() && (tempView->GetViewType() != UI_DIGITAL_CLOCK)) {
            DeleteChildren(static_cast<UIViewGroup *>(tempView)->GetChildrenHead());
        }
        if (tempView->GetParent()) {
            static_cast<UIViewGroup *>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
    }
}

void ImageDemo::SetUp()
{
    g_height = 0;
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(25, 25, Screen::GetInstance().GetWidth() - 50, Screen::GetInstance().GetHeight() - 50);
        container_->SetHorizontalScrollState(false);
    }
}

ImageDemo::~ImageDemo()
{
    DeleteChildren(container_);
    container_ = nullptr;
    gifImageView_ = nullptr;
    gifToGif_ = nullptr;
    gifToJpeg_ = nullptr;
    gifToPng_ = nullptr;
}

UIView *ImageDemo::GetView()
{
    CreateImageSwitch();
    return container_;
}

void ImageDemo::CreateImageSwitch()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel *label = new UILabel();
    container_->Add(label);
    label->SetPosition(100, g_height, 200, 29);
    label->SetText("不同类型图片切换");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 20);
    g_height += 30;

    gifImageView_ = new UIImageView();
    gifImageView_->SetPosition(48, g_height);
    gifImageView_->SetSrc(GIF_PATH1);
    gifImageView_->Resize(100, 100);
    // gifImageView_->SetAutoEnable(false);
    container_->Add(gifImageView_);
    g_height += 150;

    gifToGif_ = new UILabelButton();
    SetUpButton(gifToGif_, "切换GIF");
    gifToGif_->SetPosition(48, g_height + 10);
    gifToJpeg_ = new UILabelButton();
    SetUpButton(gifToJpeg_, "切换JPG");
    gifToJpeg_->SetPosition(48 + 120, g_height + 10);
    gifToPng_ = new UILabelButton();
    SetUpButton(gifToPng_, "切换PNG");
    gifToPng_->SetPosition(48 + 240, g_height + 10);
}

void ImageDemo::SetUpButton(UILabelButton *btn, const char *title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->Resize(100, 50);
    btn->SetText(title);
    btn->SetOnClickListener(this);
}

bool ImageDemo::OnClick(UIView &view, const ClickEvent &event)
{
    if (gifImageView_ == nullptr) {
        return false;
    }
    if (&view == gifToGif_) {
        gifImageView_->SetSrc(GIF_PATH2);
    } else if (&view == gifToJpeg_) {
        gifImageView_->SetSrc(JPG_PATH);
    } else if (&view == gifToPng_) {
        gifImageView_->SetSrc(PNG_PATH);
    }
    return true;
}

class QrcodeDemo
{
public:
    static QrcodeDemo *GetInstance()
    {
        static QrcodeDemo inst;
        return &inst;
    }

    void SetUp();
    UIView *GetView();

private:
    QrcodeDemo() {}
    ~QrcodeDemo()
    {
        DeleteChildren(container_);
        container_ = nullptr;
    }

    UIScrollView *container_ = nullptr;
};

void QrcodeDemo::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(25, 25, Screen::GetInstance().GetWidth() - 50, Screen::GetInstance().GetHeight() - 50);
        container_->SetHorizontalScrollState(false);
    }
}

UIView *QrcodeDemo::GetView()
{
    if (container_ == nullptr) {
        return nullptr;
    }

    UILabel *titleLabel = new UILabel();
    titleLabel->SetPosition(100, 100, 300, 30);
    titleLabel->SetText("qrcode");

    UIQrcode *qrcode = new UIQrcode();
    qrcode->SetPosition(100, 150, 200, 200);
    const char *str = "鸿蒙轻量级GUI";
    qrcode->SetQrcodeInfo(str);

    container_->Add(qrcode);
    container_->Add(titleLabel);
    return container_;
}

void ImageDemoStart()
{
    RootView *rootView_ = RootView::GetInstance();
    rootView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    ImageDemo *view = ImageDemo::GetInstance();
    // QrcodeDemo *view = QrcodeDemo::GetInstance();
    view->SetUp();
    rootView_->Add(view->GetView());
    rootView_->Invalidate();
}
