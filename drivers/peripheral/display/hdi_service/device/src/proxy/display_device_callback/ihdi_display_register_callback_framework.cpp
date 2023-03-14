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

#include "ihdi_display_register_callback_framework.h"

int32_t DisplayRegisterCallbackFramework::OnHotplugIn(uint32_t outputId, bool connected)
{
    DISPLAY_START;
    if (hotCallback_) {
        hotCallback_(outputId, connected, hotCallbackData_);
    }
    DISPLAY_END;
    return 0;
}

void DisplayRegisterCallbackFramework::SetHotplugInData(HotPlugCallback callback, void* data)
{
    DISPLAY_START;
    hotCallback_ = callback;
    hotCallbackData_ = data;
    DISPLAY_END;
}

int32_t DisplayRegisterCallbackFramework::OnVBlankCallback(unsigned int sequence, uint64_t ns)
{
    DISPLAY_START;
    if (vBlankCallback_) {
        vBlankCallback_(sequence, ns, vBlankCallbackData_);
    }
    DISPLAY_END;
    return 0;
}
void DisplayRegisterCallbackFramework::SetVBlankData(VBlankCallback callback, void* data)
{
    DISPLAY_START;
    vBlankCallback_ = callback;
    vBlankCallbackData_ = data;
    DISPLAY_END;
}

int32_t DisplayRegisterCallbackFramework::OnRefreshCallback(uint32_t devId)
{
    DISPLAY_START;
    if (refreshCallback_) {
        refreshCallback_(devId, refreshCallbackData_);
    }
    DISPLAY_END;
    return 0;
}
void DisplayRegisterCallbackFramework::SetRefreshData(RefreshCallback callback, void* data)
{
    DISPLAY_START;
    refreshCallback_ = callback;
    refreshCallbackData_ = data;
    DISPLAY_END;
}
