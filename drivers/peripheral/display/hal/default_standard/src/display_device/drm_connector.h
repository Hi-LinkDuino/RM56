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

#ifndef DRM_CONNECTOR_H
#define DRM_CONNECTOR_H
#include <string>
#include <unordered_map>
#include <vector>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "display_type.h"
#include "drm_encoder.h"
#include "hdi_device_common.h"
#include "hdi_shared_fd.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
const std::string PROP_DPMS = "DPMS";
const std::string PROP_CRTCID = "CRTC_ID";
const std::string PROP_BRIGHTNESS = "brightness";
class DrmDevice;
class DrmModeBlock;

class DrmMode {
public:
    DrmMode() {}
    DrmMode(drmModeModeInfo &modeInfo, uint32_t id) : mModeInfo(modeInfo), mId(id) {}
    virtual ~DrmMode() {}
    drmModeModeInfoPtr GetModeInfoPtr()
    {
        return &mModeInfo;
    }
    void ConvertToHdiMode(DisplayModeInfo &hdiMode);

private:
    drmModeModeInfo mModeInfo = { 0 };
    int32_t mId = -1;
};

class DrmModeBlock {
public:
    explicit DrmModeBlock(DrmMode &mode);
    virtual ~DrmModeBlock();
    int32_t Init(DrmMode &mode);
    uint32_t GetBlockId() const
    {
        return mBlockId;
    }

private:
    uint32_t mBlockId = DRM_INVALID_ID;
};

class DrmConnector {
public:
    DrmConnector(drmModeConnector c, FdPtr &fd);
    virtual ~DrmConnector() {}
    uint32_t GetId() const
    {
        return mId;
    }
    uint32_t GetEncoderId() const
    {
        return mEncoderId;
    }
    void GetDisplayCap(DisplayCapability &cap);
    int32_t Init(DrmDevice &drmDevice);
    int32_t PickIdleCrtcId(IdMapPtr<DrmEncoder> &encoders, IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId);
    int32_t GetDisplaySupportedModes(uint32_t *num, DisplayModeInfo *modes);
    int32_t GetPreferenceId() const
    {
        return mPreferenceId;
    }
    uint32_t GetPropCrtcId() const
    {
        return mPropCrtcId;
    }
    int32_t TryPickEncoder(IdMapPtr<DrmEncoder> &encoders, uint32_t encoderId, IdMapPtr<DrmCrtc> &crtcs,
        uint32_t &crtcId);
    // update modes will reset the preference mode id and active mode id
    int32_t UpdateModes();
    std::unique_ptr<DrmModeBlock> GetModeBlockFromId(int32_t id);
    int32_t GetModeFromId(int32_t id, DrmMode &mode);
    uint64_t GetDpmsState() const
    {
        return mDpmsState;
    }
    int32_t SetDpmsState(uint64_t dmps);
    bool IsConnected();

    int32_t GetBrightness(uint32_t& level);
    int32_t SetBrightness(uint32_t level);

private:
    static void ConvertTypeToName(uint32_t type, std::string &name);
    static void ConvertToHdiType(uint32_t type, InterfaceType &hdiType);

    void InitModes(drmModeConnector c);
    uint32_t mId;
    InterfaceType mType;
    uint32_t mPhyWidth;
    uint32_t mPhyHeight;
    uint32_t mSupportLayers = 0;
    uint32_t mVirtualDispCount = 0;
    bool mSupportWriteBack = false;
    uint32_t mPropertyCount = 0;
    uint32_t mEncoderId;
    std::vector<uint32_t> mPossibleEncoders;
    std::string mName;
    drmModeConnection mConnectState;
    uint32_t mPropDpmsId = DRM_INVALID_ID;
    uint64_t mDpmsState = 0;
    uint32_t mPropCrtcId = DRM_INVALID_ID;
    uint32_t mPropBrightnessId = DRM_INVALID_ID;
    uint32_t mBrightnessLevel = 0;
    std::unordered_map<int32_t, DrmMode> mModes;
    int32_t mPreferenceId = INVALID_MODE_ID;

    FdPtr mDrmFdPtr;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY
#endif // DRM_CONNECTOR_H
