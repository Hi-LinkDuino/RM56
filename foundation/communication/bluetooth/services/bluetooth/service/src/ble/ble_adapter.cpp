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

#include "ble_adapter.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>

#include "adapter_manager.h"
#include "base_observer_list.h"
#include "ble_feature.h"
#include "ble_utils.h"
#include "btm.h"
#include "btstack.h"
#include "class_creator.h"
#include "compat.h"
#include "interface_profile_gatt_client.h"
#include "interface_profile_manager.h"
#include "log.h"
#include "log_util.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct BleAdapter::impl {
    class GattClientCallback : public IGattClientCallback {
    public:
        explicit GattClientCallback(BleAdapter &bleAdapter) : bleAdapter_(bleAdapter)
        {}
        ~GattClientCallback()
        {}
        void OnConnectionStateChanged(int state, int newState) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s state:%{public}d", __FILE__, __LINE__, __FUNCTION__, newState);
        }

        void OnServicesChanged(const std::vector<Service> &services) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnCharacteristicRead(int ret, const Characteristic &characteristic) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s ret : %{public}d", __FILE__, __LINE__, __FUNCTION__, ret);
            std::lock_guard<std::mutex> lock(bleAdapter_.pimpl->mutexRemoteName_);
            if (GattStatus::GATT_SUCCESS == ret) {
                std::string name(characteristic.value_.get(), characteristic.value_.get() + characteristic.length_);
                bleAdapter_.pimpl->remoteDeviceName_ = name;
                bleAdapter_.pimpl->readCharacteristicFlag_ = true;
            } else {
                bleAdapter_.pimpl->readCharacteristicFlag_ = false;
            }
            bleAdapter_.pimpl->cvfull_.notify_all();
        }

        void OnCharacteristicWrite(int ret, const Characteristic &characteristic) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnCharacteristicChanged(const Characteristic &characteristic) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnDescriptorRead(int ret, const Descriptor &descriptor) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnDescriptorWrite(int ret, const Descriptor &descriptor) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnMtuChanged(int state, int mtu) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnConnectionParameterChanged(int interval, int latency, int timeout, int status) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

        void OnServicesDiscovered(int status) override
        {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        }

    private:
        BleAdapter &bleAdapter_;
    };
    impl(BleAdapter &bleAdapter);
    impl(const impl &);
    impl &operator=(const impl &);
    ~impl();

    std::recursive_mutex syncMutex_ {};
    std::unique_ptr<BaseObserverList<IAdapterBleObserver>> observer_ =
        std::make_unique<BaseObserverList<IAdapterBleObserver>>();
    std::unique_ptr<BaseObserverList<IBlePeripheralCallback>> blePeripheralCallback_ =
        std::make_unique<BaseObserverList<IBlePeripheralCallback>>();

    std::mutex mutexRpa_ {};
    std::mutex mutexAdvAdnScan_ {};
    std::mutex mutexRemoteName_ {};
    std::condition_variable cvfull_ {};

    std::unique_ptr<GattClientCallback> gattClientcallback_ {};
    IProfileGattClient *gattClientService_ {};
    std::string remoteDeviceName_ {};
    std::map<std::string, BlePeripheralDevice> peerConnDeviceList_ {};
    bool btmEnableFlag_ = false;
    bool readCharacteristicFlag_ = false;

    std::unique_ptr<BleAdvertiserImpl> bleAdvertiser_ = nullptr;
    std::unique_ptr<BleCentralManagerImpl> bleCentralManager_ = nullptr;
    std::unique_ptr<BleSecurity> bleSecurity_ = nullptr;
    BtmAclCallbacks btmAclCb_ {};
};

BleAdapter::impl::impl(BleAdapter &bleAdapter) : gattClientcallback_(std::make_unique<GattClientCallback>(bleAdapter))
{}

BleAdapter::impl::~impl()
{}

BleAdapter::BleAdapter() : utility::Context(ADAPTER_NAME_BLE, "5.0"), pimpl(std::make_unique<BleAdapter::impl>(*this))
{
    LOG_DEBUG("[BleAdapter] %{public}s:Create", Name().c_str());
}

BleAdapter::~BleAdapter()
{
    LOG_DEBUG("[BleAdapter] %{public}s:Destroy", Name().c_str());

    pimpl->bleAdvertiser_ = nullptr;
    pimpl->bleCentralManager_ = nullptr;
    pimpl->bleSecurity_ = nullptr;
}

void BleAdapter::Enable()
{
    LOG_DEBUG("[BleAdapter] %{public}s:%{public}s", __func__, Name().c_str());

    GetDispatcher()->PostTask(std::bind(&BleAdapter::EnableTask, this));
}

bool BleAdapter::EnableTask()
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool ret = (BTM_Enable(LE_CONTROLLER) == BT_NO_ERROR);
    if (!ret) {
        pimpl->btmEnableFlag_ = false;
        LOG_ERROR("[BleAdapter] %{public}s:BTM enable failed!", __func__);
    } else {
        pimpl->btmEnableFlag_ = true;
        LoadConfig();
        ret = (InitBtmAndGap() == BT_NO_ERROR);
        LOG_DEBUG("[BleAdapter] %{public}s:BTM enable successfully!", __func__);
    }

    GetContext()->OnEnable(ADAPTER_NAME_BLE, ret);

    return ret;
}

int BleAdapter::InitBtmAndGap()
{
    int ret = RegisterCallbackToBtm();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:RegisterCallbackToBtm failed!", __func__);
    }

    ret = SetLocalIrkAndIdentityAddrToBtm();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:SetLocalIrkAndIdentityAddrToBtm failed!", __func__);
    }

    ret = SetRpaAddrAndTypeToBtm();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:SetRpaAddrAndTypeToBtm failed!", __func__);
    }

    GAP_LeSecMode1Level level1 = static_cast<GAP_LeSecMode1Level>(BleConfig::GetInstance().GetBleModel1Level());
    GAP_LeSecMode2Level level2 = static_cast<GAP_LeSecMode2Level>(BleConfig::GetInstance().GetBleModel2Level());
    ret = GAPIF_LeSetSecurityMode(level1, level2);
    if (ret == BT_NOT_SUPPORT) {
        ret = GAPIF_LeSetSecurityMode(LE_MODE_1_LEVEL_3, level2);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("[BleAdapter] %{public}s:GAP_LeSetSecurityMode failed!", __func__);
        }
    }

    RegisterBleSecurityCallback(*pimpl->observer_.get());

    ret = GAPIF_LeSetMinEncKeySize(GAP_ENC_KEY_MIN_SIZE);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:GAP_LeSetMinEncKeySize failed!", __func__);
    }

    ret = BleProperties::GetInstance().SetBondableMode(GAP_BONDABLE_MODE);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:SetBondableMode failed!", __func__);
    }

    /// Save all peer paired devices to BTM
    std::vector<std::string> pairedAddrList = BleConfig::GetInstance().GetPairedAddrList();
    ReadPeerDeviceInfoFromConf(pairedAddrList);
    SavePeerDevices2BTM(pimpl->peerConnDeviceList_);
    return ret;
}

void BleAdapter::Disable()
{
    LOG_DEBUG("[BleAdapter] %{public}s:%{public}s", __func__, Name().c_str());

    GetDispatcher()->PostTask(std::bind(&BleAdapter::DisableTask, this));
}

bool BleAdapter::DisableTask()
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (!pimpl->btmEnableFlag_) {
        GetContext()->OnDisable(ADAPTER_NAME_BLE, pimpl->btmEnableFlag_);
        return false;
    }

    SavePeerDeviceInfoToConf(pimpl->peerConnDeviceList_);
    ClearPeerDeviceInfo();
    int ret = BleProperties::GetInstance().SetBondableMode(GAP_BONDABLE_MODE_NON);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:SetBondableMode failed!", __func__);
    }

    StartOrStopAdvAndScan(STOP_ADV_TYPE_ALL, STOP_SCAN_TYPE_NOR);
    if (BleFeature::GetInstance().IsLeExtendedAdvertisingSupported()) {
        ExAdvClearHandle();
    }
    ClearScanResultInfo();
    DeregisterAllCallback();

    ret = (BTM_Disable(LE_CONTROLLER) == BT_NO_ERROR);
    if (!ret) {
        LOG_ERROR("[BleAdapter] %{public}s:BTM Disable failed!", __func__);
    } else {
        LOG_DEBUG("[BleAdapter] %{public}s:BTM Disable successfully!", __func__);
    }
    GetContext()->OnDisable(ADAPTER_NAME_BLE, ret);
    return ret;
}

void BleAdapter::NotifyAllWaitContinue() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    pimpl->cvfull_.notify_all();
}

void BleAdapter::PostEnable()
{
    LOG_DEBUG("[BleAdapter] %{public}s:%{public}s", __func__, Name().c_str());

    if (!pimpl->btmEnableFlag_) {
        GetDispatcher()->PostTask(std::bind(&BleAdapter::PostEnableTask, this));
        return;
    }

    int ret = SetBleRoles();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdvertiserImpl] %{public}s:Set ble roles failed!.", __func__);
    }

    GetDispatcher()->PostTask(std::bind(&BleAdapter::PostEnableTask, this));
}

bool BleAdapter::PostEnableTask() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    return true;
}

void BleAdapter::StartOrStopAdvAndScan(
    const STOP_ALL_ADV_TYPE &stopAllAdvType, const STOP_SCAN_TYPE &scanType, bool isStartAdvAndScan) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if ((pimpl->bleAdvertiser_ != nullptr) &&
        (pimpl->bleAdvertiser_->GetAdvertisingStatus() == ADVERTISE_FAILED_ALREADY_STARTED)) {
        std::unique_lock<std::mutex> lock(pimpl->mutexAdvAdnScan_);
        pimpl->bleAdvertiser_->StartOrStopAllAdvertising(stopAllAdvType, isStartAdvAndScan);
        if (pimpl->cvfull_.wait_for(lock, std::chrono::seconds(BLE_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
            LOG_ERROR("[BleAdapter] %{public}s:StartOrStopAdvAndScan timeout!", __func__);
        }
    }

    if (pimpl->bleCentralManager_ != nullptr) {
        if (pimpl->bleCentralManager_->GetScanStatus() == SCAN_FAILED_ALREADY_STARTED) {
            std::unique_lock<std::mutex> lock(pimpl->mutexAdvAdnScan_);
            pimpl->bleCentralManager_->StartOrStopScan(scanType, isStartAdvAndScan);
            if (pimpl->cvfull_.wait_for(lock, std::chrono::seconds(BLE_THREAD_WAIT_TIMEOUT)) ==
                std::cv_status::timeout) {
                LOG_ERROR("[BleAdapter] %{public}s:StartOrStopAdvAndScan timeout!", __func__);
            }
        }
    }
}

void BleAdapter::ExAdvClearHandle() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::unique_lock<std::mutex> lock(pimpl->mutexAdvAdnScan_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        pimpl->bleAdvertiser_->GAPExAdvClearHandle();
        if (pimpl->cvfull_.wait_for(lock, std::chrono::seconds(BLE_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
            LOG_ERROR("[BleAdapter] %{public}s:ExAdvClearHandle timeout!", __func__);
        }
    }
}

void BleAdapter::LoadConfig() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    bool ret = BleProperties::GetInstance().LoadBleConfigInfo();
    if (!ret) {
        LOG_ERROR("[BleAdapter] %{public}s:LoadBleConfigInfo File failed!", __func__);
    } else {
        LOG_DEBUG("[BleAdapter] %{public}s:LoadBleConfigInfo File success!", __func__);
    }

    ret &= BleProperties::GetInstance().ConfigBleProperties();
    ret &= BleProperties::GetInstance().GetAddrFromController();
    if (ret) {
        BTM_SetOwnAddressType(BLE_ADDR_TYPE::BLE_ADDR_TYPE_PUBLIC);
    }

    if (!ret) {
        LOG_ERROR("[BleAdapter] %{public}s:ConfigBleProperties File failed!", __func__);
        BleProperties::GetInstance().SaveDefaultValues();
    } else {
        LOG_DEBUG("[BleAdapter] %{public}s:ConfigBleProperties File success!", __func__);
    }
}

int BleAdapter::DeregisterAllCallback() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    DeregisterBleSecurityCallback();
    DeregisterBleAdvertiserCallback();
    DeregisterBleCentralManagerCallback();
    return DeregisterCallbackToBtm();
}

void BleAdapter::GenResPriAddrResult(uint8_t result, const uint8_t addr[BT_ADDRESS_SIZE], void *context)
{
    HILOGI("ResPriAddr: %{public}s", GetEncryptAddr(RawAddress::ConvertToString(addr).GetAddress()).c_str());

    auto *adapter = static_cast<BleAdapter *>(context);
    std::unique_lock<std::mutex> lock(adapter->pimpl->mutexRpa_);
    BtAddr btAddr;
    (void)memcpy_s(&btAddr.addr, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
    btAddr.type = BLE_ADDR_TYPE_RANDOM;
    int ret = BTM_SetLeRandomAddress(&btAddr);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:GenResPriAddrResult failed!", __func__);
    }
    BTM_SetOwnAddressType(BLE_ADDR_TYPE_RANDOM);

    adapter->pimpl->cvfull_.notify_all();
}

int BleAdapter::SetRpaAddrAndTypeToBtm()
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    /// btm set address type and rpa address
    int ret = BT_NO_ERROR;
    switch (BleConfig::GetInstance().GetBleAddrType()) {
        case BLE_ADDR_TYPE_RPA: {
            std::unique_lock<std::mutex> lock(pimpl->mutexRpa_);
            ret = GAPIF_LeGenResPriAddr(&BleAdapter::GenResPriAddrResult, this);
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("[BleAdapter] %{public}s:GAP_LeGenResPriAddrAsync failed!", __func__);
            }
            if (pimpl->cvfull_.wait_for(lock, std::chrono::seconds(BLE_THREAD_WAIT_TIMEOUT)) ==
                std::cv_status::timeout) {
                LOG_ERROR("[BleAdapter] %{public}s:GAP_LeGenResPriAddrAsync timeout!", __func__);
            }
            break;
        }
        case BLE_ADDR_TYPE_PUBLIC:
        default:
            BTM_SetOwnAddressType(BleConfig::GetInstance().GetBleLocalAddrType());
            break;
    }
    return ret;
}

int BleAdapter::SetLocalIrkAndIdentityAddrToBtm() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::string irk = BleConfig::GetInstance().GetLocalIrk();
    if (irk.empty()) {
        std::vector<uint8_t> key;
        BleUtils::Rand16hex(key);
        BleConfig::GetInstance().SetLocalIrk(BleUtils::ConvertIntToHexString(key));
        irk = BleUtils::ConvertIntToHexString(key);
    }
    std::vector<uint8_t> vec;
    BleUtils::ConvertHexStringToInt(irk, vec);
    BtmKey btmKey;
    if (memcpy_s(btmKey.key, KEY_SIZE, &vec[0], vec.size()) != EOK) {
        LOG_DEBUG("[BleAdapter] %{public}s:memcpy_s btmKey failed!", __func__);
        return BT_OPERATION_FAILED;
    }
    BTM_SetLocalIdentityResolvingKey(&btmKey);
    /// check public address
    std::string addr = BleConfig::GetInstance().GetLocalAddress();
    int ret = BT_NO_ERROR;
    if ((addr.empty()) || (INVALID_MAC_ADDRESS.compare(addr) == 0)) {
        std::vector<uint8_t>().swap(vec);
        BleUtils::GetRandomAddress(vec, false);
        addr = RawAddress::ConvertToString(&vec[0]).GetAddress();
        BleConfig::GetInstance().SetBleLocalAddrType(BLE_ADDR_TYPE::BLE_ADDR_TYPE_RANDOM);
        BTM_SetOwnAddressType(BLE_ADDR_TYPE::BLE_ADDR_TYPE_RANDOM);
        HILOGI("GAP_LeSetStaticIdentityAddr random addr = %{public}s!", GetEncryptAddr(addr).c_str());
        ret = GAPIF_LeSetStaticIdentityAddr(&vec[0]);
        if (ret != BT_NO_ERROR) {
            LOG_DEBUG("[BleAdapter] %{public}s:GAP_LeSetStaticIdentityAddr failed!", __func__);
        }
    }

    if (!BleConfig::GetInstance().SetLocalIdentityAddr(addr)) {
        LOG_DEBUG("[BleAdapter] %{public}s:SetLocalIdentityAddr failed!", __func__);
    }
    return ret;
}

std::string BleAdapter::GetLocalAddress() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleProperties::GetInstance().GetLocalAddress();
}

std::string BleAdapter::GetLocalName() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleProperties::GetInstance().GetLocalName();
}

bool BleAdapter::SetLocalName(const std::string &name) const
{
    LOG_DEBUG("[BleAdapter] %{public}s:SetLocalName %{public}s", __func__, name.c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleProperties::GetInstance().SetLocalName(name);
}

std::string BleAdapter::GetDeviceName(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::string remoteName = "";

    if (!Compat::CompatCheck(CompatType::COMPAT_REJECT_NAME_REQUEST, device.GetAddress())) {
        int appID = RegisterGattClientApplication(device);
        remoteName = ReadRemoteDeviceNameByGatt(device, appID);
        DeregisterGattClientApplication(appID);
    }

    if (!remoteName.empty()) {
        return remoteName;
    }

    if (pimpl->bleCentralManager_ != nullptr) {
        remoteName = pimpl->bleCentralManager_->GetDeviceName(device.GetAddress());
    }

    if (remoteName.empty()) {
        std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
        auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
        if (it != pimpl->peerConnDeviceList_.end()) {
            remoteName = it->second.GetName();
        }
    }
    return remoteName;
}

int BleAdapter::RegisterGattClientApplication(const RawAddress &addr) const
{
    pimpl->gattClientService_ =
        static_cast<IProfileGattClient *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_GATT_CLIENT));
    if (pimpl->gattClientService_ != nullptr) {
        return pimpl->gattClientService_->RegisterSharedApplication(
            *pimpl->gattClientcallback_, addr, BTTransport::ADAPTER_BLE);
    }

    return RET_NO_SUPPORT;
}

void BleAdapter::DeregisterGattClientApplication(int appID) const
{
    pimpl->gattClientService_ =
        static_cast<IProfileGattClient *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_GATT_CLIENT));
    if (pimpl->gattClientService_ == nullptr) {
        return;
    }
    pimpl->gattClientService_->DeregisterApplication(appID);
}

std::string BleAdapter::ReadRemoteDeviceNameByGatt(const RawAddress &addr, int appID) const
{
    std::string name = "";
    pimpl->gattClientService_ =
        static_cast<IProfileGattClient *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_GATT_CLIENT));
    if (pimpl->gattClientService_ == nullptr) {
        return name;
    }

    auto it = pimpl->peerConnDeviceList_.find(addr.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        LOG_DEBUG("[BleAdapter] isAclConnect %{public}d ", it->second.IsAclConnected());
        if (it->second.IsAclConnected()) {
            std::unique_lock<std::mutex> lock(pimpl->mutexRemoteName_);
            // Device name
            LOG_DEBUG("Get device name from gatt. %{public}d", appID);
            Uuid uuid = Uuid::ConvertFrom16Bits(GATT_UUID_GAP_DEVICE_NAME);
            pimpl->gattClientService_->Connect(appID, true);
            pimpl->gattClientService_->ReadCharacteristicByUuid(appID, uuid);
            if (pimpl->cvfull_.wait_for(lock, std::chrono::seconds(BLE_THREAD_WAIT_TIMEOUT)) ==
                std::cv_status::timeout) {
                LOG_ERROR("[BleAdapter] %{public}s:ReadRemoteDeviceNameByGatt timeout!", __func__);
                pimpl->gattClientService_->Disconnect(appID);
                return name;
            }
            if (pimpl->readCharacteristicFlag_) {
                pimpl->gattClientService_->Disconnect(appID);
            }
            return pimpl->remoteDeviceName_;
        }
    }
    return name;
}

std::vector<Uuid> BleAdapter::GetDeviceUuids(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<Uuid> uuids;
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        uuids = it->second.GetServiceUUID();
    }
    return uuids;
}

std::vector<RawAddress> BleAdapter::GetPairedDevices() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<RawAddress> pairedList;
    for (auto it = pimpl->peerConnDeviceList_.begin(); it != pimpl->peerConnDeviceList_.end(); it++) {
        if (BLE_PAIR_PAIRED == it->second.GetPairedStatus()) {
            RawAddress rawAddr(it->second.GetRawAddress());
            pairedList.push_back(rawAddr);
        }
    }
    return pairedList;
}

std::vector<RawAddress> BleAdapter::GetConnectedDevices() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<RawAddress> pairedList;
    for (auto it = pimpl->peerConnDeviceList_.begin(); it != pimpl->peerConnDeviceList_.end(); it++) {
        RawAddress rawAddr(it->second.GetRawAddress());
        pairedList.push_back(rawAddr);
    }
    return pairedList;
}

bool BleAdapter::StartPair(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GetEncryptAddr(device.GetAddress()).c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleSecurity_ == nullptr) {
        LOG_ERROR("[BleAdapter] %{public}s:failed", __func__);
        return false;
    }

    uint8_t peerAddrType = GetPeerDeviceAddrType(device.GetAddress());
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        peerAddrType = it->second.GetAddressType();
        if (BLE_PAIR_PAIRING == it->second.GetPairedStatus()) {
            LOG_ERROR("[BleAdapter] %{public}s:StartPair failed, because of PAIR_NONE or PAIRING!", __func__);
            return false;
        }
    }

    int ret = pimpl->bleSecurity_->StartPair(device, peerAddrType);
    if (!ret) {
        LOG_ERROR("[BleAdapter] %{public}s:failed", __func__);
        return false;
    }
    LePairingStatus(device);
    return true;
}

bool BleAdapter::CancelPairing(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GetEncryptAddr(device.GetAddress()).c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        int pairState = it->second.GetPairedStatus();
        if ((BLE_PAIR_PAIRED == pairState) || (BLE_PAIR_CANCELING == pairState) || (BLE_PAIR_NONE == pairState)) {
            HILOGE("CancelPairing failed, because of BLE_PAIR_NONE, PAIR_PAIRED or PAIR_CANCELING! %{public}d",
                pairState);
            return false;
        }

        if (pimpl->bleSecurity_ == nullptr) {
            return false;
        }

        if (BT_NO_ERROR == pimpl->bleSecurity_->CancelPairing(device)) {
            it->second.SetPairedStatus(BLE_PAIR_CANCELING);
        } else {
            LOG_ERROR("[BleAdapter] %{public}s:CancelPairing failed, because of gap cancel pair failed!", __func__);
            return false;
        }
    } else {
        LOG_ERROR("[BleAdapter] %{public}s:CancelPairing failed, because of not find the remote device!", __func__);
        return false;
    }

    return true;
}

bool BleAdapter::RemovePairWithDisConnect(const RawAddress &device, bool isDisconnect) const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if ((it == pimpl->peerConnDeviceList_.end()) || (it->second.GetPairedStatus() != BLE_PAIR_PAIRED)) {
        LOG_ERROR("[BleAdapter] %{public}s:RemovePair failed, because of not find the paired device!", __func__);
        return false;
    }

    BleConfig::GetInstance().RemovePairedDevice(device.GetAddress());

    if ((it->second.IsAclConnected()) && (isDisconnect)) {
        int ret = BTM_AclDisconnect(it->second.GetConnectionHandle(), BTM_ACL_DISCONNECT_REASON);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("[BleAdapter] %{public}s:BTM_AclDisconnect failed!", __func__);
        }
    }

    // Del pair device from BTM
    StartOrStopAdvAndScan(STOP_ADV_TYPE_RESOLVING_LIST, STOP_SCAN_TYPE_RESOLVING_LIST);
    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0x00, sizeof(btAddr));
    btAddr.type = it->second.GetAddressType();
    device.ConvertToUint8(btAddr.addr);
    BTM_RemoveLePairedDevice(&btAddr);
    StartOrStopAdvAndScan(STOP_ADV_TYPE_RESOLVING_LIST, STOP_SCAN_TYPE_RESOLVING_LIST, true);

    if (isDisconnect) {
        pimpl->peerConnDeviceList_.erase(device.GetAddress());
    }
    BleConfig::GetInstance().Save();

    std::vector<RawAddress> removeDevices;
    removeDevices.push_back(device);
    AdapterManager::GetInstance()->OnPairDevicesRemoved(BTTransport::ADAPTER_BLE, removeDevices);

    if (pimpl->blePeripheralCallback_ != nullptr) {
        pimpl->blePeripheralCallback_->ForEach([device](IBlePeripheralCallback &observer) {
            observer.OnPairStatusChanged(ADAPTER_BLE, device, BLE_PAIR_NONE);
        });
    }
    return true;
}

bool BleAdapter::RemovePair(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GetEncryptAddr(device.GetAddress()).c_str());

    return RemovePairWithDisConnect(device);
}

bool BleAdapter::RemoveAllPairs()
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<RawAddress> removeDevices;
    auto it = pimpl->peerConnDeviceList_.begin();
    while (it != pimpl->peerConnDeviceList_.end()) {
        if (it->second.GetPairedStatus() != BLE_PAIR_PAIRED) {
            it++;
            continue;
        }

        std::string addr = it->second.GetRawAddress().GetAddress();
        BleConfig::GetInstance().RemovePairedDevice(addr);
        removeDevices.push_back(it->second.GetRawAddress().GetAddress());

        int ret = BT_NO_ERROR;
        if (it->second.IsAclConnected()) {
            ret = BTM_AclDisconnect(it->second.GetConnectionHandle(), BTM_ACL_DISCONNECT_REASON);
        }
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("[BleAdapter] %{public}s:BTM_AclDisconnect failed!", __func__);
        }
        pimpl->peerConnDeviceList_.erase(it++);
    }

    // Del all paired devices from BTM
    StartOrStopAdvAndScan(STOP_ADV_TYPE_RESOLVING_LIST, STOP_SCAN_TYPE_RESOLVING_LIST);
    BTM_SetLePairedDevices(nullptr, 0);
    StartOrStopAdvAndScan(STOP_ADV_TYPE_RESOLVING_LIST, STOP_SCAN_TYPE_RESOLVING_LIST, true);

    if (!removeDevices.empty()) {
        BleConfig::GetInstance().Save();
        AdapterManager::GetInstance()->OnPairDevicesRemoved(BTTransport::ADAPTER_BLE, removeDevices);
    }
    return true;
}

bool BleAdapter::IsRemovePairedDevice(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        return false;
    }
    return true;
}

bool BleAdapter::IsBondedFromLocal(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool isBondedFromLocal = false;
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        isBondedFromLocal = it->second.IsAclEncrypted();
    }
    return isBondedFromLocal;
}

bool BleAdapter::SetDevicePasskey(const RawAddress &device, int passkey, bool accept) const
{
    LOG_DEBUG("[BleAdapter] %{public}s:%{public}d %{public}d", __func__, passkey, accept);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleSecurity_ == nullptr) {
        LOG_ERROR("[BleAdapter] %{public}s:SetDevicePasskey failed!", __func__);
        return false;
    }

    if (!BLE_INVALID_MAC_ADDRESS.compare(device.GetAddress())) {
        LOG_ERROR("[BleAdapter] %{public}s:SetDevicePasskey failed, because of invalid bt address!", __func__);
        return false;
    }

    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it == pimpl->peerConnDeviceList_.end()) {
        LOG_ERROR("[BleAdapter] %{public}s:SetDevicePasskey failed, because of address not exist!", __func__);
        return false;
    }

    int ret;
    if ((BLE_PAIR_CANCELING == it->second.GetPairedStatus()) || (!accept)) {
        ret = pimpl->bleSecurity_->SetDevicePasskey(device, passkey, GAP_NOT_ACCEPT);
    } else {
        ret = pimpl->bleSecurity_->SetDevicePasskey(device, passkey, GAP_ACCEPT);
    }
    if (BT_NO_ERROR != ret) {
        LOG_ERROR("[BleAdapter] %{public}s:SetDevicePasskey failed!", __func__);
        return false;
    }
    return true;
}

bool BleAdapter::PairRequestReply(const RawAddress &device, bool accept) const
{
    LOG_DEBUG("[BleAdapter] %{public}s:%{public}d", __func__, accept);

    if (pimpl->bleSecurity_ != nullptr) {
        int addrType = GetPeerDeviceAddrType(device);
        return pimpl->bleSecurity_->PairRequestReply(device, addrType, accept);
    }
    return false;
}

bool BleAdapter::IsAclConnected(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool isAclConnected = false;
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        isAclConnected = it->second.IsAclConnected();
    }
    return isAclConnected;
}

bool BleAdapter::IsAclEncrypted(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool isAclEncrypted = false;
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        isAclEncrypted = it->second.IsAclEncrypted();
    }
    return isAclEncrypted;
}

utility::Context *BleAdapter::GetContext()
{
    return this;
}

int BleAdapter::GetPairState(const RawAddress &device) const
{
    HILOGI("addr: %{public}s", GetEncryptAddr(device.GetAddress()).c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    int pairState = BLE_PAIR_NONE;
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it == pimpl->peerConnDeviceList_.end()) {
        return pairState;
    } else {
        pairState = it->second.GetPairedStatus();
    }
    return pairState;
}

int BleAdapter::GetBondableMode() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleProperties::GetInstance().GetBondableMode();
}

bool BleAdapter::SetBondableMode(int mode) const
{
    LOG_DEBUG("[BleAdapter] %{public}s:%{public}d", __func__, mode);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return (BleProperties::GetInstance().SetBondableMode(mode) == BT_NO_ERROR);
}

bool BleAdapter::SetDevicePairingConfirmation(const RawAddress &device, bool accept) const
{
    HILOGI("addr: %{public}s, accept: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), accept);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleSecurity_ == nullptr) {
        LOG_ERROR("[BleAdapter] %{public}s:failed", __func__);
        return false;
    }

    if (BLE_INVALID_MAC_ADDRESS == device.GetAddress()) {
        LOG_ERROR("[BleAdapter] %{public}s:failed, because of invalid bt address!", __func__);
        return false;
    }

    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it == pimpl->peerConnDeviceList_.end()) {
        LOG_ERROR("[BleAdapter] %{public}s:failed, because of address not exist!", __func__);
        return false;
    }

    int ret;
    if ((BLE_PAIR_CANCELING == it->second.GetPairedStatus()) || (!accept)) {
        ret = pimpl->bleSecurity_->SetUserConfirm(device, GAP_NOT_ACCEPT);
    } else {
        ret = pimpl->bleSecurity_->SetUserConfirm(device, GAP_ACCEPT);
    }
    if (BT_NO_ERROR != ret) {
        LOG_ERROR("[BleAdapter] %{public}s:failed!", __func__);
        return false;
    }
    return true;
}

int BleAdapter::GetBleMaxAdvertisingDataLength() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleFeature::GetInstance().GetBleMaximumAdvertisingDataLength();
}

int BleAdapter::GetIoCapability() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleProperties::GetInstance().GetIoCapability();
}

bool BleAdapter::SetIoCapability(int ioCapability) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return BleProperties::GetInstance().SetIoCapability(ioCapability);
}

bool BleAdapter::IsBleEnabled() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    int status = AdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BLE);
    return (status == BTStateID::STATE_TURN_ON);
}

bool BleAdapter::IsBtDiscovering() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        return (SCAN_FAILED_ALREADY_STARTED == pimpl->bleCentralManager_->GetScanStatus());
    }
    return false;
}

void BleAdapter::RegisterBleAdvertiserCallback(IBleAdvertiserCallback &callback)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ == nullptr) {
        pimpl->bleAdvertiser_ = std::make_unique<BleAdvertiserImpl>(callback, *this, *GetDispatcher());
    }
}

void BleAdapter::DeregisterBleAdvertiserCallback() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->bleAdvertiser_ != nullptr) {
        pimpl->bleAdvertiser_->DeregisterCallbackToGap();
    }
}

void BleAdapter::RegisterBleCentralManagerCallback(IBleCentralManagerCallback &callback)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ == nullptr) {
        pimpl->bleCentralManager_ = std::make_unique<BleCentralManagerImpl>(callback, *this, *GetDispatcher());
    }
}

void BleAdapter::DeregisterBleCentralManagerCallback() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->bleCentralManager_ != nullptr) {
        pimpl->bleCentralManager_->DeregisterCallbackToGap();
    }
}

void BleAdapter::RegisterBlePeripheralCallback(IBlePeripheralCallback &callback) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->blePeripheralCallback_ != nullptr) {
        pimpl->blePeripheralCallback_->Register(callback);
    }
}

void BleAdapter::DeregisterBlePeripheralCallback(IBlePeripheralCallback &callback) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->blePeripheralCallback_ != nullptr) {
        pimpl->blePeripheralCallback_->Deregister(callback);
    }
}

void BleAdapter::RegisterBleSecurityCallback(BaseObserverList<IAdapterBleObserver> &callback)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->bleSecurity_ == nullptr) {
        pimpl->bleSecurity_ = std::make_unique<BleSecurity>(*this, *GetDispatcher(), callback);
    } else {
        pimpl->bleSecurity_->RegisterCallbackToGap();
    }
}

void BleAdapter::DeregisterBleSecurityCallback() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->bleSecurity_ != nullptr) {
        pimpl->bleSecurity_->DeregisterCallbackToGap();
    }
}

bool BleAdapter::RegisterBleAdapterObserver(IAdapterBleObserver &observer) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->observer_ != nullptr) {
        pimpl->observer_->Register(observer);
    }
    BleProperties::GetInstance().RegisterBleAdapterObserver(*pimpl->observer_.get());
    return true;
}

bool BleAdapter::DeregisterBleAdapterObserver(IAdapterBleObserver &observer) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    BleProperties::GetInstance().DeregisterBleAdapterObserver(observer);
    return true;
}

int BleAdapter::GetPeerDeviceAddrType(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    int type = BLE_ADDR_TYPE_UNKNOWN;
    if (pimpl->bleCentralManager_ != nullptr) {
        type = pimpl->bleCentralManager_->GetDeviceAddrType(device.GetAddress());
    }
    if (type == BLE_ADDR_TYPE_UNKNOWN) {
        std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
        type = BLE_ADDR_TYPE_RANDOM;
        auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
        if (it != pimpl->peerConnDeviceList_.end()) {
            type = it->second.GetAddressType();
        }
    }
    return type;
}

void BleAdapter::ReadPeerDeviceInfoFromConf(const std::vector<std::string> &pairedAddrList) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    for (auto addr : pairedAddrList) {
        RawAddress rawAddr(addr);
        if ((!INVALID_MAC_ADDRESS.compare(rawAddr.GetAddress())) || (rawAddr.GetAddress().empty())) {
            continue;
        }
        BlePeripheralDevice remote;
        remote.SetAddress(rawAddr);
        remote.SetAddressType(BleConfig::GetInstance().GetPeerAddressType(addr));

        std::string name = BleConfig::GetInstance().GetPeerName(addr);
        remote.SetName(name);

        int io = BleConfig::GetInstance().GetPeerDeviceIoCapability(addr);
        remote.SetIoCapability(io);

        remote.SetPairedStatus(BLE_PAIR_PAIRED);

        pimpl->peerConnDeviceList_.insert(std::make_pair(addr, remote));
    }
}

bool BleAdapter::SavePeerDeviceInfoToConf(const std::map<std::string, BlePeripheralDevice> &peerConnDeviceList) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool ret = false;
    for (auto it = peerConnDeviceList.begin(); it != peerConnDeviceList.end(); it++) {
        if (it->second.GetPairedStatus() != BLE_PAIR_PAIRED) {
            continue;
        }
        RawAddress rawAddr(it->second.GetRawAddress());

        int addrType = it->second.GetAddressType();
        ret = BleConfig::GetInstance().SetPeerAddressType(rawAddr.GetAddress(), addrType);

        ret &= BleConfig::GetInstance().SetPeerName(rawAddr.GetAddress(), it->second.GetName());

        int deviceType = it->second.GetDeviceType();
        ret &= BleConfig::GetInstance().SetPeerDeviceType(rawAddr.GetAddress(), deviceType);
    }

    BleConfig::GetInstance().Save();
    return ret;
}

void BleAdapter::ClearPeerDeviceInfo() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    pimpl->peerConnDeviceList_.clear();
}

void BleAdapter::ClearScanResultInfo() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        pimpl->bleCentralManager_->ClearResults();
    }
}

void BleAdapter::SavePeerDevices2BTM(const std::map<std::string, BlePeripheralDevice> &peerConnDeviceList) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    // Add pair device to BTM
    std::vector<BtmLePairedDevice> devices;
    for (auto it = peerConnDeviceList.begin(); it != peerConnDeviceList.end(); it++) {
        BtAddr btAddr;
        (void)memset_s(&btAddr, sizeof(btAddr), 0x00, sizeof(btAddr));
        RawAddress device(it->second.GetRawAddress());
        device.ConvertToUint8(btAddr.addr);

        if ((INVALID_MAC_ADDRESS.compare(device.GetAddress()) == 0) || (device.GetAddress().empty()) ||
            (BleConfig::GetInstance().GetPeerIrk(device.GetAddress()).empty()) ||
            (BleConfig::GetInstance().GetPeerIdentityAddr(device.GetAddress()).empty())) {
            continue;
        }

        BtmLePairedDevice pairedDevice;
        (void)memset_s(&pairedDevice, sizeof(pairedDevice), 0x00, sizeof(pairedDevice));
        btAddr.type = it->second.GetAddressType();
        pairedDevice.addr = btAddr;

        // Peer Identity Addr
        BtAddr peerAddr;
        (void)memset_s(&btAddr, sizeof(btAddr), 0x00, sizeof(btAddr));
        std::string addr = BleConfig::GetInstance().GetPeerIdentityAddr(device.GetAddress());
        RawAddress peerDevice(addr);
        peerAddr.type = BleConfig::GetInstance().GetPeerAddressType(device.GetAddress());
        peerDevice.ConvertToUint8(peerAddr.addr);
        pairedDevice.remoteIdentityAddress = peerAddr;

        // IRK
        std::string irk = BleConfig::GetInstance().GetPeerIrk(device.GetAddress());
        if (!irk.empty()) {
            std::vector<uint8_t> vec;
            BleUtils::ConvertHexStringToInt(irk, vec);
            if (memcpy_s(pairedDevice.remoteIdentityResolvingKey.key, KEY_SIZE, &vec[0], vec.size()) != EOK) {
                LOG_ERROR("[BleAdapter] %{public}s:SavePeerDevices2BTM memcpy_s failed!", __func__);
                return;
            }
        }
        devices.push_back(pairedDevice);
    }
    if (!devices.empty()) {
        BTM_SetLePairedDevices(&devices[0], devices.size());
        devices.clear();
    }
}

int BleAdapter::RegisterCallbackToBtm()
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    pimpl->btmAclCb_.leConnectionComplete = &BleAdapter::LeConnectionComplete;
    pimpl->btmAclCb_.leDisconnectionComplete = &BleAdapter::LeDisconnectionComplete;
    pimpl->btmAclCb_.readRssiComplete = &BleAdapter::OnReadRemoteRssiEvent;

    int ret = BTM_RegisterAclCallbacks(&pimpl->btmAclCb_, this);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:BTM_RegisterAclCallbacks failed!", __func__);
    }
    return ret;
}

int BleAdapter::DeregisterCallbackToBtm() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (!pimpl->btmEnableFlag_) {
        return BT_OPERATION_FAILED;
    }

    int ret = BTM_DeregisterAclCallbacks(&pimpl->btmAclCb_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleAdapter] %{public}s:DeregisterCallbackToBtm failed!", __func__);
    }
    return ret;
}

void BleAdapter::LeConnectionComplete(
    uint8_t status, uint16_t connectionHandle, const BtAddr *addr, uint8_t role, void *context)
{
    HILOGI("status: %{public}u", status);

    if (status != BT_NO_ERROR) {
        HILOGI("status: %{public}u", status);
        return;
    }

    auto *adapter = static_cast<BleAdapter *>(context);
    BtAddr address;
    (void)memcpy_s(&address, sizeof(BtAddr), addr, sizeof(BtAddr));
    adapter->GetDispatcher()->PostTask(
        std::bind(&BleAdapter::LeConnectionCompleteTask, adapter, status, connectionHandle, address, role));
}

void BleAdapter::LeConnectionCompleteTask(
    uint8_t status, uint16_t connectionHandle, const BtAddr &addr, uint8_t role) const
{
    LOG_DEBUG("[BleAdapter] %{public}s, handle is %{public}d", __func__, connectionHandle);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if ((pimpl->bleAdvertiser_ != nullptr) && (!BleFeature::GetInstance().IsLeExtendedAdvertisingSupported()) &&
        (role == LE_CONNECTION_ROLE_SLAVE)) {
        pimpl->bleAdvertiser_->ReStartLegacyAdvertising();
    }

    RawAddress peerAddr = RawAddress::ConvertToString(addr.addr);
    auto it = pimpl->peerConnDeviceList_.find(peerAddr.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        it->second.SetConnectionHandle(connectionHandle);
        it->second.SetRoles(role);
        it->second.SetAddressType(addr.type);
        if (pimpl->bleCentralManager_ != nullptr) {
            it->second.SetName(pimpl->bleCentralManager_->GetDeviceName(peerAddr.GetAddress()));
        }
        it->second.SetAclConnectState(BLE_CONNECTION_STATE_CONNECTED);

        if ((it->second.GetPairedStatus() == BLE_PAIR_PAIRED) && (pimpl->bleSecurity_ != nullptr)) {
            pimpl->bleSecurity_->GapLeRequestSecurity(connectionHandle, addr, role);
        }
    } else {
        BlePeripheralDevice peerDevice;
        peerDevice.SetAddress(peerAddr);
        peerDevice.SetAddressType(addr.type);
        if (pimpl->bleCentralManager_ != nullptr) {
            peerDevice.SetName(pimpl->bleCentralManager_->GetDeviceName(peerAddr.GetAddress()));
        }
        peerDevice.SetRoles(role);
        peerDevice.SetConnectionHandle(connectionHandle);
        peerDevice.SetAclConnectState(BLE_CONNECTION_STATE_CONNECTED);
        pimpl->peerConnDeviceList_.insert(std::make_pair(peerAddr.GetAddress(), peerDevice));
    }
}

void BleAdapter::LeDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (status != BT_NO_ERROR) {
        LOG_DEBUG("[BleAdapter] %{public}s:%u", __func__, status);
        return;
    }

    auto *adapter = static_cast<BleAdapter *>(context);
    adapter->GetDispatcher()->PostTask(
        std::bind(&BleAdapter::LeDisconnectionCompleteTask, adapter, status, connectionHandle, reason));
}

void BleAdapter::LeDisconnectionCompleteTask(uint8_t status, uint16_t connectionHandle, uint8_t reason) const
{
    LOG_DEBUG("[BleAdapter] %{public}s, handle is %{public}d", __func__, connectionHandle);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    for (auto it = pimpl->peerConnDeviceList_.begin(); it != pimpl->peerConnDeviceList_.end(); it++) {
        if (connectionHandle == it->second.GetConnectionHandle()) {
            LOG_DEBUG("[BleAdapter] handle is %{public}d disconnect ", connectionHandle);
            it->second.SetAclConnectState(BLE_CONNECTION_STATE_DISCONNECTED);
            break;
        }
    }
}

void BleAdapter::LePairComplete(const RawAddress &device, const int status) const
{
    LOG_DEBUG("[BleAdapter] %{public}s:result %{public}d.", __func__, status);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (status == BT_NO_ERROR) {
        auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
        if (it == pimpl->peerConnDeviceList_.end()) {
            HILOGI("addr %{public}s.", GetEncryptAddr(device.GetAddress()).c_str());
            return;
        }
        it->second.SetPairedStatus(BLE_PAIR_PAIRED);

        /// Peer Identity Addr
        BtAddr btAddr;
        (void)memset_s(&btAddr, sizeof(btAddr), 0x00, sizeof(btAddr));
        device.ConvertToUint8(btAddr.addr);
        btAddr.type = it->second.GetAddressType();

        RawAddress peerDevice(BleConfig::GetInstance().GetPeerIdentityAddr(device.GetAddress()));
        BtAddr peerAddr;
        (void)memset_s(&peerAddr, sizeof(peerAddr), 0x00, sizeof(peerAddr));
        peerAddr.type = BleConfig::GetInstance().GetPeerDeviceType(device.GetAddress());
        peerDevice.ConvertToUint8(peerAddr.addr);

        BtmLePairedDevice pairedDevice;
        (void)memset_s(&pairedDevice, sizeof(pairedDevice), 0x00, sizeof(pairedDevice));
        (void)memcpy_s(&pairedDevice.addr, sizeof(BtAddr), &btAddr, sizeof(BtAddr));
        (void)memcpy_s(&pairedDevice.remoteIdentityAddress, sizeof(BtAddr), &peerAddr, sizeof(BtAddr));

        /// IRK
        std::string irk = BleConfig::GetInstance().GetPeerIrk(device.GetAddress());
        if (!irk.empty()) {
            std::vector<uint8_t> vec;
            BleUtils::ConvertHexStringToInt(irk, vec);
            /// Add paired device to btm
            (void)memcpy_s(pairedDevice.remoteIdentityResolvingKey.key, KEY_SIZE, &vec[0], vec.size());
        }
        StartOrStopAdvAndScan(STOP_ADV_TYPE_RESOLVING_LIST, STOP_SCAN_TYPE_RESOLVING_LIST);
        BTM_AddLePairedDevice(&pairedDevice);
        StartOrStopAdvAndScan(STOP_ADV_TYPE_RESOLVING_LIST, STOP_SCAN_TYPE_RESOLVING_LIST, true);

        if (pimpl->blePeripheralCallback_ != nullptr) {
            pimpl->blePeripheralCallback_->ForEach([device](IBlePeripheralCallback &observer) {
                observer.OnPairStatusChanged(ADAPTER_BLE, device, BLE_PAIR_PAIRED);
            });
        }
    } else {
        auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
        if (it != pimpl->peerConnDeviceList_.end()) {
            it->second.SetPairedStatus(BLE_PAIR_NONE);
        }
        if (pimpl->blePeripheralCallback_ != nullptr) {
            pimpl->blePeripheralCallback_->ForEach([device](IBlePeripheralCallback &observer) {
                observer.OnPairStatusChanged(ADAPTER_BLE, device, BLE_PAIR_NONE);
            });
        }
    }
}

void BleAdapter::LePairingStatus(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        it->second.SetPairedStatus(BLE_PAIR_PAIRING);
    } else {
        LOG_DEBUG("[BleAdapter] %{public}s", __func__);
        BlePeripheralDevice peerDevice;
        peerDevice.SetAddress(device);
        peerDevice.SetPairedStatus(BLE_PAIR_PAIRING);
        pimpl->peerConnDeviceList_.insert(std::make_pair(device.GetAddress(), peerDevice));
    }

    if (pimpl->blePeripheralCallback_ != nullptr) {
        pimpl->blePeripheralCallback_->ForEach([device](IBlePeripheralCallback &observer) {
            observer.OnPairStatusChanged(ADAPTER_BLE, device, BLE_PAIR_PAIRING);
        });
    }
}

void BleAdapter::EncryptionComplete(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = pimpl->peerConnDeviceList_.find(device.GetAddress());
    if (it != pimpl->peerConnDeviceList_.end()) {
        it->second.SetAclConnectState(BLE_CONNECTION_STATE_ENCRYPTED_LE);
    }
}

void BleAdapter::OnReadRemoteRssiEvent(uint8_t status, const BtAddr *addr, int8_t rssi, void *context)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    auto *adapter = static_cast<BleAdapter *>(context);
    BtAddr address;
    (void)memcpy_s(&address, sizeof(BtAddr), addr, sizeof(BtAddr));
    adapter->GetDispatcher()->PostTask(
        std::bind(&BleAdapter::OnReadRemoteRssiEventTask, adapter, status, address, rssi));
}

void BleAdapter::OnReadRemoteRssiEventTask(uint8_t status, const BtAddr &addr, int8_t rssi) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    RawAddress device = RawAddress::ConvertToString(addr.addr);
    if (pimpl->blePeripheralCallback_ != nullptr) {
        pimpl->blePeripheralCallback_->ForEach([device, rssi, status](IBlePeripheralCallback &observer) {
            observer.OnReadRemoteRssiEvent(device, rssi, status);
        });
    }
}

bool BleAdapter::ReadRemoteRssiValue(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    BtAddr addr;
    (void)memset_s(&addr, sizeof(addr), 0x00, sizeof(addr));
    addr.type = GetPeerDeviceAddrType(device);
    device.ConvertToUint8(addr.addr);
    return (BTM_ReadRssi(&addr) == BT_NO_ERROR);
}

int BleAdapter::GetDeviceType(const RawAddress &device) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        return pimpl->bleCentralManager_->GetDeviceType(device.GetAddress());
    }
    return BLE_BT_DEVICE_TYPE_UNKNOWN;
}

uint8_t BleAdapter::GetAdvertiserHandle() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        return pimpl->bleAdvertiser_->CreateAdvertiserSetHandle();
    }
    return BLE_INVALID_ADVERTISING_HANDLE;
}

void BleAdapter::StartAdvertising(const BleAdvertiserSettingsImpl &settings, const BleAdvertiserDataImpl &advData,
    const BleAdvertiserDataImpl &scanResponse, uint8_t advHandle) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        pimpl->bleAdvertiser_->StartAdvertising(settings, advData, scanResponse, advHandle);
    }
}

void BleAdapter::StopAdvertising(uint8_t advHandle) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        pimpl->bleAdvertiser_->StopAdvertising(advHandle);
    }
}

void BleAdapter::Close(uint8_t advHandle) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        pimpl->bleAdvertiser_->Close(advHandle);
    }
}

void BleAdapter::StartScan() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        pimpl->bleCentralManager_->StartScan();
    }
}

void BleAdapter::StartScan(const BleScanSettingsImpl &setting) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        pimpl->bleCentralManager_->StartScan(setting);
    }
}

void BleAdapter::StopScan() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        pimpl->bleCentralManager_->StopScan();
    }
}

int BleAdapter::ConfigScanFilter(const int clientId, const std::vector<BleScanFilterImpl> &filters)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        return pimpl->bleCentralManager_->ConfigScanFilter(clientId, filters);
    }
    return 0;
}

void BleAdapter::RemoveScanFilter(const int clientId)
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleCentralManager_ != nullptr) {
        pimpl->bleCentralManager_->RemoveScanFilter(clientId);
    }
}

void BleAdapter::OnStartAdvertisingEvt() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->observer_ != nullptr) {
        int status = BLE_ADV_STATE_ADVERTISING;
        pimpl->observer_->ForEach(
            [status](IAdapterBleObserver &observer) { observer.OnAdvertisingStateChanged(status); });
    }
}

void BleAdapter::OnStopAdvertisingEvt() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    if (pimpl->observer_ != nullptr) {
        int status = BLE_ADV_STATE_IDLE;
        pimpl->observer_->ForEach(
            [status](IAdapterBleObserver &observer) { observer.OnAdvertisingStateChanged(status); });
    }
}

int BleAdapter::GetAdvertisingStatus() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        if (pimpl->bleAdvertiser_->GetAdvertisingStatus() == ADVERTISE_FAILED_ALREADY_STARTED) {
            return BLE_ADV_STATE_ADVERTISING;
        } else {
            return BLE_ADV_STATE_IDLE;
        }
    }
    return BLE_ADV_STATE_IDLE;
}

bool BleAdapter::IsLlPrivacySupported() const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    return BleFeature::GetInstance().IsPrivacySupported();
}

void BleAdapter::AddCharacteristicValue(uint8_t adtype, const std::string &data) const
{
    LOG_DEBUG("[BleAdapter] %{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (pimpl->bleAdvertiser_ != nullptr) {
        pimpl->bleAdvertiser_->AddCharacteristicValue(adtype, data);
    }
}

int BleAdapter::SetBleRoles() const
{
    int roles = BleProperties::GetInstance().GetBleRoles();

    LOG_DEBUG("[BleAdapter] %{public}s:%{public}d", __func__, roles);

    if (roles > (GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_OBSERVER | GAP_LE_ROLE_PREIPHERAL | GAP_LE_ROLE_CENTRAL) ||
        roles < GAP_LE_ROLE_BROADCASTER) {
        LOG_ERROR("[BleAdapter] %{public}s:Roles is invalid.", __func__);
        roles = (GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_OBSERVER | GAP_LE_ROLE_PREIPHERAL | GAP_LE_ROLE_CENTRAL);
    }
    return GAPIF_LeSetRole(roles);
}

REGISTER_CLASS_CREATOR(BleAdapter);
}  // namespace bluetooth
}  // namespace OHOS
