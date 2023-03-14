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

#include "gatt_client_profile.h"
#include "att.h"
#include "bt_def.h"
#include "gatt_connection_manager.h"
#include "gatt_profile_defines.h"
#include "gatt_service_base.h"
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct GattClientProfile::impl {
    class GattConnectionObserverImplement;
    GattClientProfileCallback *pClientCallBack_ = nullptr;
    int connectionObserverId_ = 0;
    utility::Dispatcher *dispatcher_;
    std::map<uint16_t, GattCache> cacheMap_ = {};
    std::map<uint16_t, MtuInfo> mtuInfo_ = {};
    std::list<std::pair<uint16_t, GattRequestInfo>> requestList_ = {};
    std::list<std::pair<uint16_t, GattRequestInfo>> responseList_ = {};
    std::list<std::pair<uint16_t, ReadValCache>> readValCache_ = {};
    std::unique_ptr<GattConnectionObserverImplement> connectionCallBack_ = {};
    GattClientProfile *profile_ = nullptr;
    impl(GattClientProfileCallback *pClientCallbackFunc, utility::Dispatcher *dispatcher, GattClientProfile &profile)
        : pClientCallBack_(pClientCallbackFunc),
          dispatcher_(dispatcher),
          connectionCallBack_(std::make_unique<GattConnectionObserverImplement>(profile))
    {}
    BT_DISALLOW_COPY_AND_ASSIGN(impl);

    static void ReceiveData(uint16_t connectHandle, uint16_t event, void *eventData, Buffer *buffer, void *context);
    static void ReceiveRequestResult(uint16_t connectHandle, int result, void *context);
    void RegisterCallbackToATT();
    static void DeregisterCallbackToATT();
    void RegisterCallbackToConnectManager();
    void DeregisterCallbackToConnectManager();
    void ReceiveRequestResultProcess(uint16_t connectHandle);
    void IndicateRequestRetToService(int reqId, uint16_t connectHandle, ResponesType reqType, uint8_t errorCode);
    void ReceiveDataProcess(uint16_t connectHandle, uint16_t event, AttEventData *data, Buffer *buffer,
        std::list<std::pair<uint16_t, GattRequestInfo>>::iterator attResp);
    void SetMtuInformation(uint16_t connectHandle, bool isExchanged, uint16_t mtu);
    MtuInfo GetMtuInformation(uint16_t connectHandle);
    void AddResponseList(void);
    void RemoveRequestList(std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ErrorResponseParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ErrorResponseParsing(uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ExchangeMtuParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void DiscoverAllPrimaryServiceParsing(int reqId, uint16_t connectHandle, AttEventData *data);
    void DiscoverPrimaryServiceByUuidParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void FindIncludeServicesParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void FindIncludeServicesParsing(uint16_t connectHandle, uint16_t handle, Buffer *buffer);
    void DiscoverAllCharacteristicOfServiceParsing(uint16_t connectHandle, AttEventData *data, bool isByUuid,
        std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SendDiscoverAllCharacteristicRequest(uint16_t connectHandle, uint16_t startHandle, uint16_t valueHandle,
        Uuid uuid, bool isByUuid, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void DiscoverCharacteristicByUUIDParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void DiscoverAllCharacteristicDescriptorsParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ReadCharacteristicValueParsing(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ReadLongCharacteristicValueParsing(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ReadUsingCharacteristicByUuidParsing(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ReadMultipleCharacteristicParsing(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ReadCharacteristicDescriptorsParsing(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ReadLongCharacteristicDescriptorsParsing(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void WriteCharacteristicValueParsing(
        uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void WriteCharacteristicDescriptorsParsing(
        uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void WriteLongCharacteristicValueParsing(uint16_t connectHandle, uint16_t handle, uint16_t offset, Buffer *buffer,
        std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void ExecuteWriteParsing(uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void NotificationParsing(uint16_t connectHandle, AttEventData *data, Buffer *buffer);
    void IndicationParsing(uint16_t connectHandle, const AttEventData *data, Buffer *buffer);
    void GattRequestTimeoutParsing(int reqId, uint16_t connectHandle, ResponesType respType);
    static int ConvertResponseErrorCode(uint8_t errorCode);
    void SplitReadByTypeRsp(
        uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SplitReadRsp(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SplitReadBlobRsp(
        uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SplitWriteRsp(uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SplitPrepareWriteRsp(uint16_t connectHandle, AttEventData *data, Buffer *buffer,
        std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SplitReadByTypeReqErrorParsing(
        uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter);
    void SplitReadReqErrorParsing(
        int reqId, uint16_t connectHandle, ResponesType respType, uint16_t handle, int result);
    void SplitWriteReqErrorParsing(
        int reqId, uint16_t connectHandle, ResponesType respType, uint16_t handle, int result);
    static uint16_t SplitDataPackageToUint16(uint8_t *src, uint8_t *offset);
    static uint8_t SplitDataPackageToUint8(const uint8_t *src, uint8_t *offset);
    static void SplitDataPackage(uint8_t *dest, uint8_t destMax, uint8_t *offset, uint8_t *src, uint8_t size);
    static Uuid SplitUuidPackage(const uint8_t *value, uint8_t len);
    void AddReadValueCache(uint16_t connectHandle, uint16_t handle, uint16_t offset, Buffer *data);
    uint8_t *GetReadValueCache(uint16_t connectHandle, uint16_t handle);
    void CreateCache(uint16_t connectHandle, const GattDevice device);
    void DeleteCache(uint16_t connectHandle, const GattDevice device);
    void DeleteList(uint16_t connectHandle);
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator FindIteratorByRequestInfor(uint16_t connectHandle);
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator FindIteratorByRespones(
        ResponesType reqType, int reqId);
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator FindIteratorByResponesInfor(
        uint16_t handle, uint16_t respType);
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator FindIteratorByResponesInfor(
        uint16_t handle, ResponesType respType);
};
/**
 * @brief A constructor used to create <pClientCallbackFunc> and <dispatcher> instance..
 *
 * @since 6.0

 */
GattClientProfile::GattClientProfile(GattClientProfileCallback *pClientCallbackFunc, utility::Dispatcher *dispatcher)
    : pimpl(new (std::nothrow)GattClientProfile::impl(pClientCallbackFunc, dispatcher, *this))
{
    if (pimpl == nullptr) {
        LOG_ERROR("GattClientProfile get pimpl nullptr.");
        return;
    }
    pimpl->RegisterCallbackToConnectManager();
}
/**
 * @brief A destructor used to delete the <pClientCallbackFunc> and <dispatcher> instance.
 *
 * @since 6.0
 */
GattClientProfile::~GattClientProfile()
{
    pimpl->DeregisterCallbackToConnectManager();
}
/**
 * @brief Enable gatt client profile.
 *
 * @since 6.0
 */
void GattClientProfile::Enable() const
{
    pimpl->cacheMap_.clear();
    pimpl->requestList_.clear();
    pimpl->responseList_.clear();
    pimpl->mtuInfo_.clear();
    pimpl->RegisterCallbackToATT();
}
/**
 * @brief Disable gatt client profile.
 *
 * @since 6.0
 */
void GattClientProfile::Disable() const
{
    pimpl->DeregisterCallbackToATT();
}
/**
 * @brief Register callback function to ATT.
 *
 * @since 6.0
 */
void GattClientProfile::impl::RegisterCallbackToATT()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    attCallback attDataCallback = GattClientProfile::impl::ReceiveData;
    attSendDataCallback attReqCallback = GattClientProfile::impl::ReceiveRequestResult;

    ATT_ClientDataRegister(attDataCallback, this);
    ATT_ClientSendDataRegister(attReqCallback, this);
}
/**
 * @brief Unregister callback function to att.
 *
 * @since 6.0
 */
void GattClientProfile::impl::DeregisterCallbackToATT()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    ATT_ClientDataDeregister();
    ATT_ClientSendDataDeRegister();
}
/**
 * @brief This sub-procedure is used by the client to set the ATT_MTU to the maximum possible value.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param mtu Indicates att bearer transmission unit.
 * @since 6.0
 */
void bluetooth::GattClientProfile::ExchangeMtu(int reqId, uint16_t connectHandle, uint16_t mtu) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    if (GattConnectionManager::GetInstance().GetDeviceTransport(connectHandle) == GATT_TRANSPORT_TYPE_CLASSIC) {
        pimpl->pClientCallBack_->OnExchangeMtuEvent(reqId, connectHandle, DEFAULT_CLASSIC_CONNECTION_MTU, false);
    } else if (!pimpl->GetMtuInformation(connectHandle).isExchanged_) {
        pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(EXCHANGE_MTU, mtu, reqId));
        LOG_DEBUG("%{public}s: Add requestList_: EXCHANGE_MTU", __FUNCTION__);
        ATT_ExchangeMTURequest(connectHandle, mtu);
    } else {
        pimpl->pClientCallBack_->OnExchangeMtuEvent(
            reqId, connectHandle, pimpl->GetMtuInformation(connectHandle).mtu_, false);
    }
}
/**
 * @brief This sub-procedure is used by a client to discover all the primary services on a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates starting handle of the specified service.
 * @param endHandle Indicates ending handle of the specified service.n.
 * @since 6.0
 */
void bluetooth::GattClientProfile::DiscoverAllPrimaryServices(
    int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, Add requestList_: DISCOVER_ALL_PRIMARY_SERVICE.", __FUNCTION__, connectHandle);
    BtUuid primarySvcUuid = {BT_UUID_16, {UUID_PRIMARY_SERVICE}};
    pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(DISCOVER_ALL_PRIMARY_SERVICE, reqId));
    ATT_ReadByGroupTypeRequest(connectHandle, startHandle, endHandle, &primarySvcUuid);
}

/**
 * @brief This sub-procedure is used by a client to discover all the primary services on a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates starting handle of the specified service.
 * @param endHandle Indicates ending handle of the specified service.n.
 * @since 6.0
 */
void bluetooth::GattClientProfile::DiscoverAllPrimaryServicesInter(
    int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const
{
    LOG_INFO("%{public}s: connectHandle is %{public}hu, Add requestList_: DISCOVER_ALL_PRIMARY_SERVICE.",
        __FUNCTION__, connectHandle);
    auto iter = pimpl->FindIteratorByRespones(DISCOVER_ALL_PRIMARY_SERVICE, reqId);
    if (iter == pimpl->responseList_.end()) {
        LOG_INFO("%{public}s: not find connectionHandle(%{public}hu) in response list", __FUNCTION__, connectHandle);
        return;
    }
    pimpl->responseList_.erase(iter);

    BtUuid primarySvcUuid = {BT_UUID_16, {UUID_PRIMARY_SERVICE}};
    pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(DISCOVER_ALL_PRIMARY_SERVICE, reqId));
    ATT_ReadByGroupTypeRequest(connectHandle, startHandle, endHandle, &primarySvcUuid);
}

/**
 * @brief This sub-procedure is used by a client to discover a specific primary service on a server when only
 * the Service UUID is known.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param uuid Indicates service uuid.
 * @since 6.0
 */
void GattClientProfile::DiscoverPrimaryServicesByServiceUuid(int reqId, uint16_t connectHandle, const Uuid &uuid) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint8_t uuid128[UUID_128BIT_LEN] = {0};
    Buffer *serviceUuid = BufferMalloc(uuid.GetUuidType());
    AttFindByTypeValueReq parameter;
    parameter.attType = UUID_PRIMARY_SERVICE;
    parameter.handleRange.startHandle = MIN_ATTRIBUTE_HANDLE;
    parameter.handleRange.endHandle = MAX_ATTRIBUTE_HANDLE;

    if (uuid.GetUuidType() == UUID_16BIT_LEN) {
        uint16_t uuid16 = uuid.ConvertTo16Bits();
        (void)memcpy_s(BufferPtr(serviceUuid), UUID_16BIT_LEN, &(uuid16), UUID_16BIT_LEN);
        pimpl->requestList_.emplace_back(
            std::make_pair(connectHandle, GattRequestInfo(uuid, DISCOVER_SERVICE_BY_UUID, reqId)));
    } else if (uuid.GetUuidType() == UUID_32BIT_LEN) {
        uint32_t uuid32 = uuid.ConvertTo32Bits();
        (void)memcpy_s(BufferPtr(serviceUuid), UUID_32BIT_LEN, &(uuid32), UUID_32BIT_LEN);
        pimpl->requestList_.emplace_back(
            std::make_pair(connectHandle, GattRequestInfo(uuid, DISCOVER_SERVICE_BY_UUID, reqId)));
    } else if (uuid.GetUuidType() == UUID_128BIT_LEN) {
        uuid.ConvertToBytesLE(uuid128, UUID_128BIT_LEN);
        (void)memcpy_s(BufferPtr(serviceUuid), UUID_128BIT_LEN, uuid128, UUID_128BIT_LEN);
        pimpl->requestList_.emplace_back(
            std::make_pair(connectHandle, GattRequestInfo(uuid, DISCOVER_SERVICE_BY_UUID, reqId)));
    } else {
        BufferFree(serviceUuid);
        LOG_ERROR("%{public}s: Service UUID length is failed", __FUNCTION__);
        return;
    }
    ATT_FindByTypeValueRequest(connectHandle, &parameter, serviceUuid);
    BufferFree(serviceUuid);
}
/**
 * @brief This sub-procedure is used by a client to find include service declarations within a service definition
 * on a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates starting handle of the specified service.
 * @param endHandle Indicates ending handle of the specified service.
 * @since 6.0
 */
void GattClientProfile::FindIncludedServices(
    int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const
{
    LOG_INFO("%{public}s Parameter startHandle is %hu, endHandle is %hu", __FUNCTION__, startHandle, endHandle);
    BtUuid includeSvcUuid = {BT_UUID_16, {UUID_INCLUDE_SERVICE}};

    auto iter = pimpl->cacheMap_.find(connectHandle)->second.GetIncludeServices(startHandle);
    if (iter != nullptr) {
        iter->clear();
    }
    pimpl->requestList_.emplace_back(
        connectHandle, GattRequestInfo(FIND_INCLUDE_SERVICE, startHandle, endHandle, reqId));
    LOG_DEBUG("%{public}s: Add requestList_: FIND_INCLUDE_SERVICE", __FUNCTION__);
    ATT_ReadByTypeRequest(connectHandle, startHandle, endHandle, &includeSvcUuid);
}
/**
 * @brief This sub-procedure is used by a client to find all the characteristic declarations within a service
 * definition on a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates starting handle of the specified service.
 * @param endHandle Indicates ending handle of the specified service.
 * @since 6.0
 */
void GattClientProfile::DiscoverAllCharacteristicOfService(
    int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    Uuid tempUuid;
    BtUuid characteristicUuid = {BT_UUID_16, {UUID_CHARACTERISTIC}};

    auto iter = pimpl->cacheMap_.find(connectHandle)->second.GetCharacteristics(startHandle);
    if (iter != nullptr) {
        iter->clear();
    }

    pimpl->requestList_.emplace_back(connectHandle,
        GattRequestInfo(DISCOVER_ALL_CHARACTERISTIC,
            startHandle,
            endHandle,
            tempUuid.ConvertFrom16Bits(UUID_CHARACTERISTIC),
            reqId));
    LOG_DEBUG("%{public}s: Add requestList_: DISCOVER_ALL_CHARACTERISTIC", __FUNCTION__);
    ATT_ReadByTypeRequest(connectHandle, startHandle, endHandle, &characteristicUuid);
}
/**
 * @brief This sub-procedure is used by a client to discover service characteristics on a server when only the
 * service handle ranges are known and the characteristic UUID is known.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates starting handle of the specified service.
 * @param endHandle Indicates ending handle of the specified service.
 * @param uuid Indicates characteristic uuid.
 * @since 6.0
 */
void GattClientProfile::DiscoverCharacteristicByUuid(
    int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, const Uuid &uuid) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    BtUuid tempUuid;
    uint8_t uuid128[UUID_128BIT_LEN] = {0};

    auto iter = pimpl->cacheMap_.find(connectHandle)->second.GetCharacteristics(startHandle);
    if (iter != nullptr) {
        iter->clear();
    }

    if (uuid.GetUuidType() == UUID_16BIT_LEN) {
        tempUuid = {BT_UUID_16, {uuid.ConvertTo16Bits()}};
        pimpl->requestList_.emplace_back(connectHandle,
            GattRequestInfo(DISCOVER_CHARACTERISTIC_BY_UUID,
                startHandle,
                endHandle,
                uuid.ConvertFrom16Bits(tempUuid.uuid16),
                reqId));
    } else if (uuid.GetUuidType() == UUID_32BIT_LEN) {
        tempUuid = {.type = BT_UUID_32, .uuid32 = uuid.ConvertTo32Bits()};
        pimpl->requestList_.emplace_back(connectHandle,
            GattRequestInfo(DISCOVER_CHARACTERISTIC_BY_UUID,
                startHandle,
                endHandle,
                uuid.ConvertFrom32Bits(tempUuid.uuid32),
                reqId));
    } else if (uuid.GetUuidType() == UUID_128BIT_LEN) {
        uuid.ConvertToBytesLE(uuid128, UUID_128BIT_LEN);
        tempUuid.type = BT_UUID_128;
        (void)memcpy_s(tempUuid.uuid128, UUID_128BIT_LEN, uuid128, UUID_128BIT_LEN);
        pimpl->requestList_.emplace_back(connectHandle,
            GattRequestInfo(DISCOVER_CHARACTERISTIC_BY_UUID,
                startHandle,
                endHandle,
                uuid.ConvertFromBytesLE(tempUuid.uuid128, UUID_128BIT_LEN),
                reqId));
    } else {
        LOG_ERROR("%{public}s: Call - ATT_ReadByTypeRequest - Fail!", __FUNCTION__);
        return;
    }
    ATT_ReadByTypeRequest(connectHandle, startHandle, endHandle, &tempUuid);
}
/**
 * @brief This sub-procedure is used by a client to find all the characteristic descriptor’s
 * Attribute Handles and Attribute Types.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates starting handle of the specified service.
 * @param endHandle Indicates ending handle of the specified service.
 * @since 6.0
 */
void GattClientProfile::DiscoverAllCharacteristicDescriptors(
    int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    // startHandle is handle of characteristic declaration
    if (startHandle != INVALID_ATTRIBUTE_HANDLE && startHandle <= endHandle - sizeof(startHandle) &&
        startHandle <= MAX_ATTRIBUTE_HANDLE - sizeof(startHandle)) {
        pimpl->requestList_.emplace_back(
            connectHandle, GattRequestInfo(DISCOVER_ALL_CHARACTERISTIC_DESCRIPTOR, startHandle, endHandle, reqId));
        LOG_DEBUG("%{public}s: Add requestList_: DISCOVER_ALL_CHARACTERISTIC_DESCRIPTOR", __FUNCTION__);
        ATT_FindInformationRequest(connectHandle, startHandle + sizeof(startHandle), endHandle);
    } else {
        LOG_DEBUG("%{public}s: No descriptor value in the characteristic", __FUNCTION__);
        uint16_t serviceHandle = pimpl->cacheMap_.find(connectHandle)->second.GetDescriptors(startHandle).second;
        pimpl->pClientCallBack_->OnDiscoverAllCharacteristicDescriptorsEvent(reqId,
            GATT_SUCCESS,
            serviceHandle,
            startHandle,
            *pimpl->cacheMap_.find(connectHandle)->second.GetDescriptors(startHandle).first);
    }
}
/**
 * @brief This sub-procedure is used to read a Characteristic Value from a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @since 6.0
 */
void bluetooth::GattClientProfile::ReadCharacteristicValue(int reqId, uint16_t connectHandle, uint16_t handle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(READ_CHARACTERISTIC_VALUE, handle, reqId));
    LOG_DEBUG("%{public}s: Add requestList_: READ_CHARACTERISTIC_VALUE", __FUNCTION__);
    ATT_ReadRequest(connectHandle, handle);
}
/**
 * @brief This sub-procedure is used to read a Characteristic Value by uuid from a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param uuid Indicates characteristic uuid.
 * @since 6.0
 */
void GattClientProfile::ReadUsingCharacteristicByUuid(int reqId, uint16_t connectHandle, const Uuid &uuid) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    BtUuid tempUuid;
    uint8_t uuid128[UUID_128BIT_LEN] = {0};

    if (uuid.GetUuidType() == UUID_16BIT_LEN) {
        tempUuid = {BT_UUID_16, {uuid.ConvertTo16Bits()}};
        pimpl->requestList_.emplace_back(connectHandle,
            GattRequestInfo(READ_USING_CHARACTERISTIC_UUID,
                MIN_ATTRIBUTE_HANDLE,
                MAX_ATTRIBUTE_HANDLE,
                uuid.ConvertFrom16Bits(tempUuid.uuid16),
                reqId));
        LOG_DEBUG("%{public}s: Add requestList_: READ_USING_CHARACTERISTIC_UUID", __FUNCTION__);
    } else if (uuid.GetUuidType() == UUID_128BIT_LEN) {
        uuid.ConvertToBytesLE(uuid128, UUID_128BIT_LEN);
        tempUuid.type = BT_UUID_128;
        (void)memcpy_s(tempUuid.uuid128, UUID_128BIT_LEN, uuid128, UUID_128BIT_LEN);
        pimpl->requestList_.emplace_back(connectHandle,
            GattRequestInfo(READ_USING_CHARACTERISTIC_UUID,
                MIN_ATTRIBUTE_HANDLE,
                MAX_ATTRIBUTE_HANDLE,
                uuid.ConvertFromBytesLE(tempUuid.uuid128, UUID_128BIT_LEN),
                reqId));
        LOG_DEBUG("%{public}s: Add requestList_: READ_USING_CHARACTERISTIC_UUID", __FUNCTION__);
    } else {
        LOG_ERROR("%{public}s: Call - ATT_ReadByTypeRequest - Fail!", __FUNCTION__);
        return;
    }
    ATT_ReadByTypeRequest(connectHandle, MIN_ATTRIBUTE_HANDLE, MAX_ATTRIBUTE_HANDLE, &tempUuid);
}
/**
 * @brief This sub-procedure is used to read a long characteristic values from a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @since 6.0
 */
void GattClientProfile::ReadLongCharacteristicValue(int reqId, uint16_t connectHandle, uint16_t handle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(READ_LONG_CHARACTERISTIC_VALUE, handle, reqId));
    LOG_DEBUG("%{public}s: Add requestList_: READ_LONG_CHARACTERISTIC_VALUE", __FUNCTION__);
    ATT_ReadRequest(connectHandle, handle);
}
/**
 * @brief This sub-procedure is used to read multiple characteristic values.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param value Indicates attribute handle list.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::ReadMultipleCharacteristicValue(
    int reqId, uint16_t connectHandle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
    if (buffer != nullptr) {
        pimpl->requestList_.emplace_back(
            connectHandle, GattRequestInfo(READ_MULTIPLE_CHARACTERISTIC, MIN_ATTRIBUTE_HANDLE, reqId));
        LOG_DEBUG("%{public}s: Add requestList_: READ_MULTIPLE_CHARACTERISTIC", __FUNCTION__);
        ATT_ReadMultipleRequest(connectHandle, buffer);
        BufferFree(buffer);
    }
}
/**
 * @brief This sub-procedure is used to read a characteristic descriptor from a server.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @since 6.0
 */
void GattClientProfile::ReadDescriptorValue(int reqId, uint16_t connectHandle, uint16_t handle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(READ_CHARACTERISTIC_DESCRIPTOR, handle, reqId));
    LOG_DEBUG("%{public}s: Add requestList_: READ_CHARACTERISTIC_DESCRIPTOR", __FUNCTION__);
    ATT_ReadRequest(connectHandle, handle);
}
/**
 * @brief This sub-procedure is used to read a characteristic descriptor from a server and the length of the
 * characteristic descriptor declaration is longer than can be sent in a single Read Response Attribute
 * Protocol message..
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @since 6.0
 */
void GattClientProfile::ReadLongCharacteristicDescriptor(int reqId, uint16_t connectHandle, uint16_t handle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    pimpl->requestList_.emplace_back(
        connectHandle, GattRequestInfo(READ_LONG_CHARACTERISTIC_DESCRIPTOR, handle, reqId));
    LOG_DEBUG("%{public}s: Add requestList_: READ_LONG_CHARACTERISTIC_DESCRIPTOR", __FUNCTION__);
    ATT_ReadRequest(connectHandle, handle);
}
/**
 * @brief This sub-procedure is used to write a Characteristic Value to a server when the client knows the
 * Characteristic Value Handle and the client does not need an acknowledgment that the write was successfully
 * performed.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::WriteWithoutResponse(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
    if (buffer != nullptr) {
        ATT_WriteCommand(connectHandle, handle, buffer);
        BufferFree(buffer);
    }
}
/**
 * @brief This sub-procedure is used to write a Characteristic Value to a server when the client knows the
 * Characteristic Value Handle and the ATT Bearer is not encrypted.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::SignedWriteWithoutResponse(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    if (GattConnectionManager::GetInstance().GetEncryptionInfo(connectHandle) != false) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(WRITE_WITHOUT_RESPONSE, reqId));
            LOG_DEBUG("%{public}s: Add requestList_: WRITE_WITHOUT_RESPONSE", __FUNCTION__);
            ATT_SignedWriteCommand(connectHandle, handle, buffer);
            BufferFree(buffer);
        }
    } else {
        WriteWithoutResponse(reqId, connectHandle, handle, value, len);
    }
}
/**
 * @brief This sub-procedure is used to write a Characteristic Value to a server when the client knows the
 * Characteristic Value Handle.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void bluetooth::GattClientProfile::WriteCharacteristicValue(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint16_t mtu = pimpl->GetMtuInformation(connectHandle).mtu_;
    if (len <= static_cast<size_t>(mtu - 0x03)) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            pimpl->requestList_.emplace_back(
                connectHandle, GattRequestInfo(WRITE_CHARACTERISTIC_VALUE, handle, reqId));
            LOG_DEBUG("%{public}s: handle: %{public}d, len: %{public}zu, value: %{public}d",
                __FUNCTION__, handle, len, *(value->get()));
            ATT_WriteRequest(connectHandle, handle, buffer);
            BufferFree(buffer);
        }
    } else {
        WriteLongCharacteristicValue(reqId, connectHandle, handle, value, len);
    }
}
/**
 * @brief This sub-procedure is used to write a Characteristic Value to a server when the client knows the
 * Characteristic Value Handle but the length of theCharacteristic Value is longer than can be sent in a
 * single Write Request Attribute Protocol message.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::WriteLongCharacteristicValue(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint16_t bufSize = pimpl->GetMtuInformation(connectHandle).mtu_ - 0x05;
    AttReadBlobReqPrepareWriteValue attReadBlobObj = {handle, bufSize};
    Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), 0x00, bufSize);
    if (buffer != nullptr) {
        pimpl->requestList_.emplace_back(
            connectHandle, GattRequestInfo(WRITE_LONG_CHARACTERISTIC_VALUE, handle, bufSize, len, value, reqId));
        LOG_DEBUG("%{public}s: Add requestList_: WRITE_LONG_CHARACTERISTIC_VALUE", __FUNCTION__);
        ATT_PrepareWriteRequest(connectHandle, attReadBlobObj, buffer);
        BufferFree(buffer);
    }
}
/**
 * @brief This sub-procedure is used to write a characteristic descriptor value to a server when the client
 * knows the characteristic descriptor handle.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::WriteDescriptorValue(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint16_t mtu = pimpl->GetMtuInformation(connectHandle).mtu_;
    if (len <= static_cast<size_t>(mtu - 0x03)) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            pimpl->requestList_.emplace_back(
                connectHandle, GattRequestInfo(WRITE_CHARACTERISTIC_DESCRIPTOR, handle, reqId));
            LOG_DEBUG("%{public}s: handle: %{public}d, len: %{public}zu, value: %{public}d",
                __FUNCTION__, handle, len, *(value->get()));
            ATT_WriteRequest(connectHandle, handle, buffer);
            BufferFree(buffer);
        }
    } else {
        WriteLongCharacteristicDescriptor(reqId, connectHandle, handle, value, len);
    }
}
/**
 * @brief This sub-procedure is used to write a characteristic descriptor value to a server when the client
 * knows the characteristic descriptor handle but the length of the characteristic descriptor value is
 * longer than can be sent in a single write request attribute protocol message.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::WriteLongCharacteristicDescriptor(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    WriteLongCharacteristicValue(reqId, connectHandle, handle, value, len);
}
/**
 * @brief This sub-procedure is used to write a Characteristic Value to a server when the
 * client knows the Characteristic Value Handle, and assurance is required that the
 * correct Characteristic Value is going to be written by transferring the Characteristic
 * Value to be written in both directions before the write is performed.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattClientProfile::ReliableWriteCharacteristicValue(
    int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    AttReadBlobReqPrepareWriteValue attReadBlobObj = {handle, 0};
    Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), 0x00, len);

    if (buffer != nullptr) {
        pimpl->requestList_.emplace_back(connectHandle, GattRequestInfo(RELIABLE_WRITE_VALUE, reqId));
        LOG_DEBUG("%{public}s: Add requestList_: RELIABLE_WRITE_VALUE", __FUNCTION__);
        ATT_PrepareWriteRequest(connectHandle, attReadBlobObj, buffer);
        BufferFree(buffer);
    }
}
/**
 * @brief The Attribute Flags parameter shall be set to 0x01 to immediately write all
 * pending prepared values in the order that they were prepared.
 *
 * @param reqId Indicates request id.
 * @param connectHandle Indicates identify a connection.
 * @param flag Indicates 0x00 – Cancel all prepared writes.
 *                       0x01 – Immediately write all pending prepared values.
 * @since 6.0
 */
void GattClientProfile::ExecuteWriteRequest(int reqId, uint16_t connectHandle, uint8_t flag) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    ATT_ExecuteWriteRequest(connectHandle, flag);
}

/**
 * @brief This sub-procedure is used when a server is configured to indicate a Characteristic Value to a client
 * and expects an Attribute Protocollayer acknowledgment that the indication was successfully received.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattClientProfile::HandleValueConfirmation(uint16_t connectHandle) const
{
    ATT_HandleValueConfirmation(connectHandle);
}

std::map<uint16_t, GattCache::Service> *GattClientProfile::GetServices(uint16_t connectHandle) const
{
    auto cache = pimpl->cacheMap_.find(connectHandle);
    if (cache != pimpl->cacheMap_.end()) {
        return &cache->second.GetServices();
    }
    return nullptr;
}

uint16_t GattClientProfile::GetCharacteristicEndHandle(
    uint16_t connectHandle, uint16_t svcHandle, uint16_t handle) const
{
    auto cache = pimpl->cacheMap_.find(connectHandle);
    if (cache != pimpl->cacheMap_.end()) {
        return cache->second.GetCharacteristicEndHandle(svcHandle, handle);
    }
    return 0;
}

/**
 * @brief This sub-procedure is used by the client to obtain services.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @return Returns service pointer.
 * @since 6.0
 */
const GattCache::Service *GattClientProfile::GetService(uint16_t connectHandle, int16_t handle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto cache = pimpl->cacheMap_.find(connectHandle);
    if (cache != pimpl->cacheMap_.end()) {
        auto sIt = cache->second.GetServices().find(handle);
        if (sIt != cache->second.GetServices().end()) {
            return &sIt->second;
        }
    }
    return nullptr;
}
/**
 * @brief This sub-procedure is used by the client to obtain characteristics.
 *
 * @param connectHandle Indicates identify a connection.
 * @param valueHandle Indicates attribute handle.
 * @return Returns characteristics pointer.
 * @since 6.0
 */
const GattCache::Characteristic *GattClientProfile::GetCharacteristic(
    uint16_t connectHandle, int16_t valueHandle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto cache = pimpl->cacheMap_.find(connectHandle);
    if (cache != pimpl->cacheMap_.end()) {
        return cache->second.GetCharacteristic(valueHandle);
    }
    return nullptr;
}
/**
 * @brief This sub-procedure is used by the client to obtain characteristic descriptors.
 *
 * @param connectHandle Indicates identify a connection.
 * @param valueHandle Indicates attribute handle.
 * @return Returns descriptors pointer.
 * @since 6.0
 */
const GattCache::Descriptor *GattClientProfile::GetDescriptor(uint16_t connectHandle, int16_t valueHandle) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto cache = pimpl->cacheMap_.find(connectHandle);
    if (cache != pimpl->cacheMap_.end()) {
        return cache->second.GetDescriptor(valueHandle);
    }
    return nullptr;
}
/**
 * @brief This sub-procedure is used by the client to process received data from att.
 *
 * @param connectHandle Indicates identify a connection.
 * @param event Indicates client callback event id.
 * @param eventData Indicates att data.
 * @param buffer Indicates att data.
 * @param context Indicates client callback function.
 * @since 6.0
 */
void GattClientProfile::impl::ReceiveData(
    uint16_t connectHandle, uint16_t event, void *eventData, Buffer *buffer, void *context)
{
    LOG_INFO("GattClientProfile::%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    AttEventData *data = (AttEventData *)eventData;
    auto object = static_cast<GattClientProfile::impl *>(context);

    if (event == ATT_HANDLE_VALUE_NOTIFICATION_ID) {
        object->NotificationParsing(connectHandle, data, buffer);
    } else if (event == ATT_HANDLE_VALUE_INDICATION_ID) {
        object->IndicationParsing(connectHandle, data, buffer);
    } else {
        auto attResp = object->FindIteratorByResponesInfor(connectHandle, event);
        if (attResp == object->responseList_.end()) {
            LOG_INFO("%{public}s: attResp is null", __FUNCTION__);
            return;
        }
        object->ReceiveDataProcess(connectHandle, event, data, buffer, attResp);
        object->responseList_.erase(attResp);
    }
}
/**
 * @brief This sub-procedure is used by removing request list.
 *
 * @param connectHandle Indicates identify a connection.
 * @param connectHandle Indicates identify a connection.
 * @param event Indicates client callback event id.
 * @param data Indicates att data.
 * @param buffer Indicates att data.
 * @param attResp Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReceiveDataProcess(uint16_t connectHandle, uint16_t event, AttEventData *data,
    Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator attResp)
{
    switch (event) {
        case ATT_ERROR_RESPONSE_ID:
            ErrorResponseParsing(connectHandle, data, attResp);
            break;
        case ATT_EXCHANGE_MTU_RESPONSE_ID:
            ExchangeMtuParsing(connectHandle, data, attResp);
            break;
        case ATT_FIND_INFORMATION_RESPONSE_ID:
            DiscoverAllCharacteristicDescriptorsParsing(connectHandle, data, attResp);
            break;
        case ATT_FIND_BY_TYPE_VALUE_RESPONSE_ID:
            DiscoverPrimaryServiceByUuidParsing(connectHandle, data, attResp);
            break;
        case ATT_READ_BY_TYPE_RESPONSE_ID:
            SplitReadByTypeRsp(connectHandle, data, attResp);
            break;
        case ATT_READ_RESPONSE_ID:
            SplitReadRsp(connectHandle, buffer, attResp);
            break;
        case ATT_READ_BLOB_RESPONSE_ID:
            SplitReadBlobRsp(connectHandle, buffer, attResp);
            break;
        case ATT_READ_MULTIPLE_RESPONSE_ID:
            ReadMultipleCharacteristicParsing(connectHandle, buffer, attResp);
            break;
        case ATT_READ_BY_GROUP_TYPE_RESPONSE_ID:
            DiscoverAllPrimaryServiceParsing(attResp->second.reqId_, connectHandle, data);
            break;
        case ATT_WRITE_RESPONSE_ID:
            SplitWriteRsp(connectHandle, attResp);
            break;
        case ATT_PREPARE_WRITE_RESPONSE_ID:
            SplitPrepareWriteRsp(connectHandle, data, buffer, attResp);
            break;
        case ATT_EXECUTE_WRITE_RESPONSE_ID:
            ExecuteWriteParsing(connectHandle, attResp);
            break;
        case ATT_TRANSACTION_TIME_OUT_ID:
            GattRequestTimeoutParsing(attResp->second.reqId_, connectHandle, attResp->second.reqType_);
            break;
        default:
            LOG_ERROR("GATT client profile: %{public}s. It's invalid opcode.", __FUNCTION__);
            break;
    }
}
/**
 * @brief This sub-procedure is used by the client to process received att request result.
 *
 * @param connectHandle Indicates identify a connection.
 * @param result Indicates request result.
 * @param context Indicates client callback function.
 * @since 6.0
 */
void GattClientProfile::impl::ReceiveRequestResult(uint16_t connectHandle, int result, void *context)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto object = static_cast<GattClientProfile::impl *>(context);

    if (result == GATT_SUCCESS) {
        object->AddResponseList();
    } else {
        object->dispatcher_->PostTask(std::bind(&impl::ReceiveRequestResultProcess, object, connectHandle));
    }
}
/**
 * @brief This sub-procedure is used by the client to process received att request result.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattClientProfile::impl::ReceiveRequestResultProcess(uint16_t connectHandle)
{
    auto iter = FindIteratorByRequestInfor(connectHandle);
    if (iter != requestList_.end()) {
        IndicateRequestRetToService(iter->second.reqType_, connectHandle, iter->second.reqType_, ATT_OUT_OF_RANGE);
        LOG_INFO("%{public}s: RemoveRequestList iter reqType: %{public}d", __FUNCTION__, iter->second.reqType_);
        RemoveRequestList(iter);
    } else {
        LOG_ERROR("%{public}s: requestList_ is null", __FUNCTION__);
    }
}
/**
 * @brief This sub-procedure is used by the client to process indicating request result to service.
 *
 * @param connectHandle Indicates identify a connection.
 * @param reqType Indicates send request type.
 * @param errorCode Indicates error code.
 * @since 6.0
 */
void GattClientProfile::impl::IndicateRequestRetToService(
    int reqId, uint16_t connectHandle, ResponesType reqType, uint8_t errorCode)
{
    int ret = ConvertResponseErrorCode(errorCode);
    auto sharedPtr = GattValue(std::make_shared<std::unique_ptr<uint8_t[]>>(nullptr));

    switch (reqType) {
        case DISCOVER_ALL_PRIMARY_SERVICE:
            pClientCallBack_->OnDiscoverAllPrimaryServicesEvent(
                reqId, ret, connectHandle, std::map<uint16_t, GattCache::Service>());
            break;
        case DISCOVER_SERVICE_BY_UUID:
            pClientCallBack_->OnDiscoverPrimaryServicesByServiceUUIDEvent(
                reqId, ret, connectHandle, std::map<uint16_t, GattCache::Service>());
            break;
        case FIND_INCLUDE_SERVICE:
            pClientCallBack_->OnFindIncludedServicesEvent(
                reqId, ret, connectHandle, 0, std::vector<GattCache::IncludeService>());
            break;
        case DISCOVER_ALL_CHARACTERISTIC:
        case DISCOVER_CHARACTERISTIC_BY_UUID:
            pClientCallBack_->OnDiscoverAllCharacteristicOfServiceEvent(
                reqId, ret, connectHandle, 0, std::map<uint16_t, GattCache::Characteristic>());
            break;
        case DISCOVER_ALL_CHARACTERISTIC_DESCRIPTOR:
            pClientCallBack_->OnDiscoverAllCharacteristicDescriptorsEvent(
                reqId, ret, 0, 0, std::map<uint16_t, GattCache::Descriptor>());
            break;
        case READ_CHARACTERISTIC_VALUE:
        case READ_USING_CHARACTERISTIC_UUID:
        case READ_MULTIPLE_CHARACTERISTIC:
            pClientCallBack_->OnReadCharacteristicValueEvent(reqId, 0, sharedPtr, 0, ret);
            break;
        case READ_CHARACTERISTIC_DESCRIPTOR:
        case READ_LONG_CHARACTERISTIC_DESCRIPTOR:
            pClientCallBack_->OnReadDescriptorValueEvent(reqId, 0, sharedPtr, 0, ret);
            break;
        case WRITE_CHARACTERISTIC_VALUE:
            pClientCallBack_->OnWriteCharacteristicValueEvent(reqId, connectHandle, 0, ret);
            break;
        case WRITE_CHARACTERISTIC_DESCRIPTOR:
            pClientCallBack_->OnWriteDescriptorValueEvent(reqId, connectHandle, 0, ret);
            break;
        case WRITE_LONG_CHARACTERISTIC_VALUE:
            pClientCallBack_->OnWriteLongCharacteristicValueEvent(reqId, connectHandle, 0, ret);
            break;
        case RELIABLE_WRITE_VALUE:
            pClientCallBack_->OnReliableWriteCharacteristicValueEvent(reqId, 0, sharedPtr, 0, ret);
            break;
        case EXCHANGE_MTU:
            pClientCallBack_->OnExchangeMtuEvent(reqId, connectHandle, GATT_DEFAULT_MTU, false);
            break;
        case WRITE_WITHOUT_RESPONSE:
            break;
        default:
            LOG_ERROR("%{public}s: request type is not find!", __FUNCTION__);
            break;
    }
}
/**
 * @brief This sub-procedure is used by the client to set mtu information.
 *
 * @param connectHandle Indicates identify a connection.
 * @param isExchanged Indicates the flag of MTU has been exchanged.
 * @param mtu Indicates mtu size.
 * @since 6.0
 */
void GattClientProfile::impl::SetMtuInformation(uint16_t connectHandle, bool isExchanged, uint16_t mtu)
{
    auto it = mtuInfo_.find(connectHandle);
    if (it == mtuInfo_.end()) {
        mtuInfo_.emplace(connectHandle, MtuInfo(isExchanged, mtu));
    } else {
        it->second.mtu_ = mtu;
        it->second.isExchanged_ = isExchanged;
    }
}
/**
 * @brief This sub-procedure is used by the client to get mtu information.
 *
 * @param connectHandle Indicates identify a connection.
 * @return Returns mtu information.
 * @since 6.0
 */
MtuInfo GattClientProfile::impl::GetMtuInformation(uint16_t connectHandle)
{
    MtuInfo mtuRet = {false, GATT_DEFAULT_MTU};
    auto it = mtuInfo_.find(connectHandle);
    if (it != mtuInfo_.end()) {
        mtuRet = it->second;
    }

    return mtuRet;
}
/**
 * @brief Add iter to the response list.
 *
 * @since 6.0
 */
void GattClientProfile::impl::AddResponseList(void)
{
    auto iter = requestList_.begin();
    if (iter != requestList_.end()) {
        responseList_.emplace_back(*iter);
        LOG_INFO("%{public}s: responseList size: %{public}zu", __FUNCTION__, responseList_.size());
        dispatcher_->PostTask(std::bind(&impl::RemoveRequestList, this, iter));
    }
}
/**
 * @brief Remove iter from the request list.
 *
 * @since 6.0
 */
void GattClientProfile::impl::RemoveRequestList(std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    requestList_.erase(iter);
}
/**
 * @brief This sub-procedure is used by the client to process error respond.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ErrorResponseParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle: %hu", __FUNCTION__, connectHandle);
    uint16_t handle = 0;
    int reqId = iter->second.reqId_;
    ResponesType type = iter->second.reqType_;
    auto sharedPtr = GattValue(std::make_shared<std::unique_ptr<uint8_t[]>>(nullptr));

    if (data->attErrorResponse.errorCode != ATT_ATTRIBUTE_NOT_FOUND) {
        IndicateRequestRetToService(reqId, connectHandle, type, data->attErrorResponse.errorCode);
    } else {
        switch (data->attErrorResponse.reqOpcode) {
            case READ_BY_GROUP_TYPE_REQUEST:
                pClientCallBack_->OnDiscoverAllPrimaryServicesEvent(
                    reqId, GATT_SUCCESS, connectHandle, cacheMap_.find(connectHandle)->second.GetServices());
                break;
            case READ_BY_TYPE_REQUEST:
                SplitReadByTypeReqErrorParsing(connectHandle, iter);
                break;
            case FIND_BY_TYPE_VALUE_REQUEST:
                pClientCallBack_->OnDiscoverPrimaryServicesByServiceUUIDEvent(
                    reqId, GATT_SUCCESS, connectHandle, cacheMap_.find(connectHandle)->second.GetServices());
                break;
            case FIND_INFORMATION_REQUEST:
                handle = cacheMap_.find(connectHandle)->second.GetDescriptors(iter->second.startHandle_).second;
                pClientCallBack_->OnDiscoverAllCharacteristicDescriptorsEvent(reqId,
                    GATT_SUCCESS,
                    handle,
                    iter->second.startHandle_,
                    *cacheMap_.find(connectHandle)->second.GetDescriptors(iter->second.startHandle_).first);
                break;
            case READ_REQUEST:
            case READ_BLOB_REQUEST:
                SplitReadReqErrorParsing(reqId,
                    connectHandle,
                    type,
                    data->attErrorResponse.attHandleInError,
                    ConvertResponseErrorCode(data->attErrorResponse.errorCode));
                break;
            case WRITE_REQUEST:
                SplitWriteReqErrorParsing(reqId,
                    connectHandle,
                    type,
                    data->attErrorResponse.attHandleInError,
                    ConvertResponseErrorCode(data->attErrorResponse.errorCode));
                break;
            case PREPARE_WRITE_REQUEST:
                pClientCallBack_->OnReliableWriteCharacteristicValueEvent(reqId,
                    data->attErrorResponse.attHandleInError,
                    sharedPtr,
                    0,
                    ConvertResponseErrorCode(data->attErrorResponse.errorCode));
                break;
            default:
                LOG_ERROR("%{public}s: Request opcode of error response is invalid", __FUNCTION__);
                break;
        }
    }
}
/**
 * @brief This sub-procedure is used by the client to client to process find include service error respond.
 *
 * @param connectHandle Indicates identify a connection.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ErrorResponseParsing(
    uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    int reqId = iter->second.reqId_;

    pClientCallBack_->OnFindIncludedServicesEvent(reqId,
        GATT_SUCCESS,
        connectHandle,
        iter->second.startHandle_,
        *cacheMap_.find(connectHandle)->second.GetIncludeServices(iter->second.startHandle_));
}
/**
 * @brief This sub-procedure is used by the client to exchange mtu.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ExchangeMtuParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    bool status = true;
    int reqId = iter->second.reqId_;
    uint16_t mtu = data->attExchangeMtuResponse.mtuSize;

    if (mtu < GATT_DEFAULT_MTU) {
        status = false;
        mtu = GATT_DEFAULT_MTU;
    } else if (mtu > iter->second.startHandle_) {
        mtu = iter->second.startHandle_;
    }
    SetMtuInformation(connectHandle, status, mtu);
    pClientCallBack_->OnExchangeMtuEvent(reqId, connectHandle, mtu, status);
}
/**
 * @brief This sub-procedure is used by the client to process discover all primary services.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0

 */
void GattClientProfile::impl::DiscoverAllPrimaryServiceParsing(int reqId, uint16_t connectHandle, AttEventData *data)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    BtUuid primarySvcUuid = {BT_UUID_16, {UUID_PRIMARY_SERVICE}};
    auto cache = cacheMap_.find(connectHandle);
    uint16_t startHandle = 0;
    uint16_t endHandle = 0;
    uint16_t num = data->attReadByGroupTypeResponse.readGroupResponse.num;
    uint16_t len = data->attReadByGroupTypeResponse.readGroupResponse.length;
    uint8_t value[UUID_128BIT_LEN] = {0};
    uint8_t uuidLen = len - sizeof(startHandle) - sizeof(endHandle);

    for (uint8_t i = 0; i < num; i++) {
        uint8_t offset = 0;
        startHandle = (data->attReadByGroupTypeResponse.readGroupResponse.attributeData + i)->attHandle;
        endHandle = (data->attReadByGroupTypeResponse.readGroupResponse.attributeData + i)->groupEndHandle;
        SplitDataPackage(value,
            UUID_128BIT_LEN,
            &offset,
            (data->attReadByGroupTypeResponse.readGroupResponse.attributeData + i)->attributeValue,
            uuidLen);
        Uuid uuid = SplitUuidPackage(value, uuidLen);
        dispatcher_->PostTask(std::bind(
            &GattCache::AddService, &cache->second, std::move(GattCache::Service(true, startHandle, endHandle, uuid))));
    }
    if (endHandle == MAX_ATTRIBUTE_HANDLE) {
        pClientCallBack_->OnDiscoverAllPrimaryServicesEvent(
            reqId, GATT_SUCCESS, connectHandle, cacheMap_.find(connectHandle)->second.GetServices());
        LOG_DEBUG("%{public}s Call OnDiscoverAllPrimaryServicesEvent", __FUNCTION__);
    } else {
        requestList_.emplace_back(connectHandle, GattRequestInfo(DISCOVER_ALL_PRIMARY_SERVICE, reqId));
        ATT_ReadByGroupTypeRequest(
            connectHandle, endHandle + MIN_ATTRIBUTE_HANDLE, MAX_ATTRIBUTE_HANDLE, &primarySvcUuid);
    }
}
/**
 * @brief This sub-procedure is used by the client to process discover all primary services by uuid.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::DiscoverPrimaryServiceByUuidParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);

    auto cache = cacheMap_.find(connectHandle);
    uint16_t endHandle = 0;
    uint16_t num = data->attFindByTypeValueResponse.findByTypeValueResponse.listNum;
    int reqId = iter->second.reqId_;
    int uuidType = iter->second.uuid_.GetUuidType();
    Buffer *serviceUuid = BufferMalloc(uuidType);
    AttFindByTypeValueReq parameter;

    for (uint16_t i = 0; i < num; i++) {
        uint16_t startHandle = (data->attFindByTypeValueResponse.findByTypeValueResponse.handleInfoList + i)->attHandle;
        endHandle = (data->attFindByTypeValueResponse.findByTypeValueResponse.handleInfoList + i)->groupEndHandle;
        dispatcher_->PostTask(std::bind(&GattCache::AddService,
            &cache->second,
            std::move(GattCache::Service(true, startHandle, endHandle, iter->second.uuid_))));
    }
    if (endHandle == MAX_ATTRIBUTE_HANDLE) {
        pClientCallBack_->OnDiscoverPrimaryServicesByServiceUUIDEvent(
            reqId, GATT_SUCCESS, connectHandle, cacheMap_.find(connectHandle)->second.GetServices());
        LOG_DEBUG("%{public}s Call OnDiscoverPrimaryServicesByServiceUUIDEvent", __FUNCTION__);
    } else {
        parameter.handleRange.startHandle = endHandle + MIN_ATTRIBUTE_HANDLE;
        parameter.handleRange.endHandle = MAX_ATTRIBUTE_HANDLE;
        parameter.attType = UUID_PRIMARY_SERVICE;
        if (memcpy_s(BufferPtr(serviceUuid), uuidType, &(iter->second.uuid_), uuidType) != EOK) {
            LOG_ERROR("%{public}s Call memcpy_s fail", __FUNCTION__);
            BufferFree(serviceUuid);
            return;
        }
        requestList_.emplace_back(connectHandle, GattRequestInfo(iter->second.uuid_, DISCOVER_SERVICE_BY_UUID, reqId));
        ATT_FindByTypeValueRequest(connectHandle, &parameter, serviceUuid);
    }
    BufferFree(serviceUuid);
}
/**
 * @brief This sub-procedure is used by the client to process find include 16bit uuid services.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::FindIncludeServicesParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %{public}hu.", __FUNCTION__, connectHandle);
    if (data == nullptr || data->attReadByTypeResponse.readHandleListNum.valueNum == 0) {
        LOG_ERROR("%{public}s: invalid params.", __FUNCTION__);
        return;
    }
    uint8_t offset = 0;
    int reqId = iter->second.reqId_;
    uint8_t len = data->attReadByTypeResponse.readHandleListNum.len;
    uint16_t isvcHandle = data->attReadByTypeResponse.readHandleListNum.valueList->attHandle.attHandle;
    uint16_t startHandle =
        SplitDataPackageToUint16(data->attReadByTypeResponse.readHandleListNum.valueList->attributeValue, &offset);
    uint16_t endHandle =
        SplitDataPackageToUint16(data->attReadByTypeResponse.readHandleListNum.valueList->attributeValue, &offset);
    BtUuid uuid = {BT_UUID_16, {UUID_INCLUDE_SERVICE}};
    auto cache = cacheMap_.find(connectHandle);

    if (len == sizeof(startHandle) + sizeof(endHandle) + sizeof(isvcHandle) + UUID_16BIT_LEN) {
        uint16_t uuid16Bit =
            SplitDataPackageToUint16(data->attReadByTypeResponse.readHandleListNum.valueList->attributeValue, &offset);
        Uuid svcUuid16Bit = Uuid::ConvertFrom16Bits(uuid16Bit);
        dispatcher_->PostTask(std::bind(&GattCache::AddIncludeService,
            &cache->second,
            iter->second.startHandle_,
            std::move(GattCache::IncludeService(isvcHandle, startHandle, endHandle, svcUuid16Bit))));
        dispatcher_->PostTask(std::bind(&GattCache::AddService,
            &cache->second,
            std::move(GattCache::Service(false, startHandle, endHandle, svcUuid16Bit))));

        requestList_.emplace_back(connectHandle,
            GattRequestInfo(
                FIND_INCLUDE_SERVICE, iter->second.startHandle_, iter->second.endHandle_, ++isvcHandle, reqId));
        ATT_ReadByTypeRequest(connectHandle, isvcHandle, iter->second.endHandle_, &uuid);
    } else {
        dispatcher_->PostTask(std::bind(&GattCache::AddIncludeService,
            &cache->second,
            iter->second.startHandle_,
            std::move(GattCache::IncludeService(isvcHandle, startHandle, endHandle))));
        dispatcher_->PostTask(std::bind(&GattCache::AddService,
            &cache->second,
            std::move(GattCache::Service(false, startHandle, endHandle, Uuid()))));
        Uuid nextHandle = Uuid::ConvertFrom16Bits(startHandle);
        requestList_.emplace_back(connectHandle,
            GattRequestInfo(FIND_INCLUDE_SERVICE,
                iter->second.startHandle_,
                iter->second.endHandle_,
                ++isvcHandle,
                nextHandle,
                reqId));
        ATT_ReadRequest(connectHandle, startHandle);
    }
}
/**
 * @brief This sub-procedure is used by the client toprocess find include 128bit uuid services.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle of include service.
 * @param buffer Indicates 128bit uuid.
 * @since 6.0
 */
void GattClientProfile::impl::FindIncludeServicesParsing(uint16_t connectHandle, uint16_t handle, Buffer *buffer)
{
    auto cache = cacheMap_.find(connectHandle);
    auto isvcs = cache->second.GetIncludeServices(handle);
    if (isvcs != nullptr) {
        for (auto &isvc : *isvcs) {
            if (isvc.handle_ == handle) {
                uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};
                if (memcpy_s(uuid128Bit, UUID_128BIT_LEN, BufferPtr(buffer), BufferGetSize(buffer)) != EOK) {
                    LOG_ERROR("%{public}s: memcpy_s fail.", __FUNCTION__);
                    break;
                }
                isvc.uuid_.ConvertFromBytesLE(uuid128Bit, UUID_128BIT_LEN);
                break;
            }
        }
    }
}
/**
 * @brief This sub-procedure is used by the client to process discover all characteristics.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param isByUuid Indicates if call DiscoverCharacteristicByUUID, isByUuid set true.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::DiscoverAllCharacteristicOfServiceParsing(uint16_t connectHandle, AttEventData *data,
    bool isByUuid, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %{public}hu.", __FUNCTION__, connectHandle);
    if (data == nullptr || data->attReadByTypeResponse.readHandleListNum.valueNum == 0) {
        LOG_ERROR("%{public}s: invalid params.", __FUNCTION__);
        return;
    }
    Uuid uuid;
    uint8_t properties = 0;
    uint16_t startHandle = 0;
    uint16_t valueHandle = 0;
    uint16_t len = data->attReadByTypeResponse.readHandleListNum.len;
    uint16_t num = data->attReadByTypeResponse.readHandleListNum.valueNum;
    uint8_t value[GATT_VALUE_LEN_MAX] = {0};
    uint8_t uuidLen = len - sizeof(startHandle) - sizeof(properties) - sizeof(valueHandle);
    auto cache = cacheMap_.find(connectHandle);

    for (uint16_t i = 0; i < num; i++) {
        uint8_t offset = 0;
        startHandle = (data->attReadByTypeResponse.readHandleListNum.valueList + i)->attHandle.attHandle;
        properties = SplitDataPackageToUint8(
            (data->attReadByTypeResponse.readHandleListNum.valueList + i)->attributeValue, &offset);
        valueHandle = SplitDataPackageToUint16(
            (data->attReadByTypeResponse.readHandleListNum.valueList + i)->attributeValue, &offset);
        SplitDataPackage(value,
            GATT_VALUE_LEN_MAX,
            &offset,
            (data->attReadByTypeResponse.readHandleListNum.valueList + i)->attributeValue,
            uuidLen);
        uuid = SplitUuidPackage(value, uuidLen);
        if (isByUuid) {
            if (iter->second.uuid_.operator==(uuid)) {
                dispatcher_->PostTask(std::bind(&GattCache::AddCharacteristic,
                    &cache->second,
                    iter->second.startHandle_,
                    std::move(GattCache::Characteristic(startHandle, properties, valueHandle, uuid))));
            }
        } else {
            dispatcher_->PostTask(std::bind(&GattCache::AddCharacteristic,
                &cache->second,
                iter->second.startHandle_,
                std::move(GattCache::Characteristic(startHandle, properties, valueHandle, uuid))));
        }
    }
    SendDiscoverAllCharacteristicRequest(connectHandle, ++startHandle, valueHandle, uuid, isByUuid, iter);
}
/**
 * @brief This sub-procedure is used by the client to process send discover all characteristics request.
 *
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates service handle.
 * @param valueHandle Indicates characteristic value handle.
 * @param uuid Indicates characteristic uuid.
 * @param isByUuid Indicates if call DiscoverCharacteristicByUUID, isByUuid set true.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::SendDiscoverAllCharacteristicRequest(uint16_t connectHandle, uint16_t startHandle,
    uint16_t valueHandle, Uuid uuid, bool isByUuid, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, startHandle is %hu, valueHandle is %hu.",
        __FUNCTION__,
        connectHandle,
        startHandle,
        valueHandle);
    int reqId = iter->second.reqId_;
    BtUuid characteristicUuid = {BT_UUID_16, {UUID_CHARACTERISTIC}};

    if (valueHandle == iter->second.endHandle_) {
        pClientCallBack_->OnDiscoverAllCharacteristicOfServiceEvent(reqId,
            GATT_SUCCESS,
            connectHandle,
            iter->second.startHandle_,
            *cacheMap_.find(connectHandle)->second.GetCharacteristics(iter->second.startHandle_));
    } else {
        if (isByUuid) {
            requestList_.emplace_back(connectHandle,
                GattRequestInfo(
                    DISCOVER_CHARACTERISTIC_BY_UUID, iter->second.startHandle_, iter->second.endHandle_, uuid, reqId));
        } else {
            requestList_.emplace_back(connectHandle,
                GattRequestInfo(
                    DISCOVER_ALL_CHARACTERISTIC, iter->second.startHandle_, iter->second.endHandle_, uuid, reqId));
        }
        ATT_ReadByTypeRequest(connectHandle, startHandle, iter->second.endHandle_, &characteristicUuid);
    }
}
/**
 * @brief This sub-procedure is used by the client to process discover all characteristics by uuid.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::DiscoverCharacteristicByUUIDParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    DiscoverAllCharacteristicOfServiceParsing(connectHandle, data, true, iter);
}
/**
 * @brief This sub-procedure is used by the client to process discover all characteristic descriptors.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::DiscoverAllCharacteristicDescriptorsParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    Uuid uuid;
    uint16_t attHandle = 0;
    uint16_t num = data->attFindInformationResponse.findInforRsponse.pairNum;
    uint8_t format = data->attFindInformationResponse.findInforRsponse.format;
    auto cache = cacheMap_.find(connectHandle);
    int reqId = iter->second.reqId_;

    for (uint16_t i = 0; i < num; i++) {
        if (format == UUID_16BIT_FORMAT) {
            uuid = Uuid::ConvertFrom16Bits(
                data->attFindInformationResponse.findInforRsponse.handleUuidPairs[i].uuid.uuid16);
        } else if (format == UUID_128BIT_FORMAT) {
            uuid = Uuid::ConvertFromBytesLE(
                data->attFindInformationResponse.findInforRsponse.handleUuidPairs[i].uuid.uuid128, UUID_128BIT_LEN);
        } else {
            LOG_ERROR("%{public}s: FindInformationResponse format is failed. Format = %hhu", __FUNCTION__, format);
            return;
        }
        attHandle = data->attFindInformationResponse.findInforRsponse.handleUuidPairs[i].attHandle;
        dispatcher_->PostTask(std::bind(&GattCache::AddDescriptor,
            &cache->second,
            iter->second.startHandle_,
            std::move(GattCache::Descriptor(attHandle, uuid))));
    }
    if (attHandle == iter->second.endHandle_) {
        uint16_t serviceHandle = cacheMap_.find(connectHandle)->second.GetDescriptors(iter->second.startHandle_).second;
        pClientCallBack_->OnDiscoverAllCharacteristicDescriptorsEvent(reqId,
            GATT_SUCCESS,
            serviceHandle,
            iter->second.startHandle_,
            *cacheMap_.find(connectHandle)->second.GetDescriptors(iter->second.startHandle_).first);
        LOG_DEBUG("%{public}s Call OnDiscoverAllCharacteristicDescriptorsEvent", __FUNCTION__);
    } else {
        requestList_.emplace_back(connectHandle,
            GattRequestInfo(
                DISCOVER_ALL_CHARACTERISTIC_DESCRIPTOR, iter->second.startHandle_, iter->second.endHandle_, reqId));
        ATT_FindInformationRequest(connectHandle, ++attHandle, iter->second.endHandle_);
    }
}
/**
 * @brief This sub-procedure is used by the client to process read characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReadCharacteristicValueParsing(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint16_t bufferSize = GetMtuInformation(connectHandle).mtu_ - sizeof(uint8_t);
    int reqId = iter->second.reqId_;

    if (bufferSize == BufferGetSize(buffer)) {
        LOG_DEBUG("%{public}s: Add requestList_: READ_LONG_CHARACTERISTIC_VALUE", __FUNCTION__);
        requestList_.emplace_back(
            connectHandle, GattRequestInfo(READ_LONG_CHARACTERISTIC_VALUE, iter->second.startHandle_, 0x01, reqId));
        AddReadValueCache(connectHandle, iter->second.startHandle_, bufferSize, buffer);
        ATT_ReadBlobRequest(connectHandle, iter->second.startHandle_, bufferSize);
    } else {
        auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(buffer), BufferGetSize(buffer));
        pClientCallBack_->OnReadCharacteristicValueEvent(
            reqId, iter->second.startHandle_, sharedPtr, BufferGetSize(buffer), GATT_SUCCESS);
    }
}
/**
 * @brief This sub-procedure is used by the client to process read long characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReadLongCharacteristicValueParsing(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    int reqId = iter->second.reqId_;
    uint16_t num = iter->second.endHandle_;
    uint16_t bufferSize = GetMtuInformation(connectHandle).mtu_ - sizeof(uint8_t);
    uint8_t *data = nullptr;

    if (bufferSize == BufferGetSize(buffer)) {
        LOG_DEBUG("%{public}s: Add requestList_: READ_LONG_CHARACTERISTIC_VALUE", __FUNCTION__);
        requestList_.emplace_back(
            connectHandle, GattRequestInfo(READ_LONG_CHARACTERISTIC_VALUE, iter->second.startHandle_, ++num, reqId));
        AddReadValueCache(connectHandle, iter->second.startHandle_, num * bufferSize, buffer);
        ATT_ReadBlobRequest(connectHandle, iter->second.startHandle_, bufferSize * num);
    } else {
        uint16_t totalSize = num * bufferSize + BufferGetSize(buffer);
        AddReadValueCache(connectHandle, iter->second.startHandle_, BufferGetSize(buffer), buffer);
        data = GetReadValueCache(connectHandle, iter->second.startHandle_);
        auto sharedPtr = GattServiceBase::BuildGattValue(data, totalSize);
        pClientCallBack_->OnReadCharacteristicValueEvent(
            reqId, iter->second.startHandle_, sharedPtr, totalSize, GATT_SUCCESS);
        free(data);
    }
}
/**
 * @brief This sub-procedure is used by the client to process read characteristic value by uuid.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReadUsingCharacteristicByUuidParsing(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    int reqId = iter->second.reqId_;
    uint16_t len = data->attReadByTypeResponse.readHandleListNum.len;
    uint16_t handle = data->attReadByTypeResponse.readHandleListNum.valueList->attHandle.attHandle;

    auto sharedPtr = GattServiceBase::BuildGattValue(
        data->attReadByTypeResponse.readHandleListNum.valueList->attributeValue, len - sizeof(uint16_t));
    pClientCallBack_->OnReadCharacteristicValueEvent(
        reqId, handle, sharedPtr, len - sizeof(uint16_t), GATT_SUCCESS);
}
/**
 * @brief This sub-procedure is used by the client to process read multiple characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReadMultipleCharacteristicParsing(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    int reqId = iter->second.reqId_;
    auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(buffer), BufferGetSize(buffer));
    pClientCallBack_->OnReadCharacteristicValueEvent(
        reqId, iter->second.startHandle_, sharedPtr, BufferGetSize(buffer), GATT_SUCCESS);
}
/**
 * @brief This sub-procedure is used by the client to process read characteristic descriptors.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReadCharacteristicDescriptorsParsing(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint16_t bufferSize = GetMtuInformation(connectHandle).mtu_ - sizeof(uint8_t);
    int reqId = iter->second.reqId_;

    if (bufferSize == BufferGetSize(buffer)) {
        LOG_DEBUG("%{public}s: Add requestList_: READ_LONG_CHARACTERISTIC_DESCRIPTOR", __FUNCTION__);
        requestList_.emplace_back(connectHandle,
            GattRequestInfo(READ_LONG_CHARACTERISTIC_DESCRIPTOR, iter->second.startHandle_, 0x01, reqId));
        AddReadValueCache(connectHandle, iter->second.startHandle_, bufferSize, buffer);
        ATT_ReadBlobRequest(connectHandle, iter->second.startHandle_, bufferSize);
    } else {
        auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(buffer), BufferGetSize(buffer));
        pClientCallBack_->OnReadDescriptorValueEvent(
            reqId, iter->second.startHandle_, sharedPtr, BufferGetSize(buffer), GATT_SUCCESS);
    }
}
/**
 * @brief This sub-procedure is used by the client to process read long characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ReadLongCharacteristicDescriptorsParsing(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    int reqId = iter->second.reqId_;
    uint16_t num = iter->second.endHandle_;
    uint16_t bufferSize = GetMtuInformation(connectHandle).mtu_ - sizeof(uint8_t);
    uint8_t *data = nullptr;

    if (bufferSize == BufferGetSize(buffer)) {
        LOG_DEBUG("%{public}s: Add requestList_: READ_LONG_CHARACTERISTIC_DESCRIPTOR", __FUNCTION__);
        requestList_.emplace_back(connectHandle,
            GattRequestInfo(READ_LONG_CHARACTERISTIC_DESCRIPTOR, iter->second.startHandle_, ++num, reqId));
        AddReadValueCache(connectHandle, iter->second.startHandle_, num * bufferSize, buffer);
        ATT_ReadBlobRequest(connectHandle, iter->second.startHandle_, bufferSize * num);
    } else {
        uint16_t totalSize = num * bufferSize + BufferGetSize(buffer);
        AddReadValueCache(connectHandle, iter->second.startHandle_, BufferGetSize(buffer), buffer);
        data = GetReadValueCache(connectHandle, iter->second.startHandle_);
        auto sharedPtr = GattServiceBase::BuildGattValue(data, totalSize);
        pClientCallBack_->OnReadDescriptorValueEvent(
            reqId, iter->second.startHandle_, sharedPtr, BufferGetSize(buffer), GATT_SUCCESS);
        free(data);
    }
}
/**
 * @brief This sub-procedure is used by the client to process write characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::WriteCharacteristicValueParsing(
    uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    pClientCallBack_->OnWriteCharacteristicValueEvent(
        iter->second.reqId_, connectHandle, iter->second.startHandle_, GATT_SUCCESS);
}
/**
 * @brief This sub-procedure is used by the client to process write characteristic descriptors.
 *
 * @param connectHandle Indicates identify a connection.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::WriteCharacteristicDescriptorsParsing(
    uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    pClientCallBack_->OnWriteDescriptorValueEvent(
        iter->second.reqId_, connectHandle, iter->second.startHandle_, GATT_SUCCESS);
}
/**
 * @brief This sub-procedure is used by the client to split read by type response process.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::SplitReadByTypeRsp(
    uint16_t connectHandle, AttEventData *data, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, iter->second.reqType_);
    switch (iter->second.reqType_) {
        case DISCOVER_ALL_CHARACTERISTIC:
            DiscoverAllCharacteristicOfServiceParsing(connectHandle, data, false, iter);
            break;
        case DISCOVER_CHARACTERISTIC_BY_UUID:
            DiscoverCharacteristicByUUIDParsing(connectHandle, data, iter);
            break;
        case FIND_INCLUDE_SERVICE:
            FindIncludeServicesParsing(connectHandle, data, iter);
            break;
        case READ_USING_CHARACTERISTIC_UUID:
            ReadUsingCharacteristicByUuidParsing(connectHandle, data, iter);
            break;
        default:
            LOG_ERROR("data len is %{public}d.", data->attReadByTypeResponse.readHandleListNum.len);
            break;
    }
}
/**
 * @brief This sub-procedure is used by the client to split read response process.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::SplitReadRsp(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, iter->second.reqType_);
    if (iter->second.reqType_ == READ_CHARACTERISTIC_VALUE) {
        ReadCharacteristicValueParsing(connectHandle, buffer, iter);
    } else if (iter->second.reqType_ == READ_CHARACTERISTIC_DESCRIPTOR) {
        ReadCharacteristicDescriptorsParsing(connectHandle, buffer, iter);
    } else {
        BtUuid uuid = {BT_UUID_16, {UUID_INCLUDE_SERVICE}};
        FindIncludeServicesParsing(connectHandle, iter->second.startHandle_, buffer);
        if (iter->second.valHandle_ <= iter->second.endHandle_) {
            requestList_.emplace_back(connectHandle,
                GattRequestInfo(FIND_INCLUDE_SERVICE,
                    iter->second.startHandle_,
                    iter->second.endHandle_,
                    iter->second.valHandle_,
                    iter->second.reqId_));
            ATT_ReadByTypeRequest(connectHandle, iter->second.valHandle_, iter->second.endHandle_, &uuid);
        } else {
            ErrorResponseParsing(connectHandle, iter);
        }
    }
}
/**
 * @brief This sub-procedure is used by the client to split read blob response process.
 *
 * @param connectHandle Indicates identify a connection.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::SplitReadBlobRsp(
    uint16_t connectHandle, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, iter->second.reqType_);
    if (iter->second.reqType_ == READ_LONG_CHARACTERISTIC_VALUE) {
        ReadLongCharacteristicValueParsing(connectHandle, buffer, iter);
    } else if (iter->second.reqType_ == READ_LONG_CHARACTERISTIC_DESCRIPTOR) {
        ReadLongCharacteristicDescriptorsParsing(connectHandle, buffer, iter);
    }
}
/**
 * @brief This sub-procedure is used by the client to split prepare write process.
 *
 * @param connectHandle Indicates identify a connection.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::SplitWriteRsp(
    uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, iter->second.reqType_);
    if (iter->second.reqType_ == WRITE_CHARACTERISTIC_VALUE) {
        WriteCharacteristicValueParsing(connectHandle, iter);
    } else if (iter->second.reqType_ == WRITE_CHARACTERISTIC_DESCRIPTOR) {
        WriteCharacteristicDescriptorsParsing(connectHandle, iter);
    }
}
/**
 * @brief This sub-procedure is used by the client to split prepare write process.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att event data.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::SplitPrepareWriteRsp(uint16_t connectHandle, AttEventData *data, Buffer *buffer,
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, iter->second.reqType_);
    if (iter->second.reqType_ == WRITE_LONG_CHARACTERISTIC_VALUE) {
        WriteLongCharacteristicValueParsing(connectHandle,
            data->attWriteResponse.prepareWrite.handleValue.attHandle,
            data->attWriteResponse.prepareWrite.offset,
            buffer,
            iter);
    } else if (iter->second.reqType_ == RELIABLE_WRITE_VALUE) {
        auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(buffer), BufferGetSize(buffer));
        pClientCallBack_->OnReliableWriteCharacteristicValueEvent(iter->second.reqId_,
            iter->second.startHandle_,
            sharedPtr,
            BufferGetSize(buffer),
            GATT_SUCCESS);
    } else {
        LOG_ERROR("%{public}s: Response type is %{public}d. It's invalid type.", __FUNCTION__, iter->second.reqType_);
    }
}

void GattClientProfile::impl::SplitReadByTypeReqErrorParsing(
    uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, iter->second.reqType_);
    auto sharedPtr = GattValue(std::make_shared<std::unique_ptr<uint8_t[]>>(nullptr));

    switch (iter->second.reqType_) {
        case DISCOVER_ALL_CHARACTERISTIC:
            pClientCallBack_->OnDiscoverAllCharacteristicOfServiceEvent(iter->second.reqId_,
                GATT_SUCCESS,
                connectHandle,
                iter->second.startHandle_,
                *cacheMap_.find(connectHandle)->second.GetCharacteristics(iter->second.startHandle_));
            break;
        case DISCOVER_CHARACTERISTIC_BY_UUID:
            pClientCallBack_->OnDiscoverAllCharacteristicOfServiceEvent(iter->second.reqId_,
                GATT_SUCCESS,
                connectHandle,
                iter->second.startHandle_,
                *cacheMap_.find(connectHandle)->second.GetCharacteristics(iter->second.startHandle_));
            break;
        case READ_USING_CHARACTERISTIC_UUID:
            pClientCallBack_->OnReadCharacteristicValueEvent(
                iter->second.reqId_, iter->second.startHandle_, sharedPtr, 0, GATT_SUCCESS);
            break;
        case FIND_INCLUDE_SERVICE:
            pClientCallBack_->OnFindIncludedServicesEvent(iter->second.reqId_,
                GATT_SUCCESS,
                connectHandle,
                iter->second.startHandle_,
                *cacheMap_.find(connectHandle)->second.GetIncludeServices(iter->second.startHandle_));
            break;
        default:
            LOG_ERROR("%{public}s: Response type is %{public}d. It's invalid type.", __FUNCTION__, iter->second.reqType_);
            break;
    }
}

void GattClientProfile::impl::SplitReadReqErrorParsing(
    int reqId, uint16_t connectHandle, ResponesType respType, uint16_t handle, int result)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, respType);
    auto sharedPtr = GattValue(std::make_shared<std::unique_ptr<uint8_t[]>>(nullptr));
    switch (respType) {
        case READ_CHARACTERISTIC_VALUE:
        case READ_LONG_CHARACTERISTIC_VALUE:
            pClientCallBack_->OnReadCharacteristicValueEvent(reqId, handle, sharedPtr, 0, result);
            break;
        case READ_CHARACTERISTIC_DESCRIPTOR:
        case READ_LONG_CHARACTERISTIC_DESCRIPTOR:
            pClientCallBack_->OnReadDescriptorValueEvent(reqId, handle, sharedPtr, 0, result);
            break;
        default:
            LOG_ERROR("%{public}s: It's invalid type.", __FUNCTION__);
            break;
    }
}

void GattClientProfile::impl::SplitWriteReqErrorParsing(
    int reqId, uint16_t connectHandle, ResponesType respType, uint16_t handle, int result)
{
    LOG_INFO("%{public}s: connectHandle is %hu, respType is %{public}d.", __FUNCTION__, connectHandle, respType);
    switch (respType) {
        case WRITE_CHARACTERISTIC_VALUE:
            pClientCallBack_->OnWriteCharacteristicValueEvent(reqId, connectHandle, handle, result);
            break;
        case WRITE_CHARACTERISTIC_DESCRIPTOR:
            pClientCallBack_->OnWriteDescriptorValueEvent(reqId, connectHandle, handle, result);
            break;
        default:
            LOG_ERROR("%{public}s: It's invalid type.", __FUNCTION__);
            break;
    }
}
/**
 * @brief This sub-procedure is used by the client to process write long characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates attribute handle.
 * @param offset Indicates data offset.
 * @param buffer Indicates att data.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::WriteLongCharacteristicValueParsing(uint16_t connectHandle, uint16_t handle,
    uint16_t offset, Buffer *buffer, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    Buffer *buf = nullptr;
    uint16_t len = iter->second.endHandle_ - iter->second.valHandle_;
    uint16_t bufSize = GetMtuInformation(connectHandle).mtu_ - 0x05;
    AttReadBlobReqPrepareWriteValue attReadBlobObj = {handle, (uint16_t)(BufferGetSize(buffer) + offset)};

    if (iter->second.endHandle_ <= iter->second.valHandle_) {
        ATT_ExecuteWriteRequest(connectHandle, 0x01);
        return;
    }
    if (len >= bufSize) {
        buf = GattServiceBase::BuildBuffer(iter->second.data_->get(), (size_t)iter->second.valHandle_, (size_t)bufSize);
        requestList_.emplace_back(connectHandle,
            GattRequestInfo(WRITE_LONG_CHARACTERISTIC_VALUE,
                handle,
                iter->second.valHandle_ + bufSize,
                iter->second.endHandle_,
                iter->second.data_,
                iter->second.reqId_));
    } else {
        buf = GattServiceBase::BuildBuffer(iter->second.data_->get(), (size_t)iter->second.valHandle_, (size_t)len);
        requestList_.emplace_back(connectHandle,
            GattRequestInfo(WRITE_LONG_CHARACTERISTIC_VALUE,
                handle,
                iter->second.valHandle_ + len,
                iter->second.endHandle_,
                iter->second.data_,
                iter->second.reqId_));
    }
    ATT_PrepareWriteRequest(connectHandle, attReadBlobObj, buf);
    BufferFree(buf);
}
/**
 * @brief This sub-procedure is used by the client to process execute write.
 *
 * @param connectHandle Indicates identify a connection.
 * @param iter Indicates iterator of client request information.
 * @since 6.0
 */
void GattClientProfile::impl::ExecuteWriteParsing(
    uint16_t connectHandle, std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    if (iter->second.reqType_ == WRITE_LONG_CHARACTERISTIC_VALUE) {
        pClientCallBack_->OnWriteLongCharacteristicValueEvent(
            iter->second.reqId_, connectHandle, iter->second.startHandle_, GATT_SUCCESS);
    } else {
        pClientCallBack_->OnExecuteWriteValueEvent(iter->second.reqId_, connectHandle, GATT_SUCCESS);
    }
}
/**
 * @brief This sub-procedure is used by the client to process notification.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates attribute handle.
 * @param buffer Indicates att data.
 * @since 6.0
 */
void GattClientProfile::impl::NotificationParsing(uint16_t connectHandle, AttEventData *data, Buffer *buffer)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(buffer), BufferGetSize(buffer));
    pClientCallBack_->OnCharacteristicNotifyEvent(
        connectHandle, data->attNotification.attHandle, sharedPtr, BufferGetSize(buffer), false);
}
/**
 * @brief This sub-procedure is used by the client to process indication.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates attribute handle.
 * @param buffer Indicates att data.
 * @since 6.0
 */
void GattClientProfile::impl::IndicationParsing(uint16_t connectHandle, const AttEventData *data, Buffer *buffer)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(buffer), BufferGetSize(buffer));
    pClientCallBack_->OnCharacteristicNotifyEvent(
        connectHandle, data->attIndication.attHandle, sharedPtr, BufferGetSize(buffer), true);
}
/**
 * @brief This sub-procedure is used by processing request timeout.
 *
 * @param connectHandle Indicates identify a connection.
 * @param reqType Indicates send request type.
 * @since 6.0
 */
void GattClientProfile::impl::GattRequestTimeoutParsing(int reqId, uint16_t connectHandle, ResponesType respType)
{
    IndicateRequestRetToService(reqId, connectHandle, respType, ATT_OUT_OF_RANGE);
}
/**
 * @brief Convert the att error code to the service layer error code.
 *
 * @param errorCode Indicates att error code.
 * @return Returns gatt client service's error code.
 * @since 6.0
 */
int GattClientProfile::impl::ConvertResponseErrorCode(uint8_t errorCode)
{
    int ret = 0;
    switch (errorCode) {
        case ATT_INVALID_HANDLE:
            ret = INVALID_HANDLE;
            break;
        case ATT_READ_NOT_PERMITTED:
            ret = READ_NOT_PERMITTED;
            break;
        case ATT_WRITE_NOT_PERMITTED:
            ret = WRITE_NOT_PERMITTED;
            break;
        case ATT_INSUFFICIENT_AUTHENTICATION:
            ret = INSUFFICIENT_AUTHENTICATION;
            break;
        case ATT_INVALID_OFFSET:
            ret = INSUFFICIENT_AUTHORIZATION;
            break;
        case ATT_ATTRIBUTE_NOT_FOUND:
            ret = HANDLE_NOT_FOUND;
            break;
        case ATT_INSUFFICIENT_ENCRYPTION:
            ret = INSUFFICIENT_ENCRYPTION;
            break;
        default:
            ret = GATT_FAILURE;
            break;
    }
    return ret;
}
/**
 * @brief Split att data package.
 *
 * @param dest Indicates destination data.
 * @param offset Indicates data offset.
 * @param src Indicates source data.
 * @since 6.0
 */
uint16_t GattClientProfile::impl::SplitDataPackageToUint16(uint8_t *src, uint8_t *offset)
{
    uint16_t dest = (*(src + *offset)) | ((*(src + *offset + sizeof(uint8_t))) << BIT_8);
    *offset += sizeof(uint16_t);
    return dest;
}
/**
 * @brief Split att data package.
 *
 * @param dest Indicates destination data.
 * @param offset Indicates data offset.
 * @param src Indicates source data.
 * @since 6.0
 */
uint8_t GattClientProfile::impl::SplitDataPackageToUint8(const uint8_t *src, uint8_t *offset)
{
    uint8_t dest = *src;
    *offset += sizeof(uint8_t);
    return dest;
}
/**
 * @brief Split att data package.
 *
 * @param dest Indicates destination address.
 * @param destMax -The maximum length of destination buffer.
 * @param offset Indicates address offset.
 * @param src Indicates source address.
 * @param size Indicates data size.
 * @since 6.0
 */
void GattClientProfile::impl::SplitDataPackage(
    uint8_t *dest, uint8_t destMax, uint8_t *offset, uint8_t *src, uint8_t size)
{
    if (memcpy_s(dest, destMax, (src + *offset), size) != EOK) {
        LOG_INFO("%{public}s: memcpy_s fail.", __FUNCTION__);
        return;
    }
    *offset = *offset + size;
}
/**
 * @brief Split service uuid package.
 *
 * @param value Indicates source uuid address.
 * @param len Indicates uuid length.
 * @return Returns gatt uuid.
 * @since 6.0
 */
Uuid GattClientProfile::impl::SplitUuidPackage(const uint8_t *value, uint8_t len)
{
    Uuid uuid;
    if (len == UUID_16BIT_LEN) {
        uuid = Uuid::ConvertFrom16Bits((value[1] << BIT_8) | value[0]);
    } else if (len == UUID_128BIT_LEN) {
        uuid = Uuid::ConvertFromBytesLE(value, UUID_128BIT_LEN);
    } else {
        uuid = Uuid::ConvertFrom16Bits(0);
        LOG_ERROR("%{public}s Uuid length is %hhu. It's invalid length", __FUNCTION__, len);
    }
    return uuid;
}
/**
 * @brief Find the order of operations GATT request.
 *
 * @param handle Indicates connectHandle handle.
 * @since 6.0
 */
std::list<std::pair<uint16_t, GattRequestInfo>>::iterator GattClientProfile::impl::FindIteratorByRequestInfor(
    uint16_t connectHandle)
{
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter;
    for (iter = requestList_.begin(); iter != requestList_.end(); iter++) {
        if (connectHandle == iter->first) {
            break;
        }
    }
    return iter;
}

std::list<std::pair<uint16_t, GattRequestInfo>>::iterator GattClientProfile::impl::FindIteratorByResponesInfor(
    uint16_t handle, uint16_t respType)
{
    ResponesType type;
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter;
    switch (respType) {
        case ATT_EXCHANGE_MTU_RESPONSE_ID:
            type = EXCHANGE_MTU;
            break;
        case FIND_INFORMATION_REQUEST:
        case ATT_FIND_INFORMATION_RESPONSE_ID:
            type = DISCOVER_ALL_CHARACTERISTIC_DESCRIPTOR;
            break;
        case FIND_BY_TYPE_VALUE_REQUEST:
        case ATT_FIND_BY_TYPE_VALUE_RESPONSE_ID:
            type = DISCOVER_SERVICE_BY_UUID;
            break;
        case ATT_READ_MULTIPLE_RESPONSE_ID:
            type = READ_CHARACTERISTIC_VALUE;
            break;
        case READ_BY_GROUP_TYPE_REQUEST:
        case ATT_READ_BY_GROUP_TYPE_RESPONSE_ID:
            type = DISCOVER_ALL_PRIMARY_SERVICE;
            break;
        case ATT_READ_BLOB_RESPONSE_ID:
        case READ_BY_TYPE_REQUEST:
        case ATT_READ_BY_TYPE_RESPONSE_ID:
        case READ_REQUEST:
        case ATT_READ_RESPONSE_ID:
        case WRITE_REQUEST:
        case ATT_WRITE_RESPONSE_ID:
        case ATT_PREPARE_WRITE_RESPONSE_ID:
        case ATT_EXECUTE_WRITE_RESPONSE_ID:
        default:
            for (iter = responseList_.begin(); iter != responseList_.end(); iter++) {
                if (handle == iter->first) {
                    break;
                }
            }
            return iter;
    }
    for (iter = responseList_.begin(); iter != responseList_.end(); iter++) {
        if (handle == iter->first && type == iter->second.reqType_) {
            break;
        }
    }
    return iter;
}

std::list<std::pair<uint16_t, GattRequestInfo>>::iterator GattClientProfile::impl::FindIteratorByRespones(
    ResponesType reqType, int reqId)
{
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter;
    for (iter = responseList_.begin(); iter != responseList_.end(); iter++) {
        if (reqId == iter->second.reqId_ && reqType == iter->second.reqType_) {
            break;
        }
    }
    return iter;
}
std::list<std::pair<uint16_t, GattRequestInfo>>::iterator GattClientProfile::impl::FindIteratorByResponesInfor(
    uint16_t handle, ResponesType respType)
{
    std::list<std::pair<uint16_t, GattRequestInfo>>::iterator iter;
    for (iter = responseList_.begin(); iter != responseList_.end(); iter++) {
        if (handle == iter->first && respType == iter->second.reqType_) {
            break;
        }
    }
    return iter;
}
/**
 * @brief Create cache when gatt is connected.
 *
 * @param connectHandle Indicates identify a connection.
 * @param device Indicates peer device Information.
 * @since 6.0
 */
void GattClientProfile::impl::CreateCache(uint16_t connectHandle, const GattDevice device)
{
    auto cache = cacheMap_.emplace(connectHandle, std::move(GattCache()));
    if (device.isEncryption_ == true) {
        cache.first->second.LoadFromFile(device);
    }
}
/**
 * @brief Delete cache when gatt is disconnected.
 *
 * @param connectHandle Indicates identify a connection.
 * @param device Indicates peer device Information.
 * @since 6.0
 */
void GattClientProfile::impl::DeleteCache(uint16_t connectHandle, const GattDevice device)
{
    auto cache = cacheMap_.find(connectHandle);
    if (cache != cacheMap_.end()) {
        if (device.isEncryption_ == true) {
            cache->second.StoredToFile(device);
        }
        cacheMap_.erase(cache);
        LOG_INFO("%{public}s, Device cache successfully deleted", __FUNCTION__);
    } else {
        LOG_ERROR("%{public}s:  Device cache does not exist", __FUNCTION__);
    }
}
/**
 * @brief Delete cache when gatt is disconnected.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattClientProfile::ClearCacheMap(uint16_t connectHandle) const
{
    auto cache = pimpl->cacheMap_.find(connectHandle);
    if (cache != pimpl->cacheMap_.end()) {
        cache->second.Clear();
        LOG_INFO("%{public}s, Device cache successfully deleted", __FUNCTION__);
    } else {
        LOG_ERROR("%{public}s:  Device cache does not exist", __FUNCTION__);
    }
}
/**
 * @brief Add data to ReadValueCache.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates att handle.
 * @param offset Indicates response value.
 * @param data Indicates response value.
 * @since 6.0
 */
void GattClientProfile::impl::AddReadValueCache(uint16_t connectHandle, uint16_t handle, uint16_t offset, Buffer *data)
{
    Packet *packet = nullptr;
    std::list<std::pair<uint16_t, ReadValCache>>::iterator iter;
    for (iter = readValCache_.begin(); iter != readValCache_.end(); iter++) {
        if (connectHandle == iter->first && handle == iter->second.handle_) {
            packet = iter->second.data_;
            break;
        }
    }
    if (packet == nullptr) {
        packet = PacketMalloc(0, 0, 0);
    }
    PacketPayloadAddLast(packet, data);
    if (iter == readValCache_.end()) {
        readValCache_.emplace_back(connectHandle, ReadValCache(handle, offset, packet));
    }
}
/**
 * @brief Get data from ReadValueCache.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates att handle.
 * @since 6.0
 */
uint8_t *GattClientProfile::impl::GetReadValueCache(uint16_t connectHandle, uint16_t handle)
{
    uint8_t *buffer = nullptr;
    std::list<std::pair<uint16_t, ReadValCache>>::iterator iter;
    for (iter = readValCache_.begin(); iter != readValCache_.end(); iter++) {
        if (connectHandle == iter->first && handle == iter->second.handle_) {
            buffer = (uint8_t *)malloc(PacketSize(iter->second.data_));
            if (buffer == nullptr) {
                break;
            }
            PacketRead(iter->second.data_, buffer, 0, PacketSize(iter->second.data_));
            PacketFree(iter->second.data_);
            readValCache_.erase(iter);
            break;
        }
    }
    return buffer;
}

/* @brief Delete list of request and response when gatt is disconnected.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattClientProfile::impl::DeleteList(uint16_t connectHandle)
{
    LOG_INFO("%{public}s: connectHandle is %{public}hu.", __FUNCTION__, connectHandle);
    auto reqList = requestList_.begin();
    while (reqList != requestList_.end()) {
        if (connectHandle == reqList->first) {
            requestList_.erase(reqList++);
        } else {
            reqList++;
        }
    }

    auto respList = responseList_.begin();
    while (respList != responseList_.end()) {
        if (connectHandle == respList->first) {
            responseList_.erase(respList++);
        } else {
            respList++;
        }
    }
}
/**
 * @brief Indicates connect or disconnect.
 *
 * @since 6.0
 */
class GattClientProfile::impl::GattConnectionObserverImplement : public GattConnectionObserver {
public:
    void OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        LOG_INFO("%{public}s: gatt_client connect role is %{public}d", __FUNCTION__, device.role_);
        this->clientProfile_.pimpl->CreateCache(connectionHandle, device);
    }

    void OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        LOG_INFO("%{public}s: gatt_client connect role is %{public}d", __FUNCTION__, device.role_);
        this->clientProfile_.pimpl->DeleteList(connectionHandle);
        this->clientProfile_.pimpl->SetMtuInformation(connectionHandle, false, GATT_DEFAULT_MTU);
        this->clientProfile_.pimpl->DeleteCache(connectionHandle, device);
    }

    void OnReconnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        LOG_INFO("%{public}s gatt client service", __FUNCTION__);
        this->clientProfile_.pimpl->CreateCache(connectionHandle, device);
    }

    void OnDisconnectInter(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        LOG_INFO("%{public}s gatt client service", __FUNCTION__);
        this->clientProfile_.pimpl->DeleteCache(connectionHandle, device);
    }

    GattConnectionObserverImplement(GattClientProfile &clientProfile) : clientProfile_(clientProfile)
    {}
    ~GattConnectionObserverImplement()
    {}

private:
    GattClientProfile &clientProfile_;
};
/**
 * @brief Register callback to ConnectManager.
 *
 * @since 6.0
 */
void GattClientProfile::impl::RegisterCallbackToConnectManager()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    connectionObserverId_ = GattConnectionManager::GetInstance().RegisterObserver(*connectionCallBack_);
    if (connectionObserverId_ < 0) {
        LOG_ERROR("%{public}s: GATT_profile register callback to ConnectManager failed", __FUNCTION__);
    }
}
/**
 * @brief Unregister callback to ConnectManager.
 *
 * @since 6.0
 */
void GattClientProfile::impl::DeregisterCallbackToConnectManager()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    GattConnectionManager::GetInstance().DeregisterObserver(connectionObserverId_);
}
}  // namespace bluetooth
}  // namespace OHOS
