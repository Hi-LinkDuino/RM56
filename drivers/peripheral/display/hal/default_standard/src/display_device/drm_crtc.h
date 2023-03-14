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

#ifndef DRM_CRTC_H
#define DRM_CRTC_H
#include <string>
#include <memory>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "hdi_device_common.h"
#include "hdi_display.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
const std::string PROP_ACTIVE = "ACTIVE";
const std::string PROP_MODEID = "MODE_ID";
const std::string PROP_OUTFENCE = "OUT_FENCE_PTR";

class DrmDevice;

class DrmCrtc {
public:
    DrmCrtc(drmModeCrtcPtr c, uint32_t pipe);
    virtual ~DrmCrtc() {}
    int32_t BindToDisplay(uint32_t id);
    void UnBindDisplay(uint32_t id);
    bool CanBind();
    uint32_t GetId() const
    {
        return mId;
    }
    uint32_t GetModePropId() const
    {
        return mModePropId;
    }
    uint32_t GetOutFencePropId() const
    {
        return mOutFencePropId;
    }
    uint32_t GetActivePropId() const
    {
        return mActivePropId;
    }
    uint32_t GetPipe() const
    {
        return mPipe;
    }
    int32_t Init(DrmDevice &drmDevice);
    int32_t SetActivieMode(int32_t id);
    int32_t GetActiveModeId() const
    {
        return mActiveModeId;
    }
    bool NeedModeSet()
    {
        return mNeedModeSet;
    }

private:
    uint32_t mId = 0;
    uint32_t mModePropId = 0;
    uint32_t mOutFencePropId = 0;
    uint32_t mActivePropId = 0;
    uint32_t mDisplayId = INVALIDE_DISPLAY_ID;
    uint32_t mPipe = 0;
    int32_t mActiveModeId = INVALID_MODE_ID;
    bool mNeedModeSet = false;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // DRM_CRTC_H