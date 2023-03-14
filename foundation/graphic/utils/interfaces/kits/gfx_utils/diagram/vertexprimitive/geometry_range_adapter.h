/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @addtogroup GraphicGeometry
 * @brief Defines Arc
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_RANGE_ADAPTER_H
#define GRAPHIC_LTE_GEOMETRY_RANGE_ADAPTER_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/heap_base.h"
#include "securec.h"

namespace OHOS {
const int32_t QUICK_SORT_THRESHOLD = 9;

/**
 *
 * @brief Exchange data.
 * @since 1.0
 * @version 1.0
 */
template <class T>
inline void SwapElements(T& firstValue, T& secondValue)
{
    T temp = firstValue;
    firstValue = secondValue;
    secondValue = temp;
}

template <class Array, class Less>
void ToCompareLess(Array& arr, Less less, int32_t iIndex, int32_t jIndex, int32_t base)
{
    while (true) {
        do {
            iIndex++;
        } while (less(arr[iIndex], arr[base]));
        do {
            jIndex--;
        } while (less(arr[base], arr[jIndex]));

        if (iIndex > jIndex) {
            break;
        }
        SwapElements(arr[iIndex], arr[jIndex]);
    }
}

template <class Array, class Less>
bool CompareLessThreshold(Array& arr, Less less, int32_t iIndex, int32_t jIndex,
                          int32_t base, int32_t limit, int32_t* top, int32_t* stack)
{
    auto e1 = &arr[0];
    auto e2 = &arr[0];
    jIndex = base;
    iIndex = jIndex + 1;
    for (; iIndex < limit; iIndex++) {
        for (; less(*(e1 = &(arr[jIndex + 1])), *(e2 = &(arr[jIndex]))); jIndex--) {
            SwapElements(*e1, *e2);
            if (jIndex == base) {
                break;
            }
        }
        jIndex = iIndex;
    }
    if (top > stack) {
        top -= TWO_STEP;
        base = top[0];
        limit = top[1];
        return false;
    } else {
        return true;
    }
}

/**
 *
 * @brief Quick sort algorithm.
 * @param arr Array to sort, less sort condition
 * @since 1.0
 * @version 1.0
 */
template <class Array, class Less>
void QuickSort(Array& arr, Less less);

/**
 *
 * @brief Deletes duplicate elements according to the specified criteria.
 *
 * @param arr Specify the array, equal specify the condition.
 * @since 1.0
 * @version 1.0
 */
template <class Array, class Equal>
uint32_t RemoveDuplicates(Array& arr, Equal equal);

/**
 *
 * @brief Invert an array.
 *
 * @since 1.0
 * @version 1.0
 */
template <class Array>
void InvertContainer(Array& arr)
{
    int32_t iIndex = 0;
    int32_t jIndex = arr.size() - 1;
    while (iIndex < jIndex) {
        SwapElements(arr[iIndex++], arr[jIndex--]);
    }
}

/**
 *
 * @brief Dichotomy search algorithm.
 *
 * @since 1.0
 * @version 1.0
 */
template <class Array, class Value, class Less>
uint32_t BinarySearchPos(const Array& arrData, const Value& val, Less less);

template <class Array, class Less>
void QuickSort(Array& arr, Less less)
{
    if (arr.Size() < 2) {
        return;
    }
    int32_t stack[80];
    int32_t* top = stack;
    int32_t limit = arr.Size();
    int32_t baseLimit = 0;
    while (true) {
        int32_t len = limit - baseLimit;
        int32_t iIndex;
        int32_t jIndex;
        int32_t pivot;
        if (len > QUICK_SORT_THRESHOLD) {
            pivot = baseLimit + len * HALFNUM;
            SwapElements(arr[baseLimit], arr[pivot]);
            iIndex = baseLimit + 1;
            jIndex = limit - 1;
            auto firstElement = &(arr[jIndex]);
            auto secondElement = &(arr[iIndex]);
            if (less(*firstElement, *secondElement)) {
                SwapElements(*firstElement, *secondElement);
            }
            firstElement = &(arr[baseLimit]);
            secondElement = &(arr[iIndex]);
            if (less(*firstElement, *secondElement)) {
                SwapElements(*firstElement, *secondElement);
            }
            firstElement = &(arr[jIndex]);
            secondElement = &(arr[baseLimit]);
            if (less(*firstElement, *secondElement))
                SwapElements(*firstElement, *secondElement);
            ToCompareLess(arr, less, iIndex, jIndex, baseLimit);
            SwapElements(arr[baseLimit], arr[jIndex]);
            if (jIndex - baseLimit > limit - iIndex) {
                top[0] = baseLimit;
                top[1] = jIndex;
                baseLimit = iIndex;
            } else {
                top[0] = iIndex;
                top[1] = limit;
                limit = jIndex;
            }
            top += TWO_STEP;
        } else {
            if (CompareLessThreshold(arr, less, iIndex, jIndex, baseLimit, limit, top, stack)) {
                break;
            }
        }
    }
}

template <class Array, class Value, class Less>
uint32_t BinarySearchPos(const Array& arrData, const Value& val, Less less)
{
    if (arrData.size() == 0) {
        return 0;
    }

    uint32_t arrayBegin = 0;
    uint32_t arrayEnd = arrData.size() - 1;

    if (less(val, arrData[0])) {
        return 0;
    }
    if (less(arrData[arrayEnd], val)) {
        return arrayEnd + 1;
    }

    while (arrayEnd - arrayBegin > 1) {
        uint32_t mid = (arrayEnd + arrayBegin) >> 1;
        if (less(val, arrData[mid])) {
            arrayEnd = mid;
        } else {
            arrayBegin = mid;
        }
    }

    return arrayEnd;
}

template <class Array, class Equal>
uint32_t RemoveDuplicates(Array& arr, Equal equal)
{
    const int32_t number = 2;
    if (arr.Size() < number) {
        return arr.Size();
    }
    uint32_t jIndex = 1;
    for (uint32_t iIndex = 1; iIndex < arr.Size(); iIndex++) {
        auto& element = arr[iIndex];
        if (!equal(element, arr[iIndex - 1])) {
            arr[jIndex++] = element;
        }
    }
    return jIndex;
}
} // namespace OHOS
#endif
