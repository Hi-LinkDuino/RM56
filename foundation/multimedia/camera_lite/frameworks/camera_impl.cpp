/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "camera_impl.h"
#include "media_log.h"
#include "camera_device_client.h"
#include <cstdio>

using namespace std;
namespace OHOS {
namespace Media {
CameraImpl::CameraImpl(std::string &id, const CameraAbility *ability, const CameraInfo *info)
{
    id_ = id;
    ability_ = ability;
    info_ = info;
}

string CameraImpl::GetCameraId()
{
    return id_;
}

const CameraConfig *CameraImpl::GetCameraConfig() const
{
    return config_;
}

FrameConfig *CameraImpl::GetFrameConfig(int32_t type)
{
    for (auto i : frameConfigs_) {
        if (i->GetFrameConfigType() == type) {
            return i;
        }
    }
    return nullptr;
}

void CameraImpl::Configure(CameraConfig &config)
{
    if (config_ != nullptr) {
        return;
    }

    if (config.GetFrameStateCb() == nullptr || config.GetEventHandler() == nullptr) {
        return;
    }

    if (deviceClient_ == nullptr) {
        return;
    }
    int32_t ret = deviceClient_->SetCameraConfig(config);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Set camera config failed in cameraImpl.");
        return;
    }
    config_ = &config;
}

void CameraImpl::OnConfigured(int32_t ret, CameraConfig &config)
{
    if (ret != MEDIA_OK) {
        handler_->Post([this, ret] { this->stateCb_->OnConfigureFailed(this->GetCameraId(), ret); });
        return;
    }
    handler_->Post([this] { this->stateCb_->OnConfigured(*this); });
}

void CameraImpl::Release()
{
    if (config_ != nullptr) {
        delete config_;
        config_ = nullptr;
    }
    if (deviceClient_ == nullptr) {
        return;
    }
    deviceClient_->Release();
    if (handler_ == nullptr) {
        return;
    }
    handler_->Post([this] { this->stateCb_->OnReleased(*this); });
}

int32_t CameraImpl::TriggerLoopingCapture(FrameConfig &fc)
{
    if (config_ == nullptr) {
        MEDIA_ERR_LOG("Cannot find available configuration, configure the camera first.");
        return MEDIA_INVALID_PARAM;
    }
    int32_t type = fc.GetFrameConfigType();
    if (type == FRAME_CONFIG_CAPTURE) {
        MEDIA_ERR_LOG("looping capture not support FRAME_CONFIG_CAPTURE");
        return MEDIA_ERR;
    }
    FrameConfig *curFc = GetFrameConfig(type);
    if (curFc != nullptr) {
        MEDIA_ERR_LOG("Frame config of the input type is already existed.");
        return MEDIA_ERR;
    }
    if (deviceClient_ == nullptr) {
        return MEDIA_ERR;
    }
    int32_t ret = deviceClient_->TriggerLoopingCapture(fc);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Camera device start looping capture failed.(ret=%d)", ret);
        return MEDIA_ERR;
    }
    frameConfigs_.emplace_back(&fc);
    return MEDIA_OK;
}

void CameraImpl::StopLoopingCapture(int32_t type = -1)
{
    if (deviceClient_ == nullptr) {
        return;
    }
    deviceClient_->StopLoopingCapture(type);
    if (config_ == nullptr) {
        return;
    }
    FrameStateCallback *fsc = config_->GetFrameStateCb();
    if (fsc == nullptr) {
        return;
    }
    EventHandler *eventhdl = config_->GetEventHandler();
    if (eventhdl == nullptr) {
        return;
    }

    for (auto i : frameConfigs_) {
        if (i->GetFrameConfigType() == type || type == -1) {
            eventhdl->Post([fsc, this, i] {
                FrameResult frameResult;
                fsc->OnFrameFinished(*this, *i, frameResult);
            });
        }
    }
    /* clear all configs, if type == -1 */
    if (type == -1) {
        frameConfigs_.clear();
    }
}

int32_t CameraImpl::TriggerSingleCapture(FrameConfig &fc)
{
    if (config_ == nullptr) {
        MEDIA_ERR_LOG("Cannot find available configuration, configure the camera first.");
        return MEDIA_INVALID_PARAM;
    }
    if (deviceClient_ == nullptr) {
        MEDIA_ERR_LOG("Cannot find available configuration, configure the camera first.");
        return MEDIA_INVALID_PARAM;
    }
    if (fc.GetFrameConfigType() != FRAME_CONFIG_CAPTURE) {
        MEDIA_ERR_LOG("single capture only support FRAME_CONFIG_CAPTURE");
        return MEDIA_ERR;
    }
    int32_t ret = deviceClient_->TriggerSingleCapture(dynamic_cast<FrameConfig &>(fc));
    if (ret != MEDIA_OK) {
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

const CameraAbility *CameraImpl::GetAbility()
{
    return ability_;
}

const CameraInfo *CameraImpl::GetInfo()
{
    return info_;
}

void CameraImpl::OnCreate(string cameraId)
{
    deviceClient_ = CameraDeviceClient::GetInstance();
    if (deviceClient_ == nullptr) {
        return;
    }
    deviceClient_->SetCameraId(cameraId);
    deviceClient_->SetCameraImpl(this);
    deviceClient_->SetCameraCallback();
    if (stateCb_ == nullptr || handler_ == nullptr) {
        return;
    }
    handler_->Post([this] { this->stateCb_->OnCreated(*this); });
}

void CameraImpl::OnFrameFinished(int32_t ret, FrameConfig &fc)
{
    FrameStateCallback *fsc = config_->GetFrameStateCb();
    if (fsc == nullptr) {
        return;
    }
    EventHandler *eventhdl = config_->GetEventHandler();
    if (eventhdl == nullptr) {
        return;
    }
    if (ret != MEDIA_OK) {
        eventhdl->Post([fsc, this, &fc] {
            FrameResult frameResult;
            fsc->OnFrameError(*this, fc, -1, frameResult);
        });
        return;
    }
    eventhdl->Post([fsc, this, &fc] {
        FrameResult frameResult;
        fsc->OnFrameFinished(*this, fc, frameResult);
    });
}

void CameraImpl::OnCreateFailed()
{
    if (stateCb_ == nullptr || handler_ == nullptr) {
        return;
    }
    handler_->Post([this] { this->stateCb_->OnCreateFailed(id_, MEDIA_ERR); });
}

void CameraImpl::RegistCb(CameraStateCallback &callback, EventHandler &handler)
{
    handler_ = &handler;
    stateCb_ = &callback;
}
} // namespace Media
} // namespace OHOS