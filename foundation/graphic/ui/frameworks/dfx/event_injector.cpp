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

#include "dfx/event_injector.h"
#if ENABLE_DEBUG
#include "common/input_device_manager.h"
#include "dfx/key_event_injector.h"
#include "dfx/point_event_injector.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
EventInjector::~EventInjector()
{
    if (pointEventInjector_ != nullptr) {
        InputDeviceManager::GetInstance()->Remove(pointEventInjector_);
        delete pointEventInjector_;
        pointEventInjector_ = nullptr;
    }
    if (keyEventInjector_ != nullptr) {
        InputDeviceManager::GetInstance()->Remove(keyEventInjector_);
        delete keyEventInjector_;
        keyEventInjector_ = nullptr;
    }
}

EventInjector* EventInjector::GetInstance()
{
    static EventInjector instance;
    return &instance;
}

bool EventInjector::RegisterEventInjector(EventDataType type)
{
    switch (type) {
        case EventDataType::POINT_TYPE:
            if (pointEventInjector_ == nullptr) {
                pointEventInjector_ = new PointEventInjector();
                if (pointEventInjector_ == nullptr) {
                    GRAPHIC_LOGE("EventInjector::RegisterEventInjector register pointEventInjector failed Err!\n");
                    return false;
                }
                InputDeviceManager::GetInstance()->Add(pointEventInjector_);
            }
            return true;
        case EventDataType::KEY_TYPE:
            if (keyEventInjector_ == nullptr) {
                keyEventInjector_ = new KeyEventInjector();
                if (keyEventInjector_ == nullptr) {
                    GRAPHIC_LOGE("EventInjector::RegisterEventInjector register keyEventInjector failed Err!\n");
                    return false;
                }
                InputDeviceManager::GetInstance()->Add(keyEventInjector_);
            }
            return true;
        default:
            break;
    }
    return false;
}

void EventInjector::UnregisterEventInjector(EventDataType type)
{
    switch (type) {
        case EventDataType::POINT_TYPE:
            if (pointEventInjector_ != nullptr) {
                InputDeviceManager::GetInstance()->Remove(pointEventInjector_);
                delete pointEventInjector_;
                pointEventInjector_ = nullptr;
            }
            break;
        case EventDataType::KEY_TYPE:
            if (keyEventInjector_ != nullptr) {
                InputDeviceManager::GetInstance()->Remove(keyEventInjector_);
                delete keyEventInjector_;
                keyEventInjector_ = nullptr;
            }
            break;
        default:
            break;
    }
}

bool EventInjector::IsEventInjectorRegistered(EventDataType type) const
{
    switch (type) {
        case EventDataType::POINT_TYPE:
            if (pointEventInjector_ != nullptr) {
                return true;
            }
            break;
        case EventDataType::KEY_TYPE:
            if (keyEventInjector_ != nullptr) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

bool EventInjector::SetInjectEvent(const DeviceData* dataArray, uint16_t arrayLength, EventDataType type)
{
    if (dataArray == nullptr) {
        return false;
    }
    switch (type) {
        case EventDataType::POINT_TYPE:
            if (pointEventInjector_ == nullptr) {
                return false;
            }
            for (uint16_t i = 0; i < arrayLength; i++) {
                if (!pointEventInjector_->SetPointEvent(dataArray[i])) {
                    return false;
                }
            }
            break;
        case EventDataType::KEY_TYPE:
            if (keyEventInjector_ == nullptr) {
                return false;
            }
            for (uint16_t i = 0; i < arrayLength; i++) {
                if (!keyEventInjector_->SetKey(dataArray[i])) {
                    return false;
                }
            }
            break;
        default:
            return false;
    }
    return true;
}

bool EventInjector::SetClickEvent(const Point& clickPoint)
{
    uint16_t clickArrayLen = 2; /* 2:click event point */
    if (clickArrayLen > pointEventInjector_->GetLeftSize()) {
        GRAPHIC_LOGE("front points need to be read.(left size in pointer queue is not enough)");
        return false;
    }
    bool setResult = true;
    DeviceData* dataArray = new DeviceData[clickArrayLen];
    if (dataArray == nullptr) {
        return false;
    }
    dataArray[0].point = clickPoint;
    dataArray[0].state = InputDevice::STATE_PRESS;
    dataArray[1].point = clickPoint;
    dataArray[1].state = InputDevice::STATE_RELEASE;
    if (!SetInjectEvent(dataArray, clickArrayLen, EventDataType::POINT_TYPE)) {
        setResult = false;
    }
    delete[] dataArray;
    return setResult;
}

bool EventInjector::SetLongPressEvent(const Point& longPressPoint)
{
    uint16_t pointCount = INDEV_LONG_PRESS_TIME / INDEV_READ_PERIOD + 1;
    if (pointCount > pointEventInjector_->GetLeftSize()) {
        GRAPHIC_LOGE("front points need to be read.(left size in pointer queue is not enough)");
        return false;
    }
    bool setResult = true;
    DeviceData* dataArray = new DeviceData[pointCount];
    if (dataArray == nullptr) {
        return false;
    }
    for (uint16_t i = 0; i < pointCount; i++) {
        dataArray[i].point = longPressPoint;
        dataArray[i].state = InputDevice::STATE_PRESS;
    }
    dataArray[pointCount - 1].state = InputDevice::STATE_RELEASE;
    if (!SetInjectEvent(dataArray, pointCount, EventDataType::POINT_TYPE)) {
        setResult = false;
    }
    delete[] dataArray;
    return setResult;
}

bool EventInjector::SetDragEvent(const Point& startPoint, const Point& endPoint, uint32_t dragTime)
{
    uint16_t pointCount = (dragTime / INDEV_READ_PERIOD) + 1;
    /* 3: at least 3 points in drag event */
    if (pointCount < 3) {
        GRAPHIC_LOGE("dragTime is too short.(drag event needs at least 3 points)");
        return false;
    }
    if (pointCount > pointEventInjector_->GetLeftSize()) {
        GRAPHIC_LOGE("dragTime is too long or front points need to be read.(left size in pointer queue is not enough)");
        return false;
    }
    bool setResult = true;
    int16_t negativeFlag = 1; /* 1:represent the coordinate (x, y) of endPoint is larger than startPoint. */
    DeviceData* dataArray = new DeviceData[pointCount];
    if (dataArray == nullptr) {
        return false;
    }
    if (startPoint.x == endPoint.x) {
        float pointStep = static_cast<float>(MATH_ABS(endPoint.y - startPoint.y)) / (pointCount - 1);
        if (endPoint.y < startPoint.y) {
            negativeFlag = -1; /* -1:represent the coordinate y of endPoint is smaller than startPoint. */
        }
        for (uint16_t i = 0; i < pointCount; i++) {
            dataArray[i].point.x = startPoint.x;
            dataArray[i].point.y = startPoint.y + static_cast<int16_t>(i * negativeFlag * pointStep);
            dataArray[i].state = InputDevice::STATE_PRESS;
        }
    } else {
        float slope = static_cast<float>(endPoint.y - startPoint.y) / (endPoint.x - startPoint.x);
        int16_t constPara = startPoint.y - static_cast<int16_t>(slope * startPoint.x);
        float pointStep = static_cast<float>(MATH_ABS(endPoint.x - startPoint.x)) / (pointCount - 1);
        if (endPoint.x < startPoint.x) {
            negativeFlag = -1; /* -1:represent the coordinate x of endPoint is smaller than startPoint. */
        }
        for (uint16_t i = 0; i < pointCount; i++) {
            dataArray[i].point.x = startPoint.x + static_cast<int16_t>(i * negativeFlag * pointStep);
            dataArray[i].point.y = static_cast<int16_t>(slope * (dataArray[i].point.x)) + constPara;
            dataArray[i].state = InputDevice::STATE_PRESS;
        }
    }
    dataArray[pointCount - 1].point = endPoint;
    dataArray[pointCount - 1].state = InputDevice::STATE_RELEASE;
    if (!SetInjectEvent(dataArray, pointCount, EventDataType::POINT_TYPE)) {
        setResult = false;
    }
    delete[] dataArray;
    return setResult;
}

bool EventInjector::SetKeyEvent(uint16_t keyId, uint16_t state)
{
    uint16_t kevArrayLen = 1;
    if (kevArrayLen > keyEventInjector_->GetLeftSize()) {
        GRAPHIC_LOGE("front key event need to be read.(left size in key event queue is not enough)");
        return false;
    }
    bool setResult = true;
    DeviceData* dataArray = new DeviceData[kevArrayLen];
    if (dataArray == nullptr) {
        return false;
    }
    for (uint16_t i = 0; i < kevArrayLen; i++) {
        dataArray[i].keyId = keyId;
        dataArray[i].state = state;
    }
    if (!SetInjectEvent(dataArray, kevArrayLen, EventDataType::KEY_TYPE)) {
        setResult = false;
    }
    delete[] dataArray;
    return setResult;
}

#if ENABLE_WINDOW
void EventInjector::SetWindowId(uint8_t windowId)
{
    if (pointEventInjector_ != nullptr) {
        pointEventInjector_->SetWindowId(windowId);
    }
}
#endif
} // namespace OHOS
#endif // ENABLE_DEBUG