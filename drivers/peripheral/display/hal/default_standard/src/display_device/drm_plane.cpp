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

#include "drm_plane.h"
#include "drm_device.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
DrmPlane::DrmPlane(drmModePlane &p)
    : mId(p.plane_id), mPossibleCrtcs(p.possible_crtcs), mFormats(p.formats, p.formats + p.count_formats)
{}

DrmPlane::~DrmPlane()
{
    DISPLAY_LOGD();
}

int32_t DrmPlane::Init(DrmDevice &drmDevice)
{
    DISPLAY_LOGD();
    int32_t ret;
    DrmProperty prop;
    ret = drmDevice.GetPlaneProperty(*this, PROP_FBID, prop);
    mPropFbId = prop.propId;
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("can not get plane fb id"));
    ret = drmDevice.GetPlaneProperty(*this, PROP_IN_FENCE_FD, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("cat not get plane in fence prop id"));
    mPropFenceInId = prop.propId;
    ret = drmDevice.GetPlaneProperty(*this, PROP_CRTC_ID, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("cat not get pane crtc prop id"));
    mPropCrtcId = prop.propId;
    ret = drmDevice.GetPlaneProperty(*this, PROP_TYPE, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("cat not get pane crtc prop id"));
    switch (prop.value) {
        case DRM_PLANE_TYPE_OVERLAY:
        case DRM_PLANE_TYPE_PRIMARY:
        case DRM_PLANE_TYPE_CURSOR:
            mType = static_cast<uint32_t>(prop.value);
            break;
        default:
            DISPLAY_LOGE("unknown type value %{public}" PRIu64 "", prop.value);
            return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY
