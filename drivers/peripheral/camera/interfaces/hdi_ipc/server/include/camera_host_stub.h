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

#ifndef HDI_CAMERA_HOST_SERVICE_STUB_INF_H
#define HDI_CAMERA_HOST_SERVICE_STUB_INF_H

#include <refbase.h>
#include <message_parcel.h>
#include <message_option.h>
#include "icamera_host_callback.h"
#include "icamera_device_callback.h"
#include "camera_host.h"
#include "types.h"
#include "camera.h"
#include "cmd_common.h"

namespace OHOS::Camera {
class CameraHostStub {
public:
    CameraHostStub();
    virtual ~CameraHostStub() {}
    RetCode Init();
    int32_t CameraHostStubSetCallback(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraHostStubGetCameraIds(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraHostStubGetCameraAbility(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraHostStubOpenCamera(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraHostStubSetFlashlight(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    static inline const std::u16string metaDescriptor_ = u"HDI.Camera.V1_0.Host";
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }

    int32_t CameraHostServiceStubOnRemoteRequest(int cmdId,
        MessageParcel& data, MessageParcel& reply, MessageOption& option);

private:
    std::shared_ptr<CameraHost> cameraHost_ = nullptr;
};
}

void *CameraHostStubInstance(void);

void DestroyCameraHostStub(void *obj);

int32_t CameraHostServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);

#endif // HDI_CAMERA_HOST_SERVICE_STUB_INF_H