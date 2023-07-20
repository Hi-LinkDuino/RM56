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

#include "menu_list_adapter.h"
#include <stdlib.h>
#include "common/screen.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "gfx_utils/list.h"
#include "components/ui_bes_menu.h"
#include "js_fwk_common.h"

namespace OHOS {
namespace ACELite {

namespace {
const uint16_t MENU_LIST_HEIGHT = 128;
const uint16_t IMAGE_HIGHT = 96;
const uint16_t MENU_IMAGE_HIGHT = 78;
const uint16_t STYLE_BORDER_WIDTH_VALUE = 4;
const uint16_t STYLE_BORDER_RADIUS_VALUE = 12;

const uint16_t MENU_LIST_TYPE_HEIGHT = 100;
constexpr uint16_t TEXT_DISTANCE_TO_LEFT_SIDE = 200;
constexpr uint16_t MENU_TEXT_DISTANCE_TO_LEFT_SIDE = 150;
constexpr uint16_t IMAGE_DISTANCE_TO_LEFT_SIDE = 48;
constexpr uint16_t MENU_IMAGE_DISTANCE_TO_LEFT_SIDE = 380;
constexpr uint16_t MENU_LIST_LABLE_POSTION_X = 300;
const uint32_t BUTTON_STYLE_BACKGROUND_COLOR_VALUE = 0x000000;
} // namespace

uint16_t MenuListAdapter::GetCount()
{
    return itemsCount_;
}

MenuListAdapter::~MenuListAdapter()
{
    if(views_.size() > 0) {
        std::vector<UIView*>::iterator it = views_.begin();
        for(; it != views_.end();it++){
            delete(*it);
            (*it)=nullptr;
        }
        views_.clear();
        std::vector<UIView*> views_;
        views_.swap(views_);
    }
}

bool MenuListAdapter::OnClick(UIView& view, const ClickEvent& event)
{
    if(itemClickListener_!=nullptr)
    {
        itemClickListener_->OnItemClick(view.GetViewIndex(), event);
    }
    return true;
}

int16_t MenuListAdapter::CalculateItemIndex(int16_t itemIndex)
{
    if(itemIndex < 0) {
        return FAILED;
    } else if(itemIndex < itemsCount_) {
        return itemIndex;
    }

    return FAILED;
}

UIView* MenuListAdapter::GetView(UIView* inView, int16_t index)
{
    UIViewGroup* item = nullptr;
    UILabelButton* button = nullptr;
    UIImageView* listimage = nullptr;
    if (inView == nullptr) {
        item = new UIViewGroup();
        button = new UILabelButton();
        button->SetViewId("button");
        item->Add(button);

        if(ListType_)
        {
            listimage = new UIImageView();
            listimage->SetViewId("listimage");
            item->Add(listimage);
        }
        if(item != nullptr){
            views_.push_back(item);
        }
        if(button != nullptr) {
           views_.push_back(button);
        }
        if(listimage != nullptr) {
            views_.push_back(listimage);
        }

        item->SetPosition(0, 0);
        item->SetHeight(GetVerticalResolution());
        item->SetWidth(GetHorizontalResolution());
        if(ListType_){
            item->Resize(Screen::GetInstance().GetWidth(), MENU_LIST_TYPE_HEIGHT);
        } else {
            item->Resize(Screen::GetInstance().GetWidth(), MENU_LIST_HEIGHT);
        }

        button->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::RELEASED);
        button->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::PRESSED);
        button->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::INACTIVE);
        button->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::RELEASED);
        button->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::PRESSED);
        button->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::INACTIVE);
        button->SetPosition(0, 0, GetHorizontalResolution() - 110, GetVerticalResolution());
        if(ListType_){
            button->Resize(Screen::GetInstance().GetWidth() - 110, MENU_LIST_TYPE_HEIGHT);
        } else {
            button->Resize(Screen::GetInstance().GetWidth() - 110, MENU_LIST_HEIGHT);
        }

        if(ListType_) {
            listimage->SetPosition((Screen::GetInstance().GetWidth() - 90), 0, GetHorizontalResolution() - 300, GetVerticalResolution());
            listimage->Resize(100, MENU_LIST_HEIGHT);
        }

    } else {
        item = static_cast<UIViewGroup*>(inView);
        button = (UILabelButton*)(item->GetChildById("button"));
        if(ListType_) {
            listimage = (UIImageView*)(item->GetChildById("listimage"));
        }
    }

    int16_t listItemsIndex = CalculateItemIndex(index);
    if (listItemsIndex < 0) {
        return nullptr;
    }

    item->SetOnClickListener(this);

    button->SetText(FramesPath_[index].textInfo);
    button->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 32); // 24: means font size

    button->SetViewIndex(index);
    button->SetAlign(TEXT_ALIGNMENT_LEFT);
    if(ListType_) {
        button->SetLabelPosition(MENU_TEXT_DISTANCE_TO_LEFT_SIDE, MENU_LIST_HEIGHT/2-40);// 24: means font size
        button->SetImageSrc(FramesPath_[index].imagePath, FramesPath_[index].imagePath);
        button->SetImagePosition(IMAGE_DISTANCE_TO_LEFT_SIDE, (MENU_LIST_HEIGHT - MENU_IMAGE_HIGHT) / 2);
    } else{
        button->SetLabelPosition(TEXT_DISTANCE_TO_LEFT_SIDE, MENU_LIST_HEIGHT/2-40);
        button->SetImageSrc(FramesPath_[index].imagePath, FramesPath_[index].imagePath);
        button->SetImagePosition(IMAGE_DISTANCE_TO_LEFT_SIDE, (MENU_LIST_HEIGHT - IMAGE_HIGHT) / 2);
    }

    if(ListType_) {
        listimage->SetSrc(imgPath_);
        listimage->SetPosition(MENU_IMAGE_DISTANCE_TO_LEFT_SIDE, MENU_LIST_HEIGHT/ 2);
    }

    button->SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);

    return item;
}

void MenuListAdapter::SetItemClickListener(BesItemClickListener *itemClickListener)
{
    this->itemClickListener_ = itemClickListener;
}

void MenuListAdapter::SetFrames(MenuItemInfo* path)
{
    FramesPath_ = path;
}

void MenuListAdapter::SetImageSrc(char* src)
{
    imgPath_ = src;
}

void MenuListAdapter::SetCount(uint16_t count)
{
    itemsCount_ = count;
}

void MenuListAdapter::SetListType(bool type)
{
    ListType_ = type;
}

bool MenuListAdapter::GetImgType()
{
    return ListType_;
}

int16_t MenuListAdapter::GetItemWidthWithMargin(int16_t index)
{
    // 2: two borders on both sides
    return Screen::GetInstance().GetWidth() -TEXT_DISTANCE_TO_LEFT_SIDE;
}

int16_t MenuListAdapter::GetItemHeightWithMargin(int16_t index)
{
    return MENU_LIST_HEIGHT; // 2: two borders on both sides
}

}
} // namespace OHOS
