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

#include "gatt_connection_manager.h"
#include "adapter_config.h"
#include "att.h"
#include "btm.h"
#include "gap_if.h"
#include "gap_le_if.h"
#include "gatt_defines.h"
#include "gatt_service_base.h"
#include "hisysevent.h"
#include "interface_adapter_ble.h"
#include "interface_adapter_manager.h"
#include "log.h"
#include "log_util.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
constexpr uint8_t OBSERVER_EVENT_CONNECTED = 0x0;
constexpr uint8_t OBSERVER_EVENT_DISCONNECTED = 0x1;
constexpr uint8_t OBSERVER_EVENT_CONNECTING = 0x2;
constexpr uint8_t OBSERVER_EVENT_DISCONNECTING = 0x3;
constexpr uint8_t OBSERVER_EVENT_RECONNECTED = 0x04;
constexpr uint8_t OBSERVER_EVENT_DISCONNECTED_INTER = 0x05;

constexpr uint8_t MAX_RETRY_CONNECT_TIMES = 0;
constexpr uint8_t HCI_CONNECTION_FAILED_TO_BE_ESTABLISHED = 0x3E;

const std::string GattConnectionManager::Device::STATE_IDLE = "IDLE";
const std::string GattConnectionManager::Device::STATE_CONNECTING = "CONNECTING";
const std::string GattConnectionManager::Device::STATE_CONNECTED = "CONNECTED";
const std::string GattConnectionManager::Device::STATE_DISCONNECTING = "DISCONNECTING";
const std::string GattConnectionManager::Device::STATE_DISCONNECTED = "DISCONNECTED";

using DeviceIterator = std::list<std::unique_ptr<GattConnectionManager::Device>>::iterator;

struct GattConnectionManager::impl : public GattServiceBase {
    utility::Dispatcher *dispatcher_ = nullptr;
    std::array<std::pair<bool, GattConnectionObserver *>, MAX_OBSERVER_NUM> observers_
        = {std::make_pair(false, nullptr)};
    std::list<std::unique_ptr<GattConnectionManager::Device>> devices_ = {};
    std::mutex registerMutex_ = {};
    std::mutex devicelistRWMutex_ = {};
    struct {
        uint16_t connIntervalMin_;
        uint16_t connIntervalMax_;
        uint16_t connLatency_;
        uint16_t supervisionTimeout_;
        uint16_t mtu_;
        uint16_t flushTimeout_;
        uint8_t mode_;
        uint8_t securityMode_;
        uint16_t bleMaxConnections_;
        uint16_t classicMaxConnections_;
    } connectionParameter_ = {};

    impl();

    GattConnectionManager::Device *FindOrRegister(
        const GattDevice &device, bool autoConnect, std::unique_lock<std::mutex> &deviceLock);
    GattConnectionManager::Device *FindOrRegister(
        const BtAddr &addr, uint8_t transport, std::unique_lock<std::mutex> &deviceLock);
    bool HasSpareDeviceSpace(uint8_t transport);
    GattConnectionManager::Device *FindDevice(
        const BtAddr &addr, uint8_t transport, std::unique_lock<std::mutex> &deviceLock);
    GattConnectionManager::Device *FindDevice(const GattDevice &device, std::unique_lock<std::mutex> &deviceLock);
    GattConnectionManager::Device *FindDevice(uint16_t handle, std::unique_lock<std::mutex> &deviceLock);
    void ClearDeviceList();
    void RemoveDevice(const GattDevice &device);
    void DisconnectAllDevice();
    void NotifyObserver(const GattDevice &device, uint8_t event, uint16_t connectionHandle, int ret);
    int DoConnect(GattConnectionManager::Device &device);
    int DoConnectComplete(GattConnectionManager::Device &device, const utility::Message &msg);
    static void DoDisconnect(uint16_t handle);
    static int DoDisconnectComplete(GattConnectionManager::Device &device, const utility::Message &msg);
    int DoRequestConnectionPriority(GattConnectionManager::Device &device, int connPriority);
    void NotifyPriorityUpdated(
        const GattDevice &device, uint16_t connInterval, uint16_t connLatency, uint16_t timeout, int status);
    static void ConnectionInComming(const BtAddr &addr, uint8_t transport, uint16_t connectHandle, void *data);
    static int CheckDeviceParameter(const GattDevice &device);
    static int ConvertTransport(int transport);
    static int ConvertConnectionState(const std::string &state);
    void DoShutDown();
    static uint16_t GetBleMaxConnectedDevices();
    static uint16_t GetBleMinConnectionInterval(int connPriority);
    static uint16_t GetBleMaxConnectionInterval(int connPriority);
    static uint16_t GetBleConnectionLatency(int connPriority);
    static uint16_t GetBleConnectionSupervisionTimeout(int connPriority);
    static uint16_t GetClassicMaxConnectedDevices();
    static uint16_t GetClassicConnectionMtu();
    uint8_t GetClassicConnectionMode();
    uint16_t GetClassicConnectionFlushTimeout();
    uint8_t GetClassicConnectionSecurityMode();

    static void LEConnectCompleted(uint16_t connectHandle, AttLeConnectCallback *data, void *context);
    static void LEDisconnectCompleted(uint16_t connectHandle, AttLeDisconnectCallback *data, void *context);
    static void BREDRConnectCompleted(uint16_t connectHandle, AttBredrConnectCallback *data, void *context);
    static void BREDRDisconnectCompleted(uint16_t connectHandle, AttBredrDisconnectCallback *data, void *context);
    static void BREDRConnectInd(uint16_t connectHandle, void *context);

    static void LEConnectionParamterReq(const BtAddr *addr, uint16_t connIntervalMin, uint16_t connIntervalMax,
        uint16_t connLatency, uint16_t timeout, void *context);
    static void LEConnectionUpdateComplete(uint8_t status, const BtAddr *addr, uint16_t connInterval,
        uint16_t connLatency, uint16_t timeout, void *context);

    void LEConnectionParamterReqImpl(
        BtAddr addr, uint16_t connIntervalMin, uint16_t connIntervalMax, uint16_t connLatency, uint16_t timeout);
    void LEConnectionUpdateCompleteImpl(
        uint8_t status, BtAddr addr, uint16_t connInterval, uint16_t connLatency, uint16_t timeout);

    void LEConnectCompletedImpl(uint16_t connectHandle, AttLeConnectCallback data);
    void LEDisconnectCompletedImpl(uint16_t connectHandle, AttLeDisconnectCallback data);
    void BREDRConnectCompletedImpl(uint16_t connectHandle, AttBredrConnectCallback data);
    void BREDRDisconnectCompletedImpl(uint16_t connectHandle, AttBredrDisconnectCallback data);
    static AttConnectCallback BuildAttConnectCallback();

    static void ChangeConnectionMode(bool isDirect);
    void DirectConnectTimeout(const GattDevice &device);

    BT_DISALLOW_COPY_AND_ASSIGN(impl);
};

GattConnectionManager::GattConnectionManager() : pimpl(std::make_unique<impl>())
{}

GattConnectionManager::~GattConnectionManager()
{
    pimpl->ClearDeviceList();
}

int GattConnectionManager::Connect(const GattDevice &device, bool autoConnect) const
{
    int result = pimpl->CheckDeviceParameter(device);
    if (GattStatus::GATT_SUCCESS == result) {
        {
            std::unique_lock<std::mutex> devLock;
            auto internalDevice = pimpl->FindOrRegister(device, autoConnect, devLock);
            if (internalDevice != nullptr) {
                if (internalDevice->ProcessMessage(Device::StateMachine::MSG_CONNECT)) {
                    result = GattStatus::GATT_SUCCESS;
                } else {
                    result = GattStatus::INTERNAL_ERROR;
                }
            } else {
                result = GattStatus::MAX_CONNECTIONS;
            }
        }
    }

    if (GattStatus::INTERNAL_ERROR == result) {
        pimpl->RemoveDevice(device);
    }

    return result;
}

int GattConnectionManager::Disconnect(const GattDevice &device) const
{
    LOG_DEBUG("%{public}s:%{public}s:%{public}d Entry!", __FILE__, __FUNCTION__, __LINE__);
    int ret = GattStatus::INVALID_PARAMETER;
    std::unique_lock<std::mutex> devLock;
    auto internalDevice = pimpl->FindDevice(device, devLock);
    if (internalDevice != nullptr) {
        internalDevice->AutoConnect() = false;
        if (internalDevice->ProcessMessage(Device::StateMachine::MSG_DISCONNECT)) {
            ret = GattStatus::GATT_SUCCESS;
        } else {
            ret = GattStatus::INTERNAL_ERROR;
        }
    }
    return ret;
}

int GattConnectionManager::RegisterObserver(GattConnectionObserver &observer) const
{
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    int id = -1;
    for (int i = 0; i < MAX_OBSERVER_NUM; i++) {
        if (!pimpl->observers_[i].first) {
            pimpl->observers_[i].first = true;
            pimpl->observers_[i].second = &observer;
            id = i;
            break;
        }
    }
    return id;
}

void GattConnectionManager::DeregisterObserver(int registerId) const
{
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (registerId >= 0 && registerId < MAX_OBSERVER_NUM) {
        pimpl->observers_[registerId] = std::make_pair(false, nullptr);
    }
}

const std::string &GattConnectionManager::GetDeviceState(const GattDevice &device) const
{
    std::unique_lock<std::mutex> devLock;
    auto internalDevice = pimpl->FindDevice(device, devLock);
    if (internalDevice != nullptr) {
        return internalDevice->SM().GetState()->Name();
    }
    return Device::STATE_IDLE;
}

void GattConnectionManager::GetDevices(std::vector<GattDevice> &devices) const
{
    std::lock_guard<std::mutex> lck(pimpl->devicelistRWMutex_);
    std::transform(
        pimpl->devices_.begin(), pimpl->devices_.end(), std::back_inserter(devices), [&](auto &item) -> GattDevice {
            return GattDevice(item->Info().addr_,
                item->Info().addressType_,
                item->Info().transport_,
                pimpl->ConvertConnectionState(item->SM().GetState()->Name()));
        });
}

std::pair<uint16_t, uint16_t> GattConnectionManager::GetMaximumNumberOfConnections() const
{
    return std::pair<uint16_t, uint16_t>(
        pimpl->connectionParameter_.classicMaxConnections_, pimpl->connectionParameter_.bleMaxConnections_);
}

std::tuple<std::string, uint16_t, uint16_t> GattConnectionManager::GetDeviceInformation(const GattDevice &device) const
{
    std::unique_lock<std::mutex> devLock;
    auto internalDevice = pimpl->FindDevice(device, devLock);
    if (internalDevice != nullptr) {
        return std::make_tuple(
            internalDevice->SM().GetState()->Name(), internalDevice->Handle(), internalDevice->MTU());
    }

    return std::make_tuple(Device::STATE_IDLE, 0, 0);
}

uint8_t GattConnectionManager::GetDeviceTransport(uint16_t handle) const
{
    std::unique_lock<std::mutex> devLock;
    auto internalDevice = pimpl->FindDevice(handle, devLock);
    if (internalDevice != nullptr) {
        return internalDevice->Info().transport_;
    }

    return GATT_TRANSPORT_TYPE_AUTO;
}

int GattConnectionManager::RequestConnectionPriority(uint16_t handle, int connPriority) const
{
    std::unique_lock<std::mutex> devLock;
    auto internalDevice = pimpl->FindDevice(handle, devLock);
    if (internalDevice != nullptr) {
        return (
            (internalDevice->Info().transport_ != GATT_TRANSPORT_TYPE_LE)
                ? GattStatus::INVALID_PARAMETER
                : (internalDevice->ProcessMessage(Device::StateMachine::MSG_REQUEST_CONNECTION_PRIORITY, connPriority)
                          ? GattStatus::GATT_SUCCESS
                          : GattStatus::GATT_FAILURE));
    }

    return GattStatus::REQUEST_NOT_SUPPORT;
}

bool GattConnectionManager::GetEncryptionInfo(uint16_t connectHandle) const
{
    std::unique_lock<std::mutex> devLock;
    auto device = pimpl->FindDevice(connectHandle, devLock);
    if (device != nullptr) {
        device->CheckEncryption();
        return device->Info().isEncryption_;
    }

    return false;
}

bool GattConnectionManager::GetEncryptionInfo(const GattDevice &device) const
{
    std::unique_lock<std::mutex> devLock;
    auto dev = pimpl->FindDevice(device, devLock);
    if (dev != nullptr) {
        dev->CheckEncryption();
        return dev->Info().isEncryption_;
    }

    return false;
}

int GattConnectionManager::SetConnectionType(const GattDevice &device, bool autoConnect) const
{
    std::unique_lock<std::mutex> devLock;
    auto dev = pimpl->FindDevice(device, devLock);
    if (dev != nullptr) {
        dev->AutoConnect() = autoConnect;
        return GattStatus::GATT_SUCCESS;
    }

    return GattStatus::GATT_FAILURE;
}

int GattConnectionManager::StartUp(utility::Dispatcher &dispatcher)
{
    if (pimpl->InRunningState()) {
        return GattStatus::GATT_SUCCESS;
    }

    pimpl->dispatcher_ = &dispatcher;

    ATT_ConnectRegister(pimpl->BuildAttConnectCallback(), this);

    GapServiceSecurityInfo clientServiceInfo = {OUTGOING, GATT_CLIENT, SEC_PROTOCOL_L2CAP, {BT_PSM_GATT}};
    GapServiceSecurityInfo serverServiceInfo = {INCOMING, GATT_SERVER, SEC_PROTOCOL_L2CAP, {BT_PSM_GATT}};

    if (GAPIF_RegisterServiceSecurity(nullptr, &clientServiceInfo, GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION)
        != BT_NO_ERROR) {
        LOG_WARN("%{public}s:%{public}d:%{public}s GAPIF_RegisterServiceSecurity  client failed!", __FILE__, __LINE__, __FUNCTION__);
    }

    if (GAPIF_RegisterServiceSecurity(nullptr, &serverServiceInfo, GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION)
        != BT_NO_ERROR) {
        LOG_WARN("%{public}s:%{public}d:%{public}s GAPIF_RegisterServiceSecurity  server failed!", __FILE__, __LINE__, __FUNCTION__);
    }

    GapLeConnCallback callback = {impl::LEConnectionParamterReq, impl::LEConnectionUpdateComplete, nullptr, nullptr};
    if (GAPIF_RegisterLeConnCallback(&callback, nullptr) != BT_NO_ERROR) {
        LOG_WARN("%{public}s:%{public}d:%{public}s GAPIF_RegisterLeConnCallback failed!", __FILE__, __LINE__, __FUNCTION__);
    }

    pimpl->Start();

    return GattStatus::GATT_SUCCESS;
}

int GattConnectionManager::ShutDown() const
{
    LOG_DEBUG("%{public}s:%{public}s:%{public}d Entry!", __FILE__, __FUNCTION__, __LINE__);

    if (!pimpl->InRunningState()) {
        return GattStatus::GATT_SUCCESS;
    }

    GapServiceSecurityInfo clientServiceInfo = {OUTGOING, GATT_CLIENT, SEC_PROTOCOL_L2CAP, {BT_PSM_GATT}};
    GapServiceSecurityInfo serverServiceInfo = {INCOMING, GATT_SERVER, SEC_PROTOCOL_L2CAP, {BT_PSM_GATT}};

    GAPIF_DeregisterServiceSecurity(nullptr, &clientServiceInfo);
    GAPIF_DeregisterServiceSecurity(nullptr, &serverServiceInfo);

    GAPIF_DeregisterLeConnCallback();

    pimpl->Stop();

    pimpl->DisconnectAllDevice();
    pimpl->DoShutDown();

    LOG_DEBUG("%{public}s:%{public}s:%{public}d Exit!", __FILE__, __FUNCTION__, __LINE__);

    return GattStatus::GATT_SUCCESS;
}

GattConnectionManager::impl::impl()
{
    connectionParameter_.connIntervalMin_ =
        GetBleMinConnectionInterval(static_cast<int>(GattConnectionPriority::BALANCED));
    connectionParameter_.connIntervalMax_ =
        GetBleMaxConnectionInterval(static_cast<int>(GattConnectionPriority::BALANCED));
    connectionParameter_.connLatency_ = GetBleConnectionLatency(static_cast<int>(GattConnectionPriority::BALANCED));
    connectionParameter_.supervisionTimeout_ =
        GetBleConnectionSupervisionTimeout(static_cast<int>(GattConnectionPriority::BALANCED));
    connectionParameter_.mtu_ = GetClassicConnectionMtu();
    connectionParameter_.flushTimeout_ = GetClassicConnectionFlushTimeout();
    connectionParameter_.mode_ = GetClassicConnectionMode();
    connectionParameter_.securityMode_ = GetClassicConnectionSecurityMode();
    connectionParameter_.bleMaxConnections_ = GetBleMaxConnectedDevices();
    connectionParameter_.classicMaxConnections_ = GetClassicMaxConnectedDevices();
}

GattConnectionManager::Device *GattConnectionManager::impl::FindOrRegister(
    const GattDevice &device, bool autoConnect, std::unique_lock<std::mutex> &deviceLock)
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);

    for (auto &dev : devices_) {
        if (dev->Info() == device) {
            deviceLock = std::unique_lock<std::mutex>(dev->DeviceRWMutex());
            dev->AutoConnect() = autoConnect;
            return dev.get();
        }
    }

    if (HasSpareDeviceSpace(device.transport_)) {
        auto &dev = devices_.emplace_back(std::make_unique<GattConnectionManager::Device>(device, autoConnect));
        deviceLock = std::unique_lock<std::mutex>(dev->DeviceRWMutex());
        return dev.get();
    }

    LOG_INFO("%{public}s: The maximum number of device connections has been reached", __FUNCTION__);
    return nullptr;
}

GattConnectionManager::Device *GattConnectionManager::impl::FindOrRegister(
    const BtAddr &addr, uint8_t transport, std::unique_lock<std::mutex> &deviceLock)
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);

    for (auto &dev : devices_) {
        if (dev->Info().transport_ == transport &&
            memcmp(addr.addr, dev->Addr(), RawAddress::BT_ADDRESS_BYTE_LEN) == 0) {
            deviceLock = std::unique_lock<std::mutex>(dev->DeviceRWMutex());
            return dev.get();
        }
    }

    if (HasSpareDeviceSpace(transport)) {
        auto &device =
            devices_.emplace_back(std::make_unique<GattConnectionManager::Device>(addr.addr, transport, addr.type));
        deviceLock = std::unique_lock<std::mutex>(device->DeviceRWMutex());
        return device.get();
    }

    LOG_INFO("%{public}s: The maximum number of device connections has been reached", __FUNCTION__);
    return nullptr;
}

bool GattConnectionManager::impl::HasSpareDeviceSpace(uint8_t transport)
{
    size_t count = 0;
    for (auto &device : devices_) {
        if (device->Info().transport_ == transport) {
            count++;
        }
    }
    return ((transport == GATT_TRANSPORT_TYPE_CLASSIC) ? (count < connectionParameter_.classicMaxConnections_)
                                                    : (count < connectionParameter_.bleMaxConnections_));
}

GattConnectionManager::Device *GattConnectionManager::impl::FindDevice(
    const BtAddr &addr, uint8_t transport, std::unique_lock<std::mutex> &deviceLock)
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);

    for (auto &dev : devices_) {
        if (dev->Info().transport_ == transport &&
            memcmp(addr.addr, dev->Addr(), RawAddress::BT_ADDRESS_BYTE_LEN) == 0) {
            deviceLock = std::unique_lock<std::mutex>(dev->DeviceRWMutex());
            return dev.get();
        }
    }

    return nullptr;
}

GattConnectionManager::Device *GattConnectionManager::impl::FindDevice(
    const GattDevice &device, std::unique_lock<std::mutex> &deviceLock)
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);
    for (auto &dev : devices_) {
        if (dev->Info() == device) {
            deviceLock = std::unique_lock<std::mutex>(dev->DeviceRWMutex());
            return dev.get();
        }
    }
    return nullptr;
}

GattConnectionManager::Device *GattConnectionManager::impl::FindDevice(
    uint16_t handle, std::unique_lock<std::mutex> &deviceLock)
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);
    for (auto &device : devices_) {
        if (device->Handle() == handle) {
            deviceLock = std::unique_lock<std::mutex>(device->DeviceRWMutex());
            return device.get();
        }
    }
    return nullptr;
}

void GattConnectionManager::impl::RemoveDevice(const GattDevice &device)
{
    LOG_INFO("%{public}s: enter", __FUNCTION__);
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);
    for (auto dev = devices_.begin(); dev != devices_.end(); ++dev) {
        if ((*dev)->Info() == device) {
            devices_.erase(dev);
            LOG_INFO("%{public}s: device is found and removed", __FUNCTION__);
            return;
        }
    }
    LOG_INFO("%{public}s: can not find device", __FUNCTION__);
}

void GattConnectionManager::impl::ClearDeviceList()
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);
    devices_.clear();
}

void GattConnectionManager::impl::DisconnectAllDevice()
{
    std::lock_guard<std::mutex> lck(devicelistRWMutex_);
    for (auto &device : devices_) {
        std::lock_guard<std::mutex> lock(device->DeviceRWMutex());
        device->AutoConnect() = false;
        device->Info().role_ = GATT_ROLE_MASTER;
        device->ProcessMessage(Device::StateMachine::MSG_DISCONNECT);
    }
}

void GattConnectionManager::impl::NotifyObserver(
    const GattDevice &device, uint8_t event, uint16_t connectionHandle, int ret)
{
    std::lock_guard<std::mutex> lck(registerMutex_);
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_CONNECT_STATE",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "ADDRESS", device.addr_.GetAddress(), "STATE",
        event, "ROLE", device.role_);
    for (auto &item : observers_) {
        if (item.first) {
            switch (event) {
                case OBSERVER_EVENT_CONNECTED:
                    item.second->OnConnect(device, connectionHandle, ret);
                    break;
                case OBSERVER_EVENT_DISCONNECTED:
                    item.second->OnDisconnect(device, connectionHandle, ret);
                    break;
                case OBSERVER_EVENT_CONNECTING:
                    item.second->OnConnectionChanged(device,
                        connectionHandle, static_cast<int>(BTConnectState::CONNECTING));
                    break;
                case OBSERVER_EVENT_DISCONNECTING:
                    item.second->OnConnectionChanged(device,
                        connectionHandle, static_cast<int>(BTConnectState::DISCONNECTING));
                    break;
                case OBSERVER_EVENT_RECONNECTED:
                    item.second->OnConnectionChanged(device,
                        connectionHandle, static_cast<int>(BTConnectState::CONNECTED));
                    item.second->OnReconnect(device,
                        connectionHandle, static_cast<int>(BTConnectState::CONNECTED));
                    break;
                case OBSERVER_EVENT_DISCONNECTED_INTER:
                    item.second->OnDisconnectInter(device,
                        connectionHandle, static_cast<int>(BTConnectState::CONNECTED));
                    break;
                default:
                    break;
            }
        }
    }
}

void GattConnectionManager::impl::LEConnectCompletedImpl(uint16_t connectHandle, AttLeConnectCallback data)
{
    ConnectionInComming(data.addr, GATT_TRANSPORT_TYPE_LE, connectHandle, &data);
}

void GattConnectionManager::impl::LEConnectCompleted(uint16_t connectHandle, AttLeConnectCallback *data, void *context)
{
    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(
        &impl::LEConnectCompletedImpl, GattConnectionManager::GetInstance().pimpl.get(), connectHandle, *data));

    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(
        std::bind(&impl::DoShutDown, GattConnectionManager::GetInstance().pimpl.get()));
}

void GattConnectionManager::impl::LEDisconnectCompletedImpl(uint16_t connectHandle, AttLeDisconnectCallback data)
{
    bool ret = false;
    GattConnectionManager::Device *device = nullptr;
    {
        std::unique_lock<std::mutex> devLock;
        device = FindDevice(connectHandle, devLock);
        if (device != nullptr) {
            LOG_DEBUG("%{public}s: disconnect reason:%{public}d", __FUNCTION__, data.reason);
            if (data.reason == HCI_CONNECTION_FAILED_TO_BE_ESTABLISHED &&
                device->Role() == 0 && device->RetryTimes() < MAX_RETRY_CONNECT_TIMES) {
                device->RetryTimes() ++;
                ret = device->ProcessMessage(Device::StateMachine::MSG_RECONNECT_CAUSE_0X3E, connectHandle, &data);
            } else {
                device->RetryTimes() = 0;
                ret = device->ProcessMessage(Device::StateMachine::MSG_DISCONNECT_COMPLETE, connectHandle, &data);
            }
        } else {
            LOG_ERROR("%{public}s: Call - FindDevice - Fail!", __FUNCTION__);
        }
    }

    if (ret && device->RetryTimes() == 0 && !device->AutoConnect()) {
        GattConnectionManager::GetInstance().pimpl->RemoveDevice(device->Info());
    }
}

void GattConnectionManager::impl::LEDisconnectCompleted(
    uint16_t connectHandle, AttLeDisconnectCallback *data, void *context)
{
    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(
        &impl::LEDisconnectCompletedImpl, GattConnectionManager::GetInstance().pimpl.get(), connectHandle, *data));

    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(
        std::bind(&impl::DoShutDown, GattConnectionManager::GetInstance().pimpl.get()));
}

void GattConnectionManager::impl::BREDRConnectCompletedImpl(uint16_t connectHandle, AttBredrConnectCallback data)
{
    ConnectionInComming(data.addr, GATT_TRANSPORT_TYPE_CLASSIC, connectHandle, &data);
}

void GattConnectionManager::impl::BREDRConnectCompleted(
    uint16_t connectHandle, AttBredrConnectCallback *data, void *context)
{
    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(
        &impl::BREDRConnectCompletedImpl, GattConnectionManager::GetInstance().pimpl.get(), connectHandle, *data));

    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(
        std::bind(&impl::DoShutDown, GattConnectionManager::GetInstance().pimpl.get()));
}

void GattConnectionManager::impl::BREDRDisconnectCompletedImpl(uint16_t connectHandle, AttBredrDisconnectCallback data)
{
    bool ret = false;
    GattConnectionManager::Device *device = nullptr;
    {
        std::unique_lock<std::mutex> devLock;
        device = FindDevice(connectHandle, devLock);
        if (device != nullptr) {
            ret = device->ProcessMessage(Device::StateMachine::MSG_DISCONNECT_COMPLETE, connectHandle, &data);
        } else {
            LOG_ERROR("%{public}s: Call - FindDevice - Fail!", __FUNCTION__);
        }
    }

    if (ret && !device->AutoConnect()) {
        GattConnectionManager::GetInstance().pimpl->RemoveDevice(device->Info());
    }
}

void GattConnectionManager::impl::BREDRDisconnectCompleted(
    uint16_t connectHandle, AttBredrDisconnectCallback *data, void *context)
{
    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(
        &impl::BREDRDisconnectCompletedImpl, GattConnectionManager::GetInstance().pimpl.get(), connectHandle, *data));

    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(
        std::bind(&impl::DoShutDown, GattConnectionManager::GetInstance().pimpl.get()));
}

void GattConnectionManager::impl::BREDRConnectInd(uint16_t connectHandle, void *context)
{
    GattConnectionManager &instance = GattConnectionManager::GetInstance();
    if (!instance.pimpl->InRunningState()) {
        ATT_ConnectRsp(connectHandle, BREDR_CONNECT_REJECTED, 0, nullptr);
    }

    AttConnect connectionParameter;
    connectionParameter.bredrConnParaVar.mtu = instance.pimpl->connectionParameter_.mtu_;
    connectionParameter.bredrConnParaVar.mode = instance.pimpl->connectionParameter_.mode_;
    connectionParameter.bredrConnParaVar.flushTimeout = instance.pimpl->connectionParameter_.flushTimeout_;
    /* When passively connected, GATT will not reject the connection request */
    ATT_ConnectRsp(connectHandle, BREDR_CONNECT_ACCEPT, 0, &connectionParameter);
}

void GattConnectionManager::impl::LEConnectionParamterReqImpl(BtAddr addr, uint16_t connIntervalMin,
    uint16_t connIntervalMax, uint16_t connLatency, uint16_t timeout)
{
    static const std::pair<uint16_t, uint16_t> connIntervalMinRange(0x0006, 0x0C80);
    static const std::pair<uint16_t, uint16_t> connIntervalMaxRange(0x0006, 0x0C80);
    static const std::pair<uint16_t, uint16_t> connLatencyRange(0x0000, 0x01F3);
    static const std::pair<uint16_t, uint16_t> timeoutRange(0x000A, 0x0C80);

    std::unique_lock<std::mutex> devLock;
    auto Device = FindDevice(addr, GATT_TRANSPORT_TYPE_LE, devLock);
    if (Device == nullptr ||
        connIntervalMin < connIntervalMinRange.first || connIntervalMin > connIntervalMinRange.second ||
        connIntervalMax < connIntervalMaxRange.first || connIntervalMax > connIntervalMaxRange.second ||
        connLatency < connLatencyRange.first || connLatency > connLatencyRange.second ||
        timeout < timeoutRange.first || timeout > timeoutRange.second) {
        GAPIF_LeConnectionParameterRsp(&addr, GAP_NOT_ACCEPT, nullptr);
    }

    GapLeConnectionParameter connParam = {connIntervalMin, connIntervalMax, connLatency, timeout, 0, 0};
    GAPIF_LeConnectionParameterRsp(&addr, GAP_ACCEPT, &connParam);
}

void GattConnectionManager::impl::LEConnectionParamterReq(const BtAddr *addr, uint16_t connIntervalMin,
    uint16_t connIntervalMax, uint16_t connLatency, uint16_t timeout, void *context)
{
    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(&impl::LEConnectionParamterReqImpl,
        GattConnectionManager::GetInstance().pimpl.get(),
        *addr,
        connIntervalMin,
        connIntervalMax,
        connLatency,
        timeout));
}

void GattConnectionManager::impl::LEConnectionUpdateCompleteImpl(uint8_t status, BtAddr addr, uint16_t connInterval,
    uint16_t connLatency, uint16_t timeout)
{
    Device::ConnectionPriorityResult res = {
        .interval_ = connInterval,
        .latency_ = connLatency,
        .timeout_ = timeout,
        .status = status == GAP_STATUS_SUCCESS ? GattStatus::GATT_SUCCESS : GattStatus::GATT_FAILURE
        };

    std::unique_lock<std::mutex> devLock;
    auto Device = FindDevice(addr, GATT_TRANSPORT_TYPE_LE, devLock);
    if (Device != nullptr) {
        Device->ProcessMessage(Device::StateMachine::MSG_RESPONSE_CONNECTION_PRIORITY, 0, &res);
    }
}

void GattConnectionManager::impl::LEConnectionUpdateComplete(uint8_t status, const BtAddr *addr, uint16_t connInterval,
    uint16_t connLatency, uint16_t timeout, void *context)
{
    GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(&impl::LEConnectionUpdateCompleteImpl,
        GattConnectionManager::GetInstance().pimpl.get(),
        status,
        *addr,
        connInterval,
        connLatency,
        timeout));
}

int GattConnectionManager::impl::DoConnect(GattConnectionManager::Device &device)
{
    int result = GattStatus::GATT_SUCCESS;
    const GattConnectionManager &instance = GattConnectionManager::GetInstance();

    AttConnect connectionParameter;
    BtAddr addr;
    if (memcpy_s(addr.addr, RawAddress::BT_ADDRESS_BYTE_LEN, device.Addr(), RawAddress::BT_ADDRESS_BYTE_LEN) != EOK) {
        return GattStatus::INTERNAL_ERROR;
    }

    if (device.Info().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
        device.Info().role_ = GATT_ROLE_MASTER;
        device.Info().addressType_ = addr.type = BT_PUBLIC_DEVICE_ADDRESS;
        connectionParameter.bredrConnParaVar.mtu = instance.pimpl->connectionParameter_.mtu_;
        connectionParameter.bredrConnParaVar.mode = instance.pimpl->connectionParameter_.mode_;
        connectionParameter.bredrConnParaVar.flushTimeout = instance.pimpl->connectionParameter_.flushTimeout_;
        ATT_ConnectReq(ConvertTransport(GATT_TRANSPORT_TYPE_CLASSIC), &connectionParameter, &addr, &device.Handle());
    } else if (device.Info().transport_ == GATT_TRANSPORT_TYPE_LE) {
        IAdapterBle *adapter = (IAdapterBle *)(IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BLE));
        if (adapter != nullptr && adapter->GetPeerDeviceAddrType(device.Info().addr_) <= BLE_ADDR_TYPE_RANDOM) {
            device.Info().role_ = GATT_ROLE_MASTER;
            device.Info().addressType_ = addr.type = adapter->GetPeerDeviceAddrType(device.Info().addr_);
            connectionParameter.leConnParaVar.connIntervalMin = instance.pimpl->connectionParameter_.connIntervalMin_;
            connectionParameter.leConnParaVar.connIntervalMax = instance.pimpl->connectionParameter_.connIntervalMax_;
            connectionParameter.leConnParaVar.connLatency = instance.pimpl->connectionParameter_.connLatency_;
            connectionParameter.leConnParaVar.supervisionTimeout =
                instance.pimpl->connectionParameter_.supervisionTimeout_;

            ChangeConnectionMode(!device.AutoConnect());
            if (!device.AutoConnect()) {
                device.DirectConnect().Start(DIRECT_CONNECT_TIMEOUT);
            }
            ATT_ConnectReq(ConvertTransport(GATT_TRANSPORT_TYPE_LE), &connectionParameter, &addr, &device.Handle());
        } else {
            device.Info().role_ = GATT_ROLE_INVALID;
            result = GattStatus::REQUEST_NOT_SUPPORT;
        }
    } else {
        device.Info().role_ = GATT_ROLE_INVALID;
        result = GattStatus::INTERNAL_ERROR;
        LOG_ERROR("%{public}s: Call - Connect - Fail!  - Return: %{public}d - Parameter(1): transportType: %{public}d",
            __FUNCTION__,
            result,
            device.Info().transport_);
    }

    return result;
}

int GattConnectionManager::impl::DoConnectComplete(GattConnectionManager::Device &device, const utility::Message &msg)
{
    int result = GattStatus::INTERNAL_ERROR;

    if (GATT_TRANSPORT_TYPE_CLASSIC == device.Info().transport_ &&
        ((AttBredrConnectCallback *)msg.arg2_)->status == BREDR_CONNECT_SUCCESS) {
        device.Handle() = msg.arg1_;
        device.MTU() = ((AttBredrConnectCallback *)msg.arg2_)->mtu;
        if (device.Info().role_ != GATT_ROLE_MASTER) {
            device.Info().role_ = GATT_ROLE_SLAVE;
        }

        result = GattStatus::GATT_SUCCESS;
    } else if (GATT_TRANSPORT_TYPE_LE == device.Info().transport_ &&
               ((AttLeConnectCallback *)msg.arg2_)->status == LE_CONNECT_SUCCESS) {
        device.Handle() = msg.arg1_;
        device.Info().role_ = ((AttLeConnectCallback *)msg.arg2_)->role;

        if (device.Info().role_ == GATT_ROLE_MASTER && !device.AutoConnect()) {
            device.DirectConnect().Stop();
            ChangeConnectionMode(false);
        }
        result = GattStatus::GATT_SUCCESS;
    } else {
        device.AutoConnect() = false;
    }

    return result;
}

void GattConnectionManager::impl::DoDisconnect(uint16_t handle)
{
    ATT_DisconnectReq(handle);
}

int GattConnectionManager::impl::DoDisconnectComplete(
    GattConnectionManager::Device &device, const utility::Message &msg)
{
    int result = GattStatus::INTERNAL_ERROR;

    if (device.Info().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
        if (((AttBredrDisconnectCallback *)msg.arg2_)->reason == INITIATIVECONNECT_DISCONNECT_SUCCESS ||
            ((AttBredrDisconnectCallback *)msg.arg2_)->reason == PASSIVECONNECT_DISCONNECT_SUCCESS ||
            ((AttBredrDisconnectCallback *)msg.arg2_)->reason == DISCONNECT_ABNORMAL) {
            result = GattStatus::GATT_SUCCESS;
        }
    } else if (device.Info().transport_ == GATT_TRANSPORT_TYPE_LE) {
        if (((AttLeDisconnectCallback *)msg.arg2_)->status == LE_DISCONNECT_SUCCESS) {
            result = GattStatus::GATT_SUCCESS;
        }
    }

    return result;
}

int GattConnectionManager::impl::DoRequestConnectionPriority(GattConnectionManager::Device &device, int connPriority)
{
    BtAddr addr;
    addr.type = device.Info().addressType_;
    if (memcpy_s(addr.addr, RawAddress::BT_ADDRESS_BYTE_LEN, device.Addr(), RawAddress::BT_ADDRESS_BYTE_LEN) != EOK) {
        return GattStatus::INTERNAL_ERROR;
    }

    GapLeConnectionParameter connParam = {
        GetBleMinConnectionInterval(connPriority),
        GetBleMaxConnectionInterval(connPriority),
        GetBleConnectionLatency(connPriority),
        GetBleConnectionSupervisionTimeout(connPriority),
        0,
        0
    };
    if (GAPIF_LeConnParamUpdate(&addr, &connParam) != BT_NO_ERROR) {
        return GattStatus::GATT_FAILURE;
    }

    return GattStatus::GATT_SUCCESS;
}

void GattConnectionManager::impl::NotifyPriorityUpdated(
    const GattDevice &device, uint16_t connInterval, uint16_t connLatency, uint16_t timeout, int status)
{
    std::lock_guard<std::mutex> lck(registerMutex_);
    for (auto &item : observers_) {
        if (item.first) {
            item.second->OnConnectionParameterChanged(device, connInterval, connLatency, timeout, status);
        }
    }
}

void GattConnectionManager::impl::ConnectionInComming(
    const BtAddr &addr, uint8_t transport, uint16_t connectHandle, void *data)
{
    GattConnectionManager &instance = GattConnectionManager::GetInstance();

    std::unique_lock<std::mutex> devLock;
    auto Device = instance.pimpl->FindOrRegister(addr, transport, devLock);
    if (Device != nullptr) {
        Device->Info().addressType_ = addr.type;
        Device->ProcessMessage(Device::StateMachine::MSG_CONNECT_COMPLETE, connectHandle, data);
    } else {
        instance.pimpl->DoDisconnect(connectHandle);
        LOG_INFO("%{public}s: The maximum number of device connections has been reached", __FUNCTION__);
    }
    LOG_DEBUG("%{public}s:%{public}s:%{public}d Exit!", __FILE__, __FUNCTION__, __LINE__);
}

int GattConnectionManager::impl::CheckDeviceParameter(const GattDevice &device)
{
    int result = GattStatus::REQUEST_NOT_SUPPORT;

    if ((device.transport_ == GATT_TRANSPORT_TYPE_LE &&
        IAdapterManager::GetInstance()->GetState(ADAPTER_BLE) == STATE_TURN_ON) ||
        (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC &&
        IAdapterManager::GetInstance()->GetState(ADAPTER_BREDR) == STATE_TURN_ON)) {
        return GattStatus::GATT_SUCCESS;
    }

    return result;
}

int GattConnectionManager::impl::ConvertTransport(int transport)
{
    int type = BT_TRANSPORT_INVALID;
    switch (transport) {
        case GATT_TRANSPORT_TYPE_LE:
            type = BT_TRANSPORT_LE;
            break;
        case GATT_TRANSPORT_TYPE_CLASSIC:
            type = BT_TRANSPORT_BR_EDR;
            break;
        default:
            ASSERT(false);
            break;
    }
    return type;
}

int GattConnectionManager::impl::ConvertConnectionState(const std::string &state)
{
    BTConnectState result = BTConnectState::DISCONNECTED;
    if (state.compare(GattConnectionManager::Device::STATE_CONNECTED) == 0) {
        result = BTConnectState::CONNECTED;
    } else if (state.compare(GattConnectionManager::Device::STATE_CONNECTING) == 0) {
        result = BTConnectState::CONNECTING;
    } else if (state.compare(GattConnectionManager::Device::STATE_DISCONNECTING) == 0) {
        result = BTConnectState::DISCONNECTING;
    }
    return static_cast<int>(result);
}

void GattConnectionManager::impl::DoShutDown()
{
    if (InRunningState()) {
        return;
    }

    std::lock_guard<std::mutex> lck(devicelistRWMutex_);
    for (auto &dev : devices_) {
        std::string state = dev->SM().GetState()->Name();
        if (state == GattConnectionManager::Device::STATE_DISCONNECTED) {
            continue;
        }

        if (state == GattConnectionManager::Device::STATE_CONNECTED) {
            std::unique_lock<std::mutex> devLock;
            dev->AutoConnect() = false;
            dev->Info().role_ = GATT_ROLE_MASTER;
            dev->ProcessMessage(Device::StateMachine::MSG_DISCONNECT);
        }

        return;
    }

    devices_.clear();

    ATT_ConnectDeregister();

    std::lock_guard<std::mutex> lg(registerMutex_);
    for (auto &item : observers_) {
        if (item.first) {
            item.second->OnShutDown();
        }
    }
}

uint16_t GattConnectionManager::impl::GetBleMaxConnectedDevices()
{
    int result = DEFAULT_BLE_MAX_CONNECTED_DEVICES;
    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_BLE_MAX_CONNECTED_DEVICES, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetBleMinConnectionInterval(int connPriority)
{
    int result = DEFAULT_BLE_MIN_CONNECTION_INTERVAL;
    const std::string *property = &PROPERTY_BALANCED_PRIORITY_MIN_INTERVAL;
    if (connPriority == static_cast<int>(GattConnectionPriority::LOW_POWER)) {
        property = &PROPERTY_LOW_POWER_MIN_INTERVAL;
    } else if (connPriority == static_cast<int>(GattConnectionPriority::HIGH)) {
        property = &PROPERTY_HIGH_PRIORITY_MIN_INTERVAL;
    }

    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, *property, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetBleMaxConnectionInterval(int connPriority)
{
    int result = DEFAULT_BLE_MAX_CONNECTION_INTERVAL;
    const std::string *property = &PROPERTY_BALANCED_PRIORITY_MAX_INTERVAL;
    if (connPriority == static_cast<int>(GattConnectionPriority::LOW_POWER)) {
        property = &PROPERTY_LOW_POWER_MAX_INTERVAL;
    } else if (connPriority == static_cast<int>(GattConnectionPriority::HIGH)) {
        property = &PROPERTY_HIGH_PRIORITY_MAX_INTERVAL;
    }

    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, *property, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetBleConnectionLatency(int connPriority)
{
    int result = DEFAULT_BLE_CONNECTION_LATENCY;
    const std::string *property = &PROPERTY_BALANCED_PRIORITY_LATENCY;
    if (connPriority == static_cast<int>(GattConnectionPriority::LOW_POWER)) {
        property = &PROPERTY_LOW_POWER_LATENCY;
    } else if (connPriority == static_cast<int>(GattConnectionPriority::HIGH)) {
        property = &PROPERTY_HIGH_PRIORITY_LATENCY;
    }

    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, *property, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetBleConnectionSupervisionTimeout(int connPriority)
{
    int result = DEFAULT_BLE_CONNECTION_SUPERVISION_TIMEOUT;
    const std::string *property = &PROPERTY_BALANCED_PRIORITY_TIMEOUT;
    if (connPriority == static_cast<int>(GattConnectionPriority::LOW_POWER)) {
        property = &PROPERTY_LOW_POWER_TIMEOUT;
    } else if (connPriority == static_cast<int>(GattConnectionPriority::HIGH)) {
        property = &PROPERTY_HIGH_PRIORITY_TIMEOUT;
    }

    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, *property, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetClassicMaxConnectedDevices()
{
    int result = DEFAULT_CLASSIC_MAX_CONNECTED_DEVICES;
    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_CLASSIC_MAX_CONNECTED_DEVICES, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetClassicConnectionMtu()
{
    int result = DEFAULT_CLASSIC_CONNECTION_MTU;
    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_CLASSIC_CONNECTION_MTU, result);
    return result;
}

uint8_t GattConnectionManager::impl::GetClassicConnectionMode()
{
    int result = DEFAULT_CLASSIC_CONNECTION_MODE;
    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_CLASSIC_CONNECTION_MODE, result);
    return result;
}

uint16_t GattConnectionManager::impl::GetClassicConnectionFlushTimeout()
{
    int result = DEFAULT_CLASSIC_CONNECTION_FLUSH_TIMEOUT;
    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_CLASSIC_CONNECTION_FLUSH_TIMEOUT, result);
    return result;
}

uint8_t GattConnectionManager::impl::GetClassicConnectionSecurityMode()
{
    int result = DEFAULT_CLASSIC_CONNECTION_SECURITY_MODE;
    AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_CLASSIC_CONNECTION_SECURITY_MODE, result);
    return result;
}

AttConnectCallback GattConnectionManager::impl::BuildAttConnectCallback()
{
    AttConnectCallback callback;
    callback.attBREDRConnectCompleted = BREDRConnectCompleted;
    callback.attBREDRConnectInd = BREDRConnectInd;
    callback.attBREDRDisconnectCompleted = BREDRDisconnectCompleted;
    callback.attLEConnectCompleted = LEConnectCompleted;
    callback.attLEDisconnectCompleted = LEDisconnectCompleted;

    return callback;
}

void GattConnectionManager::impl::ChangeConnectionMode(bool isDirect)
{
    static int directCount = 0;
    static std::mutex lock;
    {
        std::lock_guard<std::mutex> lg(lock);
        if (isDirect) {
            directCount++;
        } else if (directCount != 0) {
            directCount--;
        }
    }
    LOG_DEBUG("%{public}s: directCount is %{public}d", __FUNCTION__, directCount);
    if (directCount > 0) {
        BTM_SetLeConnectionModeToFast();
    } else {
        BTM_SetLeConnectionModeToSlow();
    }
}

void GattConnectionManager::impl::DirectConnectTimeout(const GattDevice &device)
{
    std::unique_lock<std::mutex> devLock;
    auto internalDevice = FindDevice(device, devLock);
    if (internalDevice != nullptr) {
        internalDevice->ProcessMessage(Device::StateMachine::MSG_DIRECT_CONNECT_TIMEOUT);
    }
}

GattConnectionManager::Device::Device(const uint8_t *addr, uint8_t transport, uint8_t type, bool autoConnect)
    : autoConnect_(autoConnect),
      mtu_(GATT_DEFAULT_MTU),
      handle_(0),
      role_(0),
      retry_(0),
      info_(RawAddress::ConvertToString(addr), type, transport),
      deviceRWMutex_(),
      sm_(*this),
      directConnect_([&addr, transport]() {
        GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(std::bind(&impl::DirectConnectTimeout,
            GattConnectionManager::GetInstance().pimpl.get(),
            GattDevice(RawAddress::ConvertToString(addr), transport)));
    })
{
    CopyAddress(addr, RawAddress::BT_ADDRESS_BYTE_LEN);
}

GattConnectionManager::Device::Device(const GattDevice &device, bool autoConnect)
    : autoConnect_(autoConnect),
      mtu_(GATT_DEFAULT_MTU),
      handle_(0),
      role_(0),
      retry_(0),
      info_(device),
      deviceRWMutex_(),
      sm_(*this),
      directConnect_([&device]() {
        GattConnectionManager::GetInstance().pimpl->dispatcher_->PostTask(
            std::bind(&impl::DirectConnectTimeout, GattConnectionManager::GetInstance().pimpl.get(), device));
    })
{
    uint8_t addr[RawAddress::BT_ADDRESS_BYTE_LEN] = {0};
    device.addr_.ConvertToUint8(addr, sizeof(addr));
    CopyAddress(addr, RawAddress::BT_ADDRESS_BYTE_LEN);
}

void GattConnectionManager::Device::CopyAddress(const uint8_t *addr, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        addr_[i] = addr[i];
    }
}

void GattConnectionManager::Device::CheckEncryption()
{
    if (Info().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
        return;
    }

    BtAddr addr;
    uint8_t encKeySize = 0;
    GAP_LeSecurityStatus status = GAP_LE_NO_ENCRYPTION;
    IAdapterBle *adapter = (IAdapterBle *)(IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BLE));
    if (adapter != nullptr) {
        (void)memcpy_s(addr.addr, RawAddress::BT_ADDRESS_BYTE_LEN, addr_, RawAddress::BT_ADDRESS_BYTE_LEN);
        addr.type = Info().addressType_;
        if (GAPIF_LeGetSecurityStatus(&addr, &status, &encKeySize) == BT_NO_ERROR && encKeySize != GAP_LE_NO_ENCRYPTION) {
            Info().isEncryption_ = true;
        } else {
            Info().isEncryption_ = false;
        }
    }
}

GattConnectionManager::Device::~Device()
{
    std::lock_guard<std::mutex> lck(deviceRWMutex_);
}

GattConnectionManager::Device::StateMachine::StateMachine(GattConnectionManager::Device &device)
{
    std::unique_ptr<State> idle = std::make_unique<Idle>(*this, device);
    Move(idle);
    std::unique_ptr<State> connecting = std::make_unique<Connecting>(*this, device);
    Move(connecting);
    std::unique_ptr<State> connected = std::make_unique<Connected>(*this, device);
    Move(connected);
    std::unique_ptr<State> disconnecting = std::make_unique<Disconnecting>(*this, device);
    Move(disconnecting);
    std::unique_ptr<State> disconnected = std::make_unique<Disconnected>(*this, device);
    Move(disconnected);

    InitState(STATE_IDLE);
}

GattConnectionManager::Device::StateMachine::~StateMachine()
{}

bool GattConnectionManager::Device::ProcessMessage(int messageId, int arg1, void *arg2)
{
    HILOGI("Call Massage ID: %{public}d, Device Address:[%{public}s] Transport:[%{public}d]", messageId,
        GetEncryptAddr(this->Info().addr_.GetAddress()).c_str(), this->Info().transport_);

    return SM().ProcessMessage(utility::Message(messageId, arg1, arg2));
}

bool &GattConnectionManager::Device::AutoConnect()
{
    return autoConnect_;
}

uint8_t *GattConnectionManager::Device::Addr()
{
    return addr_;
}

GattDevice &GattConnectionManager::Device::Info()
{
    return info_;
}

GattConnectionManager::Device::StateMachine &GattConnectionManager::Device::SM()
{
    return sm_;
}

uint16_t &GattConnectionManager::Device::MTU()
{
    return mtu_;
}

uint16_t &GattConnectionManager::Device::Handle()
{
    return handle_;
}

uint8_t &GattConnectionManager::Device::RetryTimes()
{
    return retry_;
}

uint8_t &GattConnectionManager::Device::Role()
{
    return role_;
}

std::mutex &GattConnectionManager::Device::DeviceRWMutex()
{
    return deviceRWMutex_;
}

utility::Timer &GattConnectionManager::Device::DirectConnect()
{
    return directConnect_;
}

GattConnectionManager::Device::StateMachine::StateBase::StateBase(
    const std::string &name, utility::StateMachine &stateMachine, GattConnectionManager::Device &device)
    : State(name, stateMachine), device_(device)
{}

GattConnectionManager::Device::StateMachine::Idle::Idle(
    utility::StateMachine &stateMachine, GattConnectionManager::Device &device)
    : StateBase(STATE_IDLE, stateMachine, device)
{}

void GattConnectionManager::Device::StateMachine::Idle::Entry()
{
    HILOGI("Device Address:[%{public}s] Transport:[%{public}d], Entry %{public}s State",
        GetEncryptAddr(device_.Info().addr_.GetAddress()).c_str(), device_.Info().transport_, "Idle");
}

bool GattConnectionManager::Device::StateMachine::Idle::Dispatch(const utility::Message &msg)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s : Call Massage ID: %{public}d", __FILE__, __LINE__, __PRETTY_FUNCTION__, msg.what_);
    bool result = false;

    switch (msg.what_) {
        case MSG_CONNECT:
            if (GattConnectionManager::GetInstance().pimpl->DoConnect(device_) == GattStatus::GATT_SUCCESS) {
                Transition(STATE_CONNECTING);
                result = true;
            } else {
                Transition(STATE_DISCONNECTED);
            }
            break;
        case MSG_CONNECT_COMPLETE:
            if (GattConnectionManager::GetInstance().pimpl->DoConnectComplete(device_, msg) ==
                GattStatus::GATT_SUCCESS) {
                Transition(STATE_CONNECTED);
            } else {
                Transition(STATE_DISCONNECTED);
            }
            result = true;
            break;
        case MSG_DISCONNECT:
            Transition(STATE_DISCONNECTED);
            break;
        case MSG_DISCONNECT_COMPLETE:
        case MSG_DIRECT_CONNECT_TIMEOUT:
        default:
            LOG_ERROR("Idle::%{public}s, StateMachine is %{public}d", __FUNCTION__, msg.what_);
            break;
    }
    return result;
}

GattConnectionManager::Device::StateMachine::Connecting::Connecting(
    utility::StateMachine &stateMachine, GattConnectionManager::Device &device)
    : StateBase(STATE_CONNECTING, stateMachine, device)
{}

void GattConnectionManager::Device::StateMachine::Connecting::Entry()
{
    HILOGI("Device Address:[%{public}s] Transport:[%{public}d], Entry %{public}s State",
        GetEncryptAddr(device_.Info().addr_.GetAddress()).c_str(), device_.Info().transport_, "Connecting");
    GattConnectionManager::GetInstance().pimpl->NotifyObserver(
        device_.Info(), OBSERVER_EVENT_CONNECTING, device_.handle_, GattStatus::GATT_SUCCESS);
}

bool GattConnectionManager::Device::StateMachine::Connecting::Dispatch(const utility::Message &msg)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s : Call Massage ID: %{public}d", __FILE__, __LINE__, __PRETTY_FUNCTION__, msg.what_);
    bool result = false;

    switch (msg.what_) {
        case MSG_CONNECT_COMPLETE:
            if (GattConnectionManager::GetInstance().pimpl->DoConnectComplete(device_, msg) ==
                GattStatus::GATT_SUCCESS) {
                Transition(STATE_CONNECTED);
            } else {
                Transition(STATE_DISCONNECTED);
            }
            result = true;
            break;
        case MSG_DISCONNECT_COMPLETE:
            Transition(STATE_DISCONNECTED);
            result = true;
            break;
        case MSG_DIRECT_CONNECT_TIMEOUT:
        case MSG_DISCONNECT:
            BtAddr addr;
            if (memcpy_s(addr.addr, RawAddress::BT_ADDRESS_BYTE_LEN, device_.addr_, RawAddress::BT_ADDRESS_BYTE_LEN)
                == EOK) {
                addr.type = device_.info_.addressType_;
                ATT_LeConnectCancel(&addr);
                result = true;
            }
            break;
        case MSG_CONNECT:
        default:
            LOG_ERROR("Connecting::%{public}s, StateMachine is %{public}d", __FUNCTION__, msg.what_);
            break;
    }
    return result;
}

GattConnectionManager::Device::StateMachine::Connected::Connected(
    utility::StateMachine &stateMachine, GattConnectionManager::Device &device)
    : StateBase(STATE_CONNECTED, stateMachine, device)
{}

void GattConnectionManager::Device::StateMachine::Connected::Entry()
{
    HILOGI("Device Address:[%{public}s] Transport:[%{public}d], Entry %{public}s State",
        GetEncryptAddr(device_.Info().addr_.GetAddress()).c_str(), device_.Info().transport_, "Connected");

    device_.CheckEncryption();

    GattConnectionManager::GetInstance().pimpl->NotifyObserver(
        device_.Info(), OBSERVER_EVENT_CONNECTED, device_.handle_, GattStatus::GATT_SUCCESS);
}

bool GattConnectionManager::Device::StateMachine::Connected::Dispatch(const utility::Message &msg)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s : Call Massage ID: %{public}d", __FILE__, __LINE__, __PRETTY_FUNCTION__, msg.what_);
    bool result = false;

    switch (msg.what_) {
        case MSG_DISCONNECT:
            GattConnectionManager::GetInstance().pimpl->DoDisconnect(device_.handle_);
            Transition(STATE_DISCONNECTING);
            result = true;
            break;
        case MSG_DISCONNECT_COMPLETE:
            GattConnectionManager::GetInstance().pimpl->DoDisconnectComplete(device_, msg);
            Transition(STATE_DISCONNECTED);
            result = true;
            break;
        case MSG_REQUEST_CONNECTION_PRIORITY:
            if (GattConnectionManager::GetInstance().pimpl->DoRequestConnectionPriority(device_, msg.arg1_) ==
                GattStatus::GATT_SUCCESS) {
                result = true;
            }
            break;
        case MSG_RESPONSE_CONNECTION_PRIORITY:
            GattConnectionManager::GetInstance().pimpl->NotifyPriorityUpdated(device_.Info(),
                ((ConnectionPriorityResult *)msg.arg2_)->interval_,
                ((ConnectionPriorityResult *)msg.arg2_)->latency_,
                ((ConnectionPriorityResult *)msg.arg2_)->timeout_,
                ((ConnectionPriorityResult *)msg.arg2_)->status);
            result = true;
            break;
        case MSG_DIRECT_CONNECT_TIMEOUT:
            BtAddr addr;
            if (memcpy_s(addr.addr, RawAddress::BT_ADDRESS_BYTE_LEN, device_.addr_, RawAddress::BT_ADDRESS_BYTE_LEN)
                == EOK) {
                addr.type = device_.info_.addressType_;
                ATT_LeConnectCancel(&addr);
                result = true;
            }
            break;
        case MSG_RECONNECT_CAUSE_0X3E:
            GattConnectionManager::GetInstance().pimpl->NotifyObserver(
                device_.Info(), OBSERVER_EVENT_DISCONNECTED_INTER, device_.handle_, GattStatus::GATT_SUCCESS);
            if (GattConnectionManager::GetInstance().pimpl->DoConnect(device_) == GattStatus::GATT_SUCCESS) {
                result = true;
            }
            break;
        case MSG_CONNECT_COMPLETE:
            if (GattConnectionManager::GetInstance().pimpl->DoConnectComplete(device_, msg) !=
                GattStatus::GATT_SUCCESS) {
                Transition(STATE_DISCONNECTED);
            } else {
                GattConnectionManager::GetInstance().pimpl->NotifyObserver(
                    device_.Info(), OBSERVER_EVENT_RECONNECTED, device_.handle_, GattStatus::GATT_SUCCESS);
            }
            result = true;
            break;
        case MSG_CONNECT:
        default:
            LOG_INFO("Connected::%{public}s, StateMachine is %{public}d", __FUNCTION__, msg.what_);
            break;
    }
    return result;
}

GattConnectionManager::Device::StateMachine::Disconnecting::Disconnecting(
    utility::StateMachine &stateMachine, GattConnectionManager::Device &device)
    : StateBase(STATE_DISCONNECTING, stateMachine, device)
{}

void GattConnectionManager::Device::StateMachine::Disconnecting::Entry()
{
    HILOGI("Device Address:[%{public}s] Transport:[%{public}d], Entry %{public}s State",
        GetEncryptAddr(device_.Info().addr_.GetAddress()).c_str(), device_.Info().transport_, "Disconnecting");

    GattConnectionManager::GetInstance().pimpl->NotifyObserver(
        device_.Info(), OBSERVER_EVENT_DISCONNECTING, device_.handle_, GattStatus::GATT_SUCCESS);
}

bool GattConnectionManager::Device::StateMachine::Disconnecting::Dispatch(const utility::Message &msg)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s : Call Massage ID: %{public}d", __FILE__, __LINE__, __PRETTY_FUNCTION__, msg.what_);
    bool result = false;

    switch (msg.what_) {
        case MSG_DISCONNECT_COMPLETE:
            if (GattConnectionManager::GetInstance().pimpl->DoDisconnectComplete(device_, msg) ==
                GattStatus::GATT_SUCCESS) {
                Transition(STATE_DISCONNECTED);
            } else {
                Transition(STATE_CONNECTED);
            }
            result = true;
            break;
        case MSG_CONNECT:
        case MSG_CONNECT_COMPLETE:
        case MSG_DISCONNECT:
        case MSG_DIRECT_CONNECT_TIMEOUT:
        default:
            LOG_ERROR("Disconnecting::%{public}s, StateMachine is %{public}d", __FUNCTION__, msg.what_);
            break;
    }
    return result;
}

GattConnectionManager::Device::StateMachine::Disconnected::Disconnected(
    utility::StateMachine &stateMachine, GattConnectionManager::Device &device)
    : StateBase(STATE_DISCONNECTED, stateMachine, device)
{}

void GattConnectionManager::Device::StateMachine::Disconnected::Entry()
{
    HILOGI("Device Address:[%{public}s] Transport:[%{public}d], Entry %{public}s State",
        GetEncryptAddr(device_.Info().addr_.GetAddress()).c_str(), device_.Info().transport_, "Disconnected");

    device_.CheckEncryption();

    GattConnectionManager::GetInstance().pimpl->NotifyObserver(
        device_.Info(), OBSERVER_EVENT_DISCONNECTED, device_.handle_, GattStatus::GATT_SUCCESS);

    device_.mtu_ = GATT_DEFAULT_MTU;
    device_.handle_ = 0;

    if (device_.autoConnect_) {
        device_.sm_.ProcessMessage(utility::Message(MSG_CONNECT));
    }
}

bool GattConnectionManager::Device::StateMachine::Disconnected::Dispatch(const utility::Message &msg)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s : Call Massage ID: %{public}d", __FILE__, __LINE__, __PRETTY_FUNCTION__, msg.what_);
    bool result = false;

    GattConnectionManager &instance = GattConnectionManager::GetInstance();

    switch (msg.what_) {
        case MSG_CONNECT:
            if (instance.pimpl->DoConnect(device_) == GattStatus::GATT_SUCCESS) {
                Transition(STATE_CONNECTING);
                result = true;
            }
            break;
        case MSG_CONNECT_COMPLETE:
            if (instance.pimpl->DoConnectComplete(device_, msg) == GattStatus::GATT_SUCCESS) {
                Transition(STATE_CONNECTED);
            }
            result = true;
            break;
        case MSG_DIRECT_CONNECT_TIMEOUT:
            break;
        case MSG_DISCONNECT:
        case MSG_DISCONNECT_COMPLETE:
        default:
            LOG_INFO("Disconnected::%{public}s, StateMachine is %{public}d", __FUNCTION__, msg.what_);
            break;
    }
    return result;
}
}  // namespace bluetooth
}  // namespace OHOS
