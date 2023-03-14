/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "system_info.h"

#include "ace_log.h"
#include "product_adapter.h"
#include "screen.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
SystemInfo &SystemInfo::GetInstance()
{
    static SystemInfo systemInfo;
    return systemInfo;
}

void SystemInfo::Initialize()
{
    // reset
    screenWidth_ = screenHeight_ = 0;
    ProductAdapter::GetScreenSize(screenWidth_, screenHeight_);
    if (screenWidth_ != 0 && screenHeight_ != 0) {
        aspectRatio_ = (float)screenWidth_ / (float)screenHeight_;
    } else {
        aspectRatio_ = 0;
    }
    isRoundScreen_ = (Screen::GetInstance().GetScreenShape() == ScreenShape::CIRCLE);
    deviceType_ = ProductAdapter::GetDeviceType();
}

float SystemInfo::GetAspectRatio() const
{
    return aspectRatio_;
}

uint16_t SystemInfo::GetScreenHeight() const
{
    return screenHeight_;
}

uint16_t SystemInfo::GetScreenWidth() const
{
    return screenWidth_;
}

const char *SystemInfo::GetDeviceType() const
{
    return deviceType_;
}

bool SystemInfo::IsRoundScreen() const
{
    return isRoundScreen_;
}
} // namespace ACELite
} // namespace OHOS
