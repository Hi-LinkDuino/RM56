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

#include "camera_device_callback_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "camera_device_callback.h"
#include "metadata_utils.h"
#include "cmd_common.h"

namespace OHOS::Camera {
int32_t CameraDeviceCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HDF_LOGE("%s: CameraDeviceCallbackStub::OnRemoteRequest entry!", __func__);
    switch (code) {
        case CMD_CAMERA_DEVICE_CALLBACK_ON_ERROR: {
            if (data.ReadInterfaceToken() != CameraDeviceCallbackStub::GetDescriptor()) {
                HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
                return INVALID_ARGUMENT;
            }

            ErrorType type = static_cast<ErrorType>(data.ReadUint32());
            int32_t errorMsg = data.ReadInt32();
            OnError(type, errorMsg);
            break;
        }
        case CMD_CAMERA_DEVICE_CALLBACK_ON_RESULT: {
            if (data.ReadInterfaceToken() != CameraDeviceCallbackStub::GetDescriptor()) {
                HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
                return INVALID_ARGUMENT;
            }

            uint64_t timestamp = data.ReadUint64();
            std::shared_ptr<CameraStandard::CameraMetadata> result = nullptr;
            CameraStandard::MetadataUtils::DecodeCameraMetadata(data, result);
            OnResult(timestamp, result);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}
}
