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

#ifndef OHOS_ACELITE_EVENT_LISTENER_H
#define OHOS_ACELITE_EVENT_LISTENER_H

#include "ace_log.h"
#include "async_task_manager.h"
#include "event_util.h"
#include "js_fwk_common.h"
#include "keys.h"
#include "non_copyable.h"
#include "ui_checkbox.h"
#include "ui_list.h"
#include "ui_radio_button.h"
#include "ui_scroll_view.h"
#include "ui_slider.h"
#include "ui_toggle_button.h"

#include "bestechnic/ui_bes_list.h"

namespace OHOS {
namespace ACELite {
class StateChangeListener final : public UICheckBox::OnChangeListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(StateChangeListener);
    explicit StateChangeListener(jerry_value_t fn)
    {
        fn_ = jerry_acquire_value(fn);
        isChanging_ = false;
        state_ = false;
    }

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        bool stateRes = (state == UICheckBox::UICheckBoxState::SELECTED);
        if (stateRes == state_) {
            return true;
        }
        state_ = stateRes;
        if (isChanging_) {
            return false;
        }
        isChanging_ = true;
        jerry_value_t args[1];
        jerry_value_t resultProp = jerry_create_boolean(state_);
        args[0] = jerry_create_object();
        const char * const checked = "checked";
        ReleaseJerryValue(jerryx_set_property_str(args[0], checked, resultProp), resultProp, VA_ARG_END_FLAG);
        jerry_value_t globalObject = jerry_get_global_object();
        jerry_value_t appViewModel = jerryx_get_property_str(globalObject, ATTR_APP);
        if (jerry_value_is_function(fn_)) {
            CallJSFunctionAutoRelease(fn_, appViewModel, args, 1);
        }
        ReleaseJerryValue(globalObject, appViewModel, args[0], VA_ARG_END_FLAG);
        isChanging_ = false;
        return true;
    }

    void SetState(bool state)
    {
        state_ = state;
    }

    ~StateChangeListener()
    {
        jerry_release_value(fn_);
    }

private:
    jerry_value_t fn_;
    bool state_;
    bool isChanging_; // the flag to avoid change event cycle execute
};

class ViewOnClickListener final : public UIView::OnClickListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ViewOnClickListener);

    ViewOnClickListener(jerry_value_t vm, jerry_value_t fn, bool isStopPropagation)
        : changeListener_(nullptr),
          vm_(jerry_acquire_value(vm)),
          fn_(jerry_acquire_value(fn)),
          isStopPropagation_(isStopPropagation) {}

    ~ViewOnClickListener()
    {
        AsyncTaskManager::GetInstance().CancelWithContext(this);
        jerry_release_value(vm_);
        jerry_release_value(fn_);
    }

    bool OnClick(UIView& view, const ClickEvent &event) override
    {
        if (changeListener_) {
            UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(&view);
            changeListener_->OnChange(checkbox->GetState());
        }
        if (JSUndefined::Is(fn_)) {
            return isStopPropagation_;
        }
        JSValue arg = EventUtil::CreateEvent(EventUtil::EVENT_CLICK, view, event);
        EventUtil::InvokeCallback(vm_, fn_, arg, this);

        return isStopPropagation_;
    }

    void SetComponentListener(StateChangeListener *changeListener)
    {
        changeListener_ = changeListener;
    }

private:
    StateChangeListener *changeListener_;
    jerry_value_t vm_;
    jerry_value_t fn_;
    bool isStopPropagation_;
};

class ViewOnLongPressListener final : public UIView::OnLongPressListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ViewOnLongPressListener);
    ViewOnLongPressListener(jerry_value_t vm, jerry_value_t fn, bool isStopPropagation)
        : vm_(jerry_acquire_value(vm)), fn_(jerry_acquire_value(fn)), isStopPropagation_(isStopPropagation) {}

    ~ViewOnLongPressListener()
    {
        AsyncTaskManager::GetInstance().CancelWithContext(this);
        jerry_release_value(vm_);
        jerry_release_value(fn_);
    }

    bool OnLongPress(UIView& view, const LongPressEvent &event) override
    {
        if (jerry_value_is_undefined(fn_)) {
            return isStopPropagation_;
        }

        JSValue arg = EventUtil::CreateEvent(EventUtil::EVENT_LONGPRESS, view, event);
        EventUtil::InvokeCallback(vm_, fn_, arg, this);

        return isStopPropagation_;
    }
    jerry_value_t vm_;
    jerry_value_t fn_;
    bool isStopPropagation_;
};

#ifdef JS_EXTRA_EVENT_SUPPORT

class KeyBoardEventListener final : public RootView::OnKeyActListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(KeyBoardEventListener);
    KeyBoardEventListener(jerry_value_t fn, const uint16_t id);
    ~KeyBoardEventListener();
    bool OnKeyAct(UIView &view, const KeyEvent &event) override;

private:
    jerry_value_t fn_;
    uint16_t id_;
};
#endif

class ViewOnTouchListener final : public UIView::OnDragListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ViewOnTouchListener);
    ViewOnTouchListener(jerry_value_t vm, bool isStopPropagation)
        : vm_(jerry_acquire_value(vm)),
          bindTouchStartFunc_(UNDEFINED),
          bindTouchMoveFunc_(UNDEFINED),
          bindTouchEndFunc_(UNDEFINED),
          bindSwipeFunc_(UNDEFINED),
          isStopPropagation_(isStopPropagation)
    {
    }

    ~ViewOnTouchListener()
    {
        AsyncTaskManager::GetInstance().CancelWithContext(this);
        jerry_release_value(vm_);
        jerry_release_value(bindTouchStartFunc_);
        jerry_release_value(bindTouchMoveFunc_);
        jerry_release_value(bindTouchEndFunc_);
        jerry_release_value(bindSwipeFunc_);
    }

    void SetStopPropagation(bool isStopPropogation);
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent &event) override;
    void SetBindTouchStartFuncName(jerry_value_t bindTouchStartFunc);
    void SetBindTouchMoveFuncName(jerry_value_t bindTouchMoveFunc);
    void SetBindTouchEndFuncName(jerry_value_t bindTouchEndFunc);
    void SetBindSwipeFuncName(jerry_value_t bindSwipeFunc);

private:
    jerry_value_t vm_;
    jerry_value_t bindTouchStartFunc_;
    jerry_value_t bindTouchMoveFunc_;
    jerry_value_t bindTouchEndFunc_;
    jerry_value_t bindSwipeFunc_;
    bool isStopPropagation_;
};

class SliderEventListener final : public UISlider::UISliderEventListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(SliderEventListener);
    SliderEventListener() : bindChangeFunc_(UNDEFINED) {}
    ~SliderEventListener()
    {
        jerry_release_value(bindChangeFunc_);
    }
    void OnChange(int32_t progress) override
    {
        jerry_value_t knobValue = jerry_create_number(progress);
        jerry_value_t args[1];
        args[0] = jerry_create_object();
        jerry_value_t result1 = jerryx_set_property_str(args[0], "value", knobValue);
        // progress will be deprecated in next versions.
        jerry_value_t result2 = jerryx_set_property_str(args[0], "progress", knobValue);
        if (!jerry_value_is_error(result1) && !jerry_value_is_error(result2)) {
            CallJSFunctionAutoRelease(bindChangeFunc_, UNDEFINED, args, 1);
        }
        ReleaseJerryValue(result1, result2, args[0], knobValue, VA_ARG_END_FLAG);
    }

    void SetBindChangeFuncName(jerry_value_t bindChageFunc)
    {
        bindChangeFunc_ = jerry_acquire_value(bindChageFunc);
    }

private:
    jerry_value_t bindChangeFunc_;
};

class ListEventListener final : public ListScrollListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ListEventListener);
    ListEventListener()
        : bindScrollStartFunc_(UNDEFINED),
          bindScrollEndFunc_(UNDEFINED),
          bindScrollSelectedFunc_(UNDEFINED),
          bindScrollTopFunc_(UNDEFINED),
          bindScrollBottomFunc_(UNDEFINED)
    {
    }
    ~ListEventListener()
    {
        ReleaseJerryValue(bindScrollStartFunc_, bindScrollEndFunc_, bindScrollSelectedFunc_, bindScrollTopFunc_,
            bindScrollBottomFunc_, VA_ARG_END_FLAG);
    }

    void EventExcute(const int16_t index, jerry_value_t bindScrollFunc) const
    {
        if (IS_UNDEFINED(bindScrollFunc)) {
            return;
        }

        int8_t currentState = this->GetScrollState();
        jerry_value_t currentStateValue = jerry_create_number(currentState);
        jerry_value_t componentIndex = jerry_create_number(index);
        jerry_value_t args[ARGS_LEN] = {currentStateValue, componentIndex};
        CallJSFunctionAutoRelease(bindScrollFunc, UNDEFINED, args, ARGS_LEN);
        ReleaseJerryValue(currentStateValue, componentIndex, VA_ARG_END_FLAG);
    }

// list specific event switch
#ifdef FEATURE_LIST_SPECIFIC_EVENT_ENABLE
    void OnScrollStart(int16_t index, UIView *view) override
    {
        EventExcute(index, bindScrollStartFunc_);
    }
    void OnItemSelected(int16_t index, UIView *view) override
    {
        EventExcute(index, bindScrollSelectedFunc_);
    }
    void SetBindScrollStartFuncName(jerry_value_t bindScrollStartFunc)
    {
        if (!jerry_value_is_undefined(bindScrollStartFunc)) {
            bindScrollStartFunc_ = jerry_acquire_value(bindScrollStartFunc);
        }
    }
    void SetBindScrollItemSelectedFuncName(jerry_value_t bindScrollItemSelectedFunc)
    {
        if (!jerry_value_is_undefined(bindScrollItemSelectedFunc)) {
            bindScrollSelectedFunc_ = jerry_acquire_value(bindScrollItemSelectedFunc);
        }
    }
#endif // FEATURE_LIST_SPECIFIC_EVENT_ENABLE

    void OnScrollEnd(int16_t index, UIView* view) override
    {
        EventExcute(index, bindScrollEndFunc_);
    }

    void SetBindScrollEndFuncName(jerry_value_t bindScrollEndFunc)
    {
        if (!jerry_value_is_undefined(bindScrollEndFunc)) {
            bindScrollEndFunc_ = jerry_acquire_value(bindScrollEndFunc);
        }
    }

    void OnScrollTop(int16_t index, UIView* view) override
    {
        EventExcute(index, bindScrollTopFunc_);
    }

    void SetBindScrollTopFuncName(jerry_value_t bindScrollTopFunc)
    {
        if (!jerry_value_is_undefined(bindScrollTopFunc)) {
            bindScrollTopFunc_ = jerry_acquire_value(bindScrollTopFunc);
        }
    }

    void OnScrollBottom(int16_t index, UIView* view) override
    {
        EventExcute(index, bindScrollBottomFunc_);
    }

    void SetBindScrollBottomFuncName(jerry_value_t bindScrollBottomFunc)
    {
        if (!jerry_value_is_undefined(bindScrollBottomFunc)) {
            bindScrollBottomFunc_ = jerry_acquire_value(bindScrollBottomFunc);
        }
    }

private:
    static constexpr int8_t ARGS_LEN = 2;
    jerry_value_t bindScrollStartFunc_;
    jerry_value_t bindScrollEndFunc_;
    jerry_value_t bindScrollSelectedFunc_;
    jerry_value_t bindScrollTopFunc_;
    jerry_value_t bindScrollBottomFunc_;
};

/**
 * @brief 适用于 BesListComponent 的事件监听类。
 */
class BesListEventListener : public BesListScrollListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(BesListEventListener);
    BesListEventListener()
        : bindScrollStartFunc_(UNDEFINED),
          bindScrollEndFunc_(UNDEFINED),
          bindScrollSelectedFunc_(UNDEFINED),
          bindScrollTopFunc_(UNDEFINED),
          bindScrollBottomFunc_(UNDEFINED)
    {
    }
    ~BesListEventListener()
    {
        ReleaseJerryValue(bindScrollStartFunc_, bindScrollEndFunc_, bindScrollSelectedFunc_, bindScrollTopFunc_,
            bindScrollBottomFunc_, VA_ARG_END_FLAG);
    }

    void EventExcute(const int16_t index, jerry_value_t bindScrollFunc) const
    {
        if (IS_UNDEFINED(bindScrollFunc)) {
            return;
        }

        int8_t currentState = this->GetScrollState();
        jerry_value_t currentStateValue = jerry_create_number(currentState);
        jerry_value_t componentIndex = jerry_create_number(index);
        jerry_value_t args[ARGS_LEN] = {currentStateValue, componentIndex};
        CallJSFunctionAutoRelease(bindScrollFunc, UNDEFINED, args, ARGS_LEN);
        ReleaseJerryValue(currentStateValue, componentIndex, VA_ARG_END_FLAG);
    }

// list specific event switch
#ifdef FEATURE_LIST_SPECIFIC_EVENT_ENABLE
    void OnItemSelected(int16_t index, UIView *view) override
    {
        EventExcute(index, bindScrollSelectedFunc_);
    }
    void SetBindScrollItemSelectedFuncName(jerry_value_t bindScrollItemSelectedFunc)
    {
        if (!jerry_value_is_undefined(bindScrollItemSelectedFunc)) {
            bindScrollSelectedFunc_ = jerry_acquire_value(bindScrollItemSelectedFunc);
        }
    }
#endif // FEATURE_LIST_SPECIFIC_EVENT_ENABLE

    //yongxianglai@bestechnic.com: 向 jsui 开放
    void OnScrollStart(int16_t index, UIView *view) override
    {
        EventExcute(index, bindScrollStartFunc_);
    }
    void SetBindScrollStartFuncName(jerry_value_t bindScrollStartFunc)
    {
        if (!jerry_value_is_undefined(bindScrollStartFunc)) {
            bindScrollStartFunc_ = jerry_acquire_value(bindScrollStartFunc);
        }
    }

    void OnScrollEnd(int16_t index, UIView* view) override
    {
        EventExcute(index, bindScrollEndFunc_);
    }

    void SetBindScrollEndFuncName(jerry_value_t bindScrollEndFunc)
    {
        if (!jerry_value_is_undefined(bindScrollEndFunc)) {
            bindScrollEndFunc_ = jerry_acquire_value(bindScrollEndFunc);
        }
    }

    void OnScrollTop(int16_t index, UIView* view) override
    {
        EventExcute(index, bindScrollTopFunc_);
    }

    void SetBindScrollTopFuncName(jerry_value_t bindScrollTopFunc)
    {
        if (!jerry_value_is_undefined(bindScrollTopFunc)) {
            bindScrollTopFunc_ = jerry_acquire_value(bindScrollTopFunc);
        }
    }

    void OnScrollBottom(int16_t index, UIView* view) override
    {
        EventExcute(index, bindScrollBottomFunc_);
    }

    void SetBindScrollBottomFuncName(jerry_value_t bindScrollBottomFunc)
    {
        if (!jerry_value_is_undefined(bindScrollBottomFunc)) {
            bindScrollBottomFunc_ = jerry_acquire_value(bindScrollBottomFunc);
        }
    }

private:
    static constexpr int8_t ARGS_LEN = 2;
    jerry_value_t bindScrollStartFunc_;
    jerry_value_t bindScrollEndFunc_;
    jerry_value_t bindScrollSelectedFunc_;
    jerry_value_t bindScrollTopFunc_;
    jerry_value_t bindScrollBottomFunc_;
};

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_EVENT_LISTENER_H
