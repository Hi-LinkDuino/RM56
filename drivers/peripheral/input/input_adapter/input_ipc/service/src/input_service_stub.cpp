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

#include <array>
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include "input_server_stub.h"
#include "securec.h"

#define HDF_LOG_TAG InputServerStub

namespace OHOS {
namespace Input {
InputServerStub::InputServerStub() {}

int32_t InputServerStub::InputServerStubScanInputDevice(MessageParcel &data,
                                                        MessageParcel &reply,
                                                        MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t arrLen = data.ReadUint32();
    if (arrLen > 32) {  // 32:max devices num
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    DevDesc* staArr = new DevDesc[arrLen];
    memset_s(staArr, sizeof(DevDesc) * arrLen, 0,  sizeof(DevDesc) * arrLen);
    int32_t ret = mService_->ScanInputDevice(staArr, arrLen);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(sizeof(DevDesc) * arrLen)) {
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteBuffer((void*)staArr, sizeof(DevDesc) * arrLen)) {
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubOpenInputDevice(MessageParcel &data,
                                                        MessageParcel &reply,
                                                        MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputHostStub:: %{public}s:called", __func__);
    int32_t index = data.ReadUint32();
    int32_t ret = mService_->OpenInputDevice(index);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: OpenInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteInt32(ret)) {
        HDF_LOGE("InputServerStub %{public}s: OpenInputDevice failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubCloseInputDevice(MessageParcel &data,
                                                         MessageParcel &reply,
                                                         MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub:: %{public}s:called", __func__);
    int32_t index = data.ReadUint32();
    int32_t ret = mService_->CloseInputDevice(index);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: CloseInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteInt32(ret)) {
        HDF_LOGE("InputServerStub %{public}s: CloseInputDevice failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetInputDevice(MessageParcel &data,
                                                       MessageParcel &reply,
                                                       MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t index = data.ReadUint32();
    HDF_LOGE("InputServerStub:: %{public}s:called line%{public}d index:%{public}d", __func__, __LINE__, index);
    DeviceInfo* devInfo = (DeviceInfo*)new DeviceInfo();
    if (devInfo == nullptr) {
        HDF_LOGE("InputServerStub:: %{public}s:called failed line%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    int32_t ret = mService_->GetInputDevice(index, &devInfo);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub:: %{public}s:called failed line%{public}d", __func__, __LINE__);
        delete devInfo;
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(sizeof(DeviceInfo))) {
        HDF_LOGE("InputServerStub %{public}s: GetInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteBuffer((void*)devInfo, sizeof(DeviceInfo))) {
        HDF_LOGE("InputServerStub %{public}s: GetInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    HDF_LOGE("InputServerStub:: %{public}s:called line%{public}d", __func__, __LINE__);
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetInputDeviceList(MessageParcel &data,
                                                           MessageParcel &reply,
                                                           MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub:: %{public}s:called line%{public}d", __func__, __LINE__);
    uint32_t devNum = MAX_INPUT_DEV_NUM;
    uint32_t size = data.ReadUint32();
    DeviceInfo* devInfo = new DeviceInfo[devNum];
    if (devInfo == nullptr) {
        HDF_LOGE("InputServerStub:: %{public}s:called failed line%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    int32_t ret = mService_->GetInputDeviceList(&devNum, &devInfo, size);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub:: %{public}s:called failed line%{public}d", __func__, __LINE__);
        delete[] devInfo;
        return HDF_FAILURE;
    }
    HDF_LOGE("InputServerStub:: %{public}s:called line%{public}d devNum:%{public}d", __func__, __LINE__, devNum);
    if (!reply.WriteUint32(devNum)) {
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(sizeof(DeviceInfo) * devNum)) {
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteBuffer((void*)devInfo, sizeof(DeviceInfo) * devNum)) {
        HDF_LOGE("InputServerStub %{public}s: ScanInputDevice failed line %{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    HDF_LOGE("InputServerStub:: %{public}s:called line%{public}d", __func__, __LINE__);
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubSetPowerStatus(MessageParcel &data,
                                                       MessageParcel &reply,
                                                       MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub::%{public}s start", __func__);
    int32_t index = data.ReadUint32();
    uint32_t status = data.ReadUint32();
    int32_t ret = mService_->SetPowerStatus(index, status);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("InputServerStub %{public}s: SetPowerStatus failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    HDF_LOGE("InputServerStub::%{public}s end", __func__);
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetPowerStatus(MessageParcel &data,
                                                       MessageParcel &reply,
                                                       MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub::%{public}s start", __func__);
    int32_t index = data.ReadInt32();
    uint32_t status {};
    int32_t ret = mService_->GetPowerStatus(index, &status);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: GetDeviceType failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(status)) {
        HDF_LOGE("InputServerStub %{public}s: GetDeviceType failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    HDF_LOGE("InputServerStub::%{public}s end", __func__);
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetDeviceType(MessageParcel &data,
                                                      MessageParcel &reply,
                                                      MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub::%{public}s", __func__);
    int32_t index = data.ReadInt32();
    uint32_t deviceType {};
    int32_t ret = mService_->GetDeviceType(index, &deviceType);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: GetDeviceType failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(deviceType)) {
        HDF_LOGE("InputServerStub %{public}s: GetDeviceType failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetChipInfo(MessageParcel &data,
                                                    MessageParcel &reply,
                                                    MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t index = data.ReadInt32();
    uint32_t length = data.ReadUint32();
    char* chipInfo = new char[MAX_NODE_PATH_LEN];
    int32_t ret = mService_->GetChipInfo(index, chipInfo, length);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: GetChipInfo failed line:%{public}d", __func__, __LINE__);
        delete[] chipInfo;
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(strlen(chipInfo) + 1)) {
        HDF_LOGE("InputServerStub %{public}s: GetChipInfo failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteBuffer(chipInfo, strlen(chipInfo) + 1)) {
        HDF_LOGE("InputServerStub %{public}s: GetChipInfo failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetVendorName(MessageParcel &data,
                                                      MessageParcel &reply,
                                                      MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub::%{public}s", __func__);
    int32_t index = data.ReadInt32();
    uint32_t length = data.ReadUint32();
    char* vendorName = new char[MAX_NODE_PATH_LEN];
    int32_t ret = mService_->GetVendorName(index, vendorName, length);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: GetVendorName failed line:%{public}d", __func__, __LINE__);
        delete[] chipInfo;
        return HDF_FAILURE;
    }
    HDF_LOGE("InputServerStub %{public}s: GetVendorName line:%{public}d vendorName:%{public}s",
             __func__, __LINE__, vendorName);
    if (!reply.WriteUint32(strlen(vendorName) + 1)) {
        HDF_LOGE("InputServerStub %{public}s: GetVendorName failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteBuffer(vendorName, strlen(vendorName) + 1)) {
        HDF_LOGE("InputServerStub %{public}s: GetVendorName failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubGetChipName(MessageParcel &data,
                                                    MessageParcel &reply,
                                                    MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("InputServerStub::%{public}s", __func__);
    int32_t index = data.ReadInt32();
    uint32_t length = data.ReadUint32();
    char* chipName = new char[MAX_NODE_PATH_LEN];
    int32_t ret = mService_->GetChipName(index, chipName, length);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("InputServerStub %{public}s: GetChipName failed line:%{public}d", __func__, __LINE__);
        delete[] chipName;
        return HDF_FAILURE;
    }
    if (!reply.WriteUint32(strlen(chipName) + 1)) {
        HDF_LOGE("InputServerStub %{public}s: GetChipName failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!reply.WriteBuffer(chipName, strlen(chipName) + 1)) {
        HDF_LOGE("InputServerStub %{public}s: GetChipName failed line:%{public}d", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubSetGestureMode(MessageParcel &data,
                                                       MessageParcel &reply,
                                                       MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("InputServerStub::%{public}s", __func__);
    return HDF_SUCCESS;
}
int32_t InputServerStub::InputServerStubRunCapacitanceTest(MessageParcel &data,
                                                           MessageParcel &reply,
                                                           MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("InputServerStub::%{public}s", __func__);
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubRunExtraCommand(MessageParcel &data,
                                                        MessageParcel &reply,
                                                        MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("InputServerStub::%{public}s", __func__);
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubRegisterReportCallback(MessageParcel &data,
                                                               MessageParcel &reply,
                                                               MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t index = data.ReadInt32();
    bool flag = data.ReadBool();
    HDF_LOGI("InputServerStub:: send flag:%{public}d", flag);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    sptr<InputReportEventCb> callBackInfo = iface_cast<InputReportEventCb>(remote);
    if (callBackInfo == nullptr) {
        HDF_LOGE("InputServerStub %{public}s: callBackInfo is nullptr", __func__);
        return HDF_FAILURE;
    }
    int32_t ret = mService_->RegisterReportCallback(index, callBackInfo);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("InputServerStub %{public}s: RegisterReportCallback failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubUnregisterReportCallback(MessageParcel &data,
                                                                 MessageParcel &reply,
                                                                 MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t index = data.ReadInt32();
    int32_t ret = mService_->UnregisterReportCallback(index);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("InputServerStub %{public}s: UnregisterReportCallback failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubRegisterHotPlugCallback(MessageParcel &data,
                                                                MessageParcel &reply,
                                                                MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    bool flag = data.ReadBool();
    HDF_LOGE("InputServerStub::%{public}s send flag:%{public}d", __func__, flag);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<InputReportHostCb> callBackInfo = iface_cast<InputReportHostCb>(remote);
    if (callBackInfo == nullptr) {
        HDF_LOGE("InputServerStub %{public}s: callBackInfo is nullptr", __func__);
        return HDF_FAILURE;
    }
    int32_t ret = mService_->RegisterHotPlugCallback(callBackInfo);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("InputServerStub %{public}s: RegisterReportCallback failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubUnregisterHotPlugCallback(MessageParcel &data,
                                                                  MessageParcel &reply,
                                                                  MessageOption &option) const
{
    if (data.ReadInterfaceToken() != InputServerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = mService_->UnregisterHotPlugCallback();
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("InputServerStub %{public}s: UnregisterHotPlugCallback failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputServerStub::InputServerStubOnRemoteRequest(int32_t cmdId,
                                                        MessageParcel& data,
                                                        MessageParcel& reply,
                                                        MessageOption& option)
{
    switch (cmdId) {
        case CMD_INPUT_SCAN_DEVICE: {
            return InputServerStubScanInputDevice(data, reply, option);
        }
        case CMD_INPUT_OPNE_DEVICE: {
            return InputServerStubOpenInputDevice(data, reply, option);
        }
        case CMD_INPUT_CLOSE_DEVICE: {
            return InputServerStubCloseInputDevice(data, reply, option);
        }
        case CMD_INPUT_GET_DEVICE: {
            return InputServerStubGetInputDevice(data, reply, option);
        }
        case CMD_INPUT_GET_DEVICE_LIST: {
            return InputServerStubGetInputDeviceList(data, reply, option);
        }
        case CMD_INPUT_GET_CHIP_INFO: {
            return InputServerStubGetChipInfo(data, reply, option);
        }
        case CMD_INPUT_GET_VENDOR_NAME: {
            return InputServerStubGetVendorName(data, reply, option);
        }
        case CMD_INPUT_GET_CHIP_NAME: {
            return InputServerStubGetChipName(data, reply, option);
        }
        case CMD_INPUT_REGISTER_DEVICE: {
            return InputServerStubRegisterReportCallback(data, reply, option);
        }
        case CMD_INPUT_UNREGISTER_DEVICE: {
            return InputServerStubUnregisterReportCallback(data, reply, option);
        }
        case CMD_INPUT_REGISTER_HOT_PLAUS_DEVICE: {
            return InputServerStubRegisterHotPlugCallback(data, reply, option);
        }
        case CMD_INPUT_UNREGISTER_HOT_PLAUS_DEVICE: {
            return InputServerStubUnregisterHotPlugCallback(data, reply, option);
        }
        default: {
            return HDF_ERR_INVALID_PARAM;
        }
    }
}
}  // namespace Input
}  // namespace OHOS

void *InputStubInstance()
{
    HDF_LOGI("%{public}s: line%{public}d!", __func__, __LINE__);
    using namespace OHOS::Input;
    return reinterpret_cast<void *>(new InputServerStub());
}

void InputStubRelease(void *obj)
{
    using namespace OHOS::Input;
    delete reinterpret_cast<InputServerStub *>(obj);
}

int32_t InputServiceOnRemoteRequest(void *stub, int32_t cmdId, struct HdfSBuf& data, struct HdfSBuf& reply)
{
    HDF_LOGI("%{public}s: line%{public}d!", __func__, __LINE__);
    using namespace OHOS::Input;
    InputServerStub *inuptStub = reinterpret_cast<InputServerStub *>(stub);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    (void)SbufToParcel(&reply, &replyParcel);
    if (SbufToParcel(&data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    OHOS::MessageOption option;
    return inuptStub->InputServerStubOnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}
