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

#ifndef HDI_CAMERA_DEVICE_SERVICE_STUB_INF_H
#define HDI_CAMERA_DEVICE_SERVICE_STUB_INF_H

#include <refbase.h>
#include <message_parcel.h>
#include <message_option.h>
#include "iremote_stub.h"
#include "icamera_device.h"
#include "camera.h"

namespace OHOS::Camera {
class CameraDeviceStub : public IRemoteStub<ICameraDevice> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

    int32_t CameraDeviceStubGetStreamOperator(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraDeviceStubUpdateSettings(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraDeviceStubSetResultMode(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraDeviceStubGetEnabledReuslts(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraDeviceStubEnableResult(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraDeviceStubDisableResult(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t CameraDeviceStubClose(MessageParcel& data, MessageParcel& reply, MessageOption& option);
};
}
#endif // HDI_CAMERA_DEVICE_SERVICE_STUB_INF_H