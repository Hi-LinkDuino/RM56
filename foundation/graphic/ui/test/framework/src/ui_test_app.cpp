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

#include "ui_test_app.h"

#include "auto_test_app.h"
#include "compare_tools.h"
#include "test_resource_config.h"
#include "ui_auto_test.h"
#include "ui_test.h"
#include "ui_test_group.h"

namespace OHOS {
#ifdef _WIN32
DWORD AutoTestThread(LPVOID)
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
void* AutoTestThread(void*)
#endif // _WIN32
{
#ifdef _WIN32
    const char logPath[] = ".\\auto_test_log.txt";
    CompareTools::SetLogPath(logPath, sizeof(logPath));
#else
    const char logPath[] = "./auto_test_log.txt";
    CompareTools::SetLogPath(logPath, sizeof(logPath));
#endif
    // 装载用例
    UIAutoTest::SetUpTestCase();
    AutoTestApp::GetInstance()->Start();
    AutoTestCaseGroup::TearDownTestCase();
    CompareTools::UnsetLogPath();
}

UITestApp* UITestApp::GetInstance()
{
    static UITestApp instance;
    return &instance;
}

void UITestApp::Start()
{
    if (rootView_ == nullptr) {
        rootView_ = RootView::GetInstance();
        rootView_->SetPosition(0, 0);
        rootView_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        if (mainMenu_ == nullptr) {
            mainMenu_ = new UIViewGroup();
            mainMenu_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
            rootView_->Add(mainMenu_);
            rootView_->Invalidate();
        }
    }
    Init();
}

class BtnOnClickOnAutoTestListener : public UIView::OnClickListener {
public:
    BtnOnClickOnAutoTestListener() {}
    virtual ~BtnOnClickOnAutoTestListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        ThreadCreate(AutoTestThread, nullptr, nullptr);
    }
};

void UITestApp::Init()
{
    InitBackBtn();
    InitTestLabel();
    InitMainMenu();
}

void UITestApp::InitMainMenu()
{
    if (mainMenu_ != nullptr) {
        if (testLabel_ == nullptr) {
            testLabel_ = new UILabel();
            testLabel_->Resize(300, BACK_BUTTON_HEIGHT); // 300: test Label width;
            testLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
            testLabel_->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 0);
            testLabel_->SetText("Test Demo");
            testLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: means font size
            mainMenu_->Add(testLabel_);
        }
        if (autoTestBtn_ == nullptr) {
            autoTestBtn_ = new UILabelButton();
            autoTestBtn_->Resize(163, 64); // 163: button width; 64: button height
            autoTestBtn_->SetPosition(Screen::GetInstance().GetWidth() - autoTestBtn_->GetWidth(), 0);
            autoTestBtn_->SetText("自动测试");
            auto listern = new BtnOnClickOnAutoTestListener();
            autoTestBtn_->SetOnClickListener(listern);
            autoTestBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 24); // 24: means font size
            autoTestBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::RELEASED);
            autoTestBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::PRESSED);
            autoTestBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::INACTIVE);
            autoTestBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::RELEASED);
            autoTestBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::PRESSED);
            autoTestBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::INACTIVE);
            mainMenu_->Add(autoTestBtn_);
        }
        if ((mainList_ == nullptr) && (adapter_ == nullptr)) {
            uint8_t deltaHeight = 60; // 60: UIList height(64) - first button border width(4)
            constexpr uint8_t margin = 24; // 24: x-coordinate
            mainList_ = new UIList(UIList::VERTICAL);
            mainList_->SetPosition(margin, deltaHeight);
            mainList_->Resize(Screen::GetInstance().GetWidth() - margin,
                Screen::GetInstance().GetHeight() - deltaHeight);
            mainList_->SetThrowDrag(true);
            mainList_->SetReboundSize(50); // 50: rebound size
            mainList_->SetViewId(UI_TEST_MAIN_LIST_ID);
            mainList_->SetYScrollBarVisible(true);
            if (backBtn_ == nullptr) {
                InitBackBtn();
            }
            if (testCaseLabel_ == nullptr) {
                InitTestLabel();
            }
            adapter_ = new TestCaseListAdapter(mainMenu_, backBtn_, testCaseLabel_);
            UITestGroup::SetUpTestCase();
            mainList_->SetAdapter(adapter_);
            mainMenu_->Add(mainList_);
        }
    }
}

void UITestApp::InitBackBtn()
{
    if (backBtn_ == nullptr) {
        backBtn_ = new UILabelButton();
        backBtn_->SetPosition(0, 0);
        backBtn_->Resize(163, 64); // 163: button width; 64: button height
        backBtn_->SetText("Back");
        backBtn_->SetViewId(UI_TEST_BACK_BUTTON_ID);
        backBtn_->SetLabelPosition(72, 0);                   // 72: button label x-coordinate
        backBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 24); // 24: means font size
        backBtn_->SetImageSrc(TEST_BACK_LEFT_ARROW, TEST_BACK_LEFT_ARROW);
        // 27: button Image x-coordinate; 18: half px of image height
        backBtn_->SetImagePosition(27, BACK_BUTTON_HEIGHT / 2 - 18);
        backBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::RELEASED);
        backBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::PRESSED);
        backBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::INACTIVE);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::RELEASED);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::PRESSED);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::INACTIVE);
    }
}

void UITestApp::InitTestLabel()
{
    if (testCaseLabel_ == nullptr) {
        testCaseLabel_ = new UILabel();
        testCaseLabel_->Resize(Screen::GetInstance().GetWidth(), BACK_BUTTON_HEIGHT);
        testCaseLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        testCaseLabel_->SetText("Test Case Name");
        testCaseLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: means font size
    }
}

UITestApp::~UITestApp()
{
    if (adapter_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
    if (mainList_ != nullptr) {
        delete mainList_;
        mainList_ = nullptr;
    }
    if (backBtn_ != nullptr) {
        delete backBtn_;
        backBtn_ = nullptr;
    }
    if (rootView_ != nullptr) {
        rootView_ = nullptr;
    }
    if (autoTestBtn_ != nullptr) {
        auto listener = autoTestBtn_->GetOnClickListener();
        if (listener != nullptr) {
            delete listener;
            autoTestBtn_->SetOnClickListener(nullptr);
        }
        delete autoTestBtn_;
        autoTestBtn_ = nullptr;
    }
    if (mainMenu_ != nullptr) {
        delete mainMenu_;
        mainMenu_ = nullptr;
    }
}
} // namespace OHOS
