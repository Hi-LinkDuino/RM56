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

#include "drm_crtc.h"
#include "display_common.h"
#include "drm_device.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
DrmCrtc::DrmCrtc(drmModeCrtcPtr c, uint32_t pipe) : mId(c->crtc_id), mPipe(pipe) {}

int32_t DrmCrtc::Init(DrmDevice &drmDevice)
{
    DISPLAY_LOGD();
    int32_t ret;
    DrmProperty prop;
    ret = drmDevice.GetCrtcProperty(*this, PROP_MODEID, prop);
    mModePropId = prop.propId;
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("can not get mode prop id"));

    ret = drmDevice.GetCrtcProperty(*this, PROP_OUTFENCE, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("cat not get out fence prop id"));
    mOutFencePropId = prop.propId;

    ret = drmDevice.GetCrtcProperty(*this, PROP_ACTIVE, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("cat not get out fence prop id"));
    mActivePropId = prop.propId;

    return DISPLAY_SUCCESS;
}

int32_t DrmCrtc::BindToDisplay(uint32_t id)
{
    DISPLAY_CHK_RETURN((mDisplayId != INVALIDE_DISPLAY_ID), DISPLAY_FAILURE,
        DISPLAY_LOGE("the crtc has bind to %{public}d", mDisplayId));
    mDisplayId = id;
    return DISPLAY_SUCCESS;
}

void DrmCrtc::UnBindDisplay(uint32_t id)
{
    DISPLAY_LOGD();
    if (mDisplayId == id) {
        mDisplayId = INVALIDE_DISPLAY_ID;
    } else {
        DISPLAY_LOGE("can not unbind");
    }
}

bool DrmCrtc::CanBind()
{
    return (mDisplayId == INVALIDE_DISPLAY_ID);
}

int32_t DrmCrtc::SetActivieMode(int32_t id)
{
    DISPLAY_LOGD("set activie modeid to %{public}d", id);
    DISPLAY_CHK_RETURN((id > 0), DISPLAY_PARAM_ERR, DISPLAY_LOGE("id %{public}d is invalid ", id));
    if (mActiveModeId != id) {
        mNeedModeSet = true;
    }
    mActiveModeId = id;
    return DISPLAY_SUCCESS;
}
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY
