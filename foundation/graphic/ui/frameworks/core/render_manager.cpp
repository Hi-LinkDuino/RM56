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

#include "core/render_manager.h"

#include "components/root_view.h"
#include "gfx_utils/graphic_log.h"
#include "hal_tick.h"
#include "securec.h"
#if ENABLE_WINDOW
#include "window/window_impl.h"
#endif

namespace OHOS {
RenderManager::RenderManager() : fps_(0.f), needResetFPS_(true), onFPSChangedListener_(nullptr) {}

RenderManager::~RenderManager() {}

RenderManager& RenderManager::GetInstance()
{
    static RenderManager instance;
    return instance;
}

void RenderManager::Init()
{
    Task::Init();
}

void RenderManager::Callback()
{
#if ENABLE_WINDOW
    ListNode<Window*>* winNode = winList_.Begin();
    while (winNode != winList_.End()) {
        WindowImpl* windowImpl = reinterpret_cast<WindowImpl*>(winNode->data_);
        windowImpl->Render();
        winNode = winNode->next_;
    }
#else
    RootView::GetInstance()->Measure();
    RootView::GetInstance()->Render();
#endif

#if ENABLE_FPS_SUPPORT
    UpdateFPS();
#endif
}

#if ENABLE_FPS_SUPPORT
void RenderManager::UpdateFPS()
{
    SysInfo::FPSCalculateType type = SysInfo::FPS_CT_FIXED_TIME;
    if (onFPSChangedListener_) {
        type = onFPSChangedListener_->GetFPSCalculateType();
    }
    if (type == SysInfo::FPS_CT_FIXED_TIME) {
        UpdateFPSByFixedTimeMethod();
    } else if (type == SysInfo::FPS_CT_AVERAGE_SAMPLING) {
        UpdateFPSByAverageSamplingMethod();
    } else if (type == SysInfo::FPS_CT_PRECISE_SAMPLING) {
        UpdateFPSByPreciseSamplingMethod();
    }
    OnFPSChanged(fps_);
}

void RenderManager::UpdateFPSByFixedTimeMethod()
{
    static uint16_t frameCount = 0;
    static uint32_t lastTime = HALTick::GetInstance().GetTime();
    if (needResetFPS_) {
        frameCount = 0;
        lastTime = HALTick::GetInstance().GetTime();
        needResetFPS_ = false;
        fps_ = 0.f;
        return;
    }

    frameCount++;

    uint32_t curTime = HALTick::GetInstance().GetTime();
    if (curTime - lastTime > MILLISECONDS_PER_SECOND) {
        fps_ = 1.f * frameCount / (curTime - lastTime) * MILLISECONDS_PER_SECOND;
        fps_ = (fps_ > MAX_FPS) ? MAX_FPS : fps_;
        frameCount = 0;
        lastTime = curTime;
    }
}

void RenderManager::UpdateFPSByAverageSamplingMethod()
{
    static float avgDuration = 0.f;
    static float alpha = 1.f / SAMPLE_NUMBER;
    static bool firstFrame = true;
    static uint32_t lastTime = HALTick::GetInstance().GetTime();
    if (needResetFPS_) {
        avgDuration = 0.f;
        alpha = 1.f / SAMPLE_NUMBER;
        firstFrame = true;
        lastTime = HALTick::GetInstance().GetTime();
        needResetFPS_ = false;
        fps_ = 0.f;
        return;
    }
    uint32_t curTime = HALTick::GetInstance().GetTime();
    int32_t deltaTime = curTime - lastTime;
    lastTime = curTime;

    if (firstFrame) {
        avgDuration = static_cast<float>(deltaTime);
        firstFrame = false;
    } else {
        avgDuration = avgDuration * (1 - alpha) + deltaTime * alpha;
    }
    fps_ = 1.f / avgDuration * MILLISECONDS_PER_SECOND;
}

void RenderManager::UpdateFPSByPreciseSamplingMethod()
{
    static int32_t deltaTimeQueue[SAMPLE_NUMBER] = {0};
    static int32_t frameCount = 0;
    static int32_t sumDuration = 0;
    static bool isQueueFull = false;
    static uint32_t lastTime = HALTick::GetInstance().GetTime();
    if (needResetFPS_) {
        if (memset_s(deltaTimeQueue, sizeof(deltaTimeQueue), 0, sizeof(deltaTimeQueue)) != EOK) {
            return;
        }
        frameCount = 0;
        sumDuration = 0;
        isQueueFull = false;
        lastTime = HALTick::GetInstance().GetTime();
        needResetFPS_ = false;
        fps_ = 0.f;
        return;
    }
    uint32_t curTime = HALTick::GetInstance().GetTime();
    int32_t deltaTime = curTime - lastTime;
    lastTime = curTime;

    if (!isQueueFull && (frameCount == SAMPLE_NUMBER)) {
        isQueueFull = true;
    }
    frameCount %= SAMPLE_NUMBER;
    sumDuration -= deltaTimeQueue[frameCount];
    sumDuration += deltaTime;
    deltaTimeQueue[frameCount++] = deltaTime;
    if (isQueueFull) {
        fps_ = 1.f * SAMPLE_NUMBER / sumDuration * MILLISECONDS_PER_SECOND;
    } else {
        fps_ = 1.f * frameCount / sumDuration * MILLISECONDS_PER_SECOND;
    }
}
#endif

void RenderManager::RenderRect(const Rect& rect, RootView* rootView)
{
    if (rootView == nullptr) {
        return;
    }

    Rect mask = rect;
#if ENABLE_WINDOW
    if (rootView->GetBoundWindow()) {
        Rect winRect = rootView->GetBoundWindow()->GetRect();
        winRect.SetPosition(0, 0);
        mask.Intersect(rect, winRect);
    }
#endif
#if LOCAL_RENDER
    rootView->DrawInvalidMap(mask);
#else
    UIView* topView = rootView->GetTopUIView(mask);
    rootView->DrawTop(topView, mask);
#endif
}

void RenderManager::RefreshScreen()
{
#if ENABLE_WINDOW
    ListNode<Window*>* winNode = winList_.Begin();
    while (winNode != winList_.End()) {
        WindowImpl* windowImpl = reinterpret_cast<WindowImpl*>(winNode->data_);
        RootView* rootView = windowImpl->GetRootView();
        if (rootView == nullptr) {
            winNode = winNode->next_;
            continue;
        }
        rootView->Invalidate();
        winNode = winNode->next_;
    }
#else
    RootView::GetInstance()->Invalidate();
#endif
}

#if ENABLE_WINDOW
void RenderManager::AddToDisplay(Window* window)
{
    if (window == nullptr) {
        return;
    }
    winList_.PushBack(window);
}

void RenderManager::RemoveFromDisplay(Window* window)
{
    if (window == nullptr) {
        return;
    }
    ListNode<Window*>* winNode = winList_.Begin();
    while (winNode != winList_.End()) {
        if (winNode->data_ == window) {
            winList_.Remove(winNode);
            return;
        }
        winNode = winNode->next_;
    }
}
#endif
} // namespace OHOS
