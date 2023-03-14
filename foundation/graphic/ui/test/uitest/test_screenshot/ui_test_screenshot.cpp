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

#include "ui_test_screenshot.h"
#if ENABLE_DEBUG
#include "common/screen.h"
#include "components/ui_image_view.h"
#include "imgdecode/cache_manager.h"
#include "test_resource_config.h"

namespace OHOS {
void UITestScreenshot::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    }
}

void UITestScreenshot::TearDown()
{
    if (clickLeftListener_ != nullptr) {
        delete clickLeftListener_;
        clickLeftListener_ = nullptr;
    }
    if (clickRightListener_ != nullptr) {
        delete clickRightListener_;
        clickRightListener_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestScreenshot::GetTestView()
{
    UIKit_SCREENSHOT_TEST_001();
    return container_;
}

class TestScreenshotOnClickListener : public UIView::OnClickListener {
public:
    explicit TestScreenshotOnClickListener(UIView* uiView) : uiView_(uiView) {}
    ~TestScreenshotOnClickListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        UIScreenshot::GetInstance()->ScreenshotToFile("/storage/data/screenshot.bin");
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
};

class TestSetScreenshotOnClickListener : public UIView::OnClickListener {
public:
    explicit TestSetScreenshotOnClickListener(UIView* uiView) : uiView_(uiView) {}
    ~TestSetScreenshotOnClickListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        static_cast<UIImageView*>(uiView_)->SetSrc("/storage/data/screenshot.bin");
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
};

void UITestScreenshot::UIKit_SCREENSHOT_TEST_001()
{
    const int16_t titleLableHeight = 29;
    const int16_t labelButtonWidth = 80;
    const int16_t labelButtonHeight = 40;
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("截屏功能测试");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE,
        Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE, titleLableHeight);
    UILabelButton* labelButton = SetUpButton("截屏");
    container_->Add(labelButton);
    labelButton->SetPosition(210, 50); // 210: position x; 50: position y
    labelButton->SetWidth(labelButtonWidth);
    labelButton->SetHeight(labelButtonHeight);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    if (clickLeftListener_ == nullptr) {
        clickLeftListener_ = static_cast<UIView::OnClickListener*>(
            new TestScreenshotOnClickListener(reinterpret_cast<UIView*>(labelButton)));
    }
    labelButton->SetOnClickListener(clickLeftListener_);
    UILabel* label2 = GetTitleLabel("显示截屏图片 ");
    container_->Add(label2);
    label2->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, 255,  // 255: y-coordinate
        Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE, titleLableHeight);
    UIImageView* imageView = new UIImageView();
    imageView->SetAutoEnable(false);
    imageView->SetPosition(0, 290); // 290: y-coordinate
    imageView->SetWidth(Screen::GetInstance().GetWidth());
    imageView->SetHeight(Screen::GetInstance().GetHeight() + 100); // 100: increase height
    imageView->SetSrc(BLUE_RGB565_IMAGE_PATH);
    Style style;
    CacheEntry entry;
    RetCode ret = CacheManager::GetInstance().Open(imageView->GetPath(), style, entry);
    if (ret != RetCode::OK) {
        return;
    }
    container_->Add(imageView);
    UILabelButton* labelButton2 = SetUpButton("显示");
    container_->Add(labelButton2);
    labelButton2->SetPosition(300, 50); // 300: position x; 50: position y
    labelButton2->SetWidth(labelButtonWidth);
    labelButton2->SetHeight(labelButtonHeight);
    if (clickRightListener_ == nullptr) {
        clickRightListener_ = static_cast<UIView::OnClickListener*>(
            new TestSetScreenshotOnClickListener(static_cast<UIView*>(imageView)));
    }
    labelButton2->SetOnClickListener(clickRightListener_);
}
} // namespace OHOS
#endif
