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

#include "dfx/key_event_injector.h"
#if ENABLE_DEBUG
#include "gfx_utils/graphic_log.h"

namespace {
const uint8_t MAX_LIST_SIZE = 100;
}

namespace OHOS {
KeyEventInjector::~KeyEventInjector()
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

bool KeyEventInjector::Read(DeviceData& data)
{
    if (dataList_.IsEmpty()) {
        lastState_ = INVALID_KEY_STATE;
    }
    if (dataList_.Size() > 0) {
        lastKeyId_ = dataList_.Front()->keyId;
        lastState_ = dataList_.Front()->state;

        ListNode<DeviceData*>* node = dataList_.Begin();
        if (node->data_ != nullptr) {
            DeviceData* deleteData = node->data_;
            delete deleteData;
        }
        dataList_.PopFront();
    }
    data.keyId = lastKeyId_;
    data.state = lastState_;
    return false;
}

bool KeyEventInjector::SetKey(const DeviceData& data)
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
    tmpData->keyId = data.keyId;
    tmpData->state = data.state;
    dataList_.PushBack(tmpData);
    return true;
}

uint8_t KeyEventInjector::GetLeftSize() const
{
    return MAX_LIST_SIZE - dataList_.Size();
}
}
#endif // ENABLE_DEBUG
