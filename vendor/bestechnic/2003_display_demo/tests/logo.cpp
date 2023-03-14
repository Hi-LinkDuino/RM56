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
// #include "components/ui_canvas.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "graphic_config.h"
#include "hal_tick.h"
#include "hilog/log.h"
#include <stdlib.h>

#undef LOG_TAG
#define LOG_TAG "LogoView"

using namespace OHOS;

class LogoView : public UIView::OnClickListener
{
public:
    static LogoView *GetInstance()
    {
        static LogoView instance;
        return &instance;
    }
    void Start();

private:
    RootView *rootView_ = nullptr;
    UILabelButton *btn_ = nullptr;
    UILabel *label_ = nullptr;

    LogoView() { }
    ~LogoView()
    {
        if (btn_ != nullptr) {
            delete btn_;
            btn_ = nullptr;
        }
        if (label_ != nullptr) {
            delete label_;
            label_ = nullptr;
        }
    }

};

void LogoView::Start()
{
    if (rootView_ != nullptr) {
        return;
    }
    rootView_ = RootView::GetInstance();
#if defined(LCD_USE_SMALL_SIZE_W) || defined(LCD_USE_SMALL_SIZE_H)
    uint16_t sw = Screen::GetInstance().GetWidth();
    uint16_t sh = Screen::GetInstance().GetHeight();
    uint16_t w = sw;
    uint16_t h = sh;
#if defined(LCD_USE_SMALL_SIZE_W)
    w = LCD_USE_SMALL_SIZE_W;
#endif
#if defined(LCD_USE_SMALL_SIZE_H)
    h = LCD_USE_SMALL_SIZE_H;
#endif
    rootView_->SetPosition((sw-w)/2, (sh-h)/2, w, h);
    rootView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
#else
    rootView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
#endif
    GRAPHIC_LOGD("alvin: LogoView::Start(), rootView=%p, %d-%d", rootView_, rootView_->GetWidth(), rootView_->GetHeight());
    label_ = new UILabel();
#if defined(LCD_USE_SMALL_SIZE_W) || defined(LCD_USE_SMALL_SIZE_H)
    label_->SetPosition(0, 0, 100, 64);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
#else
    label_->SetPosition(150, 50, 150, 64);
#endif
    //label_->SetText("BESFD\n良友通信");
    rootView_->Add(label_);

    btn_ = new UILabelButton();
#if defined(LCD_USE_SMALL_SIZE_W) || defined(LCD_USE_SMALL_SIZE_H)
    btn_->SetPosition(0, 200, 100, 64);
#else
    btn_->SetPosition(150, 200, 150, 64);
#endif
    //btn_->SetText("HELLO!");
    rootView_->Add(btn_);

    rootView_->Invalidate();
}

void LogoViewStart(void)
{
    LogoView::GetInstance()->Start();
}
