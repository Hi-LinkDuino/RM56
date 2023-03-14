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

#include "native_interface_xcomponent.h"

#include "frameworks/core/components/xcomponent/native_interface_xcomponent_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t OH_NativeXComponent_GetXComponentId(OH_NativeXComponent* component, char* id, uint64_t* size)
{
    if ((component == nullptr) || (id == nullptr) || (size == nullptr)) {
        return OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER;
    }
    if (((*size) == 0) || ((*size) > (OH_XCOMPONENT_ID_LEN_MAX + 1))) {
        LOGE("The referenced value of 'size' should be in the range (0, OH_XCOMPONENT_ID_LEN_MAX + 1]");
        return OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER;
    }
    return component->GetXComponentId(id, size);
}

int32_t OH_NativeXComponent_GetXComponentSize(
    OH_NativeXComponent* component, const void* window, uint64_t* width, uint64_t* height)
{
    if ((component == nullptr) || (window == nullptr) || (width == nullptr) || (height == nullptr)) {
        return OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER;
    }
    return component->GetXComponentSize(window, width, height);
}

int32_t OH_NativeXComponent_GetXComponentOffset(
    OH_NativeXComponent* component, const void* window, double* x, double* y)
{
    if ((component == nullptr) || (window == nullptr) || (x == nullptr) || (y == nullptr)) {
        return OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER;
    }
    return component->GetXComponentOffset(window, x, y);
}

int32_t OH_NativeXComponent_GetTouchEvent(
    OH_NativeXComponent* component, const void* window, OH_NativeXComponent_TouchEvent* touchEvent)
{
    if ((component == nullptr) || (window == nullptr) || (touchEvent == nullptr)) {
        return OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER;
    }
    return component->GetTouchEvent(window, touchEvent);
}

int32_t OH_NativeXComponent_RegisterCallback(OH_NativeXComponent* component, OH_NativeXComponent_Callback* callback)
{
    if ((component == nullptr) || (callback == nullptr)) {
        return OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER;
    }
    return component->RegisterCallback(callback);
}

#ifdef __cplusplus
};
#endif
