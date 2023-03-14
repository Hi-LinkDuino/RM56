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

#ifndef DRM_ENCODER_H
#define DRM_ENCODER_H
#include <string>
#include <memory>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <display_type.h>
#include <drm_crtc.h>

namespace OHOS {
namespace HDI {
namespace DISPLAY {
class DrmEncoder {
public:
    explicit DrmEncoder(drmModeEncoder e);
    virtual ~DrmEncoder() {}
    uint32_t GetCrtcId() const
    {
        return mCrtcId;
    }
    void SetCrtcId(uint32_t id)
    {
        mCrtcId = id;
    }
    uint32_t GetPossibleCrtcs() const
    {
        return mPossibleCrtcs;
    }
    int32_t PickIdleCrtcId(IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId);
    uint32_t GetId() const
    {
        return mEncoderId;
    }

private:
    uint32_t mEncoderId;
    uint32_t mCrtcId;
    uint32_t mPossibleCrtcs;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // DRM_ENCODER_H