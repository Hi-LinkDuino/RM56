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

#include "swiper_component.h"
#include "ace_log.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
   // yguohua add for swipe to start
    const char * const SwiperComponent::FUNC_SWIPETO = "swipeTo";
    const char * const SwiperComponent::INDEX_SWIPETO = "index";
   // yguohua add for swipe to end
SwiperComponent::SwiperComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      hasChildren_(false),
      index_(0),
      isLoop_(true),
      changeListener_(nullptr)
{
    SetComponentName(K_SWIPER);
    RegisterNamedFunction(FUNC_SWIPETO, swipePageTo);// yguohua add for swipe to
    swiperView_.SetLoopState(true);
#if (FEATURE_ROTATION_API == 1)
    RegisterNamedFunction(FUNC_ROTATION_NAME, HandleRotationRequest);
#endif // FEATURE_ROTATION_API
}

void SwiperComponent::ReleaseNativeViews()
{
    if (changeListener_) {
        delete changeListener_;
        changeListener_ = nullptr;
    }
}

inline UIView *SwiperComponent::GetComponentRootView() const
{
    return const_cast<UISwipeView *>(&swiperView_);
}

bool SwiperComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    if (attrKeyId == K_DURATION) {
        int16_t duration = IntegerOf(attrValue);
        if (duration < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "duration value has problem.");
            return false;
        }
        swiperView_.SetAnimatorTime(duration);
        return true;
    } else if (attrKeyId == K_LOOP) {
        if (jerry_value_is_boolean(attrValue)) {
            isLoop_ = BoolOf(attrValue);
            swiperView_.SetLoopState(isLoop_);
        } else {
            isLoop_ = true;
            swiperView_.SetLoopState(isLoop_);
        }
        return true;
    } else if (attrKeyId == K_INDEX) {
        if (!jerry_value_is_number(attrValue) && !jerry_value_is_string(attrValue)) {
            index_ = 0;
        } else {
            index_ = IntegerOf(attrValue);
        }
        return true;
    } else if (attrKeyId == K_VERTICAL) {
        if (!hasChildren_) {
            if (jerry_value_is_boolean(attrValue)) {
                swiperView_.SetDirection(BoolOf(attrValue));
            } else {
                swiperView_.SetDirection(0);
            }
        }
    }

    return false;
}

void SwiperComponent::PostUpdate(uint16_t attrKeyId)
{
    SetPageIndex();
}

bool SwiperComponent::ProcessChildren()
{
    AppendChildren(this);
    hasChildren_ = true;
    return true;
}

void SwiperComponent::AttachView(const Component *child)
{
    if (child == nullptr) {
        return;
    }
    swiperView_.Add(child->GetComponentRootView());
    swiperView_.SetLoopState(false);
    SetPageIndex();
    swiperView_.SetLoopState(isLoop_);
}

void SwiperComponent::OnVisibilityChanged(bool isVisible)
{
    if (changeListener_ == nullptr) {
        return;
    }
    if (!isVisible) {
        swiperView_.SetOnSwipeListener(nullptr);
        return;
    }
    // component will be visible
    if (swiperView_.GetOnSwipeListener() != nullptr) {
        return;
    }
    swiperView_.SetOnSwipeListener(changeListener_);
}

bool SwiperComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
                                                   jerry_value_t funcValue,
                                                   bool isStopPropagation)
{
    if (eventTypeId == K_CHANGE) {
        changeListener_ = new ChangeListener(funcValue);
        if (changeListener_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Create ChangeListener error.");
            return false;
        }
        swiperView_.SetOnSwipeListener(changeListener_);
        return true;
    }
    return false;
}

void SwiperComponent::SetPageIndex()
{
    if (jerry_value_is_undefined(GetChildren())) {
        HILOG_INFO(HILOG_MODULE_ACE, "Swiper children don't exist.");
        return;
    }
    uint16_t childrenNum = jerry_get_array_length(GetChildren());
    if (index_ < 0 || index_ >= childrenNum) {
        swiperView_.SetCurrentPage(0);
    } else {
        swiperView_.SetCurrentPage(index_);
    }
    if (childrenNum == 1) {
        swiperView_.SetBlankSize(0);
        swiperView_.SetLoopState(false);
    }
}

void SwiperComponent::ChangeListener::OnSwipe(UISwipeView &view)
{
    if (jerry_value_is_undefined(fn_)) {
        return;
    }
    UISwipeView *swipeView = reinterpret_cast<UISwipeView *>(&view);
    if (swipeView == nullptr) {
        return;
    }
    constexpr uint8_t argsNum = 1;
    jerry_value_t args[argsNum];
    args[0] = jerry_create_object();
    jerry_value_t indexValHandler = jerry_create_number(swipeView->GetCurrentPage());
    jerry_value_t result = jerryx_set_property_str(args[0], SWIPER_ATTR_INDEX, indexValHandler);
    jerry_release_value(indexValHandler);
    if (jerry_value_is_error(result)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set index value failed.");
        ReleaseJerryValue(args[0], result, VA_ARG_END_FLAG);
        return;
    }
    jerry_release_value(result);
    if (jerry_value_is_function(fn_)) {
        jerry_value_t retVal = CallJSFunctionOnRoot(fn_, args, argsNum);
        if (jerry_value_is_error(retVal)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Change event is error.");
        }
        jerry_release_value(retVal);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Change event is not a function.");
    }
    jerry_release_value(args[0]);
}

// yguohua add for swipe to start
JSValue SwiperComponent::swipePageTo(const JSValue func, const JSValue dom,
                                    const JSValue args[], const jerry_length_t size)
{
     HILOG_INFO(HILOG_MODULE_ACE, "swipePageTo.");
    UISwipeView *swipe = reinterpret_cast<UISwipeView *>(ComponentUtils::GetViewFromBindingObject(dom));
    if (swipe != nullptr) {
        int16_t startIndex = JerryGetIntegerProperty(args[0], INDEX_SWIPETO);
        swipe->SetCurrentPage(startIndex,true);
    }
    return UNDEFINED;
}
// yguohua add for swipe to end

} // namespace ACELite
} // namespace OHOS
