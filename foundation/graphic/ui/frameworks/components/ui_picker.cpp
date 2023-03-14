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

#include "components/ui_picker.h"
#include "dock/vibrator_manager.h"
#include "draw/draw_line.h"
#include "draw/draw_rect.h"
#include "themes/theme_manager.h"

namespace OHOS {
PickerListScrollListener::PickerListScrollListener(UIPicker* picker, UIList* list)
    : listView_(list),
      pickerView_(picker),
      selectView_(nullptr),
      lastSelectView_(nullptr),
      selectIndex_(0),
      isInitted_(false) {}

void PickerListScrollListener::OnItemSelected(int16_t index, UIView* view)
{
    if (!isInitted_) {
        return;
    }

    if ((lastSelectView_ != nullptr) && (listView_ != nullptr) && (pickerView_ != nullptr) && (view != nullptr)) {
        lastSelectView_->SetStyle(STYLE_TEXT_COLOR, pickerView_->GetBackgroundTextColor().full);
        if (pickerView_->backgroundFontName_ == nullptr) {
            static_cast<UILabel*>(lastSelectView_)->SetFontId(pickerView_->backgroundFontId_);
        } else {
            static_cast<UILabel*>(lastSelectView_)
                ->SetFont(pickerView_->backgroundFontName_, pickerView_->backgroundFontSize_);
        }
        view->SetStyle(STYLE_TEXT_COLOR, pickerView_->GetHighlightTextColor().full);
        if (pickerView_->highlightFontName_ == nullptr) {
            static_cast<UILabel*>(view)->SetFontId(pickerView_->highlightFontId_);
        } else {
            static_cast<UILabel*>(view)->SetFont(pickerView_->highlightFontName_, pickerView_->highlightFontSize_);
        }
        lastSelectView_ = view;
        selectIndex_ = index;
        listView_->Invalidate();
    }
}

void PickerListScrollListener::OnScrollEnd(int16_t index, UIView* view)
{
    if ((view == nullptr) || (listView_ == nullptr) || (pickerView_ == nullptr)) {
        return;
    }

    if (lastSelectView_ != nullptr) {
        lastSelectView_->SetStyle(STYLE_TEXT_COLOR, pickerView_->GetBackgroundTextColor().full);
        if (pickerView_->backgroundFontName_ == nullptr) {
            static_cast<UILabel*>(lastSelectView_)->SetFontId(pickerView_->backgroundFontId_);
        } else {
            static_cast<UILabel*>(lastSelectView_)
                ->SetFont(pickerView_->backgroundFontName_, pickerView_->backgroundFontSize_);
        }
        lastSelectView_ = view;
    }

    view->SetStyle(STYLE_TEXT_COLOR, pickerView_->GetHighlightTextColor().full);
    if (pickerView_->highlightFontName_ == nullptr) {
        static_cast<UILabel*>(view)->SetFontId(pickerView_->highlightFontId_);
    } else {
        static_cast<UILabel*>(view)->SetFont(pickerView_->highlightFontName_, pickerView_->highlightFontSize_);
    }

    listView_->Invalidate();
    selectView_ = view;
    selectIndex_ = index;

    if (pickerView_->pickerListener_) {
        pickerView_->pickerListener_->OnPickerStoped(*pickerView_);
    }
}

UIPicker::UIPicker()
    : isWidthSet_(false),
      isHeightSet_(false),
      textAdapter_(nullptr),
      maxCount_(0),
      isScrollBlankSizeSet_(false),
      scrollBlankSize_(0),
      backgroundFontSize_(0),
      highlightFontSize_(0),
      backgroundFontName_(nullptr),
      highlightFontName_(nullptr),
      itemsWidth_(0),
      itemsHeight_(0),
      rangeValue_(nullptr),
      rangeValueCount_(0),
      startValue_(0),
      endValue_(0),
      isSetAdaptered_(false),
      pickerListener_(nullptr)
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        style_ = &(theme->GetPickerBackgroundStyle());
    } else {
        style_ = &(StyleDefault::GetPickerBackgroundStyle());
    }
    backgroundFontId_ = style_->font_;
    backgroundColor_ = style_->textColor_;
    direct_ = UITextLanguageDirect::TEXT_DIRECT_LTR;

    if (theme != nullptr) {
        style_ = &(theme->GetPickerHighlightStyle());
    } else {
        style_ = &(StyleDefault::GetPickerHighlightStyle());
    }
    highlightFontId_ = style_->font_;
    highlightColor_ = style_->textColor_;

    list_.SetThrowDrag(true);
    list_.SetStyle(StyleDefault::GetBackgroundTransparentStyle());
#if ENABLE_ROTATE_INPUT
    list_.rotateFactor_ = DEFAULT_PICKER_ROTATE_FACTOR;
    list_.rotateThrowthreshold_ = PICKERVIEW_ROTATE_THROW_THRESHOLD;
    list_.rotateAccCoefficient_ = PICKERVIEW_ROTATE_DISTANCE_COEFF;
#endif
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
    list_.SetLoopState(false);
    list_.EnableAutoAlign(true);
    listListener_ = new PickerListScrollListener(this, &list_);
    list_.SetScrollStateListener(listListener_);
    Add(&list_);
}

UIPicker::~UIPicker()
{
    ClearValues();
    Remove(&list_);
    if (listListener_ != nullptr) {
        delete listListener_;
        listListener_ = nullptr;
    }

    if (backgroundFontName_ != nullptr) {
        UIFree(backgroundFontName_);
        backgroundFontName_ = nullptr;
    }

    if (highlightFontName_ != nullptr) {
        UIFree(highlightFontName_);
        highlightFontName_ = nullptr;
    }

    if (textAdapter_ != nullptr) {
        delete textAdapter_;
        textAdapter_ = nullptr;
    }
}

bool UIPicker::SetValues(int16_t start, int16_t end)
{
    if (start > end) {
        return false;
    }

    startValue_ = start;
    endValue_ = end;
    return RefreshValues(start, end);
}

bool UIPicker::SetValues(const char* value[], uint16_t count)
{
    if (value == nullptr) {
        return false;
    }

    rangeValue_ = value;
    rangeValueCount_ = count;
    return RefreshValues(value, count);
}

void UIPicker::Refresh()
{
    if (rangeValue_) {
        RefreshValues(rangeValue_, rangeValueCount_);
    } else if ((startValue_ != 0) || (endValue_ != 0)) {
        RefreshValues(startValue_, endValue_);
    }
}

bool UIPicker::RefreshValues(int16_t start, int16_t end)
{
    if ((start == 0) && (end == 0)) {
        return false;
    }
    maxCount_ = end - start + 1;
    if (!isWidthSet_ || !isHeightSet_ || !itemsHeight_) {
        return false;
    }
    uint16_t userSelectIndex = listListener_->GetSelectIndex();
    ClearList();
    InitTextAdapter();
    textAdapter_->SetData(start, end);
    RefreshList();
    RefreshSelected(userSelectIndex);
    return true;
}

bool UIPicker::RefreshValues(const char* value[], uint16_t count)
{
    if (value == nullptr) {
        return false;
    }
    maxCount_ = count;
    if (!isWidthSet_ || !isHeightSet_ || !itemsHeight_) {
        return false;
    }
    uint16_t userSelectIndex = listListener_->GetSelectIndex();
    ClearList();
    for (uint16_t i = 0; i < count; i++) {
        dataList_.PushBack(value[i]);
    }
    InitTextAdapter();
    textAdapter_->SetData(&dataList_);
    RefreshList();
    RefreshSelected(userSelectIndex);

    return true;
}

void UIPicker::RefreshList()
{
    int16_t height = GetHeight();
    itemsWidth_ = GetWidth();
    textAdapter_->SetWidth(itemsWidth_);
    textAdapter_->SetHeight(itemsHeight_);
    textAdapter_->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    if (backgroundFontName_ == nullptr) {
        textAdapter_->SetFontId(backgroundFontId_);
    } else {
        textAdapter_->SetFont(backgroundFontName_, backgroundFontSize_);
    }
    textAdapter_->GetStyle().textColor_ = backgroundColor_;
    textAdapter_->SetDirect(direct_);
    list_.SetHeight(height);
    list_.SetWidth(itemsWidth_);
    list_.LayoutCenterOfParent();
    list_.SetSelectPosition(height / 2);                   // 2: half
    if (isScrollBlankSizeSet_) {
        list_.SetScrollBlankSize(scrollBlankSize_);
    } else {
        list_.SetScrollBlankSize((height - itemsHeight_) / 2); // 2: half
    }
    if (!isSetAdaptered_) {
        list_.SetAdapter(textAdapter_);
        isSetAdaptered_ = true;
    }

    list_.RefreshList();
    RefreshSelected(0);
}

void UIPicker::ClearValues()
{
    rangeValue_ = nullptr;
    rangeValueCount_ = 0;
    maxCount_ = 0;
    ClearList();
    ClearTextAdapter();
}

void UIPicker::ClearList()
{
    itemsWidth_ = 0;
    if (listListener_) {
        listListener_->SetSelectView(nullptr);
        listListener_->SetSelectIndex(0);
        listListener_->SetInitStatus(false);
    }
    dataList_.Clear();
}

void UIPicker::ClearTextAdapter()
{
    if (textAdapter_ != nullptr) {
        delete textAdapter_;
        textAdapter_ = nullptr;
    }
    list_.SetAdapter(textAdapter_);
    list_.RefreshList();
    isSetAdaptered_ = false;
}

bool UIPicker::SetSelected(uint16_t index)
{
    return RefreshSelected(index);
}

bool UIPicker::RefreshSelected(uint16_t index)
{
    if (maxCount_ <= index) {
        GRAPHIC_LOGW("Failed to refresh selected since index is beyond range!");
        return false;
    }
    if (itemsHeight_ && (list_.GetChildrenHead() != nullptr) && isWidthSet_ && isHeightSet_) {
        listListener_->SetInitStatus(false);
        // 2: half
        int16_t yOffset = (list_.GetHeight() - itemsHeight_) / 2 -
                          itemsHeight_ * (index - list_.GetChildrenHead()->GetViewIndex());
        list_.SetScrollStateListener(nullptr);
        list_.ScrollBy(yOffset - list_.GetChildrenHead()->GetY());
        list_.SetScrollStateListener(listListener_);
        listListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
        UIView* childView = static_cast<UIView*>(list_.GetChildrenHead());
        uint16_t lastSelectIndex = listListener_->GetSelectIndex();

        int16_t viewIndex;
        while (childView != nullptr) {
            viewIndex = childView->GetViewIndex();
            if (viewIndex == lastSelectIndex) {
                childView->SetStyle(STYLE_TEXT_COLOR, GetBackgroundTextColor().full);
                if (backgroundFontName_ == nullptr) {
                    static_cast<UILabel*>(childView)->SetFontId(backgroundFontId_);
                } else {
                    static_cast<UILabel*>(childView)->SetFont(backgroundFontName_, backgroundFontSize_);
                }
            }
            if (viewIndex == index) {
                childView->SetStyle(STYLE_TEXT_COLOR, GetHighlightTextColor().full);
                if (highlightFontName_ == nullptr) {
                    static_cast<UILabel*>(childView)->SetFontId(highlightFontId_);
                } else {
                    static_cast<UILabel*>(childView)->SetFont(highlightFontName_, highlightFontSize_);
                }
                listListener_->SetSelectView(childView);
                listListener_->SetInitStatus(true);
            }
            childView = childView->GetNextSibling();
        }
        listListener_->SetSelectIndex(index);
        list_.Invalidate();
        return true;
    }
    listListener_->SetSelectIndex(index);
    return false;
}

uint16_t UIPicker::GetSelected() const
{
    return listListener_->GetSelectIndex();
}

void UIPicker::SetFontId(uint8_t backgroundFontId, uint8_t highlightFontId)
{
    backgroundFontId_ = backgroundFontId;
    if (backgroundFontName_ != nullptr) {
        UIFree(backgroundFontName_);
        backgroundFontName_ = nullptr;
    }

    highlightFontId_ = highlightFontId;
    if (highlightFontName_ != nullptr) {
        UIFree(highlightFontName_);
        highlightFontName_ = nullptr;
    }

    Refresh();
}

void UIPicker::SetBackgroundFont(const char* name, uint8_t size)
{
    Text::SetFont(name, size, backgroundFontName_, backgroundFontSize_);
    Refresh();
}

void UIPicker::SetHighlightFont(const char* name, uint8_t size)
{
    Text::SetFont(name, size, highlightFontName_, highlightFontSize_);
    Refresh();
}

void UIPicker::SetTextColor(ColorType backgroundColor, ColorType highlightColor)
{
    backgroundColor_ = backgroundColor;
    highlightColor_ = highlightColor;
    Refresh();
}

void UIPicker::SetItemHeight(int16_t height)
{
    if (height > 0) {
        itemsHeight_ = height;
        Refresh();
    }
}

void UIPicker::SetWidth(int16_t width)
{
    if (width > 0) {
        UIView::SetWidth(width);
        isWidthSet_ = true;
        Refresh();
    }
}

void UIPicker::SetHeight(int16_t height)
{
    if (height > 0) {
        UIView::SetHeight(height);
        isHeightSet_ = true;
        Refresh();
    }
}

void UIPicker::SetLoopState(bool state)
{
    list_.SetLoopState(state);
    Refresh();
}

void UIPicker::SetDirect(UITextLanguageDirect direct)
{
    direct_ = direct;
    Refresh();
}

void UIPicker::SetTextFormatter(TextFormatter* formatter)
{
    InitTextAdapter();
    textAdapter_->SetTextFormatter(formatter);
    Refresh();
}
} // namespace OHOS
