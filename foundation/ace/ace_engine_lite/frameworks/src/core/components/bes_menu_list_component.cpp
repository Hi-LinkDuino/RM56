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
#include "bes_menu_list_component.h"
#include "ace_log.h"
#include "handler.h"
#include "key_parser.h"
#include "keys.h"



namespace OHOS {
namespace ACELite {

BesMenuListComponent::BesMenuListComponent(jerry_value_t options,
                                               jerry_value_t children,
                                               AppStyleManager *styleManager)
    : Component(options, children, styleManager),
    list_(nullptr), framesSize_(0), itemClickListener_(nullptr), ListType_(false)
{
    SetComponentName(K_BES_LIST_MENU_VIEW);
}

bool BesMenuListComponent::CreateNativeViews()
{
    list_ = new UIList();
    if (list_ == nullptr) {
        return false;
    }
    list_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    list_->SetThrowDrag(true);
    list_->SetReboundSize(50);
    list_->SetAdapter(&adapter_);
    return true;
}

void BesMenuListComponent::ReleaseNativeViews()
{
    ReleaseFrame();
    if(list_ !=nullptr){
        ACE_DELETE(list_);
        list_ = nullptr;
    }
    if(itemClickListener_ != nullptr){
        ACE_DELETE(itemClickListener_);
        itemClickListener_ = nullptr;
    }
}

inline UIView *BesMenuListComponent::GetComponentRootView() const
{
    return list_;
}

void BesMenuListComponent::ReleaseFrame()
{
    if ((frames_ == nullptr) || (framesSize_ <= 0)) {
        return;
    }

    for (uint8_t idx = 0; idx < framesSize_; ++idx) {
        MenuItemInfo frame = frames_[idx];
        if (frame.imagePath != nullptr) {
            ace_free(const_cast<char *>(frame.path));
            frame.imagePath = nullptr;
        }
    }

    ACE_FREE(frames_);
}

bool BesMenuListComponent::ParseToFrames(jerry_value_t value)
{
    ReleaseFrame();

    if (!jerry_value_is_array(value)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "images is not an array value.");
        return false;
    }

    framesSize_ = jerry_get_array_length(value);
    if (framesSize_ == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "images is empty.");
        return false;
    }

    frames_ = static_cast<MenuItemInfo *>(ace_malloc(sizeof(MenuItemInfo) * framesSize_));
    if (frames_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to set images cause by out of memory.");
        return false;
    }
    if(framesSize_ <= 0)
    {
        HILOG_ERROR(HILOG_MODULE_ACE, "image num error.");
        return false;
    }

    for (uint8_t idx = 0; idx < framesSize_; ++idx) {
        jerry_value_t image = jerry_get_property_by_index(value, idx);
        if (jerry_value_is_undefined(image) || !jerry_value_is_object(image)) {
            HILOG_WARN(HILOG_MODULE_ACE, "the %{public}d frame is null or not an object.", idx);
            continue;
        }

        jerry_value_t src = jerryx_get_property_str(image, ATTR_SRC);
        jerry_value_t title = jerryx_get_property_str(image, ATTR_TITLE);
        if (jerry_value_is_undefined(src) || jerry_value_is_undefined(title)) {
            jerry_release_value(image);
            HILOG_WARN(HILOG_MODULE_ACE, "the src of %{public}d frame is null.", src);
            continue;
        }

        MenuItemInfo frame = {{0}};
        frame.imagePath = ParseImageSrc(src);
        frame.path = const_cast<char *>(ParseImageSrc(src));
        frame.textInfo = MallocStringOf(title);

        jerry_release_value(src);
        jerry_release_value(title);
        frame.imageType = IMG_SRC_FILE_PATH;
        jerry_release_value(image);
        frames_[idx] = frame;
    }

    return true;
}

void BesMenuListComponent::ParseToList(jerry_value_t value)
{
    ParseToFrames(value);

    adapter_.SetFrames(frames_);
    if(itemClickListener_ != nullptr)
    {
        itemClickListener_->SetMenu(list_);
        adapter_.SetItemClickListener(itemClickListener_);
    }
    if(ListType_) {
        adapter_.SetListType(ListType_);
        adapter_.SetImageSrc(src_);
    }
    if(framesSize_ > 0)
        adapter_.SetCount(framesSize_);
}

bool BesMenuListComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_IMAGES:{
            ParseToList(attrValue);
            return true;
        }
        case K_SRC: {
            src_ = const_cast<char *>(ParseImageSrc(attrValue));
            ListType_ = true;
            return true;
        }
        default:
            return false;
    }
}

bool BesMenuListComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
                                                   jerry_value_t funcValue,
                                                   bool isStopPropagation)
{
    if (eventTypeId == K_CLICK) {
        itemClickListener_ = new BesMenuListComponent::ListItemClickListener(GetViewModel(),funcValue,isStopPropagation);
        if (itemClickListener_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Create itemClickListener error.");
            return false;
        }
        itemClickListener_->SetMenu(list_);
        adapter_.SetItemClickListener(itemClickListener_);
        return true;
    }
    return false;
}

void BesMenuListComponent::ListItemClickListener::OnItemClick(int16_t index, const ClickEvent& event)
{
    if (jerry_value_is_undefined(fn_)) {
        return;
    }
    if(index == -1){
        return;
    }

    JSValue arg = EventUtil::CreatePersonalEvent(*beslist_,event,index);
    EventUtil::InvokeCallback(vm_, fn_, arg, this);

}

void BesMenuListComponent::Invalidate()
{
    Component* parent_ = const_cast<Component*>(GetParent());
    if(parent_!=nullptr){
        list_->RefreshList();
        parent_->Invalidate();
    }
}

} // namespace ACELite
} // namespace OHOS