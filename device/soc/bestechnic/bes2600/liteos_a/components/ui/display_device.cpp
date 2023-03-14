/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "display_device.h"
#include "components/root_view.h"
#include "draw/draw_utils.h"
#include "fbdev.h"

namespace OHOS
{
DisplayDevice *DisplayDevice::GetInstance()
{
    static DisplayDevice instance;
    if (!instance.isRegister_) {
        FbdevInit();
        instance.isRegister_ = true;
#ifdef CONFIG_DISPLAY_RM69330
        instance.SetScreenShape(CIRCLE);
#endif
    }
    return &instance;
}

BufferInfo *DisplayDevice::GetFBBufferInfo()
{
    static BufferInfo bufferInfo;
    LiteSurfaceData *surfaceData = GetDevSurfaceData();
    bufferInfo.rect = {0, 0, HORIZONTAL_RESOLUTION - 1, VERTICAL_RESOLUTION - 1};
    bufferInfo.mode = ARGB8888;
    bufferInfo.color = 0x44;
    bufferInfo.phyAddr = surfaceData->phyAddr;
    bufferInfo.virAddr = surfaceData->virAddr;
    // 3: Shift right 3 bits
    bufferInfo.stride = HORIZONTAL_RESOLUTION * (DrawUtils::GetPxSizeByColorMode(bufferInfo.mode) >> 3);
    bufferInfo.width = HORIZONTAL_RESOLUTION;
    bufferInfo.height = VERTICAL_RESOLUTION;
    this->fbAddr = surfaceData->phyAddr;
    return &bufferInfo;
}

void DisplayDevice::UpdateFBBuffer()
{
    BufferInfo *bufferInfo = DisplayDevice::GetInstance()->GetFBBufferInfo();
    if (this->fbAddr != bufferInfo->phyAddr) {
        this->fbAddr = bufferInfo->phyAddr;
        RootView::GetInstance()->UpdateBufferInfo(bufferInfo);
        // Get the newest fbAddr, ensure that the buffer is available before TaskHandler()
    }
}

void DisplayDevice::Flush()
{
    FbdevFlush();
}

} // namespace OHOS
