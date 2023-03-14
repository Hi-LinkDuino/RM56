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

#include "hdi_drm_layer.h"
#include <cinttypes>
#include <errno.h>
#include "drm_device.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
DrmGemBuffer::DrmGemBuffer(int drmfd, HdiLayerBuffer &hdl) : mDrmFd(drmfd)
{
    DISPLAY_LOGD();
    Init(mDrmFd, hdl);
}

void DrmGemBuffer::Init(int drmFd, HdiLayerBuffer &hdl)
{
    int ret;
    const int MAX_COUNT = 4;
    uint32_t pitches[MAX_COUNT] = {0};
    uint32_t gemHandles[MAX_COUNT] = {0};
    uint32_t offsets[MAX_COUNT] = {0};
    DISPLAY_LOGD("hdl %{public}" PRIx64 "", hdl.GetMemHandle());
    DISPLAY_CHK_RETURN_NOT_VALUE((drmFd < 0), DISPLAY_LOGE("can not init drmfd %{public}d", drmFd));
    mDrmFormat = DrmDevice::ConvertToDrmFormat(static_cast<PixelFormat>(hdl.GetFormat()));
    ret = drmPrimeFDToHandle(drmFd, hdl.GetFb(), &mGemHandle);
    DISPLAY_CHK_RETURN_NOT_VALUE((ret != 0), DISPLAY_LOGE("can not get handle errno %{public}d", errno));

    pitches[0] = hdl.GetStride();
    gemHandles[0] = mGemHandle;
    offsets[0] = 0;
    ret = drmModeAddFB2(drmFd, hdl.GetWight(), hdl.GetHeight(), mDrmFormat, gemHandles, pitches, offsets, &mFdId, 0);
    DISPLAY_LOGD("mGemHandle %{public}d  mFdId %{public}d", mGemHandle, mFdId);
    DISPLAY_LOGD("w: %{public}d  h: %{public}d mDrmFormat : %{public}d gemHandles: %{public}d pitches: %{public}d "
        "offsets: %{public}d",
        hdl.GetWight(), hdl.GetHeight(), mDrmFormat, gemHandles[0], pitches[0], offsets[0]);
    DISPLAY_CHK_RETURN_NOT_VALUE((ret != 0), DISPLAY_LOGE("can not add fb errno %{public}d", errno));
}

DrmGemBuffer::~DrmGemBuffer()
{
    DISPLAY_LOGD();
    if (mFdId) {
        if (drmModeRmFB(mDrmFd, mFdId)) {
            DISPLAY_LOGE("can not free fdid %{public}d errno %{public}d", mFdId, errno);
        }
    }

    if (mGemHandle) {
        struct drm_gem_close gemClose = { 0 };
        gemClose.handle = mGemHandle;
        if (drmIoctl(mDrmFd, DRM_IOCTL_GEM_CLOSE, &gemClose)) {
            DISPLAY_LOGE("can not free gem handle %{public}d errno : %{public}d", mGemHandle, errno);
        }
    }
}

bool DrmGemBuffer::IsValid()
{
    DISPLAY_LOGD();
    return (mGemHandle != INVALID_DRM_ID) && (mFdId != INVALID_DRM_ID);
}

DrmGemBuffer *HdiDrmLayer::GetGemBuffer()
{
    DISPLAY_LOGD();
    std::unique_ptr<DrmGemBuffer> ptr = std::make_unique<DrmGemBuffer>(DrmDevice::GetDrmFd(), *GetCurrentBuffer());
    lastBuffer_ = std::move(mCurrentBuffer);
    mCurrentBuffer = std::move(ptr);
    return mCurrentBuffer.get();
}
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY
