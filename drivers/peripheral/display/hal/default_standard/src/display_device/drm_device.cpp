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

#include "drm_device.h"
#include <string>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <drm_fourcc.h>
#include "display_common.h"
#include "drm_display.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
FdPtr DrmDevice::mDrmFd = nullptr;
std::shared_ptr<DrmDevice> DrmDevice::mInstance;

std::shared_ptr<HdiDeviceInterface> DrmDevice::Create()
{
    DISPLAY_LOGD();
    if (mDrmFd == nullptr) {
        const std::string name("hisilicon");
        int drmFd = drmOpen(name.c_str(), nullptr);
        if (drmFd < 0) {
            DISPLAY_LOGE("drm file:%{public}s open failed %{public}s", name.c_str(), strerror(errno));
            return nullptr;
        }
        DISPLAY_LOGD("the drm fd is %{public}d", drmFd);
        mDrmFd = std::make_shared<HdiFd>(drmFd);
    }
    if (mInstance == nullptr) {
        mInstance = std::make_shared<DrmDevice>();
    }
    return mInstance;
}

int DrmDevice::GetDrmFd()
{
    if (mDrmFd == nullptr) {
        DISPLAY_LOGE("the drmfd is null not open");
        return -1;
    }
    return mDrmFd->GetFd();
}

using PixelFormatConvertTbl = struct PixFmtConvertTbl {
    uint32_t drmFormat;
    PixelFormat pixFormat;
};

uint32_t DrmDevice::ConvertToDrmFormat(PixelFormat fmtIn)
{
    static const PixelFormatConvertTbl convertTable[] = {
        {DRM_FORMAT_XBGR8888, PIXEL_FMT_RGBX_8888}, {DRM_FORMAT_ABGR8888, PIXEL_FMT_RGBA_8888},
        {DRM_FORMAT_RGB888, PIXEL_FMT_RGB_888}, {DRM_FORMAT_RGB565, PIXEL_FMT_BGR_565},
        {DRM_FORMAT_BGRX4444, PIXEL_FMT_BGRX_4444}, {DRM_FORMAT_BGRA4444, PIXEL_FMT_BGRA_4444},
        {DRM_FORMAT_RGBA4444, PIXEL_FMT_RGBA_4444}, {DRM_FORMAT_RGBX4444, PIXEL_FMT_RGBX_4444},
        {DRM_FORMAT_BGRX5551, PIXEL_FMT_BGRX_5551}, {DRM_FORMAT_BGRA5551, PIXEL_FMT_BGRA_5551},
        {DRM_FORMAT_BGRX8888, PIXEL_FMT_BGRX_8888}, {DRM_FORMAT_ARGB8888, PIXEL_FMT_BGRA_8888},
        {DRM_FORMAT_NV12, PIXEL_FMT_YCBCR_420_SP}, {DRM_FORMAT_NV21, PIXEL_FMT_YCRCB_420_SP},
        {DRM_FORMAT_YUV420, PIXEL_FMT_YCBCR_420_P}, {DRM_FORMAT_YVU420, PIXEL_FMT_YCRCB_420_P},
        {DRM_FORMAT_NV16, PIXEL_FMT_YCBCR_422_SP}, {DRM_FORMAT_NV61, PIXEL_FMT_YCRCB_422_SP},
        {DRM_FORMAT_YUV422, PIXEL_FMT_YCBCR_422_P}, {DRM_FORMAT_YVU422, PIXEL_FMT_YCRCB_422_P},
    };
    uint32_t fmtOut = 0;
    for (uint32_t i = 0; i < sizeof(convertTable) / sizeof(convertTable[0]); i++) {
        if (convertTable[i].pixFormat == fmtIn) {
            fmtOut = convertTable[i].drmFormat;
        }
    }
    DISPLAY_LOGD("fmtIn %{public}d, outFmt %{public}d", fmtIn, fmtOut);
    return fmtOut;
}

DrmDevice::DrmDevice() {}

int32_t DrmDevice::GetCrtcProperty(const DrmCrtc &crtc, const std::string &name, DrmProperty &prop)
{
    return GetProperty(crtc.GetId(), DRM_MODE_OBJECT_CRTC, name, prop);
}

int32_t DrmDevice::GetConnectorProperty(const DrmConnector &connector, const std::string &name, DrmProperty &prop)
{
    return GetProperty(connector.GetId(), DRM_MODE_OBJECT_CONNECTOR, name, prop);
}

int32_t DrmDevice::GetPlaneProperty(const DrmPlane &plane, const std::string &name, DrmProperty &prop)
{
    return GetProperty(plane.GetId(), DRM_MODE_OBJECT_PLANE, name, prop);
}

int32_t DrmDevice::GetProperty(uint32_t objId, uint32_t objType, const std::string &name, DrmProperty &prop)
{
    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(GetDrmFd(), objId, objType);
    DISPLAY_CHK_RETURN((!props), DISPLAY_FAILURE, DISPLAY_LOGE("can not get properties"));
    bool found = false;
    for (uint32_t i = 0; i < props->count_props; i++) {
        drmModePropertyPtr p = drmModeGetProperty(GetDrmFd(), props->props[i]);
        if (strcmp(p->name, name.c_str()) == 0) {
            found = true;
            prop.propId = p->prop_id;
            prop.value = props->prop_values[i];
        }
        drmModeFreeProperty(p);
    }
    drmModeFreeObjectProperties(props);
    return found ? DISPLAY_SUCCESS : DISPLAY_NOT_SUPPORT;
}

int32_t DrmDevice::Init()
{
    int ret = drmSetClientCap(GetDrmFd(), DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
    DISPLAY_CHK_RETURN((ret), DISPLAY_FAILURE,
        DISPLAY_LOGE("DRM_CLIENT_CAP_UNIVERSAL_PLANES set failed %{public}s", strerror(errno)));
    ret = drmSetClientCap(GetDrmFd(), DRM_CLIENT_CAP_ATOMIC, 1);
    DISPLAY_CHK_RETURN((ret), DISPLAY_FAILURE,
        DISPLAY_LOGE("DRM_CLIENT_CAP_ATOMIC set failed %{public}s", strerror(errno)));

    ret = drmSetMaster(GetDrmFd());
    DISPLAY_CHK_RETURN((ret), DISPLAY_FAILURE, DISPLAY_LOGE("can not set to master errno : %{public}d", errno));

    ret = drmIsMaster(GetDrmFd());
    DISPLAY_CHK_RETURN((!ret), DISPLAY_FAILURE, DISPLAY_LOGE("is not master : %{public}d", errno));

    return DISPLAY_SUCCESS;
}

void DrmDevice::DeInit()
{
    mDisplays.clear();
    mCrtcs.clear();
}

void DrmDevice::FindAllCrtc(const drmModeResPtr &res)
{
    DISPLAY_CHK_RETURN_NOT_VALUE((res == nullptr), DISPLAY_LOGE("the res is null"));
    mCrtcs.clear();
    for (int i = 0; i < res->count_crtcs; i++) {
        drmModeCrtcPtr crtc = drmModeGetCrtc(GetDrmFd(), res->crtcs[i]);
        if (!crtc) {
            DISPLAY_LOGE("can not get crtc %{public}d", i);
            continue;
        }
        uint32_t crtc_id = crtc->crtc_id;
        std::shared_ptr<DrmCrtc> drmCrtc = std::make_shared<DrmCrtc>(crtc, i);
        int ret = drmCrtc->Init(*this);
        drmModeFreeCrtc(crtc);
        if (ret != DISPLAY_SUCCESS) {
            DISPLAY_LOGE("crtc %{public}d init failed", i);
            continue;
        }
        mCrtcs.emplace(crtc_id, std::move(drmCrtc));
    }
}

void DrmDevice::FindAllEncoder(const drmModeResPtr &res)
{
    DISPLAY_CHK_RETURN_NOT_VALUE((res == nullptr), DISPLAY_LOGE("the res is null"));
    mEncoders.clear();
    for (int i = 0; i < res->count_encoders; i++) {
        drmModeEncoderPtr encoder = drmModeGetEncoder(GetDrmFd(), res->encoders[i]);
        if (!encoder) {
            DISPLAY_LOGE("can not get encoder %{public}d", i);
            continue;
        }
        std::shared_ptr<DrmEncoder> drmEncoder = std::make_shared<DrmEncoder>(*encoder);
        mEncoders.emplace(encoder->encoder_id, std::move(drmEncoder));
        drmModeFreeEncoder(encoder);
    }
    DISPLAY_LOGD("find encoder count %{public}zd", mEncoders.size());
}

void DrmDevice::FindAllConnector(const drmModeResPtr &res)
{
    DISPLAY_CHK_RETURN_NOT_VALUE((res == nullptr), DISPLAY_LOGE("the res is null"));
    mConnectors.clear();
    int ret;
    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnectorPtr connector = drmModeGetConnector(GetDrmFd(), res->connectors[i]);
        if (!connector) {
            DISPLAY_LOGE("can not get connector mode %{public}d", i);
            continue;
        }
        std::shared_ptr<DrmConnector> drmConnector = std::make_shared<DrmConnector>(*connector, mDrmFd);
        ret = drmConnector->Init(*this);
        drmModeFreeConnector(connector);
        if (ret != DISPLAY_SUCCESS) {
            continue;
        }
        int connectorId = drmConnector->GetId();
        mConnectors.emplace(connectorId, std::move(drmConnector));
    }
    DISPLAY_LOGD("find connector count %{public}zd", mConnectors.size());
}

void DrmDevice::FindAllPlane()
{
    mPlanes.clear();
    int ret;
    drmModePlaneResPtr planeRes = drmModeGetPlaneResources(GetDrmFd());
    DISPLAY_CHK_RETURN_NOT_VALUE((planeRes == nullptr), DISPLAY_LOGE("can not get plane resource"));
    DISPLAY_LOGD("count_planes %{public}d", planeRes->count_planes);
    for (uint32_t i = 0; i < planeRes->count_planes; i++) {
        drmModePlanePtr p = drmModeGetPlane(GetDrmFd(), planeRes->planes[i]);
        if (!p) {
            DISPLAY_LOGE("can not get plane %{public}d", i);
            continue;
        }
        std::shared_ptr<DrmPlane> drmPlane = std::make_shared<DrmPlane>(*p);
        DISPLAY_LOGD();
        ret = drmPlane->Init(*this);
        DISPLAY_LOGD();
        drmModeFreePlane(p);
        if (ret != DISPLAY_SUCCESS) {
            DISPLAY_LOGE("drm plane %{public}d init failed", i);
            continue;
        }
        mPlanes.emplace_back(std::move(drmPlane));
    }
    DISPLAY_LOGD("find plane count %{public}zd", mPlanes.size());
}

std::shared_ptr<DrmEncoder> DrmDevice::GetDrmEncoderFromId(uint32_t id)
{
    auto iter = mEncoders.find(id);
    if (iter != mEncoders.end()) {
        return iter->second;
    }
    DISPLAY_LOGE("get drm encoder fail");
    return nullptr;
}

std::shared_ptr<DrmConnector> DrmDevice::GetDrmConnectorFromId(uint32_t id)
{
    auto iter = mConnectors.find(id);
    if (iter != mConnectors.end()) {
        return iter->second;
    }
    DISPLAY_LOGE("get drm connector fail");
    return nullptr;
}

std::shared_ptr<DrmCrtc> DrmDevice::GetDrmCrtcFromId(uint32_t id)
{
    auto iter = mCrtcs.find(id);
    if (iter != mCrtcs.end()) {
        return iter->second;
    }
    DISPLAY_LOGE("get drm crtc fail");
    return nullptr;
}

std::vector<std::shared_ptr<DrmPlane>> DrmDevice::GetDrmPlane(uint32_t pipe, uint32_t type)
{
    std::vector<std::shared_ptr<DrmPlane>> planes;
    for (const auto &plane : mPlanes) {
        if (plane->IsIdle() && ((1 << pipe) & plane->GetPossibleCrtcs()) && (type == plane->GetType())) {
            plane->BindToPipe(pipe);
            planes.push_back(plane);
        }
    }
    DISPLAY_LOGD("the planes count %{public}zd", planes.size());
    return planes;
}


std::unordered_map<uint32_t, std::shared_ptr<HdiDisplay>> DrmDevice::DiscoveryDisplay()
{
    int32_t ret;
    mDisplays.clear();
    drmModeResPtr res = drmModeGetResources(GetDrmFd());
    DISPLAY_CHK_RETURN((res == nullptr), mDisplays, DISPLAY_LOGE("can not get drm resource"));
    // discovery all drm resource
    FindAllCrtc(res);
    FindAllEncoder(res);
    FindAllConnector(res);
    FindAllPlane();
    DISPLAY_LOGD();
    // travel all connector
    for (auto &connectorPair : mConnectors) {
        auto connector = connectorPair.second;
        uint32_t crtcId = 0;
        ret = connector->PickIdleCrtcId(mEncoders, mCrtcs, crtcId);
        if (ret != DISPLAY_SUCCESS) {
            continue;
        }
        DISPLAY_LOGD();

        auto crtcIter = mCrtcs.find(crtcId);
        if (crtcIter == mCrtcs.end()) {
            DISPLAY_LOGE("can not find crtc for the id %{public}d", connector->GetId());
            continue;
        }
        DISPLAY_LOGD();
        auto crtc = crtcIter->second;
        DISPLAY_LOGD("crtc %{public}p", crtc.get());
        // create the display
        std::shared_ptr<HdiDisplay> display = std::make_shared<DrmDisplay>(connector, crtc, mInstance);
        DISPLAY_LOGD();
        display->Init();
        mDisplays.emplace(display->GetId(), std::move(display));
    }
    DISPLAY_LOGD("find display size %{public}zd", mDisplays.size());
    return mDisplays;
}
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY
