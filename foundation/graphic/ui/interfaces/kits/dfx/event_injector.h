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

/**
 * @addtogroup UI_DFX
 * @{
 *
 * @brief Provides test and analysis capabilities, such as stimulating input events and viewing information about a
 *        Document Object Model (DOM) tree.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file event_injector.h
 *
 * @brief Declares functions for simulating input events.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GRAPHIC_LITE_EVENT_INJECTOR_H
#define GRAPHIC_LITE_EVENT_INJECTOR_H

#include "graphic_config.h"
#if ENABLE_DEBUG
#include "gfx_utils/heap_base.h"
#include "gfx_utils/input_event_info.h"
namespace OHOS {
/**
 * @ Enumerates the event types.
 */
enum class EventDataType {
    /** Point event */
    POINT_TYPE,
    /** Key event */
    KEY_TYPE,
    /** Other events */
    OTHERS
};
class PointEventInjector;
class KeyEventInjector;

/**
 * @brief Manages all types of simulated input events, registers and unregisters event injectors, and simulates
 * input events.
 *
 * @since 1.0
 * @version 1.0
 */
class EventInjector : public HeapBase {
public:
    /**
     * @brief Obtains a singleton <b>EventInjector</b> instance.
     *
     * @return Returns the <b>EventInjector</b> instance.
     * @since 1.0
     * @version 1.0
     */
    static EventInjector* GetInstance();

    /**
     * @brief Registers an event injector of a specified type.
     *
     * @param type Indicates the event type. For details, see {@link EventDataType}.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool RegisterEventInjector(EventDataType type);

    /**
     * @brief Unregisters an event injector of a specified type.
     *
     * @param type Indicates the event type. For details, see {@link EventDataType}.
     * @since 1.0
     * @version 1.0
     */
    void UnregisterEventInjector(EventDataType type);

    /**
     * @brief Checks whether the event injector of a specified type is registered.
     *
     * @param type Indicates the event type. For details, see {@link EventDataType}.
     * @return Returns <b>true</b> if the device is registered; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsEventInjectorRegistered(EventDataType type) const;

    /**
     * @brief Uses a data array of a specified length to simulate an input event of a specified type.
     *
     * @param dataArray Indicates the pointer to the data array used for simulating the event.
     * @param arrayLength Indicates the length of the data array.
     * @param type Indicates the event type. For details, see {@link EventDataType}.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetInjectEvent(const DeviceData* dataArray, uint16_t arrayLength, EventDataType type);

    /**
     * @brief Stimulates a click event.
     *
     * @param clickPoint Indicates the coordinates of the click point.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 3.0
     * @version 5.0
     */
    bool SetClickEvent(const Point& clickPoint);

    /**
     * @brief Stimulates a long press event.
     *
     * @param longPressPoint Indicates the coordinates of the long press point.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 3.0
     * @version 5.0
     */
    bool SetLongPressEvent(const Point& longPressPoint);

    /**
     * @brief Stimulates a drag event that occurs between two points.
     *
     * @param startPoint Indicates the coordinates of the start point.
     * @param endPoint Indicates the coordinates of the end point.
     * @param dragTime Indicates the duration of dragging from the start point to the end point, in milliseconds.
     *                 The value range is [2, 499] x {@link INDEV_READ_PERIOD}.
     *                 The shorter the duration is, the faster the sliding is.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 3.0
     * @version 5.0
     */
    bool SetDragEvent(const Point& startPoint, const Point& endPoint, uint32_t dragTime);

    /**
     * @brief Simulates a key event.
     *
     * @param keyId Indicates the key ID.
     * @param state Indicates the key state. For details, see {@link InputDevice}.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 3.0
     * @version 5.0
     */
    bool SetKeyEvent(uint16_t keyId, uint16_t state);

#if ENABLE_WINDOW
    /**
     * @brief Set id of mock window.
     *
     * @param windowId id of mock window
     *
     * @since 3.0
     * @version 5.0
     */
    void SetWindowId(uint8_t windowId);
#endif

private:
    EventInjector() : pointEventInjector_(nullptr), keyEventInjector_(nullptr) {}
    virtual ~EventInjector();

    EventInjector(const EventInjector&) = delete;
    EventInjector& operator=(const EventInjector&) = delete;
    EventInjector(EventInjector&&) = delete;
    EventInjector& operator=(EventInjector&&) = delete;

    PointEventInjector* pointEventInjector_;
    KeyEventInjector* keyEventInjector_;
};
} // namespace OHOS
#endif // ENABLE_DEBUG
#endif // GRAPHIC_LITE_EVENT_INJECTOR_H
