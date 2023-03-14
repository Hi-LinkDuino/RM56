/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frame.h"
#include <thread>
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
using namespace std;

Frame::Frame(uint32_t w, uint32_t h, View::PixelFormat pixType, SurfaceDev* sfDev)
{
    this->CreateBuffer(w, h, pixType);
    this->startX_ = 0;
    this->startY_ = 0;
    sfDev_ = sfDev;
    listIndex_ = 0;
    flushFlag_ = false;
    std::thread(&Frame::FlushThreadLoop, this).detach();
}

Frame::~Frame()
{
    needStop_ = true;
    FreeBuffer();
}

void Frame::FlushThreadLoop()
{
    while (!needStop_) {
        std::unique_lock<std::mutex> locker(mutex_);
        if (!flushFlag_) {
            mCondFlush_.wait(mutex_, [&] {
                return flushFlag_;
            });
        }

        SyncBuffer();
        frameMutex_.lock();
        std::map<View*, int32_t>::iterator iter;
        for (iter = viewMapList_.begin(); iter != viewMapList_.end(); ++iter) {
            View* tmpView = (*iter).first;
            BATTERY_HILOGD(FEATURE_CHARGING, "tmpView->IsVisiable()=%{public}d", tmpView->IsVisiable());
            if (tmpView->IsVisiable()) {
                char* bufTmp = static_cast<char*>(tmpView->GetBuffer());
                DrawSubView(tmpView->startX_, tmpView->startY_, tmpView->viewWidth_, tmpView->viewHeight_, bufTmp);
            }
        }
        frameMutex_.unlock();
        sfDev_->Flip(this->GetBuffer());
        flushFlag_ = false;
    }
}

void Frame::ViewRegister(View* view)
{
    std::unique_lock<std::mutex> locker(frameMutex_);
    view->SetViewId(frameViewId + listIndex_);
    viewMapList_.insert(std::make_pair(view, frameViewId + listIndex_));
    if (view->IsFocusAble()) {
        maxActionIndex_++;
        BATTERY_HILOGD(FEATURE_CHARGING, "maxActionIndex_=%{public}d", maxActionIndex_);
    }
    listIndex_++;
}

void Frame::OnDraw()
{
    std::unique_lock<std::mutex> locker(mutex_);
    flushFlag_ = true;
    mCondFlush_.notify_all();
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
