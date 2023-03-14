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

#include "core/components/shadow/render_shadow.h"

#include "base/utils/system_properties.h"

namespace OHOS::Ace {

void RenderShadow::PerformLayout()
{
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

void RenderShadow::SetShadowProperties(const RRect& rrect, const Offset& offset)
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        return;
    }
    rrect_ = rrect;
    offset_ = offset - GetGlobalOffset();
    isDisplay_ = true;
    MarkNeedRender(true);
}

void RenderShadow::SetShadowProperties(const RRect& rrect, const Offset& offset, const Rect& clipRect)
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        return;
    }
    clipRect_ = clipRect;
    isNeedClip_ = true;
    rrect_ = rrect;
    offset_ = offset - GetGlobalOffset();
    isDisplay_ = true;
    MarkNeedRender(true);
}

void RenderShadow::CancelShadow()
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        return;
    }
    isDisplay_ = false;
    MarkNeedRender(true);
}

} // namespace OHOS::Ace
