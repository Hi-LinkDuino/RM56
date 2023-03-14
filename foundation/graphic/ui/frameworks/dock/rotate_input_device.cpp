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

#include "dock/rotate_input_device.h"
#include "gfx_utils/graphic_log.h"
#include "dock/focus_manager.h"


#if ENABLE_ROTATE_INPUT
namespace {
#ifdef _WIN32
constexpr int16_t ROTATE_INPUT_THRESHOLD = 1;
#else
constexpr int16_t ROTATE_INPUT_THRESHOLD = 10;
#endif
}

namespace OHOS {
RotateInputDevice::RotateInputDevice() : rotateStart_(false), threshold_(ROTATE_INPUT_THRESHOLD), cachedRotation_(0) {}

void RotateInputDevice::DispatchEvent(const DeviceData& data)
{
    bool cachedToRotate = false;
    if (data.rotate == 0 || rotateStart_) {
        cachedRotation_ = 0;
    } else {
        cachedRotation_ += data.rotate;
        if (MATH_ABS(cachedRotation_) >= threshold_) {
            cachedToRotate = true;
        }
    }

    if (!cachedToRotate && !rotateStart_) {
        return;
    }

    UIView* view = FocusManager::GetInstance()->GetFocusedView();
    if (view == nullptr) {
        GRAPHIC_LOGE("RotateInputDevice Failed to dispatch event without focused view!\n");
        return;
    }

    UIView* par = view;
    while (par->GetParent() != nullptr) {
        if (!par->IsVisible()) {
            return;
        }
        par = par->GetParent();
    }
    if (par->GetViewType() != UI_ROOT_VIEW) {
        GRAPHIC_LOGW("RotateInputDevice failed to dispatch event without target view attached!\n");
        return;
    }

    if (data.rotate == 0 && rotateStart_) {
        view->OnRotateEndEvent(0);
        rotateStart_ = false;
        GRAPHIC_LOGW("RotateInputDevice dispatched 0-value event!\n");
        return;
    }
    if (!rotateStart_) {
        view->OnRotateStartEvent(data.rotate);
    }
    view->OnRotateEvent(data.rotate);
    rotateStart_ = true;
    GRAPHIC_LOGI("RotateInputDevice dispatched rotate event, targetView Type = %{public}d,\
        rotate value = %{public}d\n!", static_cast<uint8_t>(view->GetViewType()), data.rotate);
}
} // namespace OHOS
#endif
