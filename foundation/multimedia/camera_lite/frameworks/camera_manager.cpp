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
#include "camera_manager.h"
#include "camera_impl.h"
#include "camera_service_client.h"
#include "camera_service_callback.h"
#include "media_log.h"

#include <cstdio>
#include <map>

using namespace std;
namespace OHOS {
namespace Media {
class CameraManagerImpl : public CameraManager, public CameraServiceCallback {
public:
    CameraManagerImpl()
    {
        cameraServiceClient_ = CameraServiceClient::GetInstance();
        cameraServiceClient_->InitCameraServiceClient(this);
    }
    ~CameraManagerImpl()
    {
        for (auto &i : cameraMapCache_) {
            delete i.second;
        }
    }

    void OnCameraServiceInitialized(list<string> &availCameraIdList) override
    {
        /* cameraService_ cannot be nullptr as OnCameraServiceInitialized is a callback of cameraService_ */
        for (auto &cameraId : availCameraIdList) {
            InitCameraAbility(cameraId);
        }
    }

    void OnCameraStatusChange(string &cameraId, CameraStauts status) override
    {
        auto p = cameraMapCache_.find(cameraId);
        switch (status) {
            case CAMERA_STATUS_UNAVAIL:
                if (p != cameraMapCache_.end()) {
                    for (auto &i : deviceCbList_) {
                        i.second->Post(
                            [i, cameraId]() { i.first->OnCameraStatus(cameraId, CAMERA_DEVICE_STATE_UNAVAILABLE); });
                    }
                    // User may still using icamera now
                    delete p->second;
                    cameraMapCache_.erase(p);
                }
                break;
            case CAMERA_STATUS_AVAIL:
                InitCameraAbility(cameraId);
                if (p == cameraMapCache_.end()) {
                    for (auto &i : deviceCbList_) {
                        i.second->Post(
                            [i, cameraId]() { i.first->OnCameraStatus(cameraId, CAMERA_DEVICE_STATE_AVAILABLE); });
                    }
                }
                break;
            case CAMERA_STATUS_CREATED:
                if (p != cameraMapCache_.end()) {
                    p->second->OnCreate(cameraId);
                }
                break;
            case CAMERA_STATUS_CREATE_FAILED:
                if (p != cameraMapCache_.end()) {
                    p->second->OnCreateFailed();
                }
                break;
            default:
                break;
        }
    }

    void RegisterCameraDeviceCallback(CameraDeviceCallback &callback, EventHandler &handler) override
    {
        deviceCbList_.emplace_back(make_pair(&callback, &handler));
        for (auto &i : cameraMapCache_) {
            string cameraId = i.first;
            handler.Post([&callback, cameraId]() { callback.OnCameraStatus(cameraId, CAMERA_DEVICE_STATE_AVAILABLE); });
        }
    }

    void UnregisterCameraDeviceCallback(CameraDeviceCallback &callback) override
    {
        for (auto p = deviceCbList_.begin(); p != deviceCbList_.end(); p++) {
            if (p->first == &callback) {
                deviceCbList_.erase(p);
                break;
            }
        }
    }

    uint8_t GetCameraModeNum() override
    {
        return cameraServiceClient_->GetCameraModeNum();
    }

    int32_t SetCameraMode(uint8_t modeIndex) override
    {
        return cameraServiceClient_->SetCameraMode(modeIndex);
    }

    list<string> GetCameraIds() override
    {
        list<string> cameraList;
        for (auto &i : cameraMapCache_) {
            MEDIA_DEBUG_LOG("%s", i.first.c_str());
            cameraList.emplace_back(i.first);
        }
        return cameraList;
    }

    const CameraAbility *GetCameraAbility(const string &cameraId) override
    {
        auto camera = cameraMapCache_.find(cameraId);
        if (camera == cameraMapCache_.end()) {
            return nullptr;
        }
        return camera->second->GetAbility();
    }

    const CameraInfo *GetCameraInfo(const string &cameraId) override
    {
        auto camera = cameraMapCache_.find(cameraId);
        if (camera == cameraMapCache_.end()) {
            return nullptr;
        }
        return camera->second->GetInfo();
    }

    void CreateCamera(const string &cameraId, CameraStateCallback &callback, EventHandler &handler) override
    {
        auto p = cameraMapCache_.find(cameraId);
        if (p == cameraMapCache_.end()) {
            MEDIA_ERR_LOG("The cameraId %s is not available", cameraId.c_str());
            handler.Post([&callback, &cameraId] { callback.OnCreateFailed(cameraId, MEDIA_ERR); });
            return;
        }
        p->second->RegistCb(callback, handler);
        cameraServiceClient_->CreateCamera(cameraId);
    }
private:
    CameraServiceClient *cameraServiceClient_;
    map<string, CameraImpl *> cameraMapCache_;
    list<pair<CameraDeviceCallback *, EventHandler *>> deviceCbList_;

    void InitCameraAbility(string &cameraId)
    {
        auto ability = cameraServiceClient_->GetCameraAbility(cameraId);
        auto info = cameraServiceClient_->GetCameraInfo(cameraId);
        CameraImpl *cam = new (nothrow) CameraImpl(cameraId, ability, info);
        if (cam == nullptr) {
            MEDIA_FATAL_LOG("New object failed.");
            return;
        }
        cameraMapCache_[cameraId] = cam;
    }
};

CameraManager *CameraManager::GetInstance()
{
    static CameraManagerImpl cameraManagerImpl;
    return &cameraManagerImpl;
}
} // namespace Media
} // namespace OHOS
