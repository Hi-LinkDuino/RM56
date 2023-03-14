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

#include "dfx/point_event_injector.h"
#if ENABLE_DEBUG
#include "gfx_utils/graphic_log.h"

namespace {
const uint16_t MAX_LIST_SIZE = 500;
}

namespace OHOS {
PointEventInjector::~PointEventInjector()
{
    ListNode<DeviceData*>* node = dataList_.Begin();
    while (node != dataList_.End()) {
        if (node->data_ != nullptr) {
            DeviceData* deleteData = node->data_;
            delete deleteData;
        }
        node = node->next_;
    }
    dataList_.Clear();
}

bool PointEventInjector::SetPointEvent(const DeviceData& data)
{
    if (dataList_.Size() >= MAX_LIST_SIZE) {
        GRAPHIC_LOGI("PointEventInjector::SetPointEvent data list is full.");
        return false;
    }

    DeviceData* tmpData = new DeviceData;
    if (tmpData == nullptr) {
        GRAPHIC_LOGE("PointEventInjector::SetPointEvent memory allocation failed Err!");
        return false;
    }
    tmpData->point = data.point;
    tmpData->state = data.state;
    dataList_.PushBack(tmpData);
    return true;
}

bool PointEventInjector::Read(DeviceData& data)
{
    if (dataList_.Size() > 0) {
        lastX_ = dataList_.Front()->point.x;
        lastY_ = dataList_.Front()->point.y;
        lastState_ = dataList_.Front()->state;

        ListNode<DeviceData*>* node = dataList_.Begin();
        if (node->data_ != nullptr) {
            DeviceData* deleteData = node->data_;
            delete deleteData;
        }
        dataList_.PopFront();
    }
    data.point = {lastX_, lastY_};
    data.state = lastState_;
#if ENABLE_WINDOW
    if (windowId_ >= 0) {
        data.winId = windowId_;
    }
#endif
    return false;
}

uint16_t PointEventInjector::GetLeftSize() const
{
    return MAX_LIST_SIZE - dataList_.Size();
}

#if ENABLE_WINDOW
void PointEventInjector::SetWindowId(int32_t windowId)
{
    windowId_ = windowId;
}
#endif
}
#endif // ENABLE_DEBUG