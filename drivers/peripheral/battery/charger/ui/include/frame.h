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

#ifndef CHARGER_UI_FRAME_H
#define CHARGER_UI_FRAME_H

#include <map>
#include <condition_variable>
#include "surface_dev.h"
#include "view.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
struct CmpByStartY {
    bool operator()(const View* v1, const View* v2) const
    {
        return v1->startY_ < v2->startY_;
    }
};

class Frame : public View {
public:
    Frame(uint32_t w, uint32_t h, View::PixelFormat pixType, SurfaceDev* sfDev);

    ~Frame() override;

    void OnDraw() override;

    void ViewRegister(View* view);

private:
    void FlushThreadLoop();

    int32_t maxActionIndex_ = 0;
    int32_t listIndex_ = 0;
    int32_t frameViewId = 0;
    bool flushFlag_ = false;
    bool needStop_ = false;
    SurfaceDev* sfDev_ = nullptr;
    std::mutex frameMutex_;
    std::condition_variable_any mCondFlush_;
    std::map<View*, int32_t, CmpByStartY> viewMapList_;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_FRAME_H
