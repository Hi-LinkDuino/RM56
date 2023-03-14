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
#include "list_component.h"
#include "ace_log.h"
#include "component.h"
#include "component_utils.h"
#include "directive/descriptor_utils.h"
#include "fatal_handler.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
const char * const ListComponent::FUNC_SCROLLTO = "scrollTo";
const char * const ListComponent::INDEX_SCROLLTO = "index";
ListComponent::ListComponent(JSValue options, JSValue children, AppStyleManager *styleManager)
    : Component(options, children, styleManager)
{
    SetComponentName(K_LIST);
    RegisterNamedFunction(FUNC_SCROLLTO, ListScrollTo);
#if (FEATURE_ROTATION_API == 1)
    RegisterNamedFunction(FUNC_ROTATION_NAME, HandleRotationRequest);
#endif // FEATURE_ROTATION_API
}

// Create list
bool ListComponent::CreateNativeViews()
{
    // set list default attributes
    list_.SetThrowDrag(true);
    // list event listener position
    const int16_t listenPosition = 10;
    list_.SetSelectPosition(listenPosition);
    return true;
}

void ListComponent::ReleaseNativeViews()
{
    list_.RemoveAll();

    // only let list component release its children when the whole application
    // is not in fatal error handling process, as FatalHandler will do the recycle
    if (!FatalHandler::GetInstance().IsFatalErrorHandling()) {
        if (!IS_UNDEFINED(GetDescriptors())) {
            DescriptorUtils::ReleaseDescriptorOrElements(GetDescriptors());
        }
    }
}

UIView *ListComponent::GetComponentRootView() const
{
    return const_cast<UIList *>(&list_);
}

bool ListComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    uint16_t stylePropNameId = GetStylePropNameId(style);
    if (stylePropNameId != K_FLEX_DIRECTION) {
        return false;
    }

    const char * const strValue = GetStyleStrValue(style);
    if (strValue == nullptr) {
        return true;
    }

    uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
    if (valueId == K_ROW) {
        list_.SetDirection(UIList::HORIZONTAL);
    }
    return true;
}

bool ListComponent::RegisterPrivateEventListener(uint16_t eventTypeId, JSValue funcValue, bool isStopPropagation)
{
    list_.SetScrollStateListener(&listEventListener_);

    switch (eventTypeId) {
// list specific event switch
#ifdef FEATURE_LIST_SPECIFIC_EVENT_ENABLE
        // when scroll start, this listening event will be triggered
        case K_SCROLLSTART:
            listEventListener_->SetBindScrollStartFuncName(funcValue);
            break;
        // when item pass through the listenning position, this listening event will be triggered
        case K_ITEM_SELECTED:
            listEventListener_->SetBindScrollItemSelectedFuncName(funcValue);
            break;
#endif // FEATURE_LIST_SPECIFIC_EVENT_ENABLE
        // when scroll end, this listening event will be triggered
        case K_SCROLLEND:
            listEventListener_.SetBindScrollEndFuncName(funcValue);
            break;
        case K_SCROLLTOP:
            listEventListener_.SetBindScrollTopFuncName(funcValue);
            break;
        case K_SCROLLBOTTOM:
            listEventListener_.SetBindScrollBottomFuncName(funcValue);
            break;
        default:
            return false;
    }
    return true;
}

bool ListComponent::ProcessChildren()
{
    // Step1. get children, if not exsit, return false.
    const JSValue descriptors = GetDescriptors();
    if (jerry_value_is_undefined(descriptors)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "List Component: process children failed!");
        return false;
    }

    // Step2. handle For type children(list-item)
    HandleListForDireactive();

    // Step3. create list adapter
    bool ret = adapter_.Initialize(descriptors);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Initialize list adapter failed.");
        return false;
    }

    return true;
}

void ListComponent::OnViewAttached()
{
    ConstrainedParameter listParameter;
    GetConstrainedParam(listParameter);
    adapter_.UpdateContentAlignParam(listParameter);
    adapter_.SetDirection(list_.GetDirection());
    // Step4. add children to list dynamically.
    if (!hasSetAdaptor_) {
        list_.SetAdapter(&adapter_);
        hasSetAdaptor_ = true;
    }
}

bool ListComponent::UpdateForView()
{
    // Step1. get new children, if not exsit, return false.
    const JSValue descriptors = GetDescriptors();
    if (jerry_value_is_undefined(descriptors)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "List Component: update ForView failed!");
        return false;
    }

    // free listItems_ array memory and reset listItemCount_.
    // here not need delete UIView and free viewNativePair_ link list memory,
    // it will be excuted in RefreshList() function.
    adapter_.CleanUp();

    // save all children's information into listItems_ array(contain For type child and common child).
    bool ret = adapter_.Initialize(descriptors);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Initialize list adapter failed.");
        return false;
    }

    // refresh the list, load new child dynamically in list.
    list_.RefreshList();
    return true;
}

JSValue ListComponent::ListScrollTo(const JSValue func, const JSValue dom,
                                    const JSValue args[], const jerry_length_t size)
{
    if (size != 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "scrollTo function parameter error");
        return UNDEFINED;
    }

    UIList *list = reinterpret_cast<UIList *>(ComponentUtils::GetViewFromBindingObject(dom));
    if (list != nullptr) {
        int16_t startIndex = JerryGetIntegerProperty(args[0], INDEX_SCROLLTO);
        list->ScrollTo(startIndex);
    }
    return UNDEFINED;
}
} // namespace ACELite
} // namespace OHOS
