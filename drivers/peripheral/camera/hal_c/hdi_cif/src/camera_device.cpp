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

#include "camera_device.h"
#include "stream_operator.h"
#include "stream_operator_callback_wrapper.h"
#include <memory>

namespace OHOS::Camera {
CameraDevice::CameraDevice() {}

CameraDevice::~CameraDevice() {}

void CameraDevice::Init(CameraDeviceCIF* device)
{
    device_ = device;
}

CamRetCode CameraDevice::GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback>& callback,
                                           OHOS::sptr<IStreamOperator>& streamOperator)
{
    if (device_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    StreamOperatorCallbackCIF cb;
    BindStreamOperatorCallback(callback);

    cb.OnCaptureStarted = StreamCBOnCaptureStarted;
    cb.OnCaptureEnded = StreamCBOnCaptureEnded;
    cb.OnCaptureError = StreamCBOnCaptureError;
    cb.OnFrameShutter = StreamCBOnFrameShutter;

    StreamOperatorCIF* op = nullptr;
    int ret = device_->GetStreamOperator(cb, op);
    OHOS::sptr<StreamOperator> spSO = new StreamOperator;
    spSO->Init(op);
    streamOperator = spSO;

    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraDevice::UpdateSettings(const std::shared_ptr<CameraSetting>& settings)
{
    if (device_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }
    CameraSettingCIF* meta = settings->get();
    int ret = device_->UpdateSettings(meta);
    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraDevice::SetResultMode(const ResultCallbackMode& mode)
{
    if (device_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int m = static_cast<int>(mode);
    int ret = device_->SetResultMode(m);
    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraDevice::GetEnabledResults(std::vector<MetaType>& results)
{
    if (device_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    MetaTypeCIF* meta = nullptr;
    int count = 0;
    int ret = device_->GetEnabledResults(&meta, &count);
    for (int i = 0; i < count; i++) {
        results.push_back(meta[i]);
    }

    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraDevice::EnableResult(const std::vector<MetaType>& results)
{
    if (device_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int count = results.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    MetaTypeCIF* meta = new MetaTypeCIF[count];
    if (meta == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }
    for (int i = 0; i < count; i++) {
        meta[i] = results[i];
    }
    int ret = device_->EnableResult(meta, count);
    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraDevice::DisableResult(const std::vector<MetaType>& results)
{
    if (device_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int count = results.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    MetaTypeCIF* meta = new MetaTypeCIF[count];
    if (meta == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }
    for (int i = 0; i < count; i++) {
        meta[i] = results[i];
    }
    int ret = device_->DisableResult(meta, count);
    return static_cast<CamRetCode>(ret);
}

void CameraDevice::Close()
{
    if (device_ == nullptr) {
        return;
    }
    device_->Close();
}

} // end namespace OHOS::Camera

