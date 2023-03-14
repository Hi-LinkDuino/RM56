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

#ifndef DRM_PLANE_H
#define DRM_PLANE_H
#include <cinttypes>
#include <string>
#include <vector>
#include <xf86drm.h>
#include <xf86drmMode.h>

namespace OHOS {
namespace HDI {
namespace DISPLAY {
const std::string PROP_FBID = "FB_ID";
const std::string PROP_IN_FENCE_FD = "IN_FENCE_FD";
const std::string PROP_CRTC_ID = "CRTC_ID";
const std::string PROP_TYPE = "type";
class DrmDevice;

class DrmPlane {
public:
    explicit DrmPlane(drmModePlane &p);
    virtual ~DrmPlane();
    int32_t Init(DrmDevice &drmDevice);
    uint32_t GetId() const
    {
        return mId;
    }
    uint32_t GetPropFbId() const
    {
        return mPropFbId;
    }
    uint32_t GetPropFenceInId() const
    {
        return mPropFenceInId;
    }
    uint32_t GetPropCrtcId() const
    {
        return mPropCrtcId;
    }
    uint32_t GetPossibleCrtcs() const
    {
        return mPossibleCrtcs;
    }
    uint32_t GetType() const
    {
        return mType;
    }
    void BindToPipe(uint32_t pipe)
    {
        mPipe = pipe;
    }
    void UnBindPipe()
    {
        mPipe = 0;
    }
    bool IsIdle() const
    {
        return (mPipe == 0);
    }

private:
    uint32_t mId = 0;
    uint32_t mPossibleCrtcs = 0;
    uint32_t mPropFbId = 0;
    uint32_t mPropFenceInId = 0;
    uint32_t mPropCrtcId = 0;
    uint32_t mPipe = 0;
    uint32_t mType = 0;
    std::vector<uint32_t> mFormats;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // DRM_PLANE_H