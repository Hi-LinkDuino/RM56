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

#ifndef BLE_SCAN_FILTER_LSF_H
#define BLE_SCAN_FILTER_LSF_H

#include <memory>
#include <mutex>

#include "i_ble_scan_filter.h"
#include "bt_def.h"
#include "btstack.h"
#include "hci_vendor_if.h"
#include "log.h"
#include "securec.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
static const int HCI_OGF_OFFSET = 10;
static const uint16_t HCI_VENDOR_OCF_LSF = 0x601;
static const uint16_t HCI_VENDOR_OCF_GET_CAPABILITIES = 0x600;

static const uint8_t LSF_OPCODE_ENABLE = 0x00;
static const uint8_t LSF_OPCODE_ADD = 0x01;
static const uint8_t LSF_OPCODE_DELETE = 0x02;
static const uint8_t LSF_OPCODE_CLEAR = 0x03;

static const uint8_t LSF_PARAM_ENABLE = 0x01;
static const uint8_t LSF_PARAM_DISABLE = 0x00;

static const uint8_t LSF_TAG_SET_PARAMETERS = 0x00;
static const uint8_t LSF_TAG_ADDRESS = 0x01;
static const uint8_t LSF_TAG_SERVICE_UUID = 0x02;
static const uint8_t LSF_TAG_SOLICIT_UUID = 0x03;
static const uint8_t LSF_TAG_NAME = 0x04;
static const uint8_t LSF_TAG_MANUFACTURER_DATA = 0x05;
static const uint8_t LSF_TAG_SERVICE_DATA = 0x06;

static const uint8_t LSF_CMD_MAX_LENGTH = 29;

static const uint8_t LSF_CMD_HEAD_LENGTH = 3;

static const uint16_t LSF_CMD_LIST_LOGIC_TYPE = 0x007F;
static const uint8_t LSF_CMD_LOGIC_TYPE = 0x01;

static const uint8_t CAP_FEATURE_TAG_BLE_SCAN_FILTER = 0x01;
static const uint8_t CAP_LSF_FEATURE_VALUE_LENGTH = 3;

static const int LSF_CMD_TIMEOUT_MS = 5000;  // 5s
static const int CAP_CMD_TIMEOUT_S = 5;  // 5s

#pragma pack(1)
struct VendorCmdParamLSFEnable {
    uint8_t opcode;
    uint8_t enable;
};

struct VendorCmdParamLSFSetParameters {
    uint8_t opcode;
    uint8_t filtIndex;
    uint8_t tag;
    uint16_t featureSelection;
    uint16_t listLogicType;
    uint8_t logicType;
};

struct VendorCmdParamLSFAddress {
    uint8_t opcode;
    uint8_t filtIndex;
    uint8_t tag;
    BtAddr address;
};

struct VendorCmdParamLSFUuid {
    uint8_t opcode;
    uint8_t filtIndex;
    uint8_t tag;
    uint8_t uuidAndMask[Uuid::UUID128_BYTES_TYPE + Uuid::UUID128_BYTES_TYPE];
};

union VendorCmdParamUuidData {
    uint16_t uuid16;
    uint32_t uuid32;
    uint8_t uuid128[Uuid::UUID128_BYTES_TYPE];
};

struct VendorCmdParamLSFName {
    uint8_t opcode;
    uint8_t filtIndex;
    uint8_t tag;
    uint8_t length;
    uint8_t name[LSF_CMD_MAX_LENGTH];
};

struct VendorCmdParamLSFData {
    uint8_t opcode;
    uint8_t filtIndex;
    uint8_t tag;
    uint8_t data[LSF_CMD_MAX_LENGTH + 1 + LSF_CMD_MAX_LENGTH + 1];
};

struct VendorEventParamLSF {
    uint8_t status;
    uint8_t opcode;
    uint8_t tag;
};

struct VendorEventParamCAP {
    uint8_t status;
    uint8_t featureTag;
    uint8_t featureSize;
    uint8_t filterSupport;
    uint8_t maxFilter;
    uint8_t maxCondition;
};
#pragma pack()

class BleScanFilterLsf : public IBleScanFilter {
public:
    BleScanFilterLsf();
    ~BleScanFilterLsf() override;
    uint8_t GetMaxFilterNumber() override;
    int AddBleScanFilter(BleScanFilterParam filter, BleScanFilterCallback callback) override;
    int DeleteBleScanFilter(BleScanFilterParam filter, BleScanFilterCallback callback) override;
    int StartBleScanFilter(BleScanFilterCallback callback) override;
    int StopBleScanFilter(BleScanFilterCallback callback) override;

private:
    bool IsReady();
    static void VendEventCommandCompleteCallback(
        uint16_t opCode, const void *param, uint8_t paramLength, void *context);
    static void HciVendorClosedCallback(void *context);
    void BleScanFilterCommandComplete(const void *param, uint8_t paramLength);
    void BleScanFilterCommandCompleteSuccess(VendorEventParamLSF eventParam);
    void BleScanFilterCommandCompleteFaild(VendorEventParamLSF eventParam);
    int ContinueBleScanFilter();
    void FinishAddBleScanFilter(int result);
    void FinishDeleteBleScanFilter(int result);
    void FinishStartBleScanFilter(int result);
    void FinishStopBleScanFilter(int result);
    void ClearCommand();
    int BleScanFilterEnable(uint8_t enable);
    int BleScanFilterSetParameters(uint16_t featureSelection, uint8_t opcode);
    int BleScanFilterAddressAdd(BtAddr address);
    int BleScanFilterUuidAdd(Uuid uuid, Uuid uuidMask, uint8_t command);
    int BleScanFilterNameAdd(std::string name);
    int BleScanFilterManufacturerDataAdd(uint16_t manufacturerId, uint16_t manufacturerIdMask,
        std::vector<uint8_t> manufacturerData, std::vector<uint8_t> manufacturerDataMask);
    int BleScanFilterServiceDataAdd(std::vector<uint8_t> serviceData, std::vector<uint8_t> serviceDataMask);
    static uint16_t MakeVendorOpCode(uint16_t ocf);
    int BleScanFilterGetVendorCapabilities() const;
    void BleScanFilterGetCapabilitiesComplete(const void *param, uint8_t paramLength);

    void CommandTimeout();
    void HandleEnableCommandResult(uint8_t tag, uint8_t status);
    void HandleAddCommandResult(uint8_t tag, uint8_t status);
    void HandleDeleteCommandResult(uint8_t tag, uint8_t status);
    void HandleClearCommandResult(uint8_t tag, uint8_t status);
    void StartCommandTimer() const;
    void StopCommandTimer() const;

    // Regist vendor event callback
    static constexpr HCIVendorEventCallback VENDOR_EVENT_CALLBACK = {
        VendEventCommandCompleteCallback,
        HciVendorClosedCallback
    };

    uint8_t filtIndex_ = 0;
    BleScanFilterParam filterParam_ = {};
    BleScanFilterCallback callback_ = {};
    uint16_t filterFlag_ = 0;
    bool isBleScanFilterOngoing_ = false;
    uint8_t ongoingOpcode_ = 0;
    uint8_t ongoingTag_ = 0;
    bool isReady_ = false;
    std::recursive_mutex mutex_ {};
    std::condition_variable cvfull_;
    std::mutex mutexWaitCallback_;
    uint8_t maxFilterNumber_ = 0;
    bool isSupportFilter_ = true;

    std::unique_ptr<utility::Timer> cmdTimer_ {nullptr};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // BLE_SCAN_FILTER_LSF_H
