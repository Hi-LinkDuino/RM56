/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "../include/ble_scan_filter_lsf.h"

namespace OHOS {
namespace bluetooth {
BleScanFilterLsf::BleScanFilterLsf()
{
    cmdTimer_ = std::make_unique<utility::Timer>(
        std::bind(&BleScanFilterLsf::CommandTimeout, this));
}

BleScanFilterLsf::~BleScanFilterLsf()
{
    if (isReady_) {
        HCIVIF_DeregisterVendorEventCallback(&VENDOR_EVENT_CALLBACK);
        isReady_ = false;
    }
}

void BleScanFilterLsf::StartCommandTimer() const
{
    cmdTimer_->Start(LSF_CMD_TIMEOUT_MS);
    HILOGI("Start command timer!");
}

void BleScanFilterLsf::StopCommandTimer() const
{
    cmdTimer_->Stop();
    HILOGI("Stop command timer!");
}

void BleScanFilterLsf::CommandTimeout()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    if (!isBleScanFilterOngoing_) {
        HILOGI("ble scan filter not ongoing.");
        return;
    }
    switch (ongoingOpcode_) {
        case LSF_OPCODE_ENABLE:
            HandleEnableCommandResult(ongoingTag_, BLE_SCAN_FILTER_FAILD);
            break;
        case LSF_OPCODE_ADD:
            HandleAddCommandResult(ongoingTag_, BLE_SCAN_FILTER_FAILD);
            break;
        case LSF_OPCODE_DELETE:
            HandleDeleteCommandResult(ongoingTag_, BLE_SCAN_FILTER_FAILD);
            break;
        case LSF_OPCODE_CLEAR:
            HandleClearCommandResult(ongoingTag_, BLE_SCAN_FILTER_FAILD);
            break;
        default:
            HILOGI("unknow opcode.");
            break;
    }
}

void BleScanFilterLsf::HandleEnableCommandResult(uint8_t tag, uint8_t status)
{
    HILOGI("tag:%{public}d, status:%{public}d", tag, status);
    switch (tag) {
        case LSF_PARAM_ENABLE:
            StopCommandTimer();
            FinishStartBleScanFilter(status);
            break;
        case LSF_PARAM_DISABLE:
            StopCommandTimer();
            FinishStopBleScanFilter(status);
            break;
        default:
            HILOGI("unknow tag.");
            break;
    }
}
void BleScanFilterLsf::HandleAddCommandResult(uint8_t tag, uint8_t status)
{
    HILOGI("tag:%{public}d, status:%{public}d", tag, status);
    switch (tag) {
        case LSF_TAG_SET_PARAMETERS:
            StopCommandTimer();
            FinishAddBleScanFilter(status);
            break;
        case LSF_TAG_ADDRESS:
        case LSF_TAG_SERVICE_UUID:
        case LSF_TAG_SOLICIT_UUID:
        case LSF_TAG_NAME:
        case LSF_TAG_MANUFACTURER_DATA:
        case LSF_TAG_SERVICE_DATA:
            if (status == BT_NO_ERROR) {
                if (ContinueBleScanFilter() != BT_NO_ERROR) {
                    StopCommandTimer();
                    FinishAddBleScanFilter(BLE_SCAN_FILTER_FAILD);
                }
            } else {
                StopCommandTimer();
                FinishAddBleScanFilter(status);
            }
            break;
        default:
            HILOGI("unknow tag.");
            break;
    }
}

void BleScanFilterLsf::HandleDeleteCommandResult(uint8_t tag, uint8_t status)
{
    HILOGI("tag:%{public}d, status:%{public}d", tag, status);
    switch (tag) {
        case LSF_TAG_SET_PARAMETERS:
            StopCommandTimer();
            FinishDeleteBleScanFilter(status);
            break;
        default:
            HILOGI("unknow tag.");
            break;
    }
}

void BleScanFilterLsf::HandleClearCommandResult(uint8_t tag, uint8_t status)
{
    HILOGI("tag:%{public}d, status:%{public}d", tag, status);
    switch (tag) {
        case LSF_TAG_SET_PARAMETERS:
            StopCommandTimer();
            if ((BleScanFilterEnable(LSF_PARAM_DISABLE) != BT_NO_ERROR)) {
                StopCommandTimer();
                FinishStopBleScanFilter(BLE_SCAN_FILTER_FAILD);
            }
            break;
        default:
            HILOGI("unknow tag.");
            break;
    }
}

bool BleScanFilterLsf::IsReady()
{
    if (!isReady_ && (HCIVIF_RegisterVendorEventCallback(&VENDOR_EVENT_CALLBACK, this) == BT_NO_ERROR)) {
        isReady_ = true;
    }

    if (!isReady_) {
        return false;
    }

    return true;
}

uint8_t BleScanFilterLsf::GetMaxFilterNumber()
{
    if (!isSupportFilter_) {
        HILOGI("not support filter.");
        return 0;
    }
    if (maxFilterNumber_ > 0) {
        return maxFilterNumber_;
    }

    if (!IsReady()) {
        HILOGI("ble scan filter not ready.");
        return 0;
    }
    std::unique_lock<std::mutex> lock(mutexWaitCallback_);
    if (BleScanFilterGetVendorCapabilities() != BT_NO_ERROR) {
        HILOGI("send vendor capabilities cmd faild.");
        return 0;
    }
    if (cvfull_.wait_for(lock,
        std::chrono::seconds(CAP_CMD_TIMEOUT_S)) == std::cv_status::timeout) {
        HILOGI("GetMaxFilterNumber timeout");
        return 0;
    }

    return maxFilterNumber_;
}

int BleScanFilterLsf::AddBleScanFilter(BleScanFilterParam filter, BleScanFilterCallback callback)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    filterParam_ = filter;
    filterFlag_ = filterParam_.filterFlag;
    filtIndex_ = filterParam_.filtIndex;
    callback_ = callback;

    if (!IsReady()) {
        HILOGI("ble scan filter not ready.");
    } else if (isBleScanFilterOngoing_) {
        HILOGI("ble scan filter is ongoing.");
    } else if ((maxFilterNumber_ > 0) && (filtIndex_ >= maxFilterNumber_)) {
        HILOGI("filter index error.");
    } else if (ContinueBleScanFilter() != BT_NO_ERROR) {
        HILOGI("ContinueBleScanFilter error.");
        StopCommandTimer();
    } else {
        isBleScanFilterOngoing_ = true;
        return BT_NO_ERROR;
    }

    FinishAddBleScanFilter(BLE_SCAN_FILTER_FAILD);
    return RET_BAD_STATUS;
}

int BleScanFilterLsf::DeleteBleScanFilter(BleScanFilterParam filter, BleScanFilterCallback callback)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    filtIndex_ = filter.filtIndex;
    callback_ = callback;

    if (!IsReady()) {
        HILOGI("ble scan filter not ready.");
    } else if (isBleScanFilterOngoing_) {
        HILOGI("ble scan filter is ongoing.");
    } else if (BleScanFilterSetParameters(0, LSF_OPCODE_DELETE) != BT_NO_ERROR) {
        HILOGI("BleScanFilterSetParameters error.");
        StopCommandTimer();
    } else {
        isBleScanFilterOngoing_ = true;
        return BT_NO_ERROR;
    }

    FinishDeleteBleScanFilter(BLE_SCAN_FILTER_FAILD);
    return RET_BAD_STATUS;
}

int BleScanFilterLsf::StartBleScanFilter(BleScanFilterCallback callback)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    callback_ = callback;

    if (!IsReady()) {
        HILOGI("ble scan filter not ready.");
    } else if (isBleScanFilterOngoing_) {
        HILOGI("ble scan filter is ongoing.");
    } else if (BleScanFilterEnable(LSF_PARAM_ENABLE) != BT_NO_ERROR) {
        HILOGI("BleScanFilterEnable error.");
        StopCommandTimer();
    } else {
        isBleScanFilterOngoing_ = true;
        return BT_NO_ERROR;
    }

    FinishStartBleScanFilter(BLE_SCAN_FILTER_FAILD);
    return RET_BAD_STATUS;
}

int BleScanFilterLsf::StopBleScanFilter(BleScanFilterCallback callback)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    callback_ = callback;

    if (!IsReady()) {
        HILOGI("ble scan filter not ready.");
    } else if (isBleScanFilterOngoing_) {
        HILOGI("ble scan filter is ongoing.");
    } else if (BleScanFilterSetParameters(0, LSF_OPCODE_CLEAR) != BT_NO_ERROR) {
        HILOGI("BleScanFilterSetParameters error.");
        StopCommandTimer();
    } else {
        isBleScanFilterOngoing_ = true;
        return BT_NO_ERROR;
    }

    FinishStopBleScanFilter(BLE_SCAN_FILTER_FAILD);
    return RET_BAD_STATUS;
}

int BleScanFilterLsf::ContinueBleScanFilter()
{
    StopCommandTimer();
    if ((filterFlag_ & FILTER_FLAG_ADDRESS) != 0) {
        filterFlag_ &= ~FILTER_FLAG_ADDRESS;
        return BleScanFilterAddressAdd(filterParam_.address);
    } else if ((filterFlag_ & FILTER_FLAG_SERVICE_UUID) != 0) {
        filterFlag_ &= ~FILTER_FLAG_SERVICE_UUID;
        return BleScanFilterUuidAdd(filterParam_.serviceUuid, filterParam_.serviceUuidMask, LSF_TAG_SERVICE_UUID);
    } else if ((filterFlag_ & FILTER_FLAG_SOLICIT_UUID) != 0) {
        filterFlag_ &= ~FILTER_FLAG_SOLICIT_UUID;
        return BleScanFilterUuidAdd(filterParam_.solicitationUuid, filterParam_.solicitationUuidMask,
            LSF_TAG_SOLICIT_UUID);
    } else if ((filterFlag_ & FILTER_FLAG_NAME) != 0) {
        filterFlag_ &= ~FILTER_FLAG_NAME;
        return BleScanFilterNameAdd(filterParam_.name);
    } else if ((filterFlag_ & FILTER_FLAG_MANUFACTURER_DATA) != 0) {
        filterFlag_ &= ~FILTER_FLAG_MANUFACTURER_DATA;
        return BleScanFilterManufacturerDataAdd(filterParam_.manufacturerId, filterParam_.manufacturerIdMask,
            filterParam_.manufacturerData, filterParam_.manufacturerDataMask);
    } else if ((filterFlag_ & FILTER_FLAG_SERVICE_DATA) != 0) {
        filterFlag_ &= ~FILTER_FLAG_SERVICE_DATA;
        return BleScanFilterServiceDataAdd(filterParam_.serviceData, filterParam_.serviceDataMask);
    } else {
        filterFlag_ = 0;
        return BleScanFilterSetParameters(filterParam_.filterFlag, LSF_OPCODE_ADD);
    }

    return BT_NO_ERROR;
}

void BleScanFilterLsf::FinishAddBleScanFilter(int result)
{
    if (callback_.addBleScanFilterResult != nullptr) {
        callback_.addBleScanFilterResult(result, callback_.context);
    }
    ClearCommand();
}

void BleScanFilterLsf::FinishDeleteBleScanFilter(int result)
{
    if (callback_.deleteBleScanFilterResult != nullptr) {
        callback_.deleteBleScanFilterResult(result, callback_.context);
    }
    ClearCommand();
}

void BleScanFilterLsf::FinishStartBleScanFilter(int result)
{
    if (callback_.startBleScanFilterResult != nullptr) {
        callback_.startBleScanFilterResult(result, callback_.context);
    }
    ClearCommand();
}

void BleScanFilterLsf::FinishStopBleScanFilter(int result)
{
    if (callback_.stopBleScanFilterResult != nullptr) {
        callback_.stopBleScanFilterResult(result, callback_.context);
    }
    ClearCommand();
}

void BleScanFilterLsf::ClearCommand()
{
    isBleScanFilterOngoing_ = false;
    ongoingOpcode_ = 0;
    ongoingTag_ = 0;
    filtIndex_ = 0;
    filterParam_ = {};
    callback_ = {};
    filterFlag_ = 0;
}

int BleScanFilterLsf::BleScanFilterGetVendorCapabilities() const
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_GET_CAPABILITIES);
    uint8_t version = 0;

    return HCIVIF_SendCmd(opCode, &version, 1);
}

int BleScanFilterLsf::BleScanFilterEnable(uint8_t enable)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    VendorCmdParamLSFEnable param = {};
    param.opcode = LSF_OPCODE_ENABLE;
    param.enable = enable;
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.enable;
    StartCommandTimer();

    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), sizeof(VendorCmdParamLSFEnable));
}

int BleScanFilterLsf::BleScanFilterSetParameters(uint16_t featureSelection, uint8_t opcode)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    size_t paramLength = LSF_CMD_HEAD_LENGTH;
    VendorCmdParamLSFSetParameters param = {};
    param.opcode = opcode;
    param.tag = LSF_TAG_SET_PARAMETERS;
    param.filtIndex = filtIndex_;
    if (opcode == LSF_OPCODE_ADD) {
        param.featureSelection = featureSelection;
        param.listLogicType = LSF_CMD_LIST_LOGIC_TYPE;
        param.logicType = LSF_CMD_LOGIC_TYPE;
        paramLength = sizeof(VendorCmdParamLSFSetParameters);
    }
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.tag;
    StartCommandTimer();

    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), paramLength);
}

int BleScanFilterLsf::BleScanFilterAddressAdd(BtAddr address)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    VendorCmdParamLSFAddress param = {};
    param.opcode = LSF_OPCODE_ADD;
    param.tag = LSF_TAG_ADDRESS;
    param.filtIndex = filtIndex_;
    param.address = address;
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.tag;
    StartCommandTimer();

    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), sizeof(VendorCmdParamLSFAddress));
}

int BleScanFilterLsf::BleScanFilterUuidAdd(Uuid uuid, Uuid uuidMask, uint8_t tag)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    size_t paramLength = 0;
    int uuidType = uuid.GetUuidType();
    VendorCmdParamLSFUuid param = {};
    param.opcode = LSF_OPCODE_ADD;
    param.tag = tag;
    param.filtIndex = filtIndex_;
    paramLength = uuidType;
    VendorCmdParamUuidData uuidData = {0};
    VendorCmdParamUuidData uuidMaskData = {0};
    if (uuidType == Uuid::UUID16_BYTES_TYPE) {
        uuidData.uuid16 = uuid.ConvertTo16Bits();
        uuidMaskData.uuid16 = uuidMask.ConvertTo16Bits();
    } else if (uuidType == Uuid::UUID32_BYTES_TYPE) {
        uuidData.uuid32 = uuid.ConvertTo32Bits();
        uuidMaskData.uuid32 = uuidMask.ConvertTo32Bits();
    } else if (uuidType == Uuid::UUID128_BYTES_TYPE) {
        if (!uuid.ConvertToBytesLE(uuidData.uuid128)) {
            HILOGI("get uuid faild.");
            return RET_BAD_STATUS;
        }
        if (!uuidMask.ConvertToBytesLE(uuidMaskData.uuid128)) {
            HILOGI("get uuid mask faild.");
            return RET_BAD_STATUS;
        }
    } else {
        HILOGI("error uuid type(%{public}d).", uuidType);
        return RET_BAD_STATUS;
    }
    if (memcpy_s(param.uuidAndMask, sizeof(param.uuidAndMask), &uuidData, paramLength) != EOK) {
        HILOGI("copy uuid faild.");
        return RET_BAD_STATUS;
    }
    if (memcpy_s(param.uuidAndMask + paramLength, sizeof(param.uuidAndMask) - paramLength,
        &uuidMaskData, paramLength) != EOK) {
        HILOGI("copy uuid mask faild.");
        return RET_BAD_STATUS;
    }
    paramLength += uuidType;
    paramLength += LSF_CMD_HEAD_LENGTH;
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.tag;
    StartCommandTimer();

    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), paramLength);
}

int BleScanFilterLsf::BleScanFilterNameAdd(std::string name)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    size_t paramLength = 0;
    VendorCmdParamLSFName param = {};
    param.opcode = LSF_OPCODE_ADD;
    param.tag = LSF_TAG_NAME;
    param.filtIndex = filtIndex_;
    if (name.size() > LSF_CMD_MAX_LENGTH) {
        param.length = LSF_CMD_MAX_LENGTH;
    } else {
        param.length = name.size();
    }
    paramLength += param.length + 1;
    if (param.length > 0) {
        if (memcpy_s(param.name, sizeof(param.name), name.c_str(), param.length) != EOK) {
            HILOGI("copy name faild.");
            return RET_BAD_STATUS;
        }
    }
    paramLength += LSF_CMD_HEAD_LENGTH;
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.tag;
    StartCommandTimer();
    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), paramLength);
}

int BleScanFilterLsf::BleScanFilterManufacturerDataAdd(uint16_t manufacturerId, uint16_t manufacturerIdMask,
    std::vector<uint8_t> manufacturerData, std::vector<uint8_t> manufacturerDataMask)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    size_t paramLength = 0;
    uint8_t manufacturerIdLength = sizeof(uint16_t);
    uint8_t manufacturerDataLength = 0;
    VendorCmdParamLSFData param = {};
    param.opcode = LSF_OPCODE_ADD;
    param.tag = LSF_TAG_MANUFACTURER_DATA;
    param.filtIndex = filtIndex_;
    param.data[paramLength++] = manufacturerDataLength + manufacturerIdLength;

    if (memcpy_s(param.data, sizeof(param.data), &manufacturerId, manufacturerIdLength) != EOK) {
        HILOGI("copy manufacturerId faild.");
        return RET_BAD_STATUS;
    }
    paramLength += manufacturerIdLength;

    if (manufacturerData.size() > LSF_CMD_MAX_LENGTH - manufacturerIdLength) {
        manufacturerDataLength = LSF_CMD_MAX_LENGTH - manufacturerIdLength;
    } else {
        manufacturerDataLength = manufacturerData.size();
    }

    if (manufacturerDataLength > 0) {
        if (memcpy_s(param.data + paramLength, sizeof(param.data) - paramLength,
            manufacturerData.data(), manufacturerDataLength) != EOK) {
            HILOGI("copy manufacturer data faild.");
            return RET_BAD_STATUS;
        }
    }
    paramLength += manufacturerDataLength;
    param.data[paramLength++] = manufacturerDataLength + manufacturerIdLength;

    if (memcpy_s(param.data + paramLength, sizeof(param.data) - paramLength,
        &manufacturerIdMask, manufacturerIdLength) != EOK) {
        HILOGI("copy manufacturerId mask faild.");
        return RET_BAD_STATUS;
    }
    paramLength += manufacturerIdLength;

    if (manufacturerDataLength > 0) {
        if (memcpy_s(param.data + paramLength, sizeof(param.data) - paramLength,
            manufacturerDataMask.data(), manufacturerDataLength) != EOK) {
            HILOGI("copy manufacturer data mask faild.");
            return RET_BAD_STATUS;
        }
    }
    paramLength += manufacturerDataLength;

    paramLength += LSF_CMD_MAX_LENGTH;
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.tag;
    StartCommandTimer();

    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), paramLength);
}

int BleScanFilterLsf::BleScanFilterServiceDataAdd(
    std::vector<uint8_t> serviceData, std::vector<uint8_t> serviceDataMask)
{
    uint16_t opCode = MakeVendorOpCode(HCI_VENDOR_OCF_LSF);
    size_t paramLength = 0;
    uint8_t serviceDataLength = 0;
    VendorCmdParamLSFData param = {};
    param.opcode = LSF_OPCODE_ADD;
    param.tag = LSF_TAG_SERVICE_DATA;
    param.filtIndex = filtIndex_;

    if (serviceData.size() > LSF_CMD_MAX_LENGTH) {
        serviceDataLength = LSF_CMD_MAX_LENGTH;
    } else {
        serviceDataLength = serviceData.size();
    }
    param.data[paramLength++] = serviceDataLength;
    if (serviceDataLength > 0) {
        if (memcpy_s(param.data, sizeof(param.data), serviceData.data(), serviceDataLength) != EOK) {
            HILOGI("copy service data faild.");
            return RET_BAD_STATUS;
        }
        paramLength += serviceDataLength;
        param.data[paramLength++] = serviceDataLength;
        if (memcpy_s(param.data + paramLength, sizeof(param.data) - paramLength, serviceDataMask.data(),
            serviceDataLength) != EOK) {
            HILOGI("copy service data mask faild.");
            return RET_BAD_STATUS;
        }
        paramLength += serviceDataLength;
    }

    paramLength += LSF_CMD_HEAD_LENGTH;
    ongoingOpcode_ = param.opcode;
    ongoingTag_ = param.tag;
    StartCommandTimer();

    return HCIVIF_SendCmd(opCode, static_cast<void *>(&param), paramLength);
}

uint16_t BleScanFilterLsf::MakeVendorOpCode(uint16_t ocf)
{
    return (ocf | (HCI_COMMAND_OGF_VENDOR_SPECIFIC << HCI_OGF_OFFSET));
}

void BleScanFilterLsf::VendEventCommandCompleteCallback(
    uint16_t opCode, const void *param, uint8_t paramLength, void *context)
{
    if ((opCode != MakeVendorOpCode(HCI_VENDOR_OCF_GET_CAPABILITIES)) &&
        (opCode != MakeVendorOpCode(HCI_VENDOR_OCF_LSF))) {
        return;
    }
    BleScanFilterLsf *filter = static_cast<BleScanFilterLsf *>(context);
    if (filter == nullptr) {
        HILOGI("BleScanFilter is null.");
        return;
    }

    if (opCode == MakeVendorOpCode(HCI_VENDOR_OCF_GET_CAPABILITIES)) {
        filter->BleScanFilterGetCapabilitiesComplete(param, paramLength);
    } else {
        filter->BleScanFilterCommandComplete(param, paramLength);
    }
}

void BleScanFilterLsf::HciVendorClosedCallback(void *context)
{
    BleScanFilterLsf *filter = static_cast<BleScanFilterLsf *>(context);
    if (filter == nullptr) {
        HILOGI("BleScanFilter is null.");
    } else {
        std::lock_guard<std::recursive_mutex> lk(filter->mutex_);
        filter->isReady_ = false;
    }
}

void BleScanFilterLsf::BleScanFilterGetCapabilitiesComplete(const void *param, uint8_t paramLength)
{
    std::lock_guard<std::mutex> lock(mutexWaitCallback_);
    VendorEventParamCAP eventParam = {};
    if (paramLength >= sizeof(VendorEventParamCAP)) {
        if (memcpy_s(&eventParam, sizeof(VendorEventParamCAP), param, sizeof(VendorEventParamCAP)) != EOK) {
            HILOGI("copy eventParam faild.");
            cvfull_.notify_all();
            return;
        }
    } else {
        HILOGI("paramLength is error.");
        cvfull_.notify_all();
        return;
    }
    if (eventParam.featureTag != CAP_FEATURE_TAG_BLE_SCAN_FILTER) {
        HILOGI("featureTag is error featureTag=%{public}d.", eventParam.featureTag);
    } else if (eventParam.status != BT_NO_ERROR) {
        HILOGI("get capabilities failed status=%{public}d.", eventParam.status);
    } else if (eventParam.featureSize != CAP_LSF_FEATURE_VALUE_LENGTH) {
        HILOGI("featureSize is error size=%{public}d.", eventParam.featureSize);
    } else {
        isSupportFilter_ = eventParam.filterSupport;
        maxFilterNumber_ = eventParam.maxFilter;
    }

    cvfull_.notify_all();
}

void BleScanFilterLsf::BleScanFilterCommandComplete(const void *param, uint8_t paramLength)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    if (paramLength >= sizeof(VendorEventParamLSF) - 1) {
        VendorEventParamLSF eventParam = {};
        uint8_t length = (paramLength > sizeof(VendorEventParamLSF)) ? sizeof(VendorEventParamLSF) : paramLength;
        if (memcpy_s(&eventParam, sizeof(VendorEventParamLSF), param, length) != EOK) {
            HILOGI("copy eventParam faild.");
            return;
        }

        if (eventParam.status == BT_NO_ERROR) {
            BleScanFilterCommandCompleteSuccess(eventParam);
        } else {
            BleScanFilterCommandCompleteFaild(eventParam);
        }
    }
}

void BleScanFilterLsf::BleScanFilterCommandCompleteSuccess(VendorEventParamLSF eventParam)
{
    if ((eventParam.opcode != ongoingOpcode_) || (eventParam.tag != ongoingTag_)) {
        HILOGI("not ongonging opcode.");
        return;
    }

    switch (eventParam.opcode) {
        case LSF_OPCODE_ENABLE:
            HandleEnableCommandResult(eventParam.tag, eventParam.status);
            break;
        case LSF_OPCODE_ADD:
            HandleAddCommandResult(eventParam.tag, eventParam.status);
            break;
        case LSF_OPCODE_DELETE:
            HandleDeleteCommandResult(eventParam.tag, eventParam.status);
            break;
        case LSF_OPCODE_CLEAR:
            HandleClearCommandResult(eventParam.tag, eventParam.status);
            break;
        default:
            HILOGI("unknow opcode.");
            break;
    }
}

void BleScanFilterLsf::BleScanFilterCommandCompleteFaild(VendorEventParamLSF eventParam)
{
    if (eventParam.opcode != ongoingOpcode_) {
        HILOGI("not ongonging opcode.");
        return;
    }

    switch (eventParam.opcode) {
        case LSF_OPCODE_ENABLE:
            HandleEnableCommandResult(ongoingTag_, eventParam.status);
            break;
        case LSF_OPCODE_ADD:
            HandleAddCommandResult(ongoingTag_, eventParam.status);
            break;
        case LSF_OPCODE_DELETE:
            HandleDeleteCommandResult(ongoingTag_, eventParam.status);
            break;
        case LSF_OPCODE_CLEAR:
            HandleClearCommandResult(ongoingTag_, eventParam.status);
            break;
        default:
            HILOGI("unknow opcode.");
            break;
    }
}
}  // namespace bluetooth
}  // namespace OHOS
