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

#include "components/ui_time_picker.h"
#include <cstdio>
#include <ctime>
#include "draw/draw_rect.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"
#include "themes/theme_manager.h"

namespace OHOS {
UITimePicker::UITimePicker()
    : selectedValue_{0},
      selectedHour_{0},
      selectedMinute_{0},
      selectedSecond_{0},
      secVisible_(false),
      loopState_{false},
      pickerWidth_(0),
      itemsHeight_(0),
      xPos_(0),
      backgroundFontSize_(0),
      highlightFontSize_(0),
      backgroundFontName_(nullptr),
      highlightFontName_(nullptr),
      pickerListener_(this),
      timePickerListener_(nullptr)
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        style_ = &(theme->GetPickerBackgroundStyle());
    } else {
        style_ = &(StyleDefault::GetPickerBackgroundStyle());
    }
    backgroundFontId_ = style_->font_;
    backgroundColor_ = style_->textColor_;
    if (theme != nullptr) {
        style_ = &(theme->GetPickerHighlightStyle());
    } else {
        style_ = &(StyleDefault::GetPickerHighlightStyle());
    }
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
    highlightFontId_ = style_->font_;
    highlightColor_ = style_->textColor_;

    hourPicker_ = nullptr;
    minutePicker_ = nullptr;
    secondPicker_ = nullptr;
}

UITimePicker::~UITimePicker()
{
    DeInitTimePicker();
    if (backgroundFontName_ != nullptr) {
        UIFree(backgroundFontName_);
        backgroundFontName_ = nullptr;
    }

    if (highlightFontName_ != nullptr) {
        UIFree(highlightFontName_);
        highlightFontName_ = nullptr;
    }
}

void UITimePicker::InitTimePicker()
{
    xPos_ = 0;
    if (secVisible_) {
        pickerWidth_ = GetWidth() / SEC_VISIBLE_COUNT;
        InitPicker(hourPicker_, TIME_START, HOUR_END);
        xPos_ = pickerWidth_;
        InitPicker(minutePicker_, TIME_START, MIN_END);
        xPos_ *= (SEC_VISIBLE_COUNT - 1);
        InitPicker(secondPicker_, TIME_START, SEC_END);
        if (secondPicker_ != nullptr) {
            secondPicker_->SetLoopState(loopState_[PICKER_SEC]);
        }
    } else {
        pickerWidth_ = GetWidth() / SEC_INVISIBLE_COUNT;
        InitPicker(hourPicker_, TIME_START, HOUR_END);
        xPos_ = pickerWidth_;
        InitPicker(minutePicker_, TIME_START, MIN_END);
    }
    if (hourPicker_ != nullptr) {
        hourPicker_->SetLoopState(loopState_[PICKER_HOUR]);
    }
    if (minutePicker_ != nullptr) {
        minutePicker_->SetLoopState(loopState_[PICKER_MIN]);
    }

    RefreshSelected(selectedValue_);
}

void UITimePicker::DeInitTimePicker()
{
    DeInitPicker(secondPicker_);
    DeInitPicker(minutePicker_);
    DeInitPicker(hourPicker_);
}

void UITimePicker::RefreshTimePicker()
{
    DeInitTimePicker();
    InitTimePicker();
}

void UITimePicker::InitPicker(UIPicker*& picker, int16_t start, int16_t end)
{
    picker = new UIPicker();
    if (picker == nullptr) {
        GRAPHIC_LOGE("new UIPicker fail");
        return;
    }
    picker->SetPosition(xPos_, 0, pickerWidth_, GetHeight());
    picker->SetItemHeight(itemsHeight_);
    picker->SetFontId(backgroundFontId_, highlightFontId_);
    if ((backgroundFontName_ == nullptr) || (highlightFontName_ == nullptr)) {
        picker->SetFontId(backgroundFontId_, highlightFontId_);
    } else {
        picker->SetBackgroundFont(backgroundFontName_, backgroundFontSize_);
        picker->SetHighlightFont(highlightFontName_, highlightFontSize_);
    }
    picker->SetTextColor(backgroundColor_, highlightColor_);
    picker->SetValues(start, end);
    picker->RegisterSelectedListener(&pickerListener_);
    Add(picker);

#if ENABLE_ROTATE_INPUT
    if (end == HOUR_END) {
        picker->GetChildrenHead()->SetViewId(HOUR_LIST_NAME);
    } else if (end == MIN_END && secondPicker_ == nullptr) {
        picker->GetChildrenHead()->SetViewId(MIN_LIST_NAME);
    } else if (end == SEC_END) {
        picker->GetChildrenHead()->SetViewId(SEC_LIST_NAME);
    }
#endif
}

void UITimePicker::DeInitPicker(UIPicker*& picker)
{
    if (picker != nullptr) {
        Remove(picker);
        picker->ClearValues();
        delete picker;
        picker = nullptr;
    }
}

void UITimePicker::TimeSelectedCallback()
{
    uint16_t hourSelect = hourPicker_->GetSelected();
    uint16_t minSelect = minutePicker_->GetSelected();
    GetValueByIndex(selectedHour_, BUF_SIZE, hourSelect, TIME_START, HOUR_END);
    GetValueByIndex(selectedMinute_, BUF_SIZE, minSelect, TIME_START, MIN_END);

    if (memset_s(selectedValue_, SELECTED_VALUE_SIZE, 0, SELECTED_VALUE_SIZE) != EOK) {
        return;
    }

    if (secVisible_) {
        uint16_t secSelect = secondPicker_->GetSelected();
        GetValueByIndex(selectedSecond_, BUF_SIZE, secSelect, TIME_START, SEC_END);
        if (sprintf_s(selectedValue_, SELECTED_VALUE_SIZE, "%s:%s:%s",
            selectedHour_, selectedMinute_, selectedSecond_) < 0) {
            return;
        }
    } else {
        if (sprintf_s(selectedValue_, SELECTED_VALUE_SIZE, "%s:%s", selectedHour_, selectedMinute_) < 0) {
            return;
        }
    }

    if (timePickerListener_ != nullptr) {
        timePickerListener_->OnTimePickerStoped(*this);
    }
}

void UITimePicker::GetValueByIndex(char* value, uint8_t len, uint16_t index, int16_t start, int16_t end)
{
    if ((value != nullptr) && (index < end - start + 1)) {
        if (sprintf_s(value, len, "%02u", index) < 0) {
            return;
        }
    }
}

bool UITimePicker::SetSelected(const char* value)
{
    if (value == nullptr) {
        return false;
    }

    if (memset_s(selectedValue_, SELECTED_VALUE_SIZE, 0, SELECTED_VALUE_SIZE) != EOK) {
        return false;
    }

    if (strcpy_s(selectedValue_, SELECTED_VALUE_SIZE, value) != EOK) {
        return false;
    }
    if (secVisible_) {
        if (sscanf_s(value, "%[^:]%*c%[^:]%*c%[^\n]", selectedHour_, BUF_SIZE,
                     selectedMinute_, BUF_SIZE, selectedSecond_, BUF_SIZE) < 3) { // 3: three variables
            return false;
        }
    } else {
        if (sscanf_s(value, "%[^:]%*c%[^\n]", selectedHour_, BUF_SIZE,
                     selectedMinute_, BUF_SIZE) < 2) { // 2: two variables
            return false;
        }
    }
    return RefreshSelected(selectedValue_);
}

bool UITimePicker::RefreshSelected(const char* value)
{
    uint32_t hourSelect;
    uint32_t minSelect;

    if (value == nullptr) {
        return false;
    }

    if (secVisible_) {
        uint32_t secSelect;
        // 3: three variables
        if (sscanf_s(value, "%u:%u:%u", &hourSelect, &minSelect, &secSelect) < 3) {
            return false;
        }
        secondPicker_->SetSelected(secSelect);
    } else {
        if (sscanf_s(value, "%u:%u", &hourSelect, &minSelect) < 2) { // 2: two variables
            return false;
        }
    }

    hourPicker_->SetSelected(hourSelect);
    minutePicker_->SetSelected(minSelect);
    return true;
}

void UITimePicker::SetItemHeight(int16_t height)
{
    itemsHeight_ = height;
    RefreshTimePicker();
}

void UITimePicker::EnableSecond(bool state)
{
    secVisible_ = state;
    RefreshTimePicker();
}

void UITimePicker::SetTextStyle(uint8_t backgroundFontId, uint8_t highlightFontId,
    ColorType backgroundColor, ColorType highlightColor)
{
    highlightFontId_ = highlightFontId;
    if (highlightFontName_ != nullptr) {
        UIFree(highlightFontName_);
        highlightFontName_ = nullptr;
    }

    backgroundFontId_ = backgroundFontId;
    if (backgroundFontName_ != nullptr) {
        UIFree(backgroundFontName_);
        backgroundFontName_ = nullptr;
    }

    highlightColor_ = highlightColor;
    backgroundColor_ = backgroundColor;
    RefreshTimePicker();
}

void UITimePicker::SetTextColor(ColorType backgroundColor, ColorType highlightColor)
{
    backgroundColor_ = backgroundColor;
    highlightColor_ = highlightColor;
    RefreshTimePicker();
}

void UITimePicker::SetBackgroundFont(const char* name, uint8_t size)
{
    Text::SetFont(name, size, backgroundFontName_, backgroundFontSize_);
    RefreshTimePicker();
}

void UITimePicker::SetHighlightFont(const char* name, uint8_t size)
{
    Text::SetFont(name, size, highlightFontName_, highlightFontSize_);
    RefreshTimePicker();
}

void UITimePicker::SetWidth(int16_t width)
{
    UIView::SetWidth(width);
    RefreshTimePicker();
}

void UITimePicker::SetHeight(int16_t height)
{
    UIView::SetHeight(height);
    RefreshTimePicker();
}

void UITimePicker::SetLoopState(const uint8_t pickerType, bool state)
{
    switch (pickerType) {
        case PICKER_HOUR:
            loopState_[PICKER_HOUR] = state;
            break;
        case PICKER_MIN:
            loopState_[PICKER_MIN] = state;
            break;
        case PICKER_SEC:
            loopState_[PICKER_SEC] = state;
            break;
        default:
            return;
    }
    RefreshTimePicker();
}
}
