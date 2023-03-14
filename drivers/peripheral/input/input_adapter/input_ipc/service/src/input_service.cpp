/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <hdf_log.h>
#include <unistd.h>
#include "input_server.h"

#define HDF_LOG_TAG InputServer

namespace OHOS {
namespace Input {
#define GET_INSTANCE_INPUT_DEVICE_MANAGER inputDeviceManager_
std::map<int32_t, OHOS::sptr<InputReportEventCb>> InputServer::reportEventPkgCallback_;
int32_t InputServer::ScanInputDevice(DevDesc *staArr, uint32_t arrLen)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->ScanDevice(staArr, arrLen);
}

int32_t InputServer::OpenInputDevice(uint32_t devIndex)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->OpenDevice(devIndex);
}

int32_t InputServer::CloseInputDevice(uint32_t devIndex)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->CloseDevice(devIndex);
}

int32_t InputServer::GetInputDevice(uint32_t devIndex, DeviceInfo **devInfo)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetDevice(devIndex, devInfo);
}

int32_t InputServer::GetInputDeviceList(uint32_t *devNum, DeviceInfo **devList, uint32_t size)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetDeviceList(devNum, devList, size);
}

int32_t InputServer::SetPowerStatus(uint32_t devIndex, uint32_t status)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->SetPowerStatus(devIndex, status);
}

int32_t InputServer::GetPowerStatus(uint32_t devIndex, uint32_t *status)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetPowerStatus(devIndex, status);
}

int32_t InputServer::GetDeviceType(uint32_t devIndex, uint32_t *type)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetDeviceType(devIndex, type);
}

int32_t InputServer::GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetChipInfo(devIndex, chipInfo, length);
}

int32_t InputServer::GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetVendorName(devIndex, vendorName, length);
}

int32_t InputServer::GetChipName(uint32_t devIndex, char *chipName, uint32_t length)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->GetChipName(devIndex, chipName, length);
}

int32_t InputServer::SetGestureMode(uint32_t devIndex, uint32_t gestureMode)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->SetGestureMode(devIndex, gestureMode);
}

int32_t InputServer::RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->RunCapacitanceTest(devIndex, testType, result, length);
}

int32_t InputServer::RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmd)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->RunExtraCommand(devIndex, cmd);
}

int32_t InputServer::RegisterReportCallback(uint32_t devIndex, OHOS::sptr<InputReportEventCb> callback)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->RegisterReportCallback(devIndex, callback);
}

int32_t InputServer::UnregisterReportCallback(uint32_t devIndex)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->UnregisterReportCallback(devIndex);
}

int32_t InputServer::RegisterHotPlugCallback(sptr<InputReportHostCb> callback)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->RegisterHotPlugCallback(callback);
}

int32_t InputServer::UnregisterHotPlugCallback(void)
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    return GET_INSTANCE_INPUT_DEVICE_MANAGER->UnregisterHotPlugCallback();
}

InputServer::InputServer()
{
    HDF_LOGI("InputServer %{public}s: line:%{public}d", __func__, __LINE__);
    inputDeviceManager_ = std::make_shared<InputDeviceManager>();
    if (inputDeviceManager_ != nullptr) {
        inputDeviceManager_->Init();
    }
}
}  // namespace Input
}  // namespace OHOS
