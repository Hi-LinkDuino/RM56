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

#ifndef OHOS_ACELITE_JS_PAGE_STATE_MACHINE_H
#define OHOS_ACELITE_JS_PAGE_STATE_MACHINE_H

#include "component.h"
#include "js_app_context.h"
#include "non_copyable.h"
#include "scroll_layer.h"
#include <list>
#include "fragment/js_fragment_state_machine.h"

#include "graphic_config.h"

#include "root_view.h"

#define JS_PAGE_RETURN_IF_ERROR(error, pagePath)                                           \
    do {                                                                                   \
        if (error != 0) {                                                                  \
            HILOG_ERROR(HILOG_MODULE_ACE, "use secure function error(%{public}d)", error); \
            ace_free(pagePath);                                                            \
            pagePath = nullptr;                                                            \
            return ERROR_SECURE_USE;                                                       \
        }                                                                                  \
    } while (0)

namespace OHOS {
namespace ACELite {
enum {
    UNDEFINED_STATE = -1,
    INIT_STATE, // 0
    READY_STATE,
    SHOW_STATE,
    BACKGROUND_STATE,
    DESTROY_STATE,
    END_STATE
};
#define PAGE_STATE_SIZE END_STATE

constexpr char PAGE_LIFECYCLE_ON_INIT[] = "onInit";
constexpr char PAGE_LIFECYCLE_CALLBACK_ON_READY[] = "onReady";
constexpr char PAGE_LIFECYCLE_CALLBACK_ON_SHOW[] = "onShow";
constexpr char PAGE_LIFECYCLE_CALLBACK_ON_HIDE[] = "onHide";
constexpr char PAGE_LIFECYCLE_CALLBACK_ON_DESTROY[] = "onDestroy";

#if JS_PAGE_SPECIFIC
extern JSPageSpecific jsPageSpecific;
#endif

#if (BES_FRATURE_PAGE_TRANSITION == 1)
class StateMachine;

/**
 * @author yongxianglai@bestechnic.com
 * @brief 页面转场动画的事件回调类
 */
class PageTransCallback : public MemoryHeap {
public:
    PageTransCallback() = default;
    virtual ~PageTransCallback() = default;

    virtual void OnStart(StateMachine& sm) {}
    virtual void Callback(UIView* view, StateMachine& sm, PageTransitionParam* const param) {}
    virtual void OnStop(UIView& view, StateMachine& sm, StateMachine* prevSm) {}
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 页面转场动画基类
 */
class BasePageTransCallback final : public AnimatorCallback {
public:
    BasePageTransCallback(StateMachine* currentSm): AnimatorCallback(), 
        sm_(currentSm) {}

    virtual ~BasePageTransCallback();

    void SetExtCallback(PageTransCallback* cb);

    /** @brief 在动画开始时回调，只会触发一次 */
    void OnStart();

    /**
     * @brief 动画的过程回调，每当动画需要执行一帧，这个函数就会被回调一次。
     * @param view 动画目标 view
     */
    virtual void Callback(UIView* view) override;

    /**
     * @brief 动画结束时，触发此回调
     * @param view 动画目标 view
     */
    virtual void OnStop(UIView& view) override;

private:
    /** @brief 动画绑定的页面（状态机） */
    StateMachine* sm_;
    PageTransCallback* cb_ = nullptr;

    uint32_t delayTime_ = 0;
};
#endif //BES_FRATURE_PAGE_TRANSITION

class State;
class StateMachine final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(StateMachine);
    StateMachine();
    ~StateMachine();
    bool Init(jerry_value_t object, jerry_value_t &jsRes);
    void ChangeState(int newState);
    void BindParameters();
    bool BindUri(jerry_value_t &jsRes);
    int8_t GetCurrentState() const
    {
        return currentState_;
    }
    void EvalPage();
    void RenderPage();
    void ShowPage() const;
    void HidePage() const;
    void SetCurrentState(int8_t newState);
    void InvokePageLifeCycleCallback(const char * const name) const;
    void ReleaseHistoryPageResource();
    void SetHiddenFlag(bool flag);
#ifdef TDD_ASSERTIONS
    // this function is just for unittest's view modle hooking purpose, should not be used in real environment
    void SetViewModel(jerry_value_t viewModel);
#endif // TDD_ASSERTIONS

    char *GetJsPagePath()
    {
        return jsPagePath_;
    }
    void RegisterParams(jerry_value_t params);


    void HandleFragmentState(int newState);
    void AddFragment(FragmentStateMachine * fragmentSM);
    void RemoveFragment(FragmentStateMachine * fragmentSM);

    /**
     * @brief 是否为当前页面 uri
     * @param uri 目标 uri
     */
    bool IsCurrentPageUri(const char* uri) const
    {
        return uri != nullptr && uri_ != nullptr && !strcmp(uri_, uri);
    }

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    /** @brief 页面是否处于销毁流程（已经不可用） */
    bool IsFinishing() const;

    /**
     * @brief 记录退场页面（状态机），解析页面转场参数
     * @param prevSm 退场页面
     * @param paramObj 包含页面转场参数的页面切换参数对象
     */
    void RecordPageTransition(StateMachine* prevSm, jerry_value_t paramObj);

    /** @brief 开始页面转场 */
    void StartPageTransition();

    /** @brief 清理因页面转场而被滞留的旧页面资源 */
    void ReleasePageTransition();

    /** 
     * @brief 设置动画监听器，负责处理动画结束后的行为。
     *           必须在 {@link #StartPageTransition()} 之前被调用，否则无效
     * @param cb 监听器
     * @return 设置是否成功
     */
    bool SetPageTransCallback(PageTransCallback* cb);

    /** @brief 是否为入场页面，而不是退场页面。如果此时未在页面切换，也会返回 false */
    bool IsEnterTransitionPage() const;

    /** 
     * @brief 倒转页面转场，仅限于入场页面执行有效 
     * @param transField 页面转场参数字段
     */
    void ReversePageTransition(jerry_value_t transField);
#endif //BES_FRATURE_PAGE_TRANSITION

private:
    void RegisterUriAndParamsToPage(const char * const uri, jerry_value_t params);
    int GenerateJsPagePath(const char * const uri);
    void DeleteViewModelProperties() const;
    void ReleaseRootObject() const;
    bool CheckJSSourceFile() const;

    int8_t currentState_;
    State *stateMap_[PAGE_STATE_SIZE];
    char *jsPagePath_;
    char *appRootPath_;
    char *uri_;
    Component *rootComponent_;
    JsAppContext *appContext_;
    jerry_value_t viewModel_;     // the object evaled from user JS code
    jerry_value_t object_;        // object transferred from one page to another page
    bool hasParams_;              // the flag representation for whether having params of object_
    bool isEntireHidden_;            // representing if the whole app is in background
    Watcher *watchersHead_;       // head of watchers list
    ScrollLayer *scrollLayer_;
    std::list<FragmentStateMachine*>* fragmentList_;      // fragment SM list

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    friend class BasePageTransCallback;

    StateMachine* GetPrevSm();

    /**
     * @brief 页面状态的附加标记，指示页面是否处于销毁流程中
     * @param isFinishing 
     */
    void SetFinishing(bool isFinishing);

    /**
     * @brief 设置页面转场参数
     * @param field  js 字段，字段定义见 {@link Router#Replace} 中的参数说明
     */
    void SetPageTransitionParam(const jerry_value_t* animField);

    /**
     * @brief 解析附加（除了动画基础的起始数值和结束数值）的动画数值。
     * @param anim 
     * @param animValuesField 
     * @param len 
     */
    void ParseExtraAnimValues(const char* const anim, jerry_value_t animValuesField, uint32_t len);

    /**
     * @brief 解析动画的基准中心
     * @param pivotName 
     * @return , BesAnimPivot
     */
    BesAnimPivot ParseAnimPivot(const char* const pivotName);

    /**
     * @brief 记录前一个页面（状态机）
     * @param prevSm 前一个页面（状态机）
     */
    void SetPrevSm(StateMachine* prevSm);

    /** 
     * @brief 倒转页面转场动画参数，一般用于页面回退
     * @see ReleasePageTransition()
     */
    void ReversePageTransitionParam();

    /**
     * @brief 解析公共的页面转场字段
     * @param commonField 
     */
    void ParseCommonParam(jerry_value_t commonField);

    /**
     * @brief 解析预置动画
     * @param presetAnim 预置动画类型
     */
    void ParsePresetAnim(const char* const presetAnim);

    /** @brief 转场旧页面（状态机） */
    StateMachine* prevSm_ = nullptr;

    /** @brief 页面转场参数 */
    PageTransitionParam* pageTransParam_ = nullptr;

    /** @brief 转场动画的回调 */
    BasePageTransCallback* pageTransCallback_ = nullptr;

    /** @brief 标记是否处于销毁过程 */
    bool isFinishing_;
#endif //BES_FRATURE_PAGE_TRANSITION
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_JS_PAGE_STATE_MACHINE_H
