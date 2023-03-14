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

#include "drm_connector.h"
#include <cinttypes>
#include <securec.h>
#include "drm_device.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
void DrmMode::ConvertToHdiMode(DisplayModeInfo &hdiMode)
{
    hdiMode.height = mModeInfo.vdisplay;
    hdiMode.width = mModeInfo.hdisplay;
    hdiMode.freshRate = mModeInfo.vrefresh;
    hdiMode.id = mId;
}

DrmConnector::DrmConnector(drmModeConnector c, FdPtr &fd)
    : mId(c.connector_id),
      mPhyWidth(c.mmWidth),
      mPhyHeight(c.mmHeight),
      mEncoderId(c.encoder_id),
      mConnectState(c.connection),
      mDrmFdPtr(fd)
{
    DISPLAY_LOGD("encoder_id %{public}d", mEncoderId);
    DISPLAY_LOGD("the connect state is %{public}d", mConnectState);

    for (int i = 0; i < c.count_encoders; i++) {
        mPossibleEncoders.push_back(c.encoders[i]);
        DISPLAY_LOGD("add possible encoder id %{public}d", c.encoders[i]);
    }

    ConvertToHdiType(c.connector_type, mType);
    ConvertTypeToName(mType, mName);
    InitModes(c);
    DISPLAY_LOGD("name %{public}s", mName.c_str());
}

void DrmConnector::InitModes(drmModeConnector c)
{
    DISPLAY_LOGD("id %{public}d", mId);
    mModes.clear();
    mPreferenceId = INVALID_MODE_ID;
    for (int i = 0; i < c.count_modes; i++) {
        drmModeModeInfoPtr mode = c.modes + i;
        DISPLAY_LOGD("mode: hdisplay %{public}d, vdisplay %{public}d vrefresh %{public}d type %{public}d",
            mode->hdisplay, mode->vdisplay, mode->vrefresh, mode->type);
        if ((mPreferenceId == INVALID_MODE_ID) && (mode->type & DRM_MODE_TYPE_PREFERRED)) {
            DISPLAY_LOGD("set it to prefernce id %{public}d", i);
            mPreferenceId = i;
        }
        mModes.emplace(i, DrmMode { *mode, i });
    }
    DISPLAY_LOGD("mode count %{public}zd", mModes.size());
}

int32_t DrmConnector::Init(DrmDevice &drmDevice)
{
    int32_t ret;
    DrmProperty prop;
    DISPLAY_LOGD();
    DISPLAY_CHK_RETURN((mDrmFdPtr == nullptr), DISPLAY_FAILURE, DISPLAY_LOGE("the mDrmFdPtr is NULL"));
    DISPLAY_CHK_RETURN((mDrmFdPtr->GetFd() == -1), DISPLAY_FAILURE, DISPLAY_LOGE("the drm fd is -1"));
    // find dpms prop
    ret = drmDevice.GetConnectorProperty(*this, PROP_DPMS, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("can not get mode prop id"));
    mPropDpmsId = prop.propId;
    mDpmsState = prop.value;
    DISPLAY_LOGD("dpms state : %{public}" PRIu64 "", mDpmsState);
    // find the crtcid
    ret = drmDevice.GetConnectorProperty(*this, PROP_CRTCID, prop);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("cat not get out fence prop id"));
    mPropCrtcId = prop.propId;
    DISPLAY_LOGD("encoder_id %{public}d", mEncoderId);
    DISPLAY_LOGD("mPropCrtcId %{public}d", mPropCrtcId);
    // find the brightness
    ret = drmDevice.GetConnectorProperty(*this, PROP_BRIGHTNESS, prop);
    if (ret == DISPLAY_SUCCESS) {
        mPropBrightnessId =  prop.propId;
        mBrightnessLevel = static_cast<uint32_t>(prop.value);
        DISPLAY_LOGD("prop brightness is %{public}d, level is %{public}d", mPropBrightnessId, mBrightnessLevel);
    } else {
        DISPLAY_LOGW("can not get the brightness prop, can not set the brightness");
    }
    return DISPLAY_SUCCESS;
}

int32_t DrmConnector::GetBrightness(uint32_t& level) {
    if (mPropBrightnessId == DRM_INVALID_ID) {
        DISPLAY_LOGE("the prop id of brightness is invalid");
        return DISPLAY_NOT_SUPPORT;
    }
    level = mBrightnessLevel;
    return DISPLAY_SUCCESS;
}

int32_t DrmConnector::SetBrightness(uint32_t level)
{
    DISPLAY_LOGD("set %{public}d", level);
    if (mPropBrightnessId == DRM_INVALID_ID) {
        DISPLAY_LOGE("the prop id of brightness is invalid");
        return DISPLAY_NOT_SUPPORT;
    }
    int ret = drmModeConnectorSetProperty(mDrmFdPtr->GetFd(), mId, mPropBrightnessId, level);
    DISPLAY_CHK_RETURN((ret != 0), DISPLAY_FAILURE, DISPLAY_LOGE("can not set dpms"));
    mBrightnessLevel = level;
    return DISPLAY_SUCCESS;
}

void DrmConnector::GetDisplayCap(DisplayCapability &cap)
{
    cap.phyHeight = mPhyHeight;
    cap.phyWidth = mPhyWidth;
    cap.type = mType;
    memcpy_s(cap.name, sizeof(cap.name), mName.c_str(), mName.size());
    if (mName.size() >= sizeof(cap.name)) {
        cap.name[sizeof(cap.name) - 1] = 0;
    } else {
        cap.name[mName.size()] = 0;
    }
    cap.supportLayers = mSupportLayers;
    cap.virtualDispCount = mVirtualDispCount;
    cap.supportWriteBack = mSupportWriteBack;
    cap.propertyCount = mPropertyCount;
}

void DrmConnector::ConvertTypeToName(uint32_t type, std::string &name)
{
    DISPLAY_LOGD("type %{public}d", type);
    switch (type) {
        case DISP_INTF_VGA:
            name = "VGA";
            break;
        case DISP_INTF_HDMI:
            name = "HDMI";
            break;
        case DISP_INTF_MIPI:
            name = "MIPI";
            break;
        default:
            name = "Unknown";
            break;
    }
}

void DrmConnector::ConvertToHdiType(uint32_t type, InterfaceType &hdiType)
{
    switch (type) {
        case DRM_MODE_CONNECTOR_VGA:
            hdiType = DISP_INTF_VGA;
            break;
        case DRM_MODE_CONNECTOR_DSI:
            hdiType = DISP_INTF_MIPI;
            break;
        case DRM_MODE_CONNECTOR_HDMIA:
        case DRM_MODE_CONNECTOR_HDMIB:
            hdiType = DISP_INTF_HDMI;
            break;
        default:
            hdiType = DISP_INTF_BUTT;
            break;
    }
}
int32_t DrmConnector::TryPickEncoder(IdMapPtr<DrmEncoder> &encoders, uint32_t encoderId, IdMapPtr<DrmCrtc> &crtcs,
    uint32_t &crtcId)
{
    int ret;
    auto encoderIter = encoders.find(encoderId);
    if (encoderIter == encoders.end()) {
        DISPLAY_LOGW("can not find encoder for id : %{public}d", encoderId);
        return DISPLAY_FAILURE;
    }

    auto &encoder = encoderIter->second;
    DISPLAY_LOGD("connector : %{public}d encoder : %{public}d", mId, encoder->GetId());
    ret = encoder->PickIdleCrtcId(crtcs, crtcId);
    DISPLAY_CHK_RETURN((ret == DISPLAY_SUCCESS), DISPLAY_SUCCESS,
        DISPLAY_LOGD("connector : %{public}d pick encoder : %{public}d", mId, encoder->GetId()));
    return DISPLAY_FAILURE;
}

int32_t DrmConnector::PickIdleCrtcId(IdMapPtr<DrmEncoder> &encoders, IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId)
{
    DISPLAY_LOGD();
    DISPLAY_LOGD("encoder_id %{public}d", mEncoderId);
    int ret = TryPickEncoder(encoders, mEncoderId, crtcs, crtcId);
    DISPLAY_CHK_RETURN((ret == DISPLAY_SUCCESS), DISPLAY_SUCCESS,
        DISPLAY_LOGD("connector : %{public}d pick endcoder : %{public}d crtcId : %{public}d",
        mId, mEncoderId, crtcId));

    for (auto encoder : mPossibleEncoders) {
        ret = TryPickEncoder(encoders, encoder, crtcs, crtcId);
        DISPLAY_CHK_RETURN((ret == DISPLAY_SUCCESS), DISPLAY_SUCCESS,
            DISPLAY_LOGD("connector : %{public}d pick endcoder : %{public}d crtcId : %{public}d", mId, mEncoderId,
            crtcId));
    }

    DISPLAY_LOGW("can not pick a crtc for connector");
    return DISPLAY_FAILURE;
}

int32_t DrmConnector::UpdateModes()
{
    int drmFd = mDrmFdPtr->GetFd();
    drmModeConnectorPtr c = drmModeGetConnector(drmFd, mId);
    DISPLAY_CHK_RETURN((c == nullptr), DISPLAY_FAILURE, DISPLAY_LOGE("can not get connector"));
    mConnectState = c->connection;
    // init the modes
    InitModes(*c);
    drmModeFreeConnector(c);
    return DISPLAY_SUCCESS;
}

int32_t DrmConnector::GetDisplaySupportedModes(uint32_t *num, DisplayModeInfo *modes)
{
    DISPLAY_CHK_RETURN((num == nullptr), DISPLAY_NULL_PTR, DISPLAY_LOGE("num is nullptr"));
    *num = static_cast<int32_t>(mModes.size());
    int i = 0;
    if (modes != nullptr) {
        for (const auto &modeMap : mModes) {
            DrmMode mode = modeMap.second;
            mode.ConvertToHdiMode(*(modes + i));
            i++;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t DrmConnector::SetDpmsState(uint64_t dmps)
{
    DISPLAY_LOGD("dmps %{public}" PRIu64 "", dmps);
    int ret = drmModeConnectorSetProperty(mDrmFdPtr->GetFd(), mId, mPropDpmsId, dmps);
    DISPLAY_CHK_RETURN((ret != 0), DISPLAY_FAILURE, DISPLAY_LOGE("can not set dpms"));
    mDpmsState = dmps;
    return DISPLAY_SUCCESS;
}

bool DrmConnector::IsConnected()
{
    return (mConnectState == DRM_MODE_CONNECTED);
}

int32_t DrmConnector::GetModeFromId(int32_t id, DrmMode &mode)
{
    DISPLAY_LOGD();
    auto iter = mModes.find(id);
    if (iter == mModes.end()) {
        return DISPLAY_FAILURE;
    }
    mode = mModes[id];
    return DISPLAY_SUCCESS;
}

std::unique_ptr<DrmModeBlock> DrmConnector::GetModeBlockFromId(int32_t id)
{
    DISPLAY_LOGD("id %{public}d", id);
    auto iter = mModes.find(id);
    DISPLAY_CHK_RETURN((iter == mModes.end()), nullptr, DISPLAY_LOGE("can not the mode %{public}d", id));
    return std::make_unique<DrmModeBlock>(mModes[id]);
}

DrmModeBlock::DrmModeBlock(DrmMode &mode)
{
    DISPLAY_LOGD();
    Init(mode);
}

int32_t DrmModeBlock::Init(DrmMode &mode)
{
    int ret;
    int drmFd;
    drmFd = DrmDevice::GetDrmFd();
    DISPLAY_CHK_RETURN((drmFd < 0), DISPLAY_FAILURE, DISPLAY_LOGE("the drm fd is invalid"));
    drmModeModeInfo modeInfo = *(mode.GetModeInfoPtr());
    ret = drmModeCreatePropertyBlob(drmFd, static_cast<void *>(&modeInfo), sizeof(modeInfo), &mBlockId);
    DISPLAY_CHK_RETURN((ret != 0), DISPLAY_FAILURE, DISPLAY_LOGE("create property blob failed"));
    DISPLAY_LOGD("mBlockId %{public}d", mBlockId);
    return DISPLAY_SUCCESS;
}

DrmModeBlock::~DrmModeBlock()
{
    DISPLAY_LOGD("mBlockId %{public}d", mBlockId);
    int drmFd;
    int ret;
    drmFd = DrmDevice::GetDrmFd();
    if ((mBlockId != DRM_INVALID_ID) && (drmFd >= 0)) {
        ret = drmModeDestroyPropertyBlob(drmFd, mBlockId);
        if (ret != 0) {
            DISPLAY_LOGE("destroy property blob failed errno %{public}d", errno);
        }
    } else {
        DISPLAY_LOGE("can not destruct the block id %{public}d drmFd %{public}d", mBlockId, drmFd);
    }
}
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY
