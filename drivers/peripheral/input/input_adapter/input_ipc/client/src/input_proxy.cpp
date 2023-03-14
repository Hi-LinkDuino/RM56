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

#include "input_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
#include "securec.h"

#define HDF_LOG_TAG InputProxy

namespace OHOS {
namespace Input {
int32_t InputProxy::SetPowerStatus(uint32_t devIndex, uint32_t status)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("%{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(status)) {
        HDF_LOGE("InputProxy %{public}s: write status failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_SET_POWER_STATUS_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetPowerStatus(uint32_t devIndex, uint32_t *status)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_POWER_STATUS_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    *status = reply.ReadUint32();
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetDeviceType(uint32_t devIndex, uint32_t *status)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_DEVICE_TYPE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    *status = reply.ReadUint32();
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(length)) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_CHIP_INFO_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    if (chipInfo == nullptr) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_FAILURE;
    }
    uint32_t retValueLen = reply.ReadUint32();
    HDF_LOGD("InputProxy %{public}s: called retValueLen:%{public}d", __func__, retValueLen);
    ret = memcpy_s(chipInfo, retValueLen, reply.ReadBuffer(retValueLen), retValueLen);
    if (ret != 0) {
        HDF_LOGE("InputProxy %{public}s: memcpy_s failed.", __func__);
        return INPUT_FAILURE;
    }
    HDF_LOGI("InputProxy %{public}s: called chipName:%{public}s", __func__, chipInfo);
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(length)) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_VENDOR_NAME_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_FAILURE;
    }
    if (vendorName == nullptr) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_FAILURE;
    }
    uint32_t retValueLen = reply.ReadUint32();
    HDF_LOGI("InputProxy %{public}s: called retValueLen:%{public}d", __func__, retValueLen);
    ret = memcpy_s(vendorName, retValueLen, reply.ReadBuffer(retValueLen), retValueLen);
    if (ret != 0) {
        HDF_LOGE("InputProxy %{public}s: memcpy_s failed.", __func__);
        return INPUT_FAILURE;
    }
    HDF_LOGI("InputProxy %{public}s: called vendorName:%{public}s", __func__, vendorName);
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetChipName(uint32_t devIndex, char *chipName, uint32_t length)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(length)) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_CHIP_NAME_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    if (chipName == nullptr) {
        HDF_LOGE("%{public}s: write length failed.", __func__);
        return INPUT_FAILURE;
    }
    uint32_t retValueLen = reply.ReadUint32();
    HDF_LOGI("InputProxy %{public}s: called retValueLen:%{public}d", __func__, retValueLen);
    ret = memcpy_s(chipName, retValueLen, reply.ReadBuffer(retValueLen), retValueLen);
    if (ret != 0) {
        HDF_LOGE("%{public}s: memcpy_s failed.", __func__);
        return INPUT_FAILURE;
    }
    HDF_LOGI("InputProxy %{public}s: called chipName:%{public}s", __func__, chipName);
    return INPUT_SUCCESS;
}

int32_t InputProxy::SetGestureMode(uint32_t devIndex, uint32_t gestureMode)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(gestureMode)) {
        HDF_LOGE("InputProxy %{public}s: write gestureMode failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_SET_GESTURE_MODE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("%{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(testType)) {
        HDF_LOGE("%{public}s: write testType failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(length)) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_RUN_CAPACITANCE_TEST_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    if (result == nullptr) {
        HDF_LOGE("InputProxy %{public}s: write length failed.", __func__);
        return INPUT_FAILURE;
    }
    ret = memcpy_s(result, reply.ReadString().length(), reply.ReadString().c_str(), reply.ReadString().length());
    if (ret != 0) {
        HDF_LOGE("InputProxy %{public}s: memcpy_s failed.", __func__);
        return INPUT_FAILURE;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmd)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_RUN_EXTRA_COMMAND_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::ScanInputDevice(DevDesc *staArr, uint32_t arrLen)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(arrLen)) {
        HDF_LOGE("InputProxy %{public}s: write arrLen failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_SCAN_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    uint32_t readValLen = reply.ReadUint32();
    HDF_LOGD("InputProxy %{public}s: called line:%{public}d readValLen:%{public}d cnt:%{public}d",
             __func__, __LINE__, readValLen, readValLen/sizeof(DevDesc));
    DevDesc* tmpStaArr = (DevDesc*)reply.ReadBuffer(readValLen);
    if (tmpStaArr != nullptr) {
        memcpy_s(staArr, readValLen, tmpStaArr, readValLen);
    }
    for (uint32_t i = 0; i < arrLen; i++) {
        HDF_LOGE("InputProxy %{public}s: called line:%{public}d index:%{public}d type:%{public}d",
                 __func__, __LINE__, (staArr+i)->devIndex, (staArr+i)->devType);
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::OpenInputDevice(uint32_t devIndex)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_OPNE_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::CloseInputDevice(uint32_t devIndex)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};
    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_CLOSE_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_INVALID_PARAM;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetInputDevice(uint32_t devIndex, DeviceInfo **devInfo)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_FAILURE;
    }
    uint32_t readLen = reply.ReadUint32();
    DeviceInfo* tmpDevInfo = (DeviceInfo*) new DeviceInfo();
    if (tmpDevInfo == nullptr) {
        HDF_LOGE("InputProxy %{public}s: called line:%{public}d tmpDevInfo:%{public}p",
                 __func__, __LINE__, tmpDevInfo);
        return INPUT_FAILURE;
    } else {
        *devInfo = tmpDevInfo;
    }
    (void)memcpy_s(tmpDevInfo, readLen, (DeviceInfo*)(reply.ReadBuffer(readLen)), readLen);
    return INPUT_SUCCESS;
}

int32_t InputProxy::GetInputDeviceList(uint32_t *devNum, DeviceInfo **devList, uint32_t size)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(size)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_GET_DEVICE_LIST_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy %{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INPUT_FAILURE;
    }
    *devNum = reply.ReadUint32();
    if (*devList == nullptr) {
        HDF_LOGE("InputProxy %{public}s: called line:%{public}d ", __func__, __LINE__);
        return INPUT_FAILURE;
    }
    uint32_t readLen = reply.ReadUint32();
    (void)memcpy_s(*devList, readLen, (DeviceInfo*)(reply.ReadBuffer(readLen)), readLen);
    HDF_LOGI("devNum:%{public}d devList:%{public}p", *devNum, *devList);
    return INPUT_SUCCESS;
}

int32_t InputProxy::RegisterReportCallback(uint32_t devIndex, OHOS::sptr<InputReportEventCb> callback)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("InputProxy %{public}s: write devIndex failed.", __func__);
        return INPUT_FAILURE;
    }
    if (callback == nullptr) {
        HDF_LOGE("InputProxy %{public}s: called line:%{public}d", __func__, __LINE__);
        return INPUT_FAILURE;
    }
    if (!data.WriteBool(true)) {
        HDF_LOGE("%s: set callback flag failed!", __func__);
        return INPUT_FAILURE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HDF_LOGE("%s: set callback write remote obj failed!", __func__);
        return INPUT_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_REGISTER_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy%{public}s: SendRequest failed, error code is %{public}d line %{public}d",
                 __func__, ret, __LINE__);
        return INPUT_FAILURE;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::UnregisterReportCallback(uint32_t devIndex)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (!data.writeInterfaceToken(InputProxy::GetDescriptor()) || !data.WriteUint32(devIndex)) {
        HDF_LOGE("%{public}s: write devIndex failed.", __func__);
        return INPUT_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_UNREGISTER_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy%{public}s: SendRequest failed, error code is %{public}d line %{public}d",
                 __func__, ret, __LINE__);
        return INPUT_FAILURE;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::RegisterHotPlugCallback(OHOS::sptr<InputReportHostCb> callback)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    if (callback == nullptr) {
        HDF_LOGE("InputProxy %{public}s: called line:%{public}d", __func__, __LINE__);
        return INPUT_FAILURE;
    }
    HDF_LOGE("InputProxy %{public}s: hostSptr is %{public}p", __func__, callback.GetRefPtr());
    if (!data.WriteBool(true)) {
        HDF_LOGE("%s: set callback flag failed!", __func__);
        return INPUT_FAILURE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HDF_LOGE("%s: set callback write remote obj failed!", __func__);
        return INPUT_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_REGISTER_HOT_PLAUS_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy%{public}s: SendRequest failed, error code is %{public}d line %{public}d",
                 __func__, ret, __LINE__);
        return INPUT_FAILURE;
    }
    return INPUT_SUCCESS;
}

int32_t InputProxy::UnregisterHotPlugCallback(void)
{
    MessageParcel data = {};
    MessageParcel reply = {};
    MessageOption option = {};

    int32_t ret = Remote()->SendRequest(CMD_INPUT_UNREGISTER_HOT_PLAUS_DEVICE_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("InputProxy%{public}s: SendRequest failed, error code is %{public}d line %{public}d",
                 __func__, ret, __LINE__);
        return INPUT_FAILURE;
    }
    return INPUT_SUCCESS;
}
}  // namespace Input
}  // namespace OHOS
