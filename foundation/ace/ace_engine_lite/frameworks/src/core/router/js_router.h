/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACELITE_JS_ROUTER_H
#define OHOS_ACELITE_JS_ROUTER_H

#include "js_page_state_machine.h"
#include "non_copyable.h"
#include "js_page_record.h"
#include "jsi.h"

namespace OHOS {
namespace ACELite {
class Router final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(Router);
    Router() : currentSm_(nullptr), newSm_(nullptr), hidden_(false)
    {
        pageRecord_ = new PageRecord();
    }
    ~Router()
    {
        if (currentSm_ != nullptr) {
            delete currentSm_;
        }

        if(pageRecord_ != nullptr){
            delete pageRecord_;
        }
    }
    jerry_value_t Replace(jerry_value_t object, bool async = true);
    jerry_value_t Push(jerry_value_t object);
    jerry_value_t Back(jerry_value_t object);

    void ReplaceSync();
    void PushSync();
    void Show();
    void Hide();

    StateMachine* GetCurrentSM()
    {
        return currentSm_;
    }

private:
#if (BES_FRATURE_PAGE_TRANSITION == 1)
    /**
     * @author yongxianglai@bestechnic.com
     * @brief 页面缓存跳转时的转场动画处理。在页面完成转场动画时，清理退场页面资源
     */
    class PushPageTransCallback final : public PageTransCallback {
    public:
        virtual void OnStop(UIView& view, StateMachine& sm, StateMachine* prevSm) { 
            //转场结束后，让旧页面真正置于后台
            if (prevSm != nullptr) {
                prevSm->ChangeState(BACKGROUND_STATE);
                prevSm->SetHiddenFlag(true);
            }
        }
    };

    /**
     * @author yongxianglai@bestechnic.com
     * @brief 页面切换时的转场动画处理。在页面完成转场动画时，清理退场页面资源
     */
    class ReplacePageTransCallback final : public PageTransCallback {
    public:
        virtual void OnStop(UIView& view, StateMachine& sm, StateMachine* prevSm) {
            if (sm.IsEnterTransitionPage()) sm.ReleasePageTransition();

            //1ms is consumed.
            JSHeapStatus heapStatus;
            JSI::GetJSHeapStatus(heapStatus);
            HILOG_INFO(HILOG_MODULE_ACE, "page[%p] transition done. allocBytes: %d peakAllocBytes: %d ",
                &sm, heapStatus.allocBytes,heapStatus.peakAllocBytes);
        }
    };

    /** 
     * @brief 在转场动画开始前的前置工作 
     * @param newSm 新页面（入场页面）
     * @param currentSm 当前页面（退场页面）
     * @param object 包含转场参数的页面参数对象
     */
    void OnPrePageTransition(StateMachine* newSm, StateMachine* currentSm, jerry_value_t object);
#endif

    StateMachine *currentSm_; // current state machine for current shown page
    StateMachine *newSm_;     // current state machine for target showing page
    bool hidden_;             // the flag representing whether the whole app is moved to background
    PageRecord *pageRecord_;  // record page
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_JS_ROUTER_H
