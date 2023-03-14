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
#include "bluetooth_ble_central_manager.h"
#include "bluetooth_ble_central_manager_callback_stub.h"
#include "bluetooth_def.h"
#include "bluetooth_log.h"
#include "bluetooth_observer_list.h"
#include "i_bluetooth_ble_central_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
struct BleCentralManager::impl {
    impl();
    ~impl();

    void BindServer();
    bool MatchesScanFilters(BluetoothBleScanResult result);
    bool MatchesScanFilter(BluetoothBleScanFilter filter, BluetoothBleScanResult result);
    bool MatchesAddrAndName(BluetoothBleScanFilter filter, BluetoothBleScanResult result);
    bool MatchesServiceUuids(BluetoothBleScanFilter filter, BluetoothBleScanResult result);
    bool MatchesUuid(bluetooth::Uuid filterUuid, bluetooth::Uuid uuid, bluetooth::Uuid uuidMask);
    bool MatchesManufacturerDatas(BluetoothBleScanFilter filter, BluetoothBleScanResult result);
    bool MatchesServiceDatas(BluetoothBleScanFilter filter, BluetoothBleScanResult result);
    std::string ParseServiceData(bluetooth::Uuid uuid, std::string data);
    bool MatchesData(std::vector<uint8_t> fData, std::string rData, std::vector<uint8_t> dataMask);

    class BluetoothBleCentralManagerCallbackImp : public BluetoothBleCentralManagerCallBackStub {
    public:
        BluetoothBleCentralManagerCallbackImp(BleCentralManager::impl &bleCentralManger)
            : bleCentralManger_(bleCentralManger) {};
        ~BluetoothBleCentralManagerCallbackImp() override = default;
        void OnScanCallback(const BluetoothBleScanResult &result) override
        {
            {
                std::lock_guard<std::mutex> lock(bleCentralManger_.blesCanFiltersMutex_);
                if (!bleCentralManger_.bleScanFilters_.empty() && bleCentralManger_.IsNeedFilterMatches_
                    && !bleCentralManger_.MatchesScanFilters(result)) {
                    HILOGE("the result does not matche the filter, ignore");
                    return;
                }
            }

            bleCentralManger_.callbacks_.ForEach([&result](std::shared_ptr<BleCentralManagerCallback> observer) {
                BluetoothBleScanResult tempResult = result;
                BleScanResult scanResult;
                for (auto &manufacturerData : tempResult.GetManufacturerData()) {
                    scanResult.AddManufacturerData(manufacturerData.first, manufacturerData.second);
                }

                for (auto &serviceUuidData : tempResult.GetServiceUuids()) {
                    UUID uuid = UUID::ConvertFrom128Bits(serviceUuidData.ConvertTo128Bits());
                    scanResult.AddServiceUuid(uuid);
                }

                for (auto &serviceData : tempResult.GetServiceData()) {
                    UUID uuid = UUID::ConvertFrom128Bits(serviceData.first.ConvertTo128Bits());
                    scanResult.AddServiceData(uuid, serviceData.second);
                }

                scanResult.SetAdvertiseFlag(tempResult.GetAdvertiseFlag());
                scanResult.SetRssi(tempResult.GetRssi());
                scanResult.SetConnectable(tempResult.IsConnectable());
                BluetoothRemoteDevice device(tempResult.GetPeripheralDevice().GetAddress(), BT_TRANSPORT_BLE);
                scanResult.SetPeripheralDevice(device);
                scanResult.SetPayload(tempResult.GetPayload());

                observer->OnScanCallback(scanResult);
            });
        }
        void OnBleBatchScanResultsEvent(std::vector<BluetoothBleScanResult> &results) override
        {
            HILOGI("enter");
            bleCentralManger_.callbacks_.ForEach([&results](std::shared_ptr<BleCentralManagerCallback> observer) {
                std::vector<BleScanResult> scanResults;
                for (auto &result : results) {
                    BleScanResult scanResult;
                    for (auto &manufacturerData : result.GetManufacturerData()) {
                        scanResult.AddManufacturerData(manufacturerData.first, manufacturerData.second);
                    }

                    for (auto &serviceData : result.GetServiceData()) {
                        UUID uuid = UUID::ConvertFrom128Bits(serviceData.first.ConvertTo128Bits());
                        scanResult.AddServiceData(uuid, serviceData.second);
                    }

                    for (auto &serviceUuidData : result.GetServiceUuids()) {
                        UUID uuid = UUID::ConvertFrom128Bits(serviceUuidData.ConvertTo128Bits());
                        scanResult.AddServiceUuid(uuid);
                    }

                    scanResult.SetAdvertiseFlag(result.GetAdvertiseFlag());
                    scanResult.SetConnectable(result.IsConnectable());
                    scanResult.SetRssi(result.GetRssi());
                    BluetoothRemoteDevice device(result.GetPeripheralDevice().GetAddress(), BT_TRANSPORT_BLE);
                    scanResult.SetPeripheralDevice(device);
                    scanResult.SetPayload(result.GetPayload());

                    scanResults.push_back(scanResult);
                }

                observer->OnBleBatchScanResultsEvent(scanResults);
            });
        }

        void OnStartOrStopScanEvent(int resultCode, bool isStartScan) override
        {
            HILOGI("resultCode: %{public}d, isStartScan: %{public}d", resultCode, isStartScan);
            bleCentralManger_.callbacks_.ForEach(
                [resultCode, isStartScan](std::shared_ptr<BleCentralManagerCallback> observer) {
                    observer->OnStartOrStopScanEvent(resultCode, isStartScan);
            });
        }

    private:
        BleCentralManager::impl &bleCentralManger_;
        BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothBleCentralManagerCallbackImp);
    };
    sptr<BluetoothBleCentralManagerCallbackImp> callbackImp_ = nullptr;

    sptr<IBluetoothBleCentralManager> proxy_ = nullptr;
    BluetoothObserverList<BleCentralManagerCallback> callbacks_;

    std::vector<BluetoothBleScanFilter> bleScanFilters_;
    bool IsNeedFilterMatches_ = true;
    std::mutex blesCanFiltersMutex_;
};

BleCentralManager::impl::impl()
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("samgr is null");
        return;
    }

    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);
    if (!hostRemote) {
        HILOGE("hostRemote is null");
        return;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    if (!hostProxy) {
        HILOGE("hostProxy is null");
        return;
    }
    sptr<IRemoteObject> remote = hostProxy->GetBleRemote(BLE_CENTRAL_MANAGER_SERVER);
    if (!remote) {
        HILOGE("remote is null");
        return;
    }
    proxy_ = iface_cast<IBluetoothBleCentralManager>(remote);
    if (!proxy_) {
        HILOGE("proxy_ is null");
        return;
    }
    callbackImp_ = new BluetoothBleCentralManagerCallbackImp(*this);
    proxy_->RegisterBleCentralManagerCallback(callbackImp_);
}

bool BleCentralManager::impl::MatchesScanFilters(BluetoothBleScanResult result)
{
    HILOGI("enter");
    for (auto filter : bleScanFilters_) {
        if (!MatchesScanFilter(filter, result)) {
            continue;
        }
        return true;
    }
    return false;
}

bool BleCentralManager::impl::MatchesScanFilter(BluetoothBleScanFilter filter, BluetoothBleScanResult result)
{
    if (!MatchesAddrAndName(filter, result)) {
        return false;
    }

    if (filter.HasServiceUuid() && !MatchesServiceUuids(filter, result)) {
        return false;
    }

    if (!MatchesManufacturerDatas(filter, result)) {
        return false;
    }

    if (!MatchesServiceDatas(filter, result)) {
        return false;
    }

    return true;
}

bool BleCentralManager::impl::MatchesAddrAndName(BluetoothBleScanFilter filter, BluetoothBleScanResult result)
{
    std::string address = result.GetPeripheralDevice().GetAddress();
    if (address.empty()) {
        return false;
    }

    BluetoothRemoteDevice device(address, BT_TRANSPORT_BLE);

    std::string deviceId = filter.GetDeviceId();
    if (!deviceId.empty() && deviceId != address) {
        return false;
    }

    std::string name = filter.GetName();
    // To avoid to call IPC if no needed.
    if (!name.empty()) {
        std::string rName = device.GetDeviceName();
        if (rName.empty() || rName != name) {
            return false;
        }
    }

    return true;
}

bool BleCentralManager::impl::MatchesServiceUuids(BluetoothBleScanFilter filter, BluetoothBleScanResult result)
{
    std::vector<bluetooth::Uuid> rUuids = result.GetServiceUuids();
    if (rUuids.empty()) {
        return false;
    }

    bluetooth::Uuid filterUuid = filter.GetServiceUuid();
    bool hasUuidMask = filter.HasServiceUuidMask();

    for (auto &uuid : rUuids) {
        if (!hasUuidMask) {
            if (filterUuid.operator==(uuid)) {
                return true;
            }
        } else {
            bluetooth::Uuid uuidMask = filter.GetServiceUuidMask();
            if (MatchesUuid(filterUuid, uuid, uuidMask)) {
                return true;
            }
        }
    }
    return false;
}

bool BleCentralManager::impl::MatchesUuid(bluetooth::Uuid filterUuid, bluetooth::Uuid uuid, bluetooth::Uuid uuidMask)
{
    HILOGI("enter");
    uint8_t uuid128[bluetooth::Uuid::UUID128_BYTES_TYPE];
    uint8_t uuidMask128[bluetooth::Uuid::UUID128_BYTES_TYPE];
    uint8_t resultUuid128[bluetooth::Uuid::UUID128_BYTES_TYPE];
    if (!filterUuid.ConvertToBytesLE(uuid128)) {
        HILOGE("Convert filter uuid faild.");
        return false;
    }
    if (!uuidMask.ConvertToBytesLE(uuidMask128)) {
        HILOGE("Convert uuid mask faild.");
        return false;
    }
    if (!uuid.ConvertToBytesLE(resultUuid128)) {
        HILOGE("Convert result uuid faild.");
        return false;
    }
    size_t maskLength = sizeof(uuidMask128);
    if (maskLength <= 0) {
        return false;
    }

    for (size_t i = 0; i < maskLength; i++) {
        if ((uuid128[i] & uuidMask128[i]) != (resultUuid128[i] & uuidMask128[i])) {
            return false;
        }
    }

    return true;
}

bool BleCentralManager::impl::MatchesManufacturerDatas(BluetoothBleScanFilter filter, BluetoothBleScanResult result)
{
    uint16_t manufacturerId = filter.GetManufacturerId();
    std::vector<uint8_t> data = filter.GetManufactureData();
    if (data.size() == 0) {
        return true;
    }

    std::vector<uint8_t> dataMask = filter.GetManufactureDataMask();

    for (auto &manufacturerData : result.GetManufacturerData()) {
        if (manufacturerId != manufacturerData.first) {
            continue;
        }

        if (MatchesData(data, manufacturerData.second, dataMask)) {
            return true;
        }
    }
    return false;
}

bool BleCentralManager::impl::MatchesServiceDatas(BluetoothBleScanFilter filter, BluetoothBleScanResult result)
{
    std::vector<uint8_t> data = filter.GetServiceData();
    if (data.size() == 0) {
        return true;
    }

    std::vector<uint8_t> dataMask = filter.GetServiceDataMask();

    for (auto &serviceData : result.GetServiceData()) {
        std::string rSData = ParseServiceData(serviceData.first, serviceData.second);
        if (MatchesData(data, rSData, dataMask)) {
            return true;
        }
    }
    return false;
}

std::string BleCentralManager::impl::ParseServiceData(bluetooth::Uuid uuid, std::string data)
{
    std::string tmpServcieData;
    int uuidType = uuid.GetUuidType();
    switch (uuidType) {
        case bluetooth::Uuid::UUID16_BYTES_TYPE: {
            uint16_t uuid16 = uuid.ConvertTo16Bits();
            tmpServcieData = std::string(reinterpret_cast<char *>(&uuid16), BLE_UUID_LEN_16);
            break;
        }
        case bluetooth::Uuid::UUID32_BYTES_TYPE: {
            uint32_t uuid32 = uuid.ConvertTo32Bits();
            tmpServcieData = std::string(reinterpret_cast<char *>(&uuid32), BLE_UUID_LEN_32);
            break;
        }
        case bluetooth::Uuid::UUID128_BYTES_TYPE: {
            uint8_t uuid128[bluetooth::Uuid::UUID128_BYTES_TYPE];
            if (!uuid.ConvertToBytesLE(uuid128)) {
                HILOGE("Convert filter uuid faild.");
            }
            tmpServcieData = std::string(reinterpret_cast<char *>(&uuid128), BLE_UUID_LEN_128);
            break;
        }
        default:
            break;
    }
    return tmpServcieData + data;
}

bool BleCentralManager::impl::MatchesData(std::vector<uint8_t> fData, std::string rData, std::vector<uint8_t> dataMask)
{
    if (rData.empty()) {
        return false;
    }

    size_t length = fData.size();
    std::vector<uint8_t> vec(rData.begin(), rData.end());
    if (vec.size() < length) {
        return false;
    }

    if (dataMask.empty() || dataMask.size() != length) {
        for (size_t i = 0; i < length; i++) {
            if (fData[i] != vec[i]) {
                return false;
            }
        }
        return true;
    }
    for (size_t i = 0; i < length; i++) {
        if ((fData[i] & dataMask[i]) != (vec[i] & dataMask[i])) {
            return false;
        }
    }
    return true;
}

BleCentralManager::impl::~impl()
{
    proxy_->DeregisterBleCentralManagerCallback(callbackImp_);
}

BleCentralManager::BleCentralManager(BleCentralManagerCallback &callback) : callback_(&callback), pimpl(nullptr)
{
    if (pimpl == nullptr) {
        pimpl = std::make_unique<impl>();
        if (pimpl == nullptr) {
            HILOGE("failed, no pimpl");
        }
    }

    HILOGI("successful");
    std::shared_ptr<BleCentralManagerCallback> pointer(&callback, [](BleCentralManagerCallback *) {});
    bool ret = pimpl->callbacks_.Register(pointer);
    if (ret)
        return;
    callback_ = &callback;
}

BleCentralManager::~BleCentralManager()
{
    callback_ = nullptr;
}

void BleCentralManager::StartScan()
{
    HILOGI("StartScan without param.");
    if (pimpl->proxy_ != nullptr) {
        pimpl->proxy_->StartScan();
    }
}

void BleCentralManager::StartScan(const BleScanSettings &settings)
{
    HILOGI("StartScan with params.");
    if (pimpl->proxy_ != nullptr) {
        BluetoothBleScanSettings setting;
        // not use report delay scan. settings.GetReportDelayMillisValue()
        setting.SetReportDelay(0);
        setting.SetScanMode(settings.GetScanMode());
        setting.SetLegacy(settings.GetLegacy());
        setting.SetPhy(settings.GetPhy());
        pimpl->proxy_->StartScan(setting);
    }
}

void BleCentralManager::StopScan()
{
    HILOGI("clientId: %{public}d", clientId_);
    std::lock_guard<std::mutex> lock(pimpl->blesCanFiltersMutex_);
    if (pimpl->proxy_ != nullptr) {
        pimpl->proxy_->StopScan();
        if (clientId_ != 0) {
            pimpl->proxy_->RemoveScanFilter(clientId_);
            clientId_ = 0;
        }
        pimpl->bleScanFilters_.clear();
        pimpl->IsNeedFilterMatches_ = true;
    }
}

void BleCentralManager::ConfigScanFilter(const std::vector<BleScanFilter>  &filters)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lock(pimpl->blesCanFiltersMutex_);
    if (pimpl->proxy_ != nullptr) {
        std::vector<BluetoothBleScanFilter> bluetoothBleScanFilters;
        for (auto filter : filters) {
            BluetoothBleScanFilter scanFilter;
            scanFilter.SetDeviceId(filter.GetDeviceId());
            scanFilter.SetName(filter.GetName());
            if (filter.HasServiceUuid()) {
                scanFilter.SetServiceUuid(bluetooth::Uuid::ConvertFromString(
                    filter.GetServiceUuid().ToString()));
            }
            if (filter.HasServiceUuidMask()) {
                scanFilter.SetServiceUuidMask(bluetooth::Uuid::ConvertFromString(
                    filter.GetServiceUuidMask().ToString()));
            }
            if (filter.HasSolicitationUuid()) {
                scanFilter.SetServiceSolicitationUuid(bluetooth::Uuid::ConvertFromString(
                    filter.GetServiceSolicitationUuid().ToString()));
            }
            if (filter.HasSolicitationUuidMask()) {
                scanFilter.SetServiceSolicitationUuidMask(bluetooth::Uuid::ConvertFromString(
                    filter.GetServiceSolicitationUuidMask().ToString()));
            }
            scanFilter.SetServiceData(filter.GetServiceData());
            scanFilter.SetServiceDataMask(filter.GetServiceDataMask());
            scanFilter.SetManufacturerId(filter.GetManufacturerId());
            scanFilter.SetManufactureData(filter.GetManufactureData());
            scanFilter.SetManufactureDataMask(filter.GetManufactureDataMask());
            bluetoothBleScanFilters.push_back(scanFilter);
            pimpl->bleScanFilters_.push_back(scanFilter);
        }
        clientId_ = pimpl->proxy_->ConfigScanFilter(clientId_, bluetoothBleScanFilters);

        if (filters.empty()) {
            HILOGE("filters is empty can not config");
            pimpl->IsNeedFilterMatches_ = false;
        }
    } else {
        HILOGI("proxy_ is nullptr");
    }
}

BleScanResult::BleScanResult()
{}

BleScanResult::~BleScanResult()
{}

std::vector<UUID> BleScanResult::GetServiceUuids() const
{
    return serviceUuids_;
}

std::map<uint16_t, std::string> BleScanResult::GetManufacturerData() const
{
    return manufacturerSpecificData_;
}

std::map<UUID, std::string> BleScanResult::GetServiceData() const
{
    return serviceData_;
}

BluetoothRemoteDevice BleScanResult::GetPeripheralDevice() const
{
    return peripheralDevice_;
}

int8_t BleScanResult::GetRssi() const
{
    return rssi_;
}

bool BleScanResult::IsConnectable() const
{
    return connectable_;
}

uint8_t BleScanResult::GetAdvertiseFlag() const
{
    return advertiseFlag_;
}

std::vector<uint8_t> BleScanResult::GetPayload() const
{
    return payload_;
}

void BleScanResult::AddManufacturerData(uint16_t manufacturerId, const std::string &data)
{
    manufacturerSpecificData_.insert(std::make_pair(manufacturerId, data));
}

void BleScanResult::AddServiceData(const UUID &uuid, const std::string &data)
{
    serviceData_.insert(std::make_pair(uuid, data));
}

void BleScanResult::AddServiceUuid(const UUID &serviceUuid)
{
    serviceUuids_.push_back(serviceUuid);
}

void BleScanResult::SetPayload(std::string payload)
{
    payload_.assign(payload.begin(), payload.end());
}

void BleScanResult::SetPeripheralDevice(const BluetoothRemoteDevice &device)
{
    peripheralDevice_ = device;
}

void BleScanResult::SetRssi(int8_t rssi)
{
    rssi_ = rssi;
}

void BleScanResult::SetConnectable(bool connectable)
{
    connectable_ = connectable;
}

void BleScanResult::SetAdvertiseFlag(uint8_t flag)
{
    advertiseFlag_ = flag;
}

BleScanSettings::BleScanSettings()
{}

BleScanSettings::~BleScanSettings()
{}

void BleScanSettings::SetReportDelay(long reportDelayMillis)
{
    reportDelayMillis_ = reportDelayMillis;
}

long BleScanSettings::GetReportDelayMillisValue() const
{
    return reportDelayMillis_;
}

int BleScanSettings::SetScanMode(int scanMode)
{
    if (scanMode < SCAN_MODE_LOW_POWER || scanMode >= SCAN_MODE_INVALID) {
        return RET_BAD_PARAM;
    }

    scanMode_ = scanMode;
    return RET_NO_ERROR;
}

int BleScanSettings::GetScanMode() const
{
    return scanMode_;
}

void BleScanSettings::SetLegacy(bool legacy)
{
    legacy_ = legacy;
}

bool BleScanSettings::GetLegacy() const
{
    return legacy_;
}

void BleScanSettings::SetPhy(int phy)
{
    phy_ = phy;
}

int BleScanSettings::GetPhy() const
{
    return phy_;
}

BleScanFilter::BleScanFilter()
{}

BleScanFilter::~BleScanFilter()
{}

void BleScanFilter::SetDeviceId(std::string deviceId)
{
    deviceId_ = deviceId;
}

std::string BleScanFilter::GetDeviceId() const
{
    return deviceId_;
}

void BleScanFilter::SetName(std::string name)
{
    name_ = name;
}

std::string BleScanFilter::GetName() const
{
    return name_;
}

void BleScanFilter::SetServiceUuid(const UUID &uuid)
{
    serviceUuid_ = uuid;
    hasServiceUuid_ = true;
}

bool BleScanFilter::HasServiceUuid()
{
    return hasServiceUuid_;
}

UUID BleScanFilter::GetServiceUuid() const
{
    return serviceUuid_;
}

void BleScanFilter::SetServiceUuidMask(const UUID &serviceUuidMask)
{
    serviceUuidMask_ = serviceUuidMask;
    hasServiceUuidMask_ = true;
}

bool BleScanFilter::HasServiceUuidMask()
{
    return hasServiceUuidMask_;
}

UUID BleScanFilter::GetServiceUuidMask() const
{
    return serviceUuidMask_;
}

void BleScanFilter::SetServiceSolicitationUuid(const UUID &serviceSolicitationUuid)
{
    serviceSolicitationUuid_ = serviceSolicitationUuid;
    hasSolicitationUuid_ = true;
}

bool BleScanFilter::HasSolicitationUuid()
{
    return hasSolicitationUuid_;
}

UUID BleScanFilter::GetServiceSolicitationUuid() const
{
    return serviceSolicitationUuid_;
}

void BleScanFilter::SetServiceSolicitationUuidMask(const UUID &serviceSolicitationUuidMask)
{
    serviceSolicitationUuidMask_ = serviceSolicitationUuidMask;
    hasSolicitationUuidMask_ = true;
}

bool BleScanFilter::HasSolicitationUuidMask()
{
    return hasSolicitationUuidMask_;
}

UUID BleScanFilter::GetServiceSolicitationUuidMask() const
{
    return serviceSolicitationUuidMask_;
}

void BleScanFilter::SetServiceData(std::vector<uint8_t> serviceData)

{
    serviceData_ = serviceData;
}

std::vector<uint8_t> BleScanFilter::GetServiceData() const
{
    return serviceData_;
}

void BleScanFilter::SetServiceDataMask(std::vector<uint8_t> serviceDataMask)
{
    serviceDataMask_ = serviceDataMask;
}

std::vector<uint8_t> BleScanFilter::GetServiceDataMask() const
{
    return serviceDataMask_;
}

void BleScanFilter::SetManufacturerId(uint16_t manufacturerId)
{
    manufacturerId_ = manufacturerId;
}

uint16_t BleScanFilter::GetManufacturerId() const
{
    return manufacturerId_;
}

void BleScanFilter::SetManufactureData(std::vector<uint8_t> manufactureData)
{
    manufactureData_ = manufactureData;
}

std::vector<uint8_t> BleScanFilter::GetManufactureData() const
{
    return manufactureData_;
}

void BleScanFilter::SetManufactureDataMask(std::vector<uint8_t> manufactureDataMask)
{
    manufactureDataMask_ = manufactureDataMask;
}

std::vector<uint8_t> BleScanFilter::GetManufactureDataMask() const
{
    return manufactureDataMask_;
}
}  // namespace Bluetooth
}  // namespace OHOS
