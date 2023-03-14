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

#include "gatt_server_service.h"
#include <future>
#include <set>
#include "adapter_config.h"
#include "class_creator.h"
#include "gatt_based_services_manager.h"
#include "gatt_connection.h"
#include "gatt_connection_manager.h"
#include "gatt_database.h"
#include "gatt_defines.h"
#include "gatt_server_profile.h"
#include "gatt_service_base.h"
#include "interface_profile.h"
#include "log.h"
#include "log_util.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
struct ServerApplication {
    explicit ServerApplication(std::shared_ptr<IGattServerCallback> callback) : services_()
    {
        callback_ = callback;
    }

    std::shared_ptr<IGattServerCallback> callback_;
    // service handle set of application
    std::set<uint16_t> services_;
};

using AppIterator = std::map<int, ServerApplication>::iterator;

struct GattServerService::impl : public GattServiceBase {
    class GattServerProfileCallbackImplement;
    class GattConnectionObserverImplement;

    int connectionObserverId_ = 0;
    GattServerService &self_;
    std::unique_ptr<GattServerProfileCallbackImplement> profileCallback_ = {nullptr};
    std::unique_ptr<GattServerProfile> profile_ = {nullptr};
    std::unique_ptr<GattConnectionObserverImplement> connectionObserver_ = {nullptr};
    std::unique_ptr<GattBasedServicesManager> basedServicesManager_ = {nullptr};
    std::mutex remotesMutex_ = {};
    // connection handle <-> connection entity
    std::map<uint16_t, GattConnection> remotes_ = {};
    // application ID <-> application entity
    std::map<int, ServerApplication> servers_ = {};
    // attribute handle <-> (application ID, parent service handle)
    std::map<uint16_t, std::pair<int, uint16_t>> attHandleMap_ = {};

    explicit impl(GattServerService &service);
    ~impl();

    void RegisterApplication(std::shared_ptr<IGattServerCallback> callback, std::promise<int> &promise);
    void DeregisterApplication(int appId, std::promise<int> &promise);
    int RegisterApplicationImpl(std::shared_ptr<IGattServerCallback> callback);
    int DeregisterApplicationImpl(int appId);

    int AddService(int appId, Service &service, bool IsAsync);
    int RemoveService(int appId, uint16_t handle);
    int ClearServices(int appId);
    void NotifyClient(
        const GattDevice &device, uint16_t valueHandle, const GattValue &value, size_t length, bool needConfirm);
    void RespondCharacteristicRead(const GattDevice &device, uint16_t valueHandle, const GattValue &value,
        size_t length, int ret, bool isUsingUuid);
    void RespondCharacteristicWrite(const GattDevice &device, uint16_t characteristicHandle, int ret);
    void RespondDescriptorRead(
        const GattDevice &device, uint16_t valueHandle, const GattValue &value, size_t length, int ret);
    void RespondDescriptorWrite(const GattDevice &device, uint16_t descriptorHandle, int ret);
    void CancelConnection(const GattDevice &device);
    void SetCharacteristicValue(uint16_t valueHandle, GattValue &value, size_t length);
    void SetCharacteristicPermission(uint16_t valueHandle, uint8_t properties, uint8_t permission);
    void OnExchangeMtuEvent(uint16_t connectionHandle, uint16_t rxMtu);
    void OnReadCharacteristicValueEvent(uint16_t connectionHandle, uint16_t valueHandle, bool isUsingUuid);
    void OnWriteCharacteristicEvent(
        uint16_t connectionHandle, uint16_t valueHandle, GattValue &value, size_t length, bool needRespones);
    void OnDescriptorReadEvent(uint16_t connectionHandle, uint16_t valueHandle);
    void OnDescriptorWriteEvent(uint16_t connectionHandle, uint16_t valueHandle, GattValue &value, size_t length);
    void OnIndicationEvent(uint16_t connectionHandle, uint16_t valueHandle, int ret);
    void OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret);
    void OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret);
    void OnConnectionChanged(const GattDevice &device, int state);
    void OnConnectionParameterChanged(const GattDevice &device, int interval, int latency, int timeout, int status);
    void OnConnetionManagerShutDown();

    std::optional<AppIterator> GetValidApplication(int appId);
    std::optional<uint16_t> GetValidDeviceConnetionHandle(const GattDevice &device);
    std::optional<uint16_t> FindDeviceConnetionHandle(const GattDevice &device);
    bluetooth::Service BuildService(const GattDatabase::Service &src);
    void AddAttHandleMap(int appId, const Service &service);
    static bool IsValidAttHandle(uint16_t handle);
    static uint16_t GetBleServerExchangeMtu();
    std::optional<AppIterator> GetValidApplicationService(int appId, uint16_t handle);
    void NotifyServiceChanged(int appId, const Service &service);

    void Enable();
    void Disable();
    void CleanApplication();
    static void GattUpdatePowerStatus(const RawAddress &addr);
};

GattServerService::GattServerService()
    : utility::Context("GattServerService", "5.0.0"), pimpl(std::make_unique<GattServerService::impl>(*this))
{
    LOG_INFO("ProfileService:%{public}s Create", Name().c_str());
}

GattServerService::~GattServerService()
{
    LOG_INFO("ProfileService:%{public}s Destroy", Name().c_str());
}

utility::Context *GattServerService::GetContext()
{
    return this;
}

std::list<RawAddress> GattServerService::GetConnectDevices()
{
    return std::list<RawAddress>();
}

int GattServerService::GetConnectState()
{
    return 0;
}

int GattServerService::GetMaxConnectNum()
{
    auto maxNum = GattConnectionManager::GetInstance().GetMaximumNumberOfConnections();
    return maxNum.first + maxNum.second;
}

int GattServerService::Connect(const RawAddress &device)
{
    return 0;
}

int GattServerService::Disconnect(const RawAddress &device)
{
    return 0;
}

int GattServerService::CancelConnection(const GattDevice &device)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::CancelConnection, pimpl.get(), device));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::SetCharacteristicValue(const Characteristic &characteristic)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    auto sharedPtr = pimpl->BuildGattValue(characteristic.value_.get(), characteristic.length_);

    GetDispatcher()->PostTask(std::bind(
        &impl::SetCharacteristicValue, pimpl.get(), characteristic.valueHandle_, sharedPtr, characteristic.length_));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::SetCharacteristicPermission(
    const Characteristic &characteristic, uint8_t properties, uint8_t permission)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(
        &impl::SetCharacteristicPermission, pimpl.get(), characteristic.valueHandle_, properties, permission));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::AddService(int appId, Service &service)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s InRunningState false ", __FILE__, __LINE__, __FUNCTION__);
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    int result = pimpl->profile_->CheckLegalityOfServiceDefinition(service);
    if (GattStatus::GATT_SUCCESS != result) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s CheckLegalityOfServiceDefinition failed, result = %{public}d",
            __FILE__, __LINE__, __FUNCTION__, result);
        return result;
    }

    GetDispatcher()->PostTask(std::bind(&impl::AddService, pimpl.get(), appId, service, true));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RemoveService(int appId, const Service &service)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s InRunningState false ", __FILE__, __LINE__, __FUNCTION__);
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (!pimpl->IsValidAttHandle(service.handle_)) {
        return GattStatus::INVALID_PARAMETER;
    }

    GetDispatcher()->PostTask(std::bind(&impl::RemoveService, pimpl.get(), appId, service.handle_));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::ClearServices(int appId)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(std::bind(&impl::ClearServices, pimpl.get(), appId));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::NotifyClient(const GattDevice &device, Characteristic &characteristic, bool needConfirm)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (!pimpl->IsValidAttHandle(characteristic.handle_)) {
        return GattStatus::INVALID_PARAMETER;
    }

    if (characteristic.value_ == nullptr || characteristic.length_ <= 0) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(characteristic.value_);
    GetDispatcher()->PostTask(std::bind(&impl::NotifyClient,
        pimpl.get(),
        device,
        characteristic.valueHandle_,
        sharedPtr,
        characteristic.length_,
        needConfirm));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RespondCharacteristicRead(const GattDevice &device, Characteristic &characteristic, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (!pimpl->IsValidAttHandle(characteristic.handle_)) {
        return GattStatus::INVALID_PARAMETER;
    }

    if (ret == GattStatus::GATT_SUCCESS && (characteristic.value_ == nullptr || characteristic.length_ <= 0)) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(characteristic.value_);
    GetDispatcher()->PostTask(std::bind(&impl::RespondCharacteristicRead,
        pimpl.get(),
        device,
        characteristic.valueHandle_,
        sharedPtr,
        characteristic.length_,
        ret,
        false));
    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RespondCharacteristicReadByUuid(
    const GattDevice &device, Characteristic &characteristic, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (!pimpl->IsValidAttHandle(characteristic.handle_)) {
        return GattStatus::INVALID_PARAMETER;
    }

    if (ret == GattStatus::GATT_SUCCESS && (characteristic.value_ == nullptr || characteristic.length_ <= 0)) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(characteristic.value_);
    GetDispatcher()->PostTask(std::bind(&impl::RespondCharacteristicRead,
        pimpl.get(),
        device,
        characteristic.valueHandle_,
        sharedPtr,
        characteristic.length_,
        ret,
        true));
    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RespondCharacteristicWrite(
    const GattDevice &device, const Characteristic &characteristic, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    GetDispatcher()->PostTask(
        std::bind(&impl::RespondCharacteristicWrite, pimpl.get(), device, characteristic.valueHandle_, ret));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RespondDescriptorRead(const GattDevice &device, Descriptor &descriptor, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (!pimpl->IsValidAttHandle(descriptor.handle_)) {
        return GattStatus::INVALID_PARAMETER;
    }

    if (ret == GattStatus::GATT_SUCCESS && (descriptor.value_ == nullptr || descriptor.length_ <= 0)) {
        return GattStatus::INVALID_PARAMETER;
    }

    auto sharedPtr = pimpl->MoveToGattValue(descriptor.value_);
    GetDispatcher()->PostTask(std::bind(
        &impl::RespondDescriptorRead, pimpl.get(), device, descriptor.handle_, sharedPtr, descriptor.length_, ret));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RespondDescriptorWrite(const GattDevice &device, const Descriptor &descriptor, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    if (!pimpl->IsValidAttHandle(descriptor.handle_)) {
        return GattStatus::INVALID_PARAMETER;
    }

    GetDispatcher()->PostTask(std::bind(&impl::RespondDescriptorWrite, pimpl.get(), device, descriptor.handle_, ret));

    return GattStatus::GATT_SUCCESS;
}

int GattServerService::RegisterApplication(std::shared_ptr<IGattServerCallback> callback)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    GetDispatcher()->PostTask(
        std::bind(&impl::RegisterApplication, pimpl.get(), std::ref(callback), std::ref(promise)));

    return future.get();
}

int GattServerService::DeregisterApplication(int appId)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    GetDispatcher()->PostTask(std::bind(&impl::DeregisterApplication, pimpl.get(), appId, std::ref(promise)));

    return future.get();
}

int GattServerService::RegisterApplicationSync(std::shared_ptr<IGattServerCallback> callback) const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    return pimpl->RegisterApplicationImpl(callback);
}

int GattServerService::DeregisterApplicationSync(int appId) const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s:%{public}d:", __FILE__, __LINE__, __FUNCTION__, appId);
    if (!pimpl->InRunningState()) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    return pimpl->DeregisterApplicationImpl(appId);
}

int GattServerService::AddServiceSync(int appId, Service &service) const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    int result = pimpl->profile_->CheckLegalityOfServiceDefinition(service);
    if (GattStatus::GATT_SUCCESS != result) {
        return result;
    }

    return pimpl->AddService(appId, service, false);
}

const std::string GattServerService::GetDatabaseHash() const
{
    return pimpl->profile_->GetDBHash();
}

class GattServerService::impl::GattConnectionObserverImplement : public GattConnectionObserver {
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
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnConnectionChanged, service_.pimpl.get(), device, state));
    }

    void OnConnectionParameterChanged(
        const GattDevice &device, int interval, int latency, int timeout, int status) override
    {
        LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        service_.GetDispatcher()->PostTask(std::bind(
            &impl::OnConnectionParameterChanged, service_.pimpl.get(), device, interval, latency, timeout, status));
    }

    void OnShutDown() override
    {
        LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        service_.GetDispatcher()->PostTask(std::bind(&impl::OnConnetionManagerShutDown, service_.pimpl.get()));
    }

    explicit GattConnectionObserverImplement(GattServerService &service) : service_(service)
    {}
    ~GattConnectionObserverImplement()
    {}

private:
    GattServerService &service_;
};

class GattServerService::impl::GattServerProfileCallbackImplement : public GattServerProfileCallback {
public:
    void OnExchangeMtuEvent(uint16_t connectHandle, uint16_t rxMtu) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnExchangeMtuEvent, service_.pimpl.get(), connectHandle, rxMtu));
    }

    void OnReadCharacteristicValueEvent(uint16_t connectHandle, uint16_t valueHandle) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnReadCharacteristicValueEvent, service_.pimpl.get(), connectHandle, valueHandle, false));
    }

    void OnReadUsingCharacteristicUuidEvent(uint16_t connectHandle, uint16_t valueHandle) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnReadCharacteristicValueEvent, service_.pimpl.get(), connectHandle, valueHandle, true));
    }

    void OnWriteWithoutResponseEvent(
        uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len) override
    {
        service_.GetDispatcher()->PostTask(std::bind(
            &impl::OnWriteCharacteristicEvent, service_.pimpl.get(), connectHandle, valueHandle, value, len, false));
    }

    void OnWriteCharacteristicValueEvent(
        uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len) override
    {
        service_.GetDispatcher()->PostTask(std::bind(
            &impl::OnWriteCharacteristicEvent, service_.pimpl.get(), connectHandle, valueHandle, value, len, true));
    }

    void OnIndicationEvent(uint16_t connectHandle, uint16_t valueHandle, int result) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnIndicationEvent, service_.pimpl.get(), connectHandle, valueHandle, result));
    }

    void OnReliableWriteEvent(uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len) override
    {
    }

    void OnExecuteWriteEvent(uint16_t connectHandle, uint16_t valueHandle, int state) override
    {
    }

    void OnDescriptorReadEvent(uint16_t connectHandle, uint16_t valueHandle) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnDescriptorReadEvent, service_.pimpl.get(), connectHandle, valueHandle));
    };

    void OnDescriptorWriteEvent(uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len) override
    {
        service_.GetDispatcher()->PostTask(
            std::bind(&impl::OnDescriptorWriteEvent, service_.pimpl.get(), connectHandle, valueHandle, value, len));
    };

    GattServerProfileCallbackImplement(GattServerService &service) : service_(service)
    {}
    ~GattServerProfileCallbackImplement()
    {}

private:
    GattServerService &service_;
};

void GattServerService::impl::Enable()
{
    profile_->Enable();

    CleanApplication();

    Start();

    basedServicesManager_->Enable();

    self_.GetContext()->OnEnable(PROFILE_NAME_GATT_SERVER, true);
}

void GattServerService::impl::Disable()
{
    basedServicesManager_->Disable();

    Stop();

    profile_->Disable();
}

void GattServerService::impl::CleanApplication()
{
    servers_.clear();
    attHandleMap_.clear();
}

void GattServerService::impl::GattUpdatePowerStatus(const RawAddress &addr)
{
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_GATT_SERVER, addr);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_GATT_SERVER, addr);
}

void GattServerService::Enable()
{
    if (pimpl->InRunningState()) {
        LOG_ERROR("ProfileService:%{public}s is already start up!", Name().c_str());
        return;
    }

    GetDispatcher()->PostTask(std::bind(&impl::Enable, pimpl.get()));
}

void GattServerService::Disable()
{
    if (!pimpl->InRunningState()) {
        LOG_ERROR("ProfileService:%{public}s is already shut down!", Name().c_str());
        return;
    }

    GetDispatcher()->PostTask(std::bind(&impl::Disable, pimpl.get()));
}

GattServerService::impl::impl(GattServerService &service)
    : self_(service),
      profileCallback_(std::make_unique<GattServerProfileCallbackImplement>(service)),
      profile_(std::make_unique<GattServerProfile>(
          profileCallback_.get(), service.GetDispatcher(), GetBleServerExchangeMtu())),
      connectionObserver_(std::make_unique<GattConnectionObserverImplement>(service)),
      basedServicesManager_(std::make_unique<GattBasedServicesManager>(service, *service.GetDispatcher()))
{
    connectionObserverId_ = GattConnectionManager::GetInstance().RegisterObserver(*connectionObserver_);
    if (connectionObserverId_ < 0) {
        LOG_ERROR("%{public}s:%{public}s:%{public}d register to connection manager failed!", __FILE__, __FUNCTION__, __LINE__);
    }
}

GattServerService::impl::~impl()
{
    GattConnectionManager::GetInstance().DeregisterObserver(connectionObserverId_);
}

int GattServerService::impl::RegisterApplicationImpl(std::shared_ptr<IGattServerCallback> callback)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (servers_.size() >= MAXIMUM_NUMBER_APPLICATION) {
        return GattStatus::MAX_APPLICATIONS;
    }

    int result = GattStatus::GATT_FAILURE;
    int appid = GattServiceBase::GetApplicationId();
    if (servers_.emplace(appid, std::move(ServerApplication(callback))).second) {
        result = GattStatus::GATT_SUCCESS;
    } else {
        LOG_ERROR("%{public}s:%{public}s:%{public}d register to application failed! same application ID!", __FILE__, __FUNCTION__, __LINE__);
    }

    return ((result == GattStatus::GATT_SUCCESS) ? appid : result);
}

int GattServerService::impl::DeregisterApplicationImpl(int appId)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto server = GetValidApplication(appId);
    if (server.has_value()) {
        ClearServices(appId);
        servers_.erase(appId);
    } else {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Invalid application Id");
    }

    return GattStatus::GATT_SUCCESS;
}

void GattServerService::impl::RegisterApplication(
    std::shared_ptr<IGattServerCallback> callback, std::promise<int> &promise)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    promise.set_value(RegisterApplicationImpl(callback));
}

void GattServerService::impl::DeregisterApplication(int appId, std::promise<int> &promise)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    promise.set_value(DeregisterApplicationImpl(appId));
}

int GattServerService::impl::AddService(int appId, Service &service, bool IsAsync)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto server = GetValidApplication(appId);
    int result = GattStatus::GATT_FAILURE;
    if (server.has_value()) {
        result = profile_->AddService(service);
        if (GattStatus::GATT_SUCCESS == result) {
            server.value()->second.services_.emplace(service.handle_);
            AddAttHandleMap(appId, service);
            NotifyServiceChanged(appId, service);
        }

        if (IsAsync && server.value()->second.callback_ != nullptr) {
            LOG_INFO("%{public}s:%{public}d:%{public}s: call OnAddService ", __FILE__, __LINE__, __FUNCTION__);
            server.value()->second.callback_->OnAddService(result, service);
        }
    }
    return result;
}

int GattServerService::impl::RemoveService(int appId, uint16_t handle)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto server = GetValidApplicationService(appId, handle);
    if (!server.has_value()) {
        LOG_INFO("%{public}s:%{public}d:%{public}s() %{public}s",
            __FILE__,
            __LINE__,
            __FUNCTION__,
            "Server Application Delete Service that not belong to it.");
        return GattStatus::GATT_FAILURE;
    }

    auto deleteService = profile_->GetService(handle);
    if (deleteService == nullptr) {
        return GattStatus::GATT_FAILURE;
    }

    Service change(deleteService->handle_, deleteService->endHandle_);
    int result = profile_->RemoveService(handle);
    if (GattStatus::GATT_SUCCESS != result) {
        return result;
    }

    NotifyServiceChanged(appId, change);

    std::list<uint16_t> needDeleteList;
    for (auto attHandle : attHandleMap_) {
        if (attHandle.second.second == handle) {
            needDeleteList.push_back(attHandle.first);
        }
    }
    for (auto attHandle : needDeleteList) {
        attHandleMap_.erase(attHandle);
    }

    server.value()->second.services_.erase(handle);
    return GattStatus::GATT_SUCCESS;
}

int GattServerService::impl::ClearServices(int appId)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto server = GetValidApplication(appId);
    int result = GattStatus::GATT_SUCCESS;
    if (server.has_value()) {
        std::vector<uint16_t> needDelHandle;
        std::copy(server.value()->second.services_.begin(),
            server.value()->second.services_.end(),
            std::back_inserter(needDelHandle));

        for (auto handle : needDelHandle) {
            result = RemoveService(appId, handle);
            if (result != GATT_SUCCESS) {
                break;
            }
        }
    }
    return result;
}

void GattServerService::impl::NotifyClient(
    const GattDevice &device, uint16_t valueHandle, const GattValue &value, size_t length, bool needConfirm)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s:Confirm:%{public}d", __FILE__, __LINE__, __FUNCTION__, needConfirm);
    auto handle = FindDeviceConnetionHandle(device);
    if (handle.has_value()) {
        auto connectionHandle = handle.value();
        if (needConfirm) {
            profile_->SendIndication(connectionHandle, valueHandle, value, length);
        } else {
            profile_->SendNotification(connectionHandle, valueHandle, value, length);
        }

        if (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(device.addr_);
        }
    }
}

void GattServerService::impl::RespondCharacteristicRead(
    const GattDevice &device, uint16_t valueHandle, const GattValue &value, size_t length, int ret, bool isUsingUuid)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto handle = GetValidDeviceConnetionHandle(device);
    if (handle.has_value()) {
        auto connectionHandle = handle.value();
        if (!isUsingUuid) {
            profile_->SendReadCharacteristicValueResp(connectionHandle, valueHandle, value, length, ret);
        } else {
            profile_->SendReadUsingCharacteristicValueResp(connectionHandle, valueHandle, value, length, ret);
        }
    }
}

void GattServerService::impl::RespondCharacteristicWrite(
    const GattDevice &device, uint16_t characteristicHandle, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto handle = GetValidDeviceConnetionHandle(device);
    if (handle.has_value()) {
        auto connectionHandle = handle.value();
        profile_->SendWriteCharacteristicValueResp(connectionHandle, characteristicHandle, ret);
    }
}

void GattServerService::impl::RespondDescriptorRead(
    const GattDevice &device, uint16_t valueHandle, const GattValue &value, size_t length, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto handle = GetValidDeviceConnetionHandle(device);
    if (handle.has_value()) {
        auto connectionHandle = handle.value();
        profile_->SendReadDescriptorResp(connectionHandle, valueHandle, value, length, ret);
    }
}

void GattServerService::impl::RespondDescriptorWrite(const GattDevice &device, uint16_t DescriptorHandle, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto handle = GetValidDeviceConnetionHandle(device);
    if (handle.has_value()) {
        auto connectionHandle = handle.value();
        profile_->SendWriteDescriptorResp(connectionHandle, DescriptorHandle, ret);
    }
}

void GattServerService::impl::CancelConnection(const GattDevice &device)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto &manager = GattConnectionManager::GetInstance();
    auto result = manager.Disconnect(device);
    if (GattStatus::GATT_SUCCESS != result) {
        if (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::CONNECT_OFF, PROFILE_NAME_GATT_SERVER, device.addr_);
        }

        for (auto &server : servers_) {
            if (server.second.callback_ != nullptr) {
                server.second.callback_->OnConnectionStateChanged(
                    device, result, ConvertConnectionState(manager.GetDeviceState(device)));
            }
        }
    }
}

void GattServerService::impl::SetCharacteristicValue(uint16_t valueHandle, GattValue &value, size_t length)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    GattDatabase::AttributeValue attValue;
    attValue.value_ = std::move(*value);
    attValue.length_ = length;
    profile_->SetAttributeValue(valueHandle, attValue);
}

void GattServerService::impl::SetCharacteristicPermission(uint16_t valueHandle, uint8_t properties, uint8_t permission)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto ccc = profile_->GetCharacteristic(valueHandle);
    if (ccc != nullptr) {
        ccc->properties_ = properties;
        auto cccValue = profile_->GetAttributeEntity(valueHandle);
        if (cccValue.has_value()) {
            cccValue.value().get().permissions_ = permission;
        }
    }
}

void GattServerService::impl::OnExchangeMtuEvent(uint16_t connectionHandle, uint16_t rxMtu)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        if (remote->second.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(remote->second.GetDevice().addr_);
        }

        remote->second.SetMtu(rxMtu);
        for (auto &server : servers_) {
            if (server.second.callback_ != nullptr) {
                server.second.callback_->OnMtuChanged(remote->second.GetDevice(), rxMtu);
            }
        }
    }
}

void GattServerService::impl::OnReadCharacteristicValueEvent(
    uint16_t connectionHandle, uint16_t valueHandle, bool isUsingUuid)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto appId = attHandleMap_.find(valueHandle);
    if (appId == attHandleMap_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Attribute map error!");
        return;
    }

    auto server = servers_.find(appId->second.first);
    if (server == servers_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Server app map error!");
        return;
    }

    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        if (remote->second.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(remote->second.GetDevice().addr_);
        }

        auto ccc = profile_->GetCharacteristic(valueHandle);
        if (ccc == nullptr) {
            LOG_ERROR("%{public}s:%{public}d:%{public}s Gatt database Error: Can not find Characteristic by value handle.",
                __FILE__,
                __LINE__,
                __FUNCTION__);
            return;
        }

        if (server->second.callback_ != nullptr) {
            if (!isUsingUuid) {
                server->second.callback_->OnCharacteristicReadRequest(
                    remote->second.GetDevice(), Characteristic(ccc->uuid_, ccc->handle_, ccc->properties_));
            } else {
                server->second.callback_->OnCharacteristicReadByUuidRequest(
                    remote->second.GetDevice(), Characteristic(ccc->uuid_, ccc->handle_, ccc->properties_));
            }
        }
    }
}

void GattServerService::impl::OnWriteCharacteristicEvent(
    uint16_t connectionHandle, uint16_t valueHandle, GattValue &value, size_t length, bool needRespones)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto appId = attHandleMap_.find(valueHandle);
    if (appId == attHandleMap_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Attribute map error!");
        return;
    }

    auto server = servers_.find(appId->second.first);
    if (server == servers_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Server app map error!");
        return;
    }

    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        if (remote->second.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(remote->second.GetDevice().addr_);
        }

        auto ccc = profile_->GetCharacteristic(valueHandle);
        if (ccc == nullptr) {
            LOG_ERROR("%{public}s:%{public}d:%{public}s Gatt database Error: Can not find Characteristic by value handle.",
                __FILE__,
                __LINE__,
                __FUNCTION__);
            return;
        }

        Characteristic gattCCC(ccc->uuid_, ccc->handle_, ccc->properties_);
        gattCCC.length_ = length;
        gattCCC.value_ = std::move(*value);
        if (server->second.callback_ != nullptr) {
            server->second.callback_->OnCharacteristicWriteRequest(remote->second.GetDevice(), gattCCC, needRespones);
        }
    }
}

void GattServerService::impl::OnDescriptorReadEvent(uint16_t connectionHandle, uint16_t valueHandle)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto appId = attHandleMap_.find(valueHandle);
    if (appId == attHandleMap_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Attribute map error!");
        return;
    }

    auto server = servers_.find(appId->second.first);
    if (server == servers_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Server app map error!");
        return;
    }

    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        if (remote->second.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(remote->second.GetDevice().addr_);
        }

        auto descriptor = profile_->GetDescriptor(valueHandle);
        if (descriptor == nullptr) {
            LOG_ERROR("%{public}s:%{public}d:%{public}s Gatt database Error: Can not find Descriptor by value handle.",
                __FILE__,
                __LINE__,
                __FUNCTION__);
            return;
        }
        if (server->second.callback_ != nullptr) {
            server->second.callback_->OnDescriptorReadRequest(
                remote->second.GetDevice(), Descriptor(descriptor->handle_, descriptor->uuid_,
                descriptor->permissions_));
        }
    }
}

void GattServerService::impl::OnDescriptorWriteEvent(
    uint16_t connectionHandle, uint16_t valueHandle, GattValue &value, size_t length)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto appId = attHandleMap_.find(valueHandle);
    if (appId == attHandleMap_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Attribute map error!");
        return;
    }

    auto server = servers_.find(appId->second.first);
    if (server == servers_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Server app map error!");
        return;
    }

    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        if (remote->second.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(remote->second.GetDevice().addr_);
        }

        auto descriptor = profile_->GetDescriptor(valueHandle);
        Descriptor gattDescriptor(descriptor->handle_, descriptor->uuid_, descriptor->permissions_);
        gattDescriptor.length_ = length;
        gattDescriptor.value_ = std::move(*value);
        if (server->second.callback_ != nullptr) {
            server->second.callback_->OnDescriptorWriteRequest(remote->second.GetDevice(), gattDescriptor);
        }
    }
}

void GattServerService::impl::OnIndicationEvent(uint16_t connectionHandle, uint16_t valueHandle, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto appId = attHandleMap_.find(valueHandle);
    if (appId == attHandleMap_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Attribute map error!");
        return;
    }

    auto server = servers_.find(appId->second.first);
    if (server == servers_.end()) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s() %{public}s", __FILE__, __LINE__, __FUNCTION__, "Server app map error!");
        return;
    }

    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        if (remote->second.GetDevice().transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            GattUpdatePowerStatus(remote->second.GetDevice().addr_);
        }

        auto ccc = profile_->GetCharacteristic(valueHandle);
        if (ccc == nullptr) {
            LOG_ERROR("%{public}s:%{public}d:%{public}s Gatt database Error: Can not find Characteristic by value handle.",
                __FILE__,
                __LINE__,
                __FUNCTION__);
            return;
        }
        if (server->second.callback_ != nullptr) {
            server->second.callback_->OnNotifyConfirm(
                remote->second.GetDevice(), Characteristic(ccc->uuid_, ccc->handle_, ccc->properties_), ret);
        }
    }
}

void GattServerService::impl::OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret)
{
    LOG_INFO("%{public}s: server service dev_role: %{public}d, ret: %{public}d", __FUNCTION__, device.role_, ret);
    if (GattStatus::GATT_SUCCESS == ret) {
        std::lock_guard<std::mutex> lck(remotesMutex_);
        remotes_.emplace(connectionHandle, GattConnection(device, 0, connectionHandle));

        if (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC) {
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::CONNECT_ON, PROFILE_NAME_GATT_SERVER, device.addr_);
        }
    }
    if (device.role_ == GATT_ROLE_SLAVE) {
        for (auto &server : servers_) {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __PRETTY_FUNCTION__);
            if (server.second.callback_ != nullptr) {
                server.second.callback_->OnConnectionStateChanged(device, ret,
                    static_cast<int>(BTConnectState::CONNECTED));
            }
        }
    }
}

void GattServerService::impl::OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    bool findDevice = false;
    auto remote = remotes_.find(connectionHandle);
    if (remote != remotes_.end()) {
        std::lock_guard<std::mutex> lck(remotesMutex_);
        remotes_.erase(connectionHandle);
        findDevice = true;
    } else {
        for (auto it = remotes_.begin(); it != remotes_.end(); it++) {
            if (it->second.GetDevice() == device) {
                remotes_.erase(it);
                findDevice = true;
                break;
            }
        }
    }

    if (!findDevice) {
        return;
    }

    if (device.transport_ == GATT_TRANSPORT_TYPE_CLASSIC && ret == GattStatus::GATT_SUCCESS) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_GATT_SERVER, device.addr_);
    }

    if (device.role_ == GATT_ROLE_SLAVE) {
        LOG_INFO("%{public}s: server service dev_role: %{public}d", __FUNCTION__, device.role_);
        for (auto &server : servers_) {
            LOG_DEBUG("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __PRETTY_FUNCTION__);
            if (server.second.callback_ != nullptr) {
                server.second.callback_->OnConnectionStateChanged(device, ret,
                    static_cast<int>(BTConnectState::DISCONNECTED));
            }
        }
    }
}

void GattServerService::impl::OnConnectionChanged(const GattDevice &device, int state)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (device.role_ != GATT_ROLE_SLAVE) {
        LOG_ERROR("%{public}s: device role is %{public}d", __FUNCTION__, device.role_);
        return;
    }
    if (state == static_cast<int>(BTConnectState::CONNECTED)) {
        return;
    }
    for (auto &server : servers_) {
        if (server.second.callback_ != nullptr) {
            server.second.callback_->OnConnectionStateChanged(device, GattStatus::GATT_SUCCESS, state);
        }
    }
}

void GattServerService::impl::OnConnectionParameterChanged(
    const GattDevice &device, int interval, int latency, int timeout, int status)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    for (auto &server : servers_) {
        if (server.second.callback_ != nullptr) {
            server.second.callback_->OnConnectionParameterChanged(device, interval, latency, timeout, status);
        }
    }
}

void GattServerService::impl::OnConnetionManagerShutDown()
{
    self_.GetContext()->OnDisable(PROFILE_NAME_GATT_SERVER, true);
}

bluetooth::Service GattServerService::impl::BuildService(const GattDatabase::Service &src)
{
    bluetooth::Service dst(src.uuid_, src.handle_, src.handle_, src.endHandle_);
    for (auto &inclueService : src.includeServices_) {
        dst.includeServices_.push_back(bluetooth::Service(
            inclueService.uuid_, inclueService.handle_, inclueService.startHandle_, inclueService.endHandle_));
    }

    for (auto &ccc : src.characteristics_) {
        auto cccValue = profile_->GetAttributeEntity(ccc.second.valueHandle_);

        bluetooth::Characteristic gattCCC(ccc.second.uuid_,
            ccc.second.handle_,
            ccc.second.properties_,
            cccValue.value().get().permissions_,
            cccValue.value().get().value_.value_.get(),
            cccValue.value().get().value_.length_);

        for (auto &descriptor : ccc.second.descriptors_) {
            auto descriptorValue = profile_->GetAttributeEntity(descriptor.second.handle_);

            bluetooth::Descriptor gattDescriptor(descriptorValue.value().get().type_,
                descriptor.second.handle_,
                descriptorValue.value().get().permissions_,
                descriptorValue.value().get().value_.value_.get(),
                descriptorValue.value().get().value_.length_);

            gattCCC.descriptors_.push_back(std::move(gattDescriptor));
        }

        dst.characteristics_.push_back(std::move(gattCCC));
    }

    return dst;
}

void GattServerService::impl::AddAttHandleMap(int appId, const bluetooth::Service &service)
{
    for (auto &ccc : service.characteristics_) {
        attHandleMap_.emplace(ccc.valueHandle_, std::make_pair(appId, service.handle_));
        for (auto &descriptor : ccc.descriptors_) {
            attHandleMap_.emplace(descriptor.handle_, std::make_pair(appId, service.handle_));
        }
    }
}

bool GattServerService::impl::IsValidAttHandle(uint16_t handle)
{
    return (handle >= MIN_ATTRIBUTE_HANDLE && handle <= MAX_ATTRIBUTE_HANDLE);
}

uint16_t GattServerService::impl::GetBleServerExchangeMtu()
{
    int result = DEFAULT_BLE_GATT_SERVER_EXCHANGE_MTU;
    if (AdapterConfig::GetInstance()->GetValue(SECTION_GATT_SERVICE, PROPERTY_BLE_GATTSERVER_EXCHANGE_MTU, result)) {
        return result;
    }
    return result;
}

std::optional<AppIterator> GattServerService::impl::GetValidApplication(int appId)
{
    auto it = servers_.find(appId);
    if (it != servers_.end()) {
        return it;
    }
    return std::nullopt;
}

std::optional<uint16_t> GattServerService::impl::GetValidDeviceConnetionHandle(const GattDevice &device)
{
    for (auto &remote : remotes_) {
        if (remote.second.GetDevice() == device) {
            return remote.first;
        }
    }
    return std::nullopt;
}

std::optional<uint16_t> GattServerService::impl::FindDeviceConnetionHandle(const GattDevice &device)
{
    for (auto &remote : remotes_) {
        if (remote.second.GetDevice().addr_ == device.addr_) {
            return remote.first;
        }
    }
    return std::nullopt;
}

std::optional<AppIterator> GattServerService::impl::GetValidApplicationService(int appId, uint16_t handle)
{
    auto server = servers_.find(appId);
    if (server != servers_.end()) {
        auto service = server->second.services_.find(handle);
        if (service != server->second.services_.end()) {
            return server;
        }
    }
    return std::nullopt;
}

void GattServerService::impl::NotifyServiceChanged(int appId, const Service &service)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    for (auto& server : servers_) {
        if (server.first == appId) {
            continue;
        }
        LOG_INFO("%{public}d:%{public}s: call OnServiceChanged", __LINE__, __FUNCTION__);
        if (server.second.callback_ != nullptr) {
            server.second.callback_->OnServiceChanged(service);
        }
    }
}

REGISTER_CLASS_CREATOR(GattServerService);
}  // namespace bluetooth
}  // namespace OHOS
