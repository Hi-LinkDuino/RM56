/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "ble_central_manager_impl.h"

#include <dlfcn.h>

#include "ble_adapter.h"
#include "ble_feature.h"
#include "ble_properties.h"
#include "ble_utils.h"
#include "common/adapter_manager.h"
#include "hisysevent.h"
#include "ble_scan_filter/include/ble_scan_filter_lsf.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
constexpr char BLE_SCAN_FILTER_LIB_NAME[] = "libble_scan_filter.z.so";

struct BleCentralManagerImpl::impl {
    /**
     * @brief register scan callback to gap
     *
     * @return @c status.
     */
    int RegisterCallbackToGap();
    /**
     * @brief register extend scan callback to gap
     *
     * @return @c status.
     */
    int RegisterExScanCallbackToGap();
    /**
     * @brief Set report delay.
     *
     * @return @c status
     */
    void StartReportDelay();

    std::recursive_mutex mutex_ {};
    /// callback type
    CALLBACK_TYPE callBackType_ = CALLBACK_TYPE_FIRST_MATCH;
    /// scan status
    int scanStatus_ = SCAN_NOT_STARTED;
    // stop scan type
    STOP_SCAN_TYPE stopScanType_ = STOP_SCAN_TYPE_NOR;
    /// scan result list
    std::vector<BleScanResultImpl> bleScanResult_ {};
    /// Is stop scan
    bool isStopScan_ = false;
    /// scan settings
    BleScanSettingsImpl settings_ {};
    /// scan parameters
    BleScanParams scanParams_ {};
    /// Report delay timer
    std::unique_ptr<utility::Timer> timer_ = nullptr;
    std::map<std::string, std::vector<uint8_t>> incompleteData_ {};
    BleCentralManagerImpl *bleCentralManagerImpl_ = nullptr;

    std::map<uint8_t, BleScanFilterImpl> filters_;
    std::list<uint8_t> releaseFiltIndex_;
    std::queue<BleScanFilterImpl> waitFilters_;
    int currentFilterClientId_ = 0;
    uint8_t currentFiltIndex_ = 0;
    uint8_t venderMaxFilterNumber_ = 0;
    int filterStatus_ = BLE_SCAN_FILTER_STATUS_IDLE;

    /// Adv data cache
    class BleAdvertisingDataCache {
    public:
        // Set the adv data to cache by device address
        const std::vector<uint8_t> &SetAdvData(
            uint8_t addrType, const RawAddress &address, std::vector<uint8_t> advData)
        {
            auto it = SearchByAddress(addrType, address);
            if (it != itemDatas_.end()) {
                it->advData_ = std::move(advData);
                return it->advData_;
            }

            if (itemDatas_.size() > MAX_CACHE) {
                itemDatas_.pop_back();
            }

            itemDatas_.emplace_front(addrType, address, std::move(advData));
            return itemDatas_.front().advData_;
        }

        // Append adv data for device address
        const std::vector<uint8_t> &AppendAdvData(
            uint8_t addrType, const RawAddress &address, std::vector<uint8_t> advData)
        {
            auto it = SearchByAddress(addrType, address);
            if (it != itemDatas_.end()) {
                it->advData_.insert(it->advData_.end(), advData.begin(), advData.end());
                return it->advData_;
            }

            if (itemDatas_.size() > MAX_CACHE) {
                itemDatas_.pop_back();
            }

            itemDatas_.emplace_front(addrType, address, std::move(advData));
            return itemDatas_.front().advData_;
        }

        // Clear adv data by device addr
        void ClearAdvData(uint8_t addrType, const RawAddress &address)
        {
            auto it = SearchByAddress(addrType, address);
            if (it != itemDatas_.end()) {
                itemDatas_.erase(it);
            }
        }

        // Clear all data
        void ClearAllData()
        {
            for (auto item : itemDatas_) {
                item.advData_.clear();
            }
        }

    private:
        struct ItemData {
            uint8_t addrType_;
            RawAddress address_;
            std::vector<uint8_t> advData_;

            ItemData(uint8_t addrType, const RawAddress &address, std::vector<uint8_t> advData)
                : addrType_(addrType), address_(address), advData_(advData)
            {}
        };

        std::list<ItemData>::iterator SearchByAddress(uint8_t addrType, const RawAddress &address)
        {
            for (auto it = itemDatas_.begin(); it != itemDatas_.end(); it++) {
                if (it->addrType_ == addrType && it->address_ == address) {
                    return it;
                }
            }
            return itemDatas_.end();
        }

        // Keep max 7 devices address in the cache
        const size_t MAX_CACHE = 7;
        std::list<ItemData> itemDatas_ {};
    };

    BleAdvertisingDataCache advDataCache_ {};
};

BleCentralManagerImpl::BleCentralManagerImpl(
    IBleCentralManagerCallback &callback, IAdapterBle &bleAdapter, utility::Dispatcher &dispatch)
    : centralManagerCallbacks_(&callback),
      bleAdapter_(&bleAdapter),
      dispatcher_(&dispatch),
      pimpl(std::make_unique<BleCentralManagerImpl::impl>())
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    pimpl->bleCentralManagerImpl_ = this;
    pimpl->scanParams_.ownAddrType = BLE_ADDR_TYPE_PUBLIC;
    pimpl->scanParams_.scanFilterPolicy = BLE_SCAN_FILTER_ALLOW_ALL;
    SetActiveScan(true);

#ifdef LSF_ENABLE
    bleScanFilter_ = new BleScanFilterLsf();
#else
    LoadBleScanFilterLib();
#endif
}

BleCentralManagerImpl::~BleCentralManagerImpl()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if (pimpl->timer_ != nullptr) {
        pimpl->timer_->Stop();
        pimpl->timer_ = nullptr;
    }

#ifdef LSF_ENABLE
    if (bleScanFilter_ != nullptr) {
        delete bleScanFilter_;
    }
#else
    UnloadBleScanFilterLib();
#endif
}

int BleCentralManagerImpl::impl::RegisterCallbackToGap()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    GapScanCallback scanCallbacks{};
    scanCallbacks.advertisingReport = &BleCentralManagerImpl::AdvertisingReport;
    scanCallbacks.scanSetParamResult = &BleCentralManagerImpl::ScanSetParamResult;
    scanCallbacks.scanSetEnableResult = &BleCentralManagerImpl::ScanSetEnableResult;
    return GAPIF_RegisterScanCallback(&scanCallbacks, bleCentralManagerImpl_);
}

int BleCentralManagerImpl::impl::RegisterExScanCallbackToGap()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    GapExScanCallback exScanCallbacks{};
    exScanCallbacks.exAdvertisingReport = &BleCentralManagerImpl::ExAdvertisingReport;
    exScanCallbacks.scanExSetParamResult = &BleCentralManagerImpl::ScanExSetParamResult;
    exScanCallbacks.scanExSetEnableResult = &BleCentralManagerImpl::ScanExSetEnableResult;
    exScanCallbacks.directedAdvertisingReport = &BleCentralManagerImpl::DirectedAdvertisingReport;
    exScanCallbacks.scanTimeoutEvent = &BleCentralManagerImpl::ScanTimeoutEvent;
    return GAPIF_RegisterExScanCallback(&exScanCallbacks, bleCentralManagerImpl_);
}

int BleCentralManagerImpl::DeregisterCallbackToGap() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    int ret;
    if (BleFeature::GetInstance().IsLeExtendedAdvertisingSupported()) {
        ret = GAPIF_DeregisterExScanCallback();
    } else {
        ret = GAPIF_DeregisterScanCallback();
    }
    return ret;
}

void BleCentralManagerImpl::AdvertisingReport(
    uint8_t advType, const BtAddr *peerAddr, GapAdvReportParam reportParam, const BtAddr *currentAddr, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        bool isScannable = (advType == SCAN_ADV_IND || advType == SCAN_ADV_SCAN_IND);
        bool isScanResp = (advType == SCAN_SCAN_RSP);
        bool isStart = isScannable && !isScanResp;

        RawAddress rawAddress(RawAddress::ConvertToString(peerAddr->addr));
        std::vector<uint8_t> datas(reportParam.data, reportParam.data + reportParam.dataLen);
        std::vector<uint8_t> const &advData =
            isStart ? centralManager->pimpl->advDataCache_.SetAdvData(peerAddr->type, rawAddress, std::move(datas))
                    : centralManager->pimpl->advDataCache_.AppendAdvData(peerAddr->type, rawAddress, std::move(datas));

        if (isScannable && !isScanResp) {
            return;
        }

        std::vector<uint8_t> mergeData(advData);
        centralManager->pimpl->advDataCache_.ClearAdvData(peerAddr->type, rawAddress);

        BtAddr addr;
        (void)memset_s(&addr, sizeof(addr), 0x00, sizeof(addr));
        addr.type = peerAddr->type;
        (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, peerAddr->addr, BT_ADDRESS_SIZE);
        LOG_INFO("AdvertisingReport Data=%{public}s", BleUtils::ConvertIntToHexString(mergeData).c_str());
        centralManager->dispatcher_->PostTask(std::bind(
            &BleCentralManagerImpl::AdvertisingReportTask, centralManager, advType, addr, mergeData, reportParam.rssi));
    }
}

void BleCentralManagerImpl::ExAdvertisingReport(
    uint8_t advType, const BtAddr *addr, GapExAdvReportParam reportParam, const BtAddr *currentAddr, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *pCentralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((pCentralManager != nullptr) && (pCentralManager->dispatcher_)) {
        bool isLegacy = (advType & (1 << BLE_ADV_EVT_LEGACY_BIT));
        std::vector<uint8_t> mergeData(reportParam.data, reportParam.data + reportParam.dataLen);
        if (isLegacy) {
            bool isScannable = (advType & (1 << BLE_LEGACY_ADV_SCAN_IND));
            bool isScanResp = (advType & (1 << BLE_LEGACY_SCAN_RESPONSE));
            bool isStart = isScannable && !isScanResp;

            RawAddress rawAddress(RawAddress::ConvertToString(addr->addr));
            std::vector<uint8_t> datas(reportParam.data, reportParam.data + reportParam.dataLen);
            std::vector<uint8_t> const &advData =
                isStart ? pCentralManager->pimpl->advDataCache_.SetAdvData(addr->type, rawAddress, std::move(datas))
                        : pCentralManager->pimpl->advDataCache_.AppendAdvData(addr->type, rawAddress, std::move(datas));

            if (isScannable && !isScanResp) {
                LOG_DEBUG("[BleCentralManagerImpl] Waiting for scan response");
                return;
            }

            mergeData = advData;
            pCentralManager->pimpl->advDataCache_.ClearAdvData(addr->type, rawAddress);
        }

        BtAddr peerAddr;
        peerAddr.type = addr->type;
        (void)memcpy_s(peerAddr.addr, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

        BtAddr peerCurrentAddr;
        if (currentAddr != nullptr) {
            peerCurrentAddr.type = currentAddr->type;
            (void)memcpy_s(peerCurrentAddr.addr, BT_ADDRESS_SIZE, currentAddr->addr, BT_ADDRESS_SIZE);
        } else {
            peerCurrentAddr = peerAddr;
        }

        LOG_INFO("ExAdvertisingReport Data=%{public}s", BleUtils::ConvertIntToHexString(mergeData).c_str());
        pCentralManager->dispatcher_->PostTask(std::bind(&BleCentralManagerImpl::ExAdvertisingReportTask,
            pCentralManager,
            advType,
            peerAddr,
            mergeData,
            reportParam.rssi,
            peerCurrentAddr));
    }
}

void BleCentralManagerImpl::AdvertisingReportTask(
    uint8_t advType, const BtAddr &peerAddr, const std::vector<uint8_t> &data, int8_t rssi) const
{
    HILOGI("Data = %{public}s", BleUtils::ConvertIntToHexString(data).c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    RawAddress advertisedAddress(RawAddress::ConvertToString(peerAddr.addr));
    bool ret = false;
    BlePeripheralDevice device;
    if (FindDevice(advertisedAddress.GetAddress(), device)) {
        ret = AddPeripheralDevice(advType, peerAddr, data, rssi, device);
        if (ret) {  /// LE General Discoverable Mode LE Limited Discoverable Mode
            return;
        }
        HandleGapEvent(BLE_GAP_SCAN_RESULT_EVT, 0);
        return;
    }

    ret = AddPeripheralDevice(advType, peerAddr, data, rssi, device);
    if (ret) {  /// LE General Discoverable Mode LE Limited Discoverable Mode
        return;
    }
    HandleGapEvent(BLE_GAP_SCAN_RESULT_EVT, 0);
}

bool BleCentralManagerImpl::ExtractIncompleteData(uint8_t advType, const std::string &advertisedAddress,
    const std::vector<uint8_t> &data, std::vector<uint8_t> &completeData) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if ((advType & BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_MORE) == BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_MORE) {
        auto iter = pimpl->incompleteData_.find(advertisedAddress);
        if (iter == pimpl->incompleteData_.end()) {
            pimpl->incompleteData_.insert(std::make_pair(advertisedAddress, data));
        } else {
            iter->second.insert(iter->second.end(), data.begin(), data.end());
        }
        return true;
    } else if ((advType & BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_NO_MORE) == 0 &&
               (advType & BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_MORE) == 0) {
        auto iter = pimpl->incompleteData_.find(advertisedAddress);
        if (iter != pimpl->incompleteData_.end()) {
            iter->second.insert(iter->second.end(), data.begin(), data.end());
            completeData = iter->second;
        }
    } else if ((advType & BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_NO_MORE) == BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_NO_MORE) {
        auto iter = pimpl->incompleteData_.find(advertisedAddress);
        if (iter != pimpl->incompleteData_.end()) {
            iter->second.insert(iter->second.end(), data.begin(), data.end());
            completeData = iter->second;
        }
    }
    return false;
}

void BleCentralManagerImpl::ExAdvertisingReportTask(uint8_t advType, const BtAddr &peerAddr,
    const std::vector<uint8_t> &data, int8_t rssi, const BtAddr &peerCurrentAddr) const
{
    HILOGI("Data = %{public}s", BleUtils::ConvertIntToHexString(data).c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    RawAddress advAddress(RawAddress::ConvertToString(peerAddr.addr));
    /// Set whether only legacy advertisments should be returned in scan results.
    if (pimpl->settings_.GetLegacy()) {
        if ((advType & BLE_LEGACY_ADV_NONCONN_IND_WITH_EX_ADV) == 0) {
            HILOGI("Excepted addr: %{public}s, advType = %{public}d",
                GetEncryptAddr(advAddress.GetAddress()).c_str(), advType);
            return;
        }
    }

    /// incomplete data
    RawAddress advCurrentAddress(RawAddress::ConvertToString(peerCurrentAddr.addr));
    HILOGI("peerAddr: %{public}s, peerCurrentAddr: %{public}s", GetEncryptAddr(advAddress.GetAddress()).c_str(),
        GetEncryptAddr(advCurrentAddress.GetAddress()).c_str());
    std::vector<uint8_t> incompleteData(data.begin(), data.end());
    if (ExtractIncompleteData(advType, advCurrentAddress.GetAddress(), data, incompleteData)) {
        return;
    }

    bool ret = false;
    BlePeripheralDevice device;
    if (FindDevice(advAddress.GetAddress(), device)) {
        ret = AddPeripheralDevice(advType, peerAddr, incompleteData, rssi, device);
        if (ret) {  /// not discovery
            pimpl->incompleteData_.clear();
            return;
        }
        HandleGapExScanEvent(BLE_GAP_EX_SCAN_RESULT_EVT, 0);
        pimpl->incompleteData_.clear();
        return;
    }

    ret = AddPeripheralDevice(advType, peerAddr, incompleteData, rssi, device);
    if (ret) {  /// not discovery
        pimpl->incompleteData_.clear();
        return;
    }
    pimpl->incompleteData_.clear();
    HandleGapExScanEvent(BLE_GAP_EX_SCAN_RESULT_EVT, 0);
}

bool BleCentralManagerImpl::AddPeripheralDevice(uint8_t advType, const BtAddr &peerAddr,
    const std::vector<uint8_t> &data, int8_t rssi, const BlePeripheralDevice &dev) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    BlePeripheralDevice device = dev;
    RawAddress advertisedAddress(RawAddress::ConvertToString(peerAddr.addr));
    device.SetAddress(advertisedAddress);
    device.SetManufacturerData("");
    device.SetRSSI(rssi);
    if (data.size() > 0) {
        BlePeripheralDeviceParseAdvData parseAdvData = {
            .payload = const_cast<uint8_t *>(data.data()),
            .length = data.size(),
        };
        device.ParseAdvertiserment(parseAdvData);
        uint8_t falg = device.GetAdFlag();
        if (CheckBleScanMode(falg)) {
            return true;
        }
    } else {
        uint8_t falg = device.GetAdFlag();
        if (CheckBleScanMode(falg)) {
            return true;
        }
    }
    device.SetAddressType(peerAddr.type);
    if ((advType == SCAN_ADV_SCAN_IND) || (advType == SCAN_ADV_NONCONN_IND)) {
        device.SetConnectable(false);
    } else if ((advType == SCAN_ADV_IND) || (advType == SCAN_ADV_DIRECT_IND)) {
        device.SetConnectable(true);
    }

    BleScanResultImpl result;
    result.SetPeripheralDevice(device);
    pimpl->bleScanResult_.push_back(result);

    return false;
}

bool BleCentralManagerImpl::CheckBleScanMode(uint8_t falg)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:adv flag = %u", __func__, falg);

    switch (BleConfig::GetInstance().GetBleScanMode()) {
        case BLE_SCAN_MODE_NON_DISC:
            if (falg == BLE_ADV_FLAG_NON_LIMIT_DISC) {
                return false;
            }
            break;
        case BLE_SCAN_MODE_GENERAL:
            if ((falg & BLE_ADV_FLAG_LIMIT_DISC) == BLE_ADV_FLAG_LIMIT_DISC ||
                (falg & BLE_ADV_FLAG_GEN_DISC) == BLE_ADV_FLAG_GEN_DISC) {
                return false;
            }
            break;
        case BLE_SCAN_MODE_LIMITED:
            if ((falg & BLE_ADV_FLAG_LIMIT_DISC) == BLE_ADV_FLAG_LIMIT_DISC) {
                return false;
            }
            break;
        case BLE_SCAN_MODE_ALL:
        default:
            return false;
    }
    return true;
}

void BleCentralManagerImpl::AddBleScanFilterResult(uint8_t result, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(std::bind(
            &BleCentralManagerImpl::HandleAddBleScanFilterResult, centralManager, result));
    }
}

void BleCentralManagerImpl::DeleteBleScanFilterResult(uint8_t result, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(std::bind(
            &BleCentralManagerImpl::HandleDeleteBleScanFilterResult, centralManager, result));
    }
}

void BleCentralManagerImpl::StartBleScanFilterResult(uint8_t result, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(std::bind(
            &BleCentralManagerImpl::HandleStartBleScanFilterResult, centralManager, result));
    }
}

void BleCentralManagerImpl::StopBleScanFilterResult(uint8_t result, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(std::bind(
            &BleCentralManagerImpl::HandleStopBleScanFilterResult, centralManager, result));
    }
}

void BleCentralManagerImpl::HandleAddBleScanFilterResult(uint8_t result)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (result != BLE_SCAN_FILTER_FINISHED) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s result is faild.", __func__);
        DoFilterStatusBad();
        return;
    }
    HandleWaitFilters();
}

void BleCentralManagerImpl::HandleDeleteBleScanFilterResult(uint8_t result)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (result != BLE_SCAN_FILTER_FINISHED) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s result is faild.", __func__);
        DoFilterStatusBad();
        return;
    }
    HandleWaitFilters();
}

void BleCentralManagerImpl::HandleStartBleScanFilterResult(uint8_t result)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (result != BLE_SCAN_FILTER_FINISHED) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s result is faild.", __func__);
        DoFilterStatusBad();
        return;
    }
    HandleWaitFilters();
}

void BleCentralManagerImpl::HandleStopBleScanFilterResult(uint8_t result)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (result != BLE_SCAN_FILTER_FINISHED) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s result is faild.", __func__);

        if (pimpl->filterStatus_ == BLE_SCAN_FILTER_STATUS_BAD) {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s result is faild.", __func__);
            return;
        }
        DoFilterStatusBad();
    }

    HandleWaitFilters();
}

void BleCentralManagerImpl::ScanSetParamResult(uint8_t status, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(std::bind(
            &BleCentralManagerImpl::HandleGapEvent, centralManager, BLE_GAP_SCAN_PARAM_SET_COMPLETE_EVT, status));
    }
}

void BleCentralManagerImpl::ScanExSetParamResult(uint8_t status, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(std::bind(&BleCentralManagerImpl::HandleGapExScanEvent,
            centralManager,
            BLE_GAP_EX_SCAN_PARAM_SET_COMPLETE_EVT,
            status));
    }
}

void BleCentralManagerImpl::ScanSetEnableResult(uint8_t status, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(
            std::bind(&BleCentralManagerImpl::ScanSetEnableResultTask, centralManager, status));
        (static_cast<BleAdapter *>(centralManager->bleAdapter_))->NotifyAllWaitContinue();
    }
}

void BleCentralManagerImpl::ScanSetEnableResultTask(uint8_t status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (pimpl->isStopScan_) {
        if (pimpl->stopScanType_ == STOP_SCAN_TYPE_RESOLVING_LIST) {
            HandleGapEvent(BLE_GAP_RESOLVING_LIST_ADV_SCAN_STOP_COMPLETE_EVT, status);
        } else {
            HandleGapEvent(BLE_GAP_SCAN_STOP_COMPLETE_EVT, status);
        }
    } else {
        if (pimpl->stopScanType_ == STOP_SCAN_TYPE_RESOLVING_LIST) {
            HandleGapEvent(BLE_GAP_RESOLVING_LIST_ADV_SCAN_START_COMPLETE_EVT, status);
        } else {
            HandleGapEvent(BLE_GAP_SCAN_START_COMPLETE_EVT, status);
        }
    }
}

void BleCentralManagerImpl::ScanExSetEnableResult(uint8_t status, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(
            std::bind(&BleCentralManagerImpl::ScanExSetEnableResultTask, centralManager, status));
        (static_cast<BleAdapter *>(centralManager->bleAdapter_))->NotifyAllWaitContinue();
    }
}

void BleCentralManagerImpl::ScanExSetEnableResultTask(uint8_t status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (pimpl->isStopScan_) {
        if (pimpl->stopScanType_ == STOP_SCAN_TYPE_RESOLVING_LIST) {
            HandleGapExScanEvent(BLE_GAP_EX_RESOLVING_LIST_ADV_SCAN_STOP_COMPLETE_EVT, status);
        } else {
            HandleGapExScanEvent(BLE_GAP_EX_SCAN_STOP_COMPLETE_EVT, status);
        }
    } else {
        if (pimpl->stopScanType_ == STOP_SCAN_TYPE_RESOLVING_LIST) {
            HandleGapExScanEvent(BLE_GAP_EX_RESOLVING_LIST_ADV_SCAN_START_COMPLETE_EVT, status);
        } else {
            HandleGapExScanEvent(BLE_GAP_EX_SCAN_START_COMPLETE_EVT, status);
        }
    }
}

void BleCentralManagerImpl::StartScan() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:<-- Start scan start", __func__);

    int status = AdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BLE);
    if (status != BTStateID::STATE_TURN_ON) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:%{public}s", __func__, "Bluetooth adapter is invalid.");
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_NOT_STARTED, true);
        return;
    }

    uint16_t interval = BLE_SCAN_MODE_LOW_POWER_INTERVAL_MS;
    SetInterval(interval);
    uint16_t window = BLE_SCAN_MODE_LOW_POWER_WINDOW_MS;
    SetWindow(window);
    pimpl->callBackType_ = CALLBACK_TYPE_FIRST_MATCH;
    Start(false);

    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:<-- Start scan end", __func__);
}

void BleCentralManagerImpl::StartScan(const BleScanSettingsImpl &setting) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:<-- Start scan start", __func__);

    int status = AdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BLE);
    if (status != BTStateID::STATE_TURN_ON) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:%{public}s", __func__, "Bluetooth adapter is invalid.");
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_NOT_STARTED, true);
        return;
    }

    pimpl->settings_ = setting;
    int matchingMode;
    if (setting.GetReportDelayMillisValue() > 0) {
        matchingMode = CALLBACK_TYPE_ALL_MATCHES;
    } else {
        matchingMode = CALLBACK_TYPE_FIRST_MATCH;
    }
    SetScanModeDuration(setting.GetScanMode(), matchingMode);
    if (matchingMode == CALLBACK_TYPE_FIRST_MATCH) {
        pimpl->callBackType_ = CALLBACK_TYPE_FIRST_MATCH;
    } else {
        pimpl->callBackType_ = CALLBACK_TYPE_ALL_MATCHES;
    }
    Start(false);

    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:<-- Start scan end", __func__);
}

void BleCentralManagerImpl::StopScan() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:-> Stop scan start", __func__);

    if (pimpl->scanStatus_ == SCAN_NOT_STARTED) {
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_NOT_STARTED, false);
        return;
    }

    Stop();

    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:-> Stop scan end", __func__);
}

void BleCentralManagerImpl::StartOrStopScan(const STOP_SCAN_TYPE &scanType, bool isStartScan) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:-> StartOrStopScan scan start", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    int ret;
    if (BleFeature::GetInstance().IsExtendedScanSupported()) {
        ret = SetExScanEnable(isStartScan);
    } else {
        ret = SetScanEnable(isStartScan);
    }

    if (ret != BT_NO_ERROR) {
        LOG_ERROR("stop or start extend scan: err: %{public}d isScan : %{public}d", ret, isStartScan);
        return;
    }
    pimpl->stopScanType_ = scanType;
    pimpl->isStopScan_ = !isStartScan;
    if (isStartScan) {
        pimpl->scanStatus_ = SCAN_FAILED_ALREADY_STARTED;
        LOG_DEBUG("start extend scan successful");
    } else {
        if (scanType != STOP_SCAN_TYPE_RESOLVING_LIST) {
            pimpl->scanStatus_ = SCAN_NOT_STARTED;
        } else {
            pimpl->scanStatus_ = SCAN_FAILED_ALREADY_STARTED;
        }
        LOG_DEBUG("stop extend scan successful");
    }
}

void BleCentralManagerImpl::SetActiveScan(bool active) const
{
    HILOGI("active: %{public}d", active);

    if (active) {
        pimpl->scanParams_.scanType = BLE_SCAN_TYPE_ACTIVE;
    } else {
        pimpl->scanParams_.scanType = BLE_SCAN_TYPE_PASSIVE;
    }
}

void BleCentralManagerImpl::SetInterval(uint16_t intervalMSecs) const
{
    HILOGI("intervalMSecs: %{public}hu", intervalMSecs);

    pimpl->scanParams_.scanInterval = intervalMSecs / BLE_SCAN_UNIT_TIME;
}

void BleCentralManagerImpl::SetWindow(uint16_t windowMSecs) const
{
    HILOGI("windowMSecs: %{public}hu", windowMSecs);

    pimpl->scanParams_.scanWindow = windowMSecs / BLE_SCAN_UNIT_TIME;
}

void BleCentralManagerImpl::ClearResults() const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    pimpl->bleScanResult_.clear();
}

void BleCentralManagerImpl::SetScanModeDuration(int scanMode, int type) const
{
    HILOGI("scanMode: %{public}d, type: %{public}d", scanMode, type);

    switch (scanMode) {
        case SCAN_MODE_LOW_POWER:
            SetLowPowerDuration(type);
            break;
        case SCAN_MODE_BALANCED:
            SetBalancedDuration(type);
            break;
        case SCAN_MODE_LOW_LATENCY:
            SetLowLatencyDuration(type);
            break;
        case SCAN_MODE_OP_P2_60_3000:
            SetDutyCycle2Duration(type);
            break;
        case SCAN_MODE_OP_P10_60_600:
            SetDutyCycle10Duration(type);
            break;
        case SCAN_MODE_OP_P25_60_240:
            SetDutyCycle25Duration(type);
            break;
        case SCAN_MODE_OP_P100_1000_1000:
            SetDutyCycle100Duration(type);
            break;
        default:
            break;
    }
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "BLE_SCAN_DUTY_CYCLE",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "WINDOW", pimpl->scanParams_.scanWindow,
        "INTERVAL", pimpl->scanParams_.scanInterval, "TYPE", pimpl->callBackType_);
}

void BleCentralManagerImpl::SetLowPowerDuration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_LOW_POWER_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_LOW_POWER_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_LOW_POWER_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_LOW_POWER_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::SetBalancedDuration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_BALANCED_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_BALANCED_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_BALANCED_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BALANCED_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::SetLowLatencyDuration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_LOW_LATENCY_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_LOW_LATENCY_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_LOW_LATENCY_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_LOW_LATENCY_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::SetDutyCycle2Duration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_OP_P2_60_3000_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_OP_P2_60_3000_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_OP_P2_60_3000_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_OP_P2_60_3000_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::SetDutyCycle10Duration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_OP_P10_60_600_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_OP_P10_60_600_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_OP_P10_60_600_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_OP_P10_60_600_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::SetDutyCycle25Duration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_OP_P25_60_240_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_OP_P25_60_240_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_OP_P25_60_240_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_OP_P25_60_240_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::SetDutyCycle100Duration(int type) const
{
    if (type == CALLBACK_TYPE_ALL_MATCHES) {
        uint16_t interval = BLE_SCAN_MODE_BATCH_OP_P100_1000_1000_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_BATCH_OP_P100_1000_1000_WINDOW_MS;
        SetWindow(window);
    } else {
        uint16_t interval = BLE_SCAN_MODE_OP_P100_1000_1000_INTERVAL_MS;
        SetInterval(interval);
        uint16_t window = BLE_SCAN_MODE_OP_P100_1000_1000_WINDOW_MS;
        SetWindow(window);
    }
}

void BleCentralManagerImpl::TimerCallback(void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    auto *centralManager = static_cast<BleCentralManagerImpl *>(context);
    if ((centralManager != nullptr) && (centralManager->dispatcher_ != nullptr)) {
        centralManager->dispatcher_->PostTask(
            std::bind(&BleCentralManagerImpl::HandleGapEvent, centralManager, BLE_GAP_SCAN_DELAY_REPORT_RESULT_EVT, 0));
        centralManager->dispatcher_->PostTask(std::bind(&BleCentralManagerImpl::StopScan, centralManager));
    }
}

int BleCentralManagerImpl::SetScanParamToGap() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    GapLeScanParam param;
    param.scanType = pimpl->scanParams_.scanType;
    param.param.scanInterval = pimpl->scanParams_.scanInterval;
    param.param.scanWindow = pimpl->scanParams_.scanWindow;
    return GAPIF_LeScanSetParam(param, pimpl->scanParams_.scanFilterPolicy);
}

int BleCentralManagerImpl::SetExScanParamToGap() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::vector<GapLeScanParam> params;
    uint8_t scanPhys = GAP_EX_SCAN_PHY_1M;
    if (pimpl->settings_.GetLegacy()) {
        GapLeScanParam param;
        param.scanType = pimpl->scanParams_.scanType;
        param.param.scanInterval = pimpl->scanParams_.scanInterval;
        param.param.scanWindow = pimpl->scanParams_.scanWindow;
        params.push_back(param);
    } else {
        switch (pimpl->settings_.GetPhy()) {
            case PHY_LE_CODED:
                scanPhys = GAP_EX_SCAN_PHY_CODED;
                GapLeScanParam paramCoded;
                paramCoded.scanType = pimpl->scanParams_.scanType;
                paramCoded.param.scanInterval = pimpl->scanParams_.scanInterval;
                paramCoded.param.scanWindow = pimpl->scanParams_.scanWindow;
                params.push_back(paramCoded);
                break;
            case PHY_LE_ALL_SUPPORTED:
                scanPhys = GAP_EX_SCAN_PHY_1M | GAP_EX_SCAN_PHY_CODED;
                GapLeScanParam param1M;
                param1M.scanType = pimpl->scanParams_.scanType;
                param1M.param.scanInterval = pimpl->scanParams_.scanInterval;
                param1M.param.scanWindow = pimpl->scanParams_.scanWindow;
                params.push_back(param1M);

                GapLeScanParam paramCodedAll;
                paramCodedAll.scanType = pimpl->scanParams_.scanType;
                paramCodedAll.param.scanInterval = pimpl->scanParams_.scanInterval;
                paramCodedAll.param.scanWindow = pimpl->scanParams_.scanWindow;
                params.push_back(paramCodedAll);
                break;
            case PHY_LE_1M:
            default:
                scanPhys = GAP_EX_SCAN_PHY_1M;
                GapLeScanParam param;
                param.scanType = pimpl->scanParams_.scanType;
                param.param.scanInterval = pimpl->scanParams_.scanInterval;
                param.param.scanWindow = pimpl->scanParams_.scanWindow;
                params.push_back(param);
                break;
        }
    }
    return GAPIF_LeExScanSetParam(pimpl->scanParams_.scanFilterPolicy, scanPhys, &params[0]);
}

int BleCentralManagerImpl::SetScanEnable(bool enable) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:%{public}d", __func__, enable);

    return GAPIF_LeScanSetEnable(enable, isDuplicates_);
}

int BleCentralManagerImpl::SetExScanEnable(bool enable) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    return GAPIF_LeExScanSetEnable(enable, isDuplicates_, 0, 0);
}

void BleCentralManagerImpl::impl::StartReportDelay()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if ((settings_.GetReportDelayMillisValue() > 0) && (callBackType_ == CALLBACK_TYPE_ALL_MATCHES)) {
        if (timer_ == nullptr) {
            timer_ = std::make_unique<utility::Timer>(std::bind(&TimerCallback, bleCentralManagerImpl_));
        }
        timer_->Start(settings_.GetReportDelayMillisValue());
    }
}

bool BleCentralManagerImpl::SetLegacyScanParamToGap() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    bool ret = true;
    ret = pimpl->RegisterCallbackToGap();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:RegisterCallbackToGap failed.", __func__);
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return false;
    }

    pimpl->isStopScan_ = false;
    ret = SetScanParamToGap();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:SetScanParamToGap failed.", __func__);
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return false;
    }
    return ret;
}

bool BleCentralManagerImpl::SetExtendScanParamToGap() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    bool ret = true;
    ret = pimpl->RegisterExScanCallbackToGap();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:RegisterExScanCallbackToGap failed.", __func__);
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return false;
    }

    pimpl->isStopScan_ = false;
    ret = SetExScanParamToGap();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:SetExScanParamToGap failed.", __func__);
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return false;
    }
    return ret;
}

bool BleCentralManagerImpl::SetScanParamOrExScanParamToGap() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    bool ret = true;
    if (BleFeature::GetInstance().IsExtendedScanSupported()) {
        ret = SetExtendScanParamToGap();
    } else {
        ret = SetLegacyScanParamToGap();
    }
    return ret;
}

NO_SANITIZE("cfi") void BleCentralManagerImpl::LoadBleScanFilterLib()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if (bleScanFilterLib_ == nullptr) {
        bleScanFilterLib_ = dlopen(BLE_SCAN_FILTER_LIB_NAME, RTLD_LAZY | RTLD_NODELETE);
    }
    if (bleScanFilterLib_ == nullptr) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s Load lib failed", __func__);
        return;
    }
    if (bleScanFilter_ != nullptr) {
        LOG_DEBUG("[BleCentralManagerImpl] %{public}s bleScanFilter_ is not null.", __func__);
        return;
    }
    createBleScanFilter createBleScanFilterFunc =
        (createBleScanFilter)dlsym(bleScanFilterLib_, "CreateBleScanFilter");
    if (createBleScanFilterFunc == nullptr) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s Load symbol CreateBleScanFilter failed", __func__);
        if (bleScanFilterLib_ != nullptr) {
            dlclose(bleScanFilterLib_);
            bleScanFilterLib_ = nullptr;
        }
        return;
    }
    bleScanFilter_ = createBleScanFilterFunc();
    if (bleScanFilter_ == nullptr) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s Load bleScanFilter failed", __func__);
        if (bleScanFilterLib_ != nullptr) {
            dlclose(bleScanFilterLib_);
            bleScanFilterLib_ = nullptr;
        }
    }
}

void BleCentralManagerImpl::UnloadBleScanFilterLib()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if (bleScanFilterLib_ != nullptr) {
        destroyBleScanFilter destroyBleScanFilterFunc =
            (destroyBleScanFilter)dlsym(bleScanFilterLib_, "DestroyBleScanFilter");
        if (destroyBleScanFilterFunc == nullptr) {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s Load symbol DestroyBleScanFilter failed", __func__);
        } else if (bleScanFilter_ != nullptr) {
            destroyBleScanFilterFunc(bleScanFilter_);
        }
        dlclose(bleScanFilterLib_);
        bleScanFilterLib_ = nullptr;
    }
    bleScanFilter_ = nullptr;
}

int BleCentralManagerImpl::ConfigScanFilter(const int oldClientId, const std::vector<BleScanFilterImpl> &filters)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:-> Config scan filter filterStatus_=%{public}d",
        __func__, pimpl->filterStatus_);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);

    int clientId = 0;
    if (!CheckScanFilterConfig(filters)) {
        return clientId;
    }

    if (oldClientId != 0) {
        clientId = oldClientId;
    } else {
        if (pimpl->currentFilterClientId_ >= INT_MAX) {
            pimpl->currentFilterClientId_ = 0;
        }
        clientId = ++pimpl->currentFilterClientId_;
    }

    if (filters.empty()) {
        BleScanFilterImpl filter;
        PushFilterToWaitList(filter, clientId, FILTER_ACTION_ADD);
    } else {
        for (auto filter : filters) {
            PushFilterToWaitList(filter, clientId, FILTER_ACTION_ADD);
        }
    }
    PushStartOrStopAction(clientId, FILTER_ACTION_START);

    if ((pimpl->filterStatus_ == BLE_SCAN_FILTER_STATUS_BAD) &&
        (pimpl->filters_.size() <= pimpl->venderMaxFilterNumber_)) {
        TryConfigScanFilter(clientId);
    }

    if (pimpl->filterStatus_ == BLE_SCAN_FILTER_STATUS_IDLE) {
        HandleWaitFilters();
    }

    return clientId;
}

bool BleCentralManagerImpl::CheckScanFilterConfig(const std::vector<BleScanFilterImpl> &filters)
{
    if (bleScanFilter_ == nullptr) {
        return false;
    }

    int status = AdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BLE);
    if (status != BTStateID::STATE_TURN_ON) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:%{public}s", __func__, "Bluetooth adapter is invalid.");
        return false;
    }

    uint8_t filterSize;
    if (filters.empty()) {
        filterSize = 1;
    } else {
        filterSize = filters.size();
    }

    if ((filterSize + pimpl->filters_.size()) >= UCHAR_MAX) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:%{public}s", __func__, "filter array is full.");
        return false;
    }
    if (pimpl->venderMaxFilterNumber_ == 0) {
        pimpl->venderMaxFilterNumber_ = bleScanFilter_->GetMaxFilterNumber();
        LOG_DEBUG("[BleCentralManagerImpl] %{public}s:-> vender max filter number_ is %{public}d",
            __func__, pimpl->venderMaxFilterNumber_);
    }

    if (pimpl->venderMaxFilterNumber_ <= 0) {
        return false;
    }

    return true;
}

void BleCentralManagerImpl::PushFilterToWaitList(BleScanFilterImpl filter, int clientId, uint8_t action)
{
    filter.SetFilterAction(action);
    if (action == FILTER_ACTION_ADD) {
        filter.SetClientId(clientId);
        if (pimpl->releaseFiltIndex_.empty()) {
            filter.SetFiltIndex(pimpl->currentFiltIndex_++);
        } else {
            filter.SetFiltIndex(*pimpl->releaseFiltIndex_.begin());
            pimpl->releaseFiltIndex_.remove(filter.GetFiltIndex());
        }

        pimpl->filters_[filter.GetFiltIndex()] = filter;
    }

    if (pimpl->filterStatus_ != BLE_SCAN_FILTER_STATUS_BAD) {
        pimpl->waitFilters_.push(filter);
    }
}

void BleCentralManagerImpl::PushStartOrStopAction(const int clientId, uint8_t action)
{
    BleScanFilterImpl filterImpl;
    PushFilterToWaitList(filterImpl, clientId, action);
}

void BleCentralManagerImpl::HandleWaitFilters()
{
    if (pimpl->waitFilters_.empty()) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:%{public}s", __func__, "there is no wait action.");
        pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_IDLE;
        return;
    }

    BleScanFilterImpl waitFilter = pimpl->waitFilters_.front();
    pimpl->waitFilters_.pop();

    uint8_t action = waitFilter.GetFilterAction();
    LOG_ERROR("[BleCentralManagerImpl] %{public}s: wait action %{public}d", __func__, action);
    switch (action) {
        case FILTER_ACTION_ADD:
            AddBleScanFilter(waitFilter);
            break;
        case FILTER_ACTION_DELETE:
            DeleteBleScanFilter(waitFilter);
            break;
        case FILTER_ACTION_START:
            StartBleScanFilter();
            break;
        case FILTER_ACTION_STOP:
            if (pimpl->waitFilters_.empty() && pimpl->filters_.empty()) {
                StopBleScanFilter();
            }
            break;
        default: {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s: error action %{public}d", __func__, action);
        }
    }
}

void BleCentralManagerImpl::TryConfigScanFilter(int clientId)
{
    LOG_ERROR("[BleCentralManagerImpl] %{public}s: ", __func__);
    pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_IDLE;

    pimpl->currentFiltIndex_ = 0;
    pimpl->releaseFiltIndex_.clear();
    std::map<uint8_t, BleScanFilterImpl> filters;

    for (auto it = pimpl->filters_.begin(); it != pimpl->filters_.end(); it++) {
        BleScanFilterImpl filter = it->second;
        filter.SetFiltIndex(pimpl->currentFiltIndex_++);
        pimpl->waitFilters_.push(filter);
        filters[filter.GetFiltIndex()] = filter;
    }
    pimpl->filters_ = filters;

    PushStartOrStopAction(clientId, FILTER_ACTION_START);
}

void BleCentralManagerImpl::RemoveScanFilter(const int clientId)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:-> Remove scan filter", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);

    if (bleScanFilter_ == nullptr) {
        return;
    }

    for (auto it = pimpl->filters_.begin(); it != pimpl->filters_.end();) {
        if (it->second.GetClientId() == clientId) {
            PushFilterToWaitList(it->second, clientId, FILTER_ACTION_DELETE);
            pimpl->releaseFiltIndex_.push_back(it->first);
            pimpl->filters_.erase(it++);
        } else {
            it++;
        }
    }
    pimpl->releaseFiltIndex_.sort();
    PushStartOrStopAction(clientId, FILTER_ACTION_STOP);

    if (pimpl->filterStatus_ == BLE_SCAN_FILTER_STATUS_IDLE) {
        HandleWaitFilters();
    }
}

void BleCentralManagerImpl::AddBleScanFilter(BleScanFilterImpl filter)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    if (bleScanFilter_ == nullptr) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s bleScanFilter_ is null.", __func__);
        DoFilterStatusBad();
        return;
    }

    pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_WORKING;

    BleScanFilterParam filterParam;
    filterParam.filtIndex = filter.GetFiltIndex();
    BleScanFilterParamAddDeviceAddress(filterParam, filter);
    BleScanFilterParamAddName(filterParam, filter);
    BleScanFilterParamAddServiceUuid(filterParam, filter);
    BleScanFilterParamAddSolicitationUuid(filterParam, filter);
    BleScanFilterParamAddServiceData(filterParam, filter);
    BleScanFilterParamAddManufactureData(filterParam, filter);

    BleScanFilterCallback filterCallback;
    filterCallback.addBleScanFilterResult = &AddBleScanFilterResult;
    filterCallback.deleteBleScanFilterResult = nullptr;
    filterCallback.startBleScanFilterResult = nullptr;
    filterCallback.stopBleScanFilterResult = nullptr;
    filterCallback.context = pimpl->bleCentralManagerImpl_;
    bleScanFilter_->AddBleScanFilter(filterParam, filterCallback);
}

void BleCentralManagerImpl::DeleteBleScanFilter(BleScanFilterImpl filter)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    if (bleScanFilter_ == nullptr) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s bleScanFilter_ is null.", __func__);
        DoFilterStatusBad();
        return;
    }

    pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_WORKING;

    BleScanFilterParam filterParam;
    filterParam.filtIndex = filter.GetFiltIndex();

    BleScanFilterCallback filterCallback;
    filterCallback.addBleScanFilterResult = nullptr;
    filterCallback.deleteBleScanFilterResult = &DeleteBleScanFilterResult;
    filterCallback.startBleScanFilterResult = nullptr;
    filterCallback.stopBleScanFilterResult = nullptr;
    filterCallback.context = pimpl->bleCentralManagerImpl_;
    bleScanFilter_->DeleteBleScanFilter(filterParam, filterCallback);
}

void BleCentralManagerImpl::BleScanFilterParamAddDeviceAddress(
    BleScanFilterParam &filterParam, BleScanFilterImpl filter)
{
    std::string deviceId = filter.GetDeviceId();
    if (!deviceId.empty()) {
        BtAddr btAddr;
        RawAddress(deviceId).ConvertToUint8(btAddr.addr);
        btAddr.type = BT_DEVICE_ADDRESS_TYPE_ALL;
        filterParam.address = btAddr;
        filterParam.filterFlag |= FILTER_FLAG_ADDRESS;
    }
}

void BleCentralManagerImpl::BleScanFilterParamAddName(
    BleScanFilterParam &filterParam, BleScanFilterImpl filter)
{
    std::string name = filter.GetName();
    if (!name.empty()) {
        filterParam.name = name;
        filterParam.filterFlag |= FILTER_FLAG_NAME;
    }
}

void BleCentralManagerImpl::BleScanFilterParamAddServiceUuid(
    BleScanFilterParam &filterParam, BleScanFilterImpl filter)
{
    if (filter.HasServiceUuid()) {
        filterParam.serviceUuid = filter.GetServiceUuid();
        if (filter.HasServiceUuidMask()) {
            filterParam.serviceUuidMask = filter.GetServiceUuidMask();
        } else {
            filterParam.serviceUuidMask = Uuid::ConvertFrom128Bits(DEFAULT_UUID_MASK);
        }
        filterParam.filterFlag |= FILTER_FLAG_SERVICE_UUID;
    }
}

void BleCentralManagerImpl::BleScanFilterParamAddSolicitationUuid(
    BleScanFilterParam &filterParam, BleScanFilterImpl filter)
{
    if (filter.HasSolicitationUuid()) {
        filterParam.solicitationUuid = filter.GetServiceSolicitationUuid();
        if (filter.HasSolicitationUuidMask()) {
            filterParam.solicitationUuidMask = filter.GetServiceSolicitationUuidMask();
        } else {
            filterParam.solicitationUuidMask = Uuid::ConvertFrom128Bits(DEFAULT_UUID_MASK);
        }
        filterParam.filterFlag |= FILTER_FLAG_SOLICIT_UUID;
    }
}

void BleCentralManagerImpl::BleScanFilterParamAddServiceData(
    BleScanFilterParam &filterParam, BleScanFilterImpl filter)
{
    std::vector<uint8_t> serviceData = filter.GetServiceData();
    std::vector<uint8_t> serviceDataMask = filter.GetServiceDataMask();
    if (!serviceData.empty()) {
        if (serviceData.size() != serviceDataMask.size()) {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s:serviceDataMask size is different with serviceData",
                __func__);
            serviceDataMask.clear();
            for (size_t i = 0; i < serviceData.size(); i++) {
                serviceDataMask.push_back(0xFF);
            }
        }
        filterParam.serviceData = serviceData;
        filterParam.serviceDataMask = serviceDataMask;
        filterParam.filterFlag |= FILTER_FLAG_SERVICE_DATA;
    }
}

void BleCentralManagerImpl::BleScanFilterParamAddManufactureData(
    BleScanFilterParam &filterParam, BleScanFilterImpl filter)
{
    uint16_t manufacturerId = filter.GetManufacturerId();
    std::vector<uint8_t> manufactureData = filter.GetManufactureData();
    std::vector<uint8_t> manufactureDataMask = filter.GetManufactureDataMask();
    if (!manufactureData.empty()) {
        if (manufactureData.size() != manufactureDataMask.size()) {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s:manufactureDataMask size is different with manufactureData",
                __func__);
            manufactureDataMask.clear();
            for (size_t i = 0; i < manufactureData.size(); i++) {
                manufactureDataMask.push_back(0xFF);
            }
        }
        filterParam.manufacturerId = manufacturerId;
        filterParam.manufacturerIdMask = 0xFFFF;
        filterParam.manufacturerData = manufactureData;
        filterParam.manufacturerDataMask = manufactureDataMask;
        filterParam.filterFlag |= FILTER_FLAG_MANUFACTURER_DATA;
    }
}

void BleCentralManagerImpl::StartBleScanFilter()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if (bleScanFilter_ == nullptr) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s bleScanFilter_ is null.", __func__);
        DoFilterStatusBad();
        return;
    }

    pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_WORKING;
    BleScanFilterCallback filterCallback {};
    filterCallback.addBleScanFilterResult = nullptr;
    filterCallback.deleteBleScanFilterResult = nullptr;
    filterCallback.startBleScanFilterResult = &StartBleScanFilterResult;
    filterCallback.stopBleScanFilterResult = nullptr;
    filterCallback.context = pimpl->bleCentralManagerImpl_;
    bleScanFilter_->StartBleScanFilter(filterCallback);
}

void BleCentralManagerImpl::StopBleScanFilter()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if (bleScanFilter_ == nullptr) {
        if (pimpl->filterStatus_ == BLE_SCAN_FILTER_STATUS_BAD) {
            return;
        }
        LOG_ERROR("[BleCentralManagerImpl] %{public}s bleScanFilter_ is null.", __func__);
        DoFilterStatusBad();
        return;
    }

    if (pimpl->filterStatus_ != BLE_SCAN_FILTER_STATUS_BAD) {
        pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_WORKING;
    }

    BleScanFilterCallback filterCallback {};
    filterCallback.addBleScanFilterResult = nullptr;
    filterCallback.deleteBleScanFilterResult = nullptr;
    filterCallback.startBleScanFilterResult = nullptr;
    filterCallback.stopBleScanFilterResult = &StopBleScanFilterResult;
    filterCallback.context = pimpl->bleCentralManagerImpl_;
    bleScanFilter_->StopBleScanFilter(filterCallback);
}

void BleCentralManagerImpl::DoFilterStatusBad()
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
    pimpl->filterStatus_ = BLE_SCAN_FILTER_STATUS_BAD;
    pimpl->waitFilters_ = std::queue<BleScanFilterImpl>();
    StopBleScanFilter();
}

bool BleCentralManagerImpl::Start(bool isContinue) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    if (pimpl->scanStatus_ == SCAN_FAILED_ALREADY_STARTED) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:%{public}s", __func__, "Scan already started.");
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_ALREADY_STARTED, true);
        return true;
    }

    pimpl->scanStatus_ = SCAN_FAILED_ALREADY_STARTED;
    pimpl->stopScanType_ = STOP_SCAN_TYPE_NOR;

    if (!isContinue) {
        ClearResults();
    }
    return SetScanParamOrExScanParamToGap();
}

void BleCentralManagerImpl::Stop() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    int ret;
    if (BleFeature::GetInstance().IsExtendedScanSupported()) {
        ret = SetExScanEnable(false);
    } else {
        ret = SetScanEnable(false);
    }
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("stop scanning: err: %{public}d", ret);
        pimpl->scanStatus_ = SCAN_FAILED_ALREADY_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, false);
        return;
    } else {
        std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
        pimpl->stopScanType_ = STOP_SCAN_TYPE_NOR;
        pimpl->isStopScan_ = true;
        pimpl->settings_.SetReportDelay(0);
    }
}

int BleCentralManagerImpl::GetDeviceType(const std::string &address) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    for (auto value : pimpl->bleScanResult_) {
        if (!address.compare(value.GetPeripheralDevice().GetRawAddress().GetAddress())) {
            return value.GetPeripheralDevice().GetDeviceType();
        }
    }
    return BLE_BT_DEVICE_TYPE_UNKNOWN;
}

int BleCentralManagerImpl::GetDeviceAddrType(const std::string &address) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    for (auto value : pimpl->bleScanResult_) {
        if (!address.compare(value.GetPeripheralDevice().GetRawAddress().GetAddress())) {
            return value.GetPeripheralDevice().GetAddressType();
        }
    }
    return BLE_ADDR_TYPE_UNKNOWN;
}

std::string BleCentralManagerImpl::GetDeviceName(const std::string &address) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    for (auto value : pimpl->bleScanResult_) {
        if (!address.compare(value.GetPeripheralDevice().GetRawAddress().GetAddress())) {
            return value.GetPeripheralDevice().GetName();
        }
    }
    return std::string("");
}

bool BleCentralManagerImpl::FindDevice(const std::string &address, BlePeripheralDevice &dev) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    for (auto it = pimpl->bleScanResult_.begin(); it != pimpl->bleScanResult_.end(); it++) {
        if (!address.compare(it->GetPeripheralDevice().GetRawAddress().GetAddress())) {
            dev = it->GetPeripheralDevice();
            pimpl->bleScanResult_.erase(it);
            return true;
        }
    }
    return false;
}

int BleCentralManagerImpl::GetScanStatus() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s status:%{public}d", __func__, pimpl->scanStatus_);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    return pimpl->scanStatus_;
}

void BleCentralManagerImpl::DirectedAdvertisingReport(uint8_t advType, const BtAddr *addr,
    GapDirectedAdvReportParam reportParam, const BtAddr *currentAddr, void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
}

void BleCentralManagerImpl::ScanTimeoutEvent(void *context)
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);
}

void BleCentralManagerImpl::GapScanParamSetCompleteEvt(int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (status != BT_NO_ERROR) {
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Set scan param failed! %{public}d.", __func__, status);
        return;
    }

    pimpl->StartReportDelay();
    if (pimpl->isStopScan_) {
        return;
    }
    int ret = SetScanEnable(true);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:SetScanEnable param failed! %{public}d.", __func__, ret);
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return;
    }
}

void BleCentralManagerImpl::GapScanResultEvt() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Scan result", __func__);

    if ((centralManagerCallbacks_ != nullptr) && (pimpl->callBackType_ == CALLBACK_TYPE_FIRST_MATCH)) {
        std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
        centralManagerCallbacks_->OnScanCallback(pimpl->bleScanResult_.back());
    }
}

void BleCentralManagerImpl::GapScanDelayReportResultEvt() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Scan batch results", __func__);

    if ((centralManagerCallbacks_ != nullptr) && (pimpl->callBackType_ == CALLBACK_TYPE_ALL_MATCHES)) {
        if (pimpl->timer_ != nullptr) {
            pimpl->timer_->Stop();
        }
        std::lock_guard<std::recursive_mutex> legacyLock(pimpl->mutex_);
        centralManagerCallbacks_->OnBleBatchScanResultsEvent(pimpl->bleScanResult_);
    }
}

void BleCentralManagerImpl::GapScanStartCompleteEvt(int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Start scan", __func__);

    if (status != BT_NO_ERROR) {
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Start scan failed! %{public}d.", __func__, status);
    }
    int tmpStatus = (status != BT_NO_ERROR ? SCAN_FAILED_INTERNAL_ERROR : SCAN_SUCCESS);
    centralManagerCallbacks_->OnStartOrStopScanEvent(tmpStatus, true);
}

void BleCentralManagerImpl::GapScanStopCompleteEvt(int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Stop scan", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (status != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Stop scan failed! %{public}d.", __func__, status);
    }
    pimpl->advDataCache_.ClearAllData();
    pimpl->scanStatus_ = SCAN_NOT_STARTED;
    int tmpStatus = (status != BT_NO_ERROR ? SCAN_FAILED_INTERNAL_ERROR : SCAN_SUCCESS);
    centralManagerCallbacks_->OnStartOrStopScanEvent(tmpStatus, false);
}

void BleCentralManagerImpl::GapScanResolvingCompletEvt(int status, bool isStart) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s: ", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (status != BT_NO_ERROR) {
        pimpl->scanStatus_ = isStart ? SCAN_NOT_STARTED : SCAN_FAILED_ALREADY_STARTED;
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Resovling stop scan failed! %{public}d.", __func__, status);
    }
}

void BleCentralManagerImpl::HandleGapEvent(const BLE_GAP_CB_EVENT &event, int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:[event no: %{public}d]", __func__, static_cast<int>(event));

    switch (event) {
        case BLE_GAP_SCAN_PARAM_SET_COMPLETE_EVT:
            GapScanParamSetCompleteEvt(status);
            break;
        case BLE_GAP_SCAN_RESULT_EVT:
            GapScanResultEvt();
            break;
        case BLE_GAP_SCAN_DELAY_REPORT_RESULT_EVT:
            GapScanDelayReportResultEvt();
            break;
        case BLE_GAP_SCAN_START_COMPLETE_EVT:
            GapScanStartCompleteEvt(status);
            break;
        case BLE_GAP_RESOLVING_LIST_ADV_SCAN_START_COMPLETE_EVT:
            GapScanResolvingCompletEvt(status, true);
            break;
        case BLE_GAP_SCAN_STOP_COMPLETE_EVT:
            GapScanStopCompleteEvt(status);
            break;
        case BLE_GAP_RESOLVING_LIST_ADV_SCAN_STOP_COMPLETE_EVT:
            GapScanResolvingCompletEvt(status, false);
            break;
        default: {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s:Invalid event! %{public}d.", __func__, event);
            break;
        }
    }
}

void BleCentralManagerImpl::GapExScanParamSetCompleteEvt(int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (status != BT_NO_ERROR) {
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Set scan param failed! %{public}d.", __func__, status);
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return;
    }
    pimpl->StartReportDelay();
    if (pimpl->isStopScan_) {
        return;
    }
    int ret = SetExScanEnable(true);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:SetExScanEnable param failed! %{public}d.", __func__, ret);
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        centralManagerCallbacks_->OnStartOrStopScanEvent(SCAN_FAILED_INTERNAL_ERROR, true);
        return;
    }
}

void BleCentralManagerImpl::GapExScanResultEvt() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Scan result", __func__);

    if ((centralManagerCallbacks_ != nullptr) && (pimpl->callBackType_ == CALLBACK_TYPE_FIRST_MATCH)) {
        std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
        centralManagerCallbacks_->OnScanCallback(pimpl->bleScanResult_.back());
    }
}

void BleCentralManagerImpl::GapExScanDelayReportResultEvt() const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Scan batch results", __func__);

    if ((centralManagerCallbacks_ != nullptr) && (pimpl->callBackType_ == CALLBACK_TYPE_ALL_MATCHES)) {
        if (pimpl->timer_ != nullptr) {
            pimpl->timer_->Stop();
        }
        std::lock_guard<std::recursive_mutex> exAdvLock(pimpl->mutex_);
        centralManagerCallbacks_->OnBleBatchScanResultsEvent(pimpl->bleScanResult_);
    }
}

void BleCentralManagerImpl::GapExScanStartCompleteEvt(int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Start scan", __func__);

    if (status != BT_NO_ERROR) {
        pimpl->scanStatus_ = SCAN_NOT_STARTED;
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Start scan failed! %{public}d.", __func__, status);
    }
    int tmpStatus = (status != BT_NO_ERROR ? SCAN_FAILED_INTERNAL_ERROR : SCAN_SUCCESS);
    centralManagerCallbacks_->OnStartOrStopScanEvent(tmpStatus, true);
}

void BleCentralManagerImpl::GapExScanStopCompleteEvt(int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:Stop scan", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (status != BT_NO_ERROR) {
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Stop scan failed! %{public}d.", __func__, status);
    }
    pimpl->scanStatus_ = SCAN_NOT_STARTED;
    pimpl->advDataCache_.ClearAllData();
    int tmpStatus = (status != BT_NO_ERROR ? SCAN_FAILED_INTERNAL_ERROR : SCAN_SUCCESS);
    centralManagerCallbacks_->OnStartOrStopScanEvent(tmpStatus, false);
}

void BleCentralManagerImpl::GapExScanResolvingCompletEvt(int status, bool isStart) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->mutex_);
    if (status != BT_NO_ERROR) {
        pimpl->scanStatus_ = isStart ? SCAN_NOT_STARTED : SCAN_FAILED_ALREADY_STARTED;
        LOG_ERROR("[BleCentralManagerImpl] %{public}s:Resolving stop scan failed! %{public}d.", __func__, status);
    }
}

void BleCentralManagerImpl::HandleGapExScanEvent(const BLE_GAP_CB_EVENT &event, int status) const
{
    LOG_DEBUG("[BleCentralManagerImpl] %{public}s:[event no: %{public}d]", __func__, static_cast<int>(event));

    switch (event) {
        case BLE_GAP_EX_SCAN_PARAM_SET_COMPLETE_EVT:
            GapExScanParamSetCompleteEvt(status);
            break;
        case BLE_GAP_EX_SCAN_RESULT_EVT:
            GapExScanResultEvt();
            break;
        case BLE_GAP_EX_SCAN_DELAY_REPORT_RESULT_EVT:
            GapExScanDelayReportResultEvt();
            break;
        case BLE_GAP_EX_SCAN_START_COMPLETE_EVT:
            GapExScanStartCompleteEvt(status);
            break;
        case BLE_GAP_EX_RESOLVING_LIST_ADV_SCAN_START_COMPLETE_EVT:
            GapExScanResolvingCompletEvt(status, true);
            break;
        case BLE_GAP_EX_SCAN_STOP_COMPLETE_EVT:
            GapExScanStopCompleteEvt(status);
            break;
        case BLE_GAP_EX_RESOLVING_LIST_ADV_SCAN_STOP_COMPLETE_EVT:
            GapExScanResolvingCompletEvt(status, false);
            break;
        default: {
            LOG_ERROR("[BleCentralManagerImpl] %{public}s:Invalid event! %{public}d.", __func__, event);
            break;
        }
    }
}
}  // namespace bluetooth
}  // namespace OHOS
