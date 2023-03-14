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

#include "gatt_client_service.h"
#include <future>
#include <queue>
#include <set>
#include "class_creator.h"
#include "gatt_cache.h"
#include "gatt_client_profile.h"
#include "gatt_connection.h"
#include "gatt_connection_manager.h"
#include "gatt_defines.h"
#include "gatt_service_base.h"
#include "interface_profile.h"
#include "log.h"
#include "log_util.h"
#include "power_manager.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct ClientApplication {
    struct Discover {
        struct Task {
            enum Type { SERVICE, INCLUDE_SERVICE, CHARACTERISTICS, DESCRIPTOR };
            int type_ = 0;
            uint16_t startHandle_ = 0;
            uint16_t endHandle_ = 0;
        };
        std::queue<Task> tasks_ = {};
        std::set<uint16_t> discovered_ = {};
        ClientApplication &client_;
        GattClientProfile &profile_;

        bool DiscoverNext(int appId);
        void Clear();
        Discover(ClientApplication &client, GattClientProfile &profile) : client_(client), profile_(profile)
        {}
    };

    ClientApplication(
        IGattClientCallback &callback, const GattDevice &device, GattClientProfile &profile, bool isShared)
        : isShared_(isShared),
          connState_((int)BTConnectState::DISCONNECTED),
          callback_(callback),
          connection_(device),
          discover_(*this, profile)
    {}

    ClientApplication(const ClientApplication &src)
        : isShared_(src.isShared_),
          connState_((int)BTConnectState::DISCONNECTED),
          callback_(src.callback_),
          connection_(src.connection_),
          discover_(*this, src.discover_.profile_)
    {}

    ClientApplication &operator=(const ClientApplication &src) = delete;

    bool isShared_;
    int connState_;
    IGattClientCallback &callback_;
    GattConnection connection_;
    Discover discover_;
};

using AppIterator = std::map<int, ClientApplication>::iterator;

struct GattClientService::impl : public GattServiceBase {
    class GattClientProfileCallbackImplement;
    class GattConnectionObserverImplement;

    int connectionObserverId_ = 0;
    GattClientService &self_;
    std::unique_ptr<GattClientProfileCallbackImplement> profileCallback_ = {nullptr};
    std::unique_ptr<GattClientProfile> profile_ = {nullptr};
    std::unique_ptr<GattConnectionObserverImplement> connectionObserver_ = {nullptr};
    // connection handle <-> application ID
    std::map<uint16_t, std::set<int>> handleMap_ = {};
    // application ID <-> application entity
    std::map<int, ClientApplication> clients_ = {};

    explicit impl(GattClientService &service);
    ~impl();

    void RegisterApplication(
        IGattClientCallback &callback, const GattDevice &device, std::promise<int> &promise, bool isShared);
    void DeregisterApplication(int appId, std::promise<int> &promise);
    void Connect(int appId, bool autoConnect);
    void Disconnect(int appId);

    void DiscoveryServices(int appId);
    void ReadCharacteristic(int appId, uint16_t handle);
    void ReadCharacteristicByUuid(int appId, const Uuid &uuid);
    void WriteCharacteristic(
        int appId, uint16_t handle, const GattValue &value, int length, bool withoutRespond = false);
    void SignedWriteCharacteristic(int appId, uint16_t handle, const GattValue &value, int length);
    void ReadDescriptor(int appId, uint16_t handle);
    void WriteDescriptor(int appId, uint16_t handle, const GattValue &value, int length);
    void RequestExchangeMtu(int appId, int mtu);
    void RequestConnectionPriority(int appId, int connPriority);
    void GetServices(int appId, std::promise<void> &promise, std::vector<Service> &services);

    void OnDiscoverAllPrimaryServicesEvent(
        int requestId, int ret, uint16_t connectHandle, const std::map<uint16_t, GattCache::Service> &services);
    void OnFindIncludedServicesEvent(int requestId, int ret, uint16_t connectHandle, uint16_t serviceHandle,
        const std::vector<GattCache::IncludeService> &services);
    void OnDiscoverAllCharacteristicOfServiceEvent(int requestId, int ret, uint16_t connectHandle,
        uint16_t serviceHandle, const std::map<uint16_t, GattCache::Characteristic> &characteristics);
    void OnDiscoverAllCharacteristicDescriptorsEvent(
        int requestId, int ret, uint16_t serviceHandle, uint16_t characteristicHandle);
    void OnReadCharacteristicValueEvent(
        int requestId, uint16_t valueHandle, GattValue &value, size_t length, int ret);
    void OnWriteCharacteristicValueEvent(int requestId, uint16_t connectHandle, uint16_t valueHandle, int ret);
    void OnReadDescriptorValueEvent(
        int requestId, uint16_t valueHandle, GattValue &value, size_t length, int ret);
    void OnWriteDescriptorValueEvent(int requestId, uint16_t connectHandle, uint16_t valueHandle, int ret);
    void OnCharacteristicNotifyEvent(
        uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t length, bool needConfirm);
    void OnExchangeMtuEvent(int requestId, uint16_t connectHandle, uint16_t rxMtu, bool status);
    void OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret);
    void OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret);
    void OnConnectionChanged(const GattDevice &device, uint16_t connectionHandle, int state);
    void OnConnectionParameterChanged(const GattDevice &device, int interval, int latency, int timeout, int status);
    void OnConnetionManagerShutDown();

    std::optional<AppIterator> GetValidApplication(int appId);
    std::optional<AppIterator> GetValidApplication(const GattDevice &device);

    void Enable();
    void Disable();
    void CleanApplication();
    static void BuildService(GattCache::Service &src, Service &dest);
    static void GattUpdatePowerStatus(const RawAddress &addr);
};

GattClientService::GattClientService()
    : utility::Context(PROFILE_NAME_GATT_CLIENT, "5.0.0"), pimpl(std::make_unique<GattClientService::impl>(*this))
{
    LOG_INFO("ProfileService:%{public}s Create", Name().c_str());
}

GattClientService::~GattClientService()
{}

utility::Context *GattClientService::GetContext()
{
    return this;
}

int GattClientService::Connect(int appId, bool autoConnect)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::Connect, pimpl.get(), appId, autoConnect));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::Disconnect(int appId)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::Disconnect, pimpl.get(), appId));

    return GattStatus::GATT_SUCCESS;
}

std::list<RawAddress> GattClientService::GetConnectDevices()
{
    return std::list<RawAddress>();
}

int GattClientService::Connect(const RawAddress &device)
{
    return 0;
}

int GattClientService::Disconnect(const RawAddress &device)
{
    return 0;
}

int GattClientService::GetConnectState()
{
    return 0;
}

int GattClientService::GetMaxConnectNum()
{
    auto maxNum = GattConnectionManager::GetInstance().GetMaximumNumberOfConnections();
    return maxNum.first + maxNum.second;
}

int GattClientService::RegisterApplication(IGattClientCallback &callback, const RawAddress &addr, uint8_t transport)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    GetDispatcher()->PostTask(std::bind(&impl::RegisterApplication,
        pimpl.get(),
        std::ref(callback),
        GattDevice(addr, transport),
        std::ref(promise),
        false));

    return future.get();
}

int GattClientService::RegisterSharedApplication(
    IGattClientCallback &callback, const RawAddress &addr, uint8_t transport)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    GetDispatcher()->PostTask(std::bind(&impl::RegisterApplication,
        pimpl.get(),
        std::ref(callback),
        GattDevice(addr, transport),
        std::ref(promise),
        true));

    return future.get();
}

int GattClientService::DeregisterApplication(int appId)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    GetDispatcher()->PostTask(std::bind(&impl::DeregisterApplication, pimpl.get(), appId, std::ref(promise)));

    return future.get();
}

int GattClientService::DiscoveryServices(int appId)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::DiscoveryServices, pimpl.get(), appId));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::ReadCharacteristic(int appId, const Characteristic &characteristic)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::ReadCharacteristic, pimpl.get(), appId, characteristic.handle_));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::ReadCharacteristicByUuid(int appId, const Uuid &uuid)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::ReadCharacteristicByUuid, pimpl.get(), appId, uuid));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::WriteCharacteristic(int appId, Characteristic &characteristic, bool withoutRespond)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (characteristic.value_ == nullptr || characteristic.length_ <= 0) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(characteristic.value_);
    GetDispatcher()->PostTask(std::bind(&impl::WriteCharacteristic,
        pimpl.get(),
        appId,
        characteristic.handle_,
        sharedPtr,
        characteristic.length_,
        withoutRespond));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::SignedWriteCharacteristic(int appId, Characteristic &characteristic)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (characteristic.value_ == nullptr || characteristic.length_ <= 0) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(characteristic.value_);
    GetDispatcher()->PostTask(std::bind(&impl::SignedWriteCharacteristic,
        pimpl.get(),
        appId,
        characteristic.handle_,
        sharedPtr,
        characteristic.length_));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::ReadDescriptor(int appId, const Descriptor &descriptor)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::ReadDescriptor, pimpl.get(), appId, descriptor.handle_));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::WriteDescriptor(int appId, Descriptor &descriptor)
{
    LOG_INFO("%{public}s: appId=%{public}d", __FUNCTION__, appId);
    if (!pimpl->InRunningState()) {
        LOG_ERROR("%{public}s: pimpl->InRunningState is false", __FUNCTION__);
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (descriptor.value_ == nullptr || descriptor.length_ <= 0) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(descriptor.value_);
    GetDispatcher()->PostTask(
        std::bind(&impl::WriteDescriptor, pimpl.get(), appId, descriptor.handle_, sharedPtr, descriptor.length_));

    return GattStatus::GATT_SUCCESS;
}

int GattClientService::RequestExchangeMtu(int appId, int mtu)
{
    LOG_INFO("%{public}s: appId=%{public}d, mtu=%{public}d", __FUNCTION__, appId, mtu);
    if (!pimpl->InRunningState()) {
        LOG_ERROR("%{public}s: pimpl->InRunningState is false", __FUNCTION__);
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::RequestExchangeMtu, pimpl.get(), appId, mtu));

    return GattStatus::GATT_SUCCESS;
}

std::vector<GattDevice> GattClientService::GetAllDevice()
{
    std::vector<GattDevice> devices;
    GattConnectionManager::GetInstance().GetDevices(devices);

    return devices;
}

int GattClientService::RequestConnectionPriority(int appId, int connPriority)
{
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (connPriority != static_cast<int>(GattConnectionPriority::LOW_POWER) &&
        connPriority != static_cast<int>(GattConnectionPriority::BALANCED) &&
        connPriority != static_cast<int>(GattConnectionPriority::HIGH)) {
        return GattStatus::INVALID_PARAMETER;
    }

    GetDispatcher()->PostTask(std::bind(&impl::RequestConnectionPriority, pimpl.get(), appId, connPriority));

    return GattStatus::GATT_SUCCESS;
}

std::vector<Service> GattClientService::GetServices(int appId)
{
    if (!pimpl->InRunningState()) {
        return std::vector<Service>();
    }

    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    std::vector<Service> services;
    GetDispatcher()->PostTask(std::bind(&impl::GetServices, pimpl.get(), appId, std::ref(promise), std::ref(services)));
    future.wait();

    return services;
}

class GattClientService::impl::GattConnectionObserverImplement : public GattConnectionObserver {
public:
    void OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        HILOGI("addr: %{public}s ret: %{public}d", GetEncryptAddr(device.addr_.GetAddress()).c_str(), ret);
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnConnect, service_.pimpl.get(), device, connectionHandle, ret));
    }

    void OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        HILOGI("addr: %{public}s ret: %{public}d", GetEncryptAddr(device.addr_.GetAddress()).c_str(), ret);
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnDisconnect, service_.pimpl.get(), device, connectionHandle, ret));
    }

    void OnConnectionChanged(const GattDevice &device, uint16_t connectionHandle, int state) override
    {
        HILOGI("addr: %{public}s ret: %{public}d", GetEncryptAddr(device.addr_.GetAddress()).c_str(), state);
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnConnectionChanged,
            service_.pimpl.get(), device, connectionHandle, state));
    }

    void OnConnectionParameterChanged(
        const GattDevice &device, int interval, int latency, int timeout, int status) override
    {
        service_.GetDispatcher()->PostTask(std::bind(
            &impl::OnConnectionParameterChanged, service_.pimpl.get(), device, interval, latency, timeout, status));
    }

    void OnShutDown() override
    {
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnConnetionManagerShutDown, service_.pimpl.get()));
    }

    explicit GattConnectionObserverImplement(GattClientService &service) : service_(service)
    {}
    ~GattConnectionObserverImplement()
    {}

private:
    GattClientService &service_;
};

class GattClientService::impl::GattClientProfileCallbackImplement : public GattClientProfileCallback {
public:
    void OnDiscoverAllPrimaryServicesEvent(
        int reqId, int ret, uint16_t connectHandle, const std::map<uint16_t, GattCache::Service> &services) override
    {
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnDiscoverAllPrimaryServicesEvent,
            service_.pimpl.get(),
            reqId,
            ret,
            connectHandle,
            std::ref(services)));
    }

    void OnFindIncludedServicesEvent(int reqId, int ret, uint16_t connectHandle, uint16_t serviceHandle,
        const std::vector<GattCache::IncludeService> &services) override
    {
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnFindIncludedServicesEvent,
            service_.pimpl.get(),
            reqId,
            ret,
            connectHandle,
            serviceHandle,
            std::ref(services)));
    }

    void OnDiscoverAllCharacteristicOfServiceEvent(int reqId, int ret, uint16_t connectHandle, uint16_t serviceHandle,
        const std::map<uint16_t, GattCache::Characteristic> &characteristics) override
    {
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnDiscoverAllCharacteristicOfServiceEvent,
            service_.pimpl.get(),
            reqId,
            ret,
            connectHandle,
            serviceHandle,
            std::ref(characteristics)));
    }

    void OnDiscoverAllCharacteristicDescriptorsEvent(
        int reqId, int ret, uint16_t serviceHandle, uint16_t characteristicHandle,
        const std::map<uint16_t, GattCache::Descriptor> &descriptors) override
    {
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnDiscoverAllCharacteristicDescriptorsEvent,
            service_.pimpl.get(),
            reqId,
            ret,
            serviceHandle,
            characteristicHandle));
    }

    void OnReadCharacteristicValueEvent(
        int reqId, uint16_t handle, GattValue &value, size_t len, int result) override
    {
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnReadCharacteristicValueEvent,
            service_.pimpl.get(),
            reqId,
            handle,
            value,
            len,
            result));
    }

    void OnWriteCharacteristicValueEvent(int reqId, uint16_t connectHandle, uint16_t handle, int ret) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnWriteCharacteristicValueEvent, service_.pimpl.get(), reqId, connectHandle, handle, ret));
    }

    void OnReadDescriptorValueEvent(
        int reqId, uint16_t handle, GattValue &value, size_t len, int result) override
    {
        service_.GetDispatcher()->PostTask(std::bind(
            &impl::OnReadDescriptorValueEvent, service_.pimpl.get(), reqId, handle, value, len, result));
    }

    void OnWriteDescriptorValueEvent(int reqId, uint16_t connectHandle, uint16_t handle, int ret) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnWriteDescriptorValueEvent, service_.pimpl.get(), reqId, connectHandle, handle, ret));
    }

    void OnCharacteristicNotifyEvent(
        uint16_t connectHandle, uint16_t handle, GattValue &value, size_t len, bool needConfirm) override
    {
        service_.GetDispatcher()->PostTask(std::bind(
            &impl::OnCharacteristicNotifyEvent, service_.pimpl.get(), connectHandle, handle, value, len, needConfirm));
    }

    void OnExchangeMtuEvent(int reqId, uint16_t connectHandle, uint16_t rxMtu, bool status) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnExchangeMtuEvent, service_.pimpl.get(), reqId, connectHandle, rxMtu, status));
    }

    GattClientProfileCallbackImplement(GattClientService &service) : service_(service)
    {}
    ~GattClientProfileCallbackImplement()
    {}

private:
    GattClientService &service_;
};

GattClientService::impl::impl(GattClientService &service)
    : self_(service),
      profileCallback_(std::make_unique<GattClientProfileCallbackImplement>(service)),
      profile_(std::make_unique<GattClientProfile>(profileCallback_.get(), service.GetDispatcher())),
      connectionObserver_(std::make_unique<GattConnectionObserverImplement>(service))
{
    connectionObserverId_ = GattConnectionManager::GetInstance().RegisterObserver(*connectionObserver_);
    if (connectionObserverId_ < 0) {
        LOG_ERROR("%{public}s:%{public}s:%{public}d register to connection manager failed!", __FILE__, __FUNCTION__, __LINE__);
    }
}

GattClientService::impl::~impl()
{
    GattConnectionManager::GetInstance().DeregisterObserver(connectionObserverId_);
}

void GattClientService::impl::Connect(int appId, bool autoConnect)
{
    static const int TUPLE_INDEX_FIRST = 0;
    static const int TUPLE_INDEX_SECOND = 1;
    static const int TUPLE_INDEX_THIRD = 2;
    auto client = GetValidApplication(appId);
    if (client.has_value()) {
        int result = GattStatus::GATT_SUCCESS;
        auto &manager = GattConnectionManager::GetInstance();
        auto &device = client.value()->second.connection_.GetDevice();

        auto deviceInfo = GattConnectionManager::GetInstance().GetDeviceInformation(device);
        if (std::get<TUPLE_INDEX_FIRST>(deviceInfo).compare(GattConnectionManager::Device::STATE_IDLE) == 0 ||
            std::get<TUPLE_INDEX_FIRST>(deviceInfo).compare(GattConnectionManager::Device::STATE_DISCONNECTED) == 0) {
            result = manager.Connect(device, autoConnect);
            if (GattStatus::GATT_SUCCESS != result) {
                client.value()->second.connState_ = ConvertConnectionState(manager.GetDeviceState(device));
                client.value()->second.callback_.OnConnectionStateChanged(result, client.value()->second.connState_);
            }
        } else {
            if (std::get<TUPLE_INDEX_FIRST>(deviceInfo).compare(GattConnectionManager::Device::STATE_CONNECTED) == 0) {
                client.value()->second.connection_.SetHandle(std::get<TUPLE_INDEX_SECOND>(deviceInfo));
                client.value()->second.connection_.SetMtu(std::get<TUPLE_INDEX_THIRD>(deviceInfo));

                auto it = handleMap_.emplace(
                    std::make_pair<int, std::set<int>>(std::get<TUPLE_INDEX_SECOND>(deviceInfo), {}));
                it.first->second.emplace(appId);

                result = GattStatus::GATT_SUCCESS;
                GattConnectionManager::GetInstance().SetConnectionType(device, autoConnect);
            }

            client.value()->second.connState_ = ConvertConnectionState(std::get<TUPLE_INDEX_FIRST>(deviceInfo));
            client.value()->second.callback_.OnConnectionStateChanged(result, client.value()->second.connState_);
        }
    }
}

void GattClientService::impl::Disconnect(int appId)
{
    bool needDisconnect = true;
    auto client = GetValidApplication(appId);
    if (client.has_value()) {
        auto &manager = GattConnectionManager::GetInstance();
        auto &device = client.value()->second.connection_.GetDevice();

        client.value()->second.connState_ = (int)BTConnectState::DISCONNECTED;
        auto map = handleMap_.find(client.value()->second.connection_.GetHandle());
        if (map != handleMap_.end()) {
            map->second.erase(appId);
            for (auto aid : map->second) {
                auto app = GetValidApplication(aid);
                if (app.has_value() && app.value()->second.connState_ != (int)BTConnectState::DISCONNECTED) {
                    needDisconnect = false;
                    break;
                }
            }
        } else {
            needDisconnect = false;
        }

        if (!needDisconnect) {
            client.value()->second.callback_.OnConnectionStateChanged(
                GattStatus::GATT_SUCCESS, client.value()->second.connState_);
            return;
        }

        int result = manager.Disconnect(device);
        if (GattStatus::GATT_SUCCESS != result) {
            if (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
                IPowerManager::GetInstance().StatusUpdate(
                    RequestStatus::CONNECT_OFF, PROFILE_NAME_GATT_CLIENT, device.addr_);
            }

            client.value()->second.callback_.OnConnectionStateChanged(
                result, ConvertConnectionState(manager.GetDeviceState(device)));
        }
    }
}

void GattClientService::impl::DiscoveryServices(int appId)
{
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            client.callback_.OnServicesDiscovered(GattStatus::REQUEST_NOT_SUPPORT);
            return;
        }

        if (client.discover_.tasks_.size() != 0) {
            client.callback_.OnServicesDiscovered(GattStatus::REMOTE_DEVICE_BUSY);
            return;
        }

        profile_->ClearCacheMap(client.connection_.GetHandle());
        client.discover_.Clear();

        ClientApplication::Discover::Task task = {};
        task.type_ = ClientApplication::Discover::Task::Type::SERVICE;
        task.startHandle_ = MIN_ATTRIBUTE_HANDLE;
        task.endHandle_ = MAX_ATTRIBUTE_HANDLE;
        client.discover_.tasks_.push(task);

        client.discover_.DiscoverNext(appId);
    }
}

void GattClientService::impl::ReadCharacteristic(int appId, uint16_t handle)
{
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            client.callback_.OnCharacteristicRead(GattStatus::REQUEST_NOT_SUPPORT, Characteristic(handle));
            return;
        }
        profile_->ReadCharacteristicValue(appId, client.connection_.GetHandle(), handle + 1);
    }
}

void GattClientService::impl::ReadCharacteristicByUuid(int appId, const Uuid &uuid)
{
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            LOG_DEBUG("%{public}s: %{public}hu: is not found in the map", __func__, client.connection_.GetHandle());
            client.callback_.OnCharacteristicRead(GattStatus::REQUEST_NOT_SUPPORT, Characteristic(0));
            return;
        }
        profile_->ReadUsingCharacteristicByUuid(appId, client.connection_.GetHandle(), uuid);
    }
}

void GattClientService::impl::WriteCharacteristic(
    int appId, uint16_t handle, const GattValue &value, int length, bool withoutRespond)
{
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            if (!withoutRespond) {
                client.callback_.OnCharacteristicWrite(GattStatus::REQUEST_NOT_SUPPORT, Characteristic(handle));
            }
            return;
        }
        if (withoutRespond) {
            profile_->WriteWithoutResponse(appId, client.connection_.GetHandle(), handle + 1, value, length);
        } else {
            profile_->WriteCharacteristicValue(appId, client.connection_.GetHandle(), handle + 1, value, length);
        }
    }
}

void GattClientService::impl::SignedWriteCharacteristic(int appId, uint16_t handle, const GattValue &value, int length)
{
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            return;
        }

        profile_->SignedWriteWithoutResponse(appId, client.connection_.GetHandle(), handle + 1, value, length);
    }
}

void GattClientService::impl::ReadDescriptor(int appId, uint16_t handle)
{
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            client.callback_.OnDescriptorRead(GattStatus::REQUEST_NOT_SUPPORT, Descriptor(handle));
            return;
        }
        profile_->ReadDescriptorValue(appId, client.connection_.GetHandle(), handle);
    }
}

void GattClientService::impl::WriteDescriptor(int appId, uint16_t handle, const GattValue &value, int length)
{
    LOG_INFO("%{public}s: appId=%{public}d", __FUNCTION__, appId);
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            client.callback_.OnDescriptorWrite(GattStatus::REQUEST_NOT_SUPPORT, Descriptor(handle));
            return;
        }
        profile_->WriteDescriptorValue(appId, client.connection_.GetHandle(), handle, value, length);
    }
}

void GattClientService::impl::RequestExchangeMtu(int appId, int mtu)
{
    LOG_INFO("%{public}s: appId=%{public}d, mtu=%{public}d", __FUNCTION__, appId, mtu);
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            LOG_ERROR("%{public}s: failed and return REQUEST_NOT_SUPPORT(-18)", __FUNCTION__);
            client.callback_.OnMtuChanged(GattStatus::REQUEST_NOT_SUPPORT, mtu);
            return;
        }
        profile_->ExchangeMtu(appId, client.connection_.GetHandle(), mtu);
    }
}

void GattClientService::impl::RequestConnectionPriority(int appId, int connPriority)
{
    LOG_INFO("%{public}s: appId=%{public}d, priority=%{public}d", __FUNCTION__, appId, connPriority);
    auto it = GetValidApplication(appId);
    if (it.has_value()) {
        auto &client = it.value()->second;
        if (handleMap_.find(client.connection_.GetHandle()) == handleMap_.end()) {
            LOG_ERROR("%{public}s: failed and return REQUEST_NOT_SUPPORT(-18)", __FUNCTION__);
            client.callback_.OnConnectionParameterChanged(0, 0, 0, GattStatus::REQUEST_NOT_SUPPORT);
            return;
        }
        int result = GattConnectionManager::GetInstance().RequestConnectionPriority(
            client.connection_.GetHandle(), connPriority);
        if (GattStatus::GATT_SUCCESS != result) {
            client.callback_.OnConnectionParameterChanged(0, 0, 0, result);
        }
    }
}

void GattClientService::impl::GetServices(
    int appId, std::promise<void> &promise, std::vector<Service> &services)
{
    auto client = GetValidApplication(appId);
    if (client.has_value() && client.value()->second.connState_ == (int)BTConnectState::CONNECTED) {
        auto svcs = profile_->GetServices(client.value()->second.connection_.GetHandle());
        if (svcs != nullptr) {
            for (auto &svc : *svcs) {
                Service svcTmp(svc.second.uuid_, svc.second.handle_, svc.second.handle_, svc.second.endHandle_);

                BuildService(svc.second, svcTmp);
                services.push_back(std::move(svcTmp));
            }
        }
    }

    promise.set_value();
}

void GattClientService::impl::OnDiscoverAllPrimaryServicesEvent(
    int requestId, int ret, uint16_t connectHandle, const std::map<uint16_t, GattCache::Service> &services)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        if (GattStatus::GATT_SUCCESS == ret) {
            for (auto &serv : services) {
                ClientApplication::Discover::Task task = {};
                task.type_ = ClientApplication::Discover::Task::Type::INCLUDE_SERVICE;
                task.startHandle_ = serv.second.handle_;
                task.endHandle_ = serv.second.endHandle_;
                it.value()->second.discover_.tasks_.push(task);

                task.type_ = ClientApplication::Discover::Task::Type::CHARACTERISTICS;
                it.value()->second.discover_.tasks_.push(task);
            }
            if (!it.value()->second.discover_.DiscoverNext(requestId)) {
                return;
            }
        }

        it.value()->second.discover_.Clear();
        it.value()->second.callback_.OnServicesDiscovered(ret);
    }
}

void GattClientService::impl::OnFindIncludedServicesEvent(int requestId, int ret, uint16_t connectHandle,
    uint16_t serviceHandle, const std::vector<GattCache::IncludeService> &services)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        if (GattStatus::GATT_SUCCESS == ret) {
            for (auto &serv : services) {
                ClientApplication::Discover::Task task = {};
                task.type_ = ClientApplication::Discover::Task::Type::SERVICE;
                task.startHandle_ = serv.startHandle_;
                task.endHandle_ = serv.startHandle_;

                it.value()->second.discover_.tasks_.push(task);
            }
            if (!it.value()->second.discover_.DiscoverNext(requestId)) {
                return;
            }
        }

        it.value()->second.discover_.Clear();
        it.value()->second.callback_.OnServicesDiscovered(ret);
    }
}

void GattClientService::impl::OnDiscoverAllCharacteristicOfServiceEvent(int requestId, int ret, uint16_t connectHandle,
    uint16_t serviceHandle, const std::map<uint16_t, GattCache::Characteristic> &characteristics)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        if (GattStatus::GATT_SUCCESS == ret) {
            for (auto &ccc : characteristics) {
                ClientApplication::Discover::Task task = {};
                task.type_ = ClientApplication::Discover::Task::Type::DESCRIPTOR;
                task.startHandle_ = ccc.second.handle_;
                task.endHandle_ = profile_->GetCharacteristicEndHandle(
                    it.value()->second.connection_.GetHandle(), serviceHandle, ccc.second.handle_);

                it.value()->second.discover_.tasks_.push(task);
            }

            if (!it.value()->second.discover_.DiscoverNext(requestId)) {
                return;
            }
        }

        it.value()->second.discover_.Clear();
        it.value()->second.callback_.OnServicesDiscovered(ret);
    }
}

void GattClientService::impl::OnDiscoverAllCharacteristicDescriptorsEvent(int requestId, int ret,
    uint16_t serviceHandle, uint16_t characteristicHandle)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        if (GattStatus::GATT_SUCCESS == ret) {
            if (!it.value()->second.discover_.DiscoverNext(requestId)) {
                return;
            }
        }

        it.value()->second.discover_.Clear();
        it.value()->second.callback_.OnServicesDiscovered(ret);
    }
}

void GattClientService::impl::OnReadCharacteristicValueEvent(
    int requestId, uint16_t valueHandle, GattValue &value, size_t length, int ret)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        Characteristic gattCCC(valueHandle - 1);
        if (value) {
            gattCCC.value_ = std::move(*value);
            gattCCC.length_ = length;
            it.value()->second.callback_.OnCharacteristicRead(ret, gattCCC);
        } else {
            it.value()->second.callback_.OnCharacteristicRead(ret, gattCCC);
        }
    }
}

void GattClientService::impl::OnWriteCharacteristicValueEvent(
    int requestId, uint16_t connectHandle, uint16_t valueHandle, int ret)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        it.value()->second.callback_.OnCharacteristicWrite(ret, Characteristic());
    }
}

void GattClientService::impl::OnReadDescriptorValueEvent(
    int requestId, uint16_t valueHandle, GattValue &value, size_t length, int ret)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        Descriptor gattDescriptor;
        if (value) {
            gattDescriptor.value_ = std::move(*value);
            gattDescriptor.length_ = length;
            it.value()->second.callback_.OnDescriptorRead(ret, gattDescriptor);
        } else {
            it.value()->second.callback_.OnDescriptorRead(ret, gattDescriptor);
        }
    }
}

void GattClientService::impl::OnWriteDescriptorValueEvent(
    int requestId, uint16_t connectHandle, uint16_t valueHandle, int ret)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        it.value()->second.callback_.OnDescriptorWrite(ret, Descriptor());
    }
}

void GattClientService::impl::OnCharacteristicNotifyEvent(
    uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t length, bool needConfirm)
{
    if (needConfirm) {
        profile_->HandleValueConfirmation(connectHandle);
    }

    for (auto appId : handleMap_.find(connectHandle)->second) {
        auto client = GetValidApplication(appId);
        if (!client.has_value()) {
            continue;
        }

        if (client.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(client.value()->second.connection_.GetDevice().addr_);
        }

        auto ccc = profile_->GetCharacteristic(connectHandle, valueHandle);
        if (ccc == nullptr) {
            LOG_ERROR("%{public}s:%{public}d:%{public}s Gatt Cache Error: Can not find Characteristic by value handle.",
                __FILE__,
                __LINE__,
                __FUNCTION__);
            return;
        }

        if (Uuid::ConvertFrom16Bits(UUID_SERVICE_CHANGED) == ccc->uuid_) {
            client.value()->second.callback_.OnServicesChanged(std::vector<Service>());
        } else {
            Characteristic gattCCC(ccc->uuid_, ccc->handle_, ccc->properties_);
            gattCCC.value_ = std::make_unique<uint8_t[]>(length);
            if (memcpy_s(gattCCC.value_.get(), length, (*value).get(), length) != EOK) {
                return;
            }
            gattCCC.length_ = length;
            client.value()->second.callback_.OnCharacteristicChanged(gattCCC);
        }
    }
}

void GattClientService::impl::OnExchangeMtuEvent(int requestId, uint16_t connectHandle, uint16_t rxMtu, bool status)
{
    auto it = GetValidApplication(requestId);
    if (it.has_value()) {
        if (it.value()->second.connection_.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(it.value()->second.connection_.GetDevice().addr_);
        }

        it.value()->second.callback_.OnMtuChanged(status ? GattStatus::GATT_SUCCESS : GattStatus::GATT_FAILURE, rxMtu);
    }
}

void GattClientService::impl::RegisterApplication(
    IGattClientCallback &callback, const GattDevice &device, std::promise<int> &promise, bool isShared)
{
    int result = GattStatus::DEVICE_ALREADY_BIND;
    int appid = GattServiceBase::GetApplicationId();
    if (clients_.size() >= MAXIMUM_NUMBER_APPLICATION) {
        promise.set_value(GattStatus::MAX_APPLICATIONS);
        return;
    }

    if (isShared) {
        if (clients_.emplace(appid, ClientApplication(callback, device, *profile_, isShared)).second) {
            result = GattStatus::GATT_SUCCESS;
        } else {
            result = GattStatus::GATT_FAILURE;
            LOG_ERROR(
                "%{public}s:%{public}s:%{public}d register to application failed! same application ID!", __FILE__, __FUNCTION__, __LINE__);
        }
    } else {
        auto app = GetValidApplication(device);
        if (!app.has_value()) {
            if (clients_.emplace(appid, ClientApplication(callback, device, *profile_, isShared)).second) {
                result = GattStatus::GATT_SUCCESS;
            } else {
                result = GattStatus::GATT_FAILURE;
                LOG_ERROR("%{public}s:%{public}s:%{public}d register to application failed! same application ID!",
                    __FILE__,
                    __FUNCTION__,
                    __LINE__);
            }
        }
    }

    promise.set_value(((result == GattStatus::GATT_SUCCESS) ? appid : result));
}

void GattClientService::impl::DeregisterApplication(int appId, std::promise<int> &promise)
{
    auto client = GetValidApplication(appId);
    if (client.has_value()) {
        auto map = handleMap_.find(client.value()->second.connection_.GetHandle());
        if (map != handleMap_.end()) {
            map->second.erase(appId);
        }
        clients_.erase(client.value());
    }

    promise.set_value(GattStatus::GATT_SUCCESS);
}

void GattClientService::impl::OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret)
{
    auto map = handleMap_.emplace(std::make_pair<int, std::set<int>>(connectionHandle, {}));
    if (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_ON, PROFILE_NAME_GATT_CLIENT, device.addr_);
    }

    for (auto &it : clients_) {
        if (it.second.connection_.GetDevice() == device) {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s, dev_role: %{public}d",
                __FILE__, __LINE__, __FUNCTION__, device.role_);
            auto &client = it.second;

            client.connection_.SetHandle(connectionHandle);
            map.first->second.emplace(it.first);

            client.connState_ = static_cast<int>(BTConnectState::CONNECTED);
            if (device.role_ == GATT_ROLE_MASTER) {
                client.callback_.OnConnectionStateChanged(ret, client.connState_);
            }
        }
    }
}

void GattClientService::impl::OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret)
{
    LOG_INFO("%{public}s: client service dev_role: %{public}d, ret: %{public}d", __FUNCTION__, device.role_, ret);
    handleMap_.erase(connectionHandle);
    for (auto &it : clients_) {
        if (it.second.connection_.GetDevice() == device) {
            auto &client = it.second;

            client.connection_.SetHandle(0);
            client.connection_.SetMtu(0);
            client.connState_ = static_cast<int>(BTConnectState::DISCONNECTED);
            if (device.role_ == GATT_ROLE_MASTER) {
                client.callback_.OnConnectionStateChanged(ret, client.connState_);
            }
        }
    }
}

void GattClientService::impl::OnConnectionChanged(const GattDevice &device, uint16_t connectionHandle, int state)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (device.role_ != GATT_ROLE_MASTER) {
        LOG_ERROR("%{public}s: device role is %{public}d", __FUNCTION__, device.role_);
        return;
    }
    for (auto &it : clients_) {
        if (it.second.connection_.GetDevice() == device) {
            if (it.second.connState_ == state) {
                auto &client = it.second;
                client.connection_.SetHandle(connectionHandle);
                client.discover_.profile_.DiscoverAllPrimaryServicesInter(
                    it.first, client.connection_.GetHandle(), MIN_ATTRIBUTE_HANDLE, MAX_ATTRIBUTE_HANDLE);
            } else {
                it.second.connState_ = state;
                it.second.callback_.OnConnectionStateChanged(GattStatus::GATT_SUCCESS, it.second.connState_);
            }
        }
    }
}

void GattClientService::impl::OnConnectionParameterChanged(
    const GattDevice &device, int interval, int latency, int timeout, int status)
{
    for (auto it : clients_) {
        if (it.second.connection_.GetDevice() == device) {
            it.second.callback_.OnConnectionParameterChanged(interval, latency, timeout, status);
        }
    }
}

void GattClientService::impl::OnConnetionManagerShutDown()
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s :Gatt Client Service Done", __FILE__, __LINE__, __FUNCTION__);
    self_.GetContext()->OnDisable(PROFILE_NAME_GATT_CLIENT, true);
}

std::optional<AppIterator> GattClientService::impl::GetValidApplication(int appId)
{
    auto it = clients_.find(appId);
    if (it != clients_.end()) {
        return it;
    }
    return std::nullopt;
}

std::optional<AppIterator> GattClientService::impl::GetValidApplication(const GattDevice &device)
{
    for (auto it = clients_.begin(); it != clients_.end(); it++) {
        if (it->second.connection_.GetDevice() == device) {
            return it;
        }
    }
    return std::nullopt;
}

void GattClientService::impl::Enable()
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s :Gatt Client Service Entry", __FILE__, __LINE__, __FUNCTION__);
    if (GattStatus::GATT_SUCCESS != GattConnectionManager::GetInstance().StartUp(*self_.GetDispatcher())) {
        self_.GetContext()->OnEnable(PROFILE_NAME_GATT_CLIENT, false);
        return;
    }

    profile_->Enable();

    CleanApplication();

    Start();

    LOG_DEBUG("%{public}s:%{public}d:%{public}s :Gatt Client Service Done", __FILE__, __LINE__, __FUNCTION__);

    self_.GetContext()->OnEnable(PROFILE_NAME_GATT_CLIENT, true);
}

void GattClientService::impl::Disable()
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s :Gatt Client Service Entry", __FILE__, __LINE__, __FUNCTION__);

    Stop();

    profile_->Disable();

    GattConnectionManager::GetInstance().ShutDown();
}

void GattClientService::impl::CleanApplication()
{
    handleMap_.clear();
    clients_.clear();
}

void GattClientService::impl::BuildService(GattCache::Service &src, Service &dest)
{
    for (auto &isvc : src.includeServices_) {
        dest.includeServices_.push_back(Service(isvc.uuid_, isvc.handle_, isvc.handle_, isvc.endHandle_));
    }

    for (auto &ccc : src.characteristics_) {
        Characteristic cccTmp(ccc.second.uuid_, ccc.second.handle_, ccc.second.properties_);
        for (auto &desc : ccc.second.descriptors_) {
            cccTmp.descriptors_.push_back(Descriptor(desc.second.handle_, desc.second.uuid_, 0));
        }
        dest.characteristics_.push_back(std::move(cccTmp));
    }
}

void GattClientService::impl::GattUpdatePowerStatus(const RawAddress &addr)
{
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_GATT_CLIENT, addr);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_GATT_CLIENT, addr);
}

bool ClientApplication::Discover::DiscoverNext(int appId)
{
    bool doOnce = true;
    do {
        doOnce = true;
        if (tasks_.empty()) {
            return true;
        }

        auto &task = tasks_.front();
        switch (task.type_) {
            case Task::Type::SERVICE:
                if (discovered_.emplace(task.startHandle_).second) {
                    profile_.DiscoverAllPrimaryServices(
                        appId, client_.connection_.GetHandle(), task.startHandle_, task.endHandle_);
                } else {
                    doOnce = false;
                }
                break;
            case Task::Type::INCLUDE_SERVICE:
                profile_.FindIncludedServices(
                    appId, client_.connection_.GetHandle(), task.startHandle_, task.endHandle_);
                break;
            case Task::Type::CHARACTERISTICS:
                profile_.DiscoverAllCharacteristicOfService(
                    appId, client_.connection_.GetHandle(), task.startHandle_, task.endHandle_);
                break;
            case Task::Type::DESCRIPTOR:
                profile_.DiscoverAllCharacteristicDescriptors(
                    appId, client_.connection_.GetHandle(), task.startHandle_, task.endHandle_);
                break;
            default:
                break;
        }
        tasks_.pop();
    } while (!doOnce);

    return false;
}

void ClientApplication::Discover::Clear()
{
    while (tasks_.size() != 0) {
        tasks_.pop();
    }
    discovered_.clear();
}

void GattClientService::Enable()
{
    if (pimpl->InRunningState()) {
        LOG_ERROR("ProfileService:%{public}s is already start up!", Name().c_str());
        return;
    }

    GetDispatcher()->PostTask(std::bind(&impl::Enable, pimpl.get()));
}

void GattClientService::Disable()
{
    if (!pimpl->InRunningState()) {
        LOG_ERROR("ProfileService:%{public}s is already shut down!", Name().c_str());
        return;
    }

    GetDispatcher()->PostTask(std::bind(&impl::Disable, pimpl.get()));
}

REGISTER_CLASS_CREATOR(GattClientService);
}  // namespace bluetooth
}  // namespace OHOS
