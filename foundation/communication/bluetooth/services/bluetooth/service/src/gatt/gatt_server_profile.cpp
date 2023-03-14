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

#include "gatt_server_profile.h"
#include "att.h"
#include "bt_def.h"
#include "gatt_connection_manager.h"
#include "gatt_service_base.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
struct GattServerProfile::impl {
    class GattConnectionObserverImplement;
    impl(GattServerProfileCallback *pServerCallbackFunc, utility::Dispatcher *dispatcher, uint16_t maxMtu,
        GattServerProfile &profile)
        : requestList_(),
          connectionCallBack_(std::make_unique<GattConnectionObserverImplement>(profile)),
          pServerCallBack_(pServerCallbackFunc),
          dispatcher_(dispatcher),
          db_(),
          mtu_(maxMtu)
    {}
    std::map<uint16_t, uint16_t> mtuInfo_ = {};
    std::list<std::pair<uint16_t, GattResponesInfor>> requestList_ = {};
    std::list<std::pair<uint16_t, GattResponesInfor>> responseList_ = {};
    std::list<std::pair<uint16_t, DeviceInfo>> devList_ = {};
    std::unique_ptr<GattConnectionObserverImplement> connectionCallBack_ = {};
    GattServerProfileCallback *pServerCallBack_ = nullptr;
    utility::Dispatcher *dispatcher_ = nullptr;
    GattDatabase db_ = {};
    uint16_t mtu_ = 0;
    GattServerProfile *profile_ = nullptr;
    int connectionObserverId_ = 0;
    BT_DISALLOW_COPY_AND_ASSIGN(impl);

    static void ReceiveData(uint16_t connectHandle, uint16_t event, void *eventData, Buffer *buffer, void *context);
    static void ReceiveResponseResult(uint16_t connectHandle, int result, void *context);
    void ReceiveResponseResultPostTask(uint16_t connectHandle, int result);
    void RegisterCallbackToATT();
    static void DeregisterCallbackToATT();
    void RegisterCallbackToConnectManager();
    void DeregisterCallbackToConnectManager();
    void AddResponseList();
    void SplitReadByTypeReqMessage(uint16_t connectHandle, AttEventData *data);
    void ExchangeMTUResponse(uint16_t connectHandle, uint16_t mtu);
    void DiscoverAllPrimaryServiceResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void DiscoverAllPrimaryServiceResponse(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle);
    void DiscoverCharacteristicResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void DiscoverCharacteristicResponse(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle);
    void DiscoverPrimaryServiceByUuidResponsePostTask(uint16_t connectHandle, Buffer *value, AttEventData *data);
    void DiscoverPrimaryServiceByUuidResponse(
        uint16_t connectHandle, Buffer *value, uint16_t startHandle, uint16_t endHandle);
    void FindIncludedServiceResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void FindIncludedServiceResponse(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle);
    void DiscoverAllCharacteristicDescriptorResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void DiscoverAllCharacteristicDescriptorResponse(uint16_t connectHandle, uint16_t attHandle, uint16_t endHandle);
    void ReadValueResponsePostTask(uint16_t connectHandle, uint16_t attHandle);
    void ReadValueResponse(uint16_t connectHandle, uint16_t attHandle);
    void ReadUsingCharacteristicByUuidResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void ReadUsingCharacteristicByUuidResponseStep1(
        uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, Uuid uuid);
    RetVal ReadUsingCharacteristicByUuidResponseStep2(uint16_t connectHandle, uint16_t startHandle, uint16_t num,
        AttReadByTypeRspDataList *list, Uuid uuid, uint8_t *offset);
    void ReadBlobValueResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void ReadBlobValueResponse(uint16_t connectHandle, uint16_t attHandle, uint16_t offset);
    void ReadMultipleCharacteristicValueResponsePostTask(uint16_t connectHandle, AttEventData *data, Buffer *value);
    void ReadMultipleCharacteristicValueResponse(uint16_t connectHandle, Buffer *value);
    void WriteValueResponsePostTask(uint16_t connectHandle, AttEventData *data, Buffer *value);
    void WriteValueResponse(uint16_t connectHandle, uint16_t attHandle, Buffer *value);
    void WriteWithoutResponsePostTask(uint16_t connectHandle, AttEventData *data, Buffer *value);
    void WriteWithoutResponse(uint16_t connectHandle, uint16_t attHandle, Buffer *value);
    void WriteLongCharacteristicValueResponsePostTask(uint16_t connectHandle, AttEventData *data, Buffer *value);
    void WriteLongCharacteristicValueResponse(
        uint16_t connectHandle, uint16_t attHandle, uint16_t offset, Buffer *value);
    void ExecuteWriteResponsePostTask(uint16_t connectHandle, AttEventData *data);
    void ExecuteWriteResponse(uint16_t connectHandle, bool flag);
    void HandleValueConfirmationResponsePostTask(uint16_t connectHandle, int ret);
    void HandleValueConfirmationResponse(uint16_t connectHandle, int ret);
    void SendAttReadByTypeResponse(
        uint16_t connectHandle, uint16_t handle, uint8_t len, AttReadByTypeRspDataList *value, uint16_t num);
    static bool CheckAttHandleParameter(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, uint8_t requestId);
    static bool CheckUuidType(uint16_t connectHandle, Uuid *uuid, AttEventData *data);
    bool FindServiceEndingHandle(uint16_t attHandle);
    bool FindServiceByHandle(uint16_t attHandle, Uuid uuid);
    bool FindCharacteristicDeclarationByHandle(uint16_t attHandle, Uuid uuid);
    bool FindCharacteristicValueByUuid(uint16_t attHandle, Uuid uuid);
    bool FindCharacteristicDescriptorByUuid(uint16_t attHandle, Uuid uuid);
    Buffer *AssembleServicePackage(uint16_t attHandle);
    Buffer *AssembleCharacteristicPackage(uint16_t attHandle);
    Buffer *AssembleDescriptorPackage(uint16_t connectHandle, uint16_t attHandle);
    static void AssembleAttReadByGroupTypeRspPackage(
        AttReadGoupAttributeData *list, const GattDatabase::Service &service, uint8_t num);
    void AssembleAttReadByTypeRspSvcPackage(
        AttReadByTypeRspDataList *list, uint16_t attHandle, uint8_t num, uint8_t *offset);
    void AssembleAttReadByTypeRspCharacteristicPackage(
        AttReadByTypeRspDataList *list, uint16_t attHandle, uint8_t num, uint8_t *offset);
    bool AssembleAttReadByTypeRspDescPackage(
        AttReadByTypeRspDataList *list, uint16_t connectHandle, uint16_t attHandle, uint8_t num, uint8_t *offset);
    void AssembleAttFindInforRspSvcPackage(AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len);
    void AssembleAttFindInforRspCharacteristicPackage(
        AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len);
    void AssembleAttFindInforRspCharacteristicValPackage(
        AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len);
    void AssembleAttFindInforRspDescPackage(AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len);
    static void AssembleDataPackage(uint8_t *dest, uint8_t destMax, uint8_t *offset, uint8_t *src, uint8_t size);
    static void AssembleDataPackage(uint8_t *dest, uint8_t *offset, uint8_t src);
    static void FreeDataPackage(AttReadByTypeRspDataList *list, uint8_t num);
    bool CharacteristicPropertyIsReadable(uint16_t attHandle);
    bool DescriptorPropertyIsReadable(uint16_t attHandle);
    bool CharacteristicPropertyIsWritable(uint16_t attHandle);
    bool DescriptorPropertyIsWritable(uint16_t attHandle);
    AttError WriteDescriptorProcess(uint16_t connectHandle, uint16_t attHandle, Buffer *value);
    uint16_t GetMtuInformation(uint16_t connectHandle);
    void SetMtuInformation(uint16_t connectHandle, uint16_t mtu);
    std::list<std::pair<uint16_t, GattResponesInfor>>::iterator FindIteratorByResponesInfor(
        uint16_t handle, ResponesType respType);
    void DeleteList(uint16_t connectHandle);
    void AddDeviceList(uint16_t connectHandle, GattDevice device);
    void AddCccdValue(uint16_t connectHandle, uint16_t attHandle, uint16_t value);
    void DeleteCccdValue(uint16_t connectHandle);
    uint16_t GetCccdValue(uint16_t connectHandle, uint16_t attHandle);
};
/**
 * @brief A constructor used to create <pServerCallbackFunc> <dispatcher> and <maxMtu> instance..
 *
 * @since 6.0
 */
GattServerProfile::GattServerProfile(
    GattServerProfileCallback *pServerCallbackFunc, utility::Dispatcher *dispatcher, uint16_t maxMtu)
    : pimpl(new (std::nothrow)GattServerProfile::impl(pServerCallbackFunc, dispatcher, maxMtu, *this))
{
    if (pimpl == nullptr) {
        LOG_ERROR("GattServerProfile get pimpl error");
        return;
    }
    pimpl->RegisterCallbackToConnectManager();
}
/**
 * @brief A destructor used to delete the <pServerCallbackFunc> <dispatcher> and <maxMtu> instance.
 *
 * @since 6.0
 */
GattServerProfile::~GattServerProfile()
{
    pimpl->DeregisterCallbackToConnectManager();
}
/**
 * @brief Enable gatt server profile.
 *
 * @since 6.0
 */
void GattServerProfile::Enable() const
{
    pimpl->requestList_.clear();
    pimpl->responseList_.clear();
    pimpl->mtuInfo_.clear();
    pimpl->RegisterCallbackToATT();
}
/**
 * @brief Disable gatt server profile.
 *
 * @since 6.0
 */
void GattServerProfile::Disable() const
{
    pimpl->DeregisterCallbackToATT();
    pimpl->db_.RemoveAllServices();
}
/**
 * @brief Register callback function to ATT.
 *
 * @since 6.0
 */
void GattServerProfile::impl::RegisterCallbackToATT()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    attCallback attDataCallback = GattServerProfile::impl::ReceiveData;
    attSendDataCallback attReqCallback = GattServerProfile::impl::ReceiveResponseResult;

    ATT_ServerDataRegister(attDataCallback, this);
    ATT_ServerSendDataRegister(attReqCallback, this);
}
/**
 * @brief Unregister callback function to att.
 *
 * @since 6.0
 */
void GattServerProfile::impl::DeregisterCallbackToATT()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    ATT_ServerDataDeregister();
    ATT_ServerSendDataDeRegister();
}
/**
 * @brief Add iter to the response list.
 *
 * @since 6.0
 */
void GattServerProfile::impl::AddResponseList(void)
{
    auto iter = requestList_.begin();
    responseList_.emplace_back(*iter);
    LOG_INFO("%{public}s: server responseList size: %{public}zu", __FUNCTION__, responseList_.size());
}
/**
 * @brief This sub-procedure is used by the server to process split read by type request message.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::SplitReadByTypeReqMessage(uint16_t connectHandle, AttEventData *data)
{
    switch (data->attReadByTypeRequest.readHandleRangeUuid.uuid->uuid16) {
        case UUID_INCLUDE_SERVICE:
            FindIncludedServiceResponsePostTask(connectHandle, data);
            break;
        case UUID_CHARACTERISTIC:
            DiscoverCharacteristicResponsePostTask(connectHandle, data);
            break;
        default:
            ReadUsingCharacteristicByUuidResponsePostTask(connectHandle, data);
            break;
    }
}
/**
 * @brief This sub-procedure is used by the server to process received data from att.
 *
 * @param connectHandle Indicates identify a connection.
 * @param event Indicates client callback event id.
 * @param eventData Indicates att data.
 * @param buffer Indicates att data.
 * @param context Indicates client callback function.
 * @since 6.0
 */
void GattServerProfile::impl::ReceiveData(
    uint16_t connectHandle, uint16_t event, void *eventData, Buffer *buffer, void *context)
{
    LOG_INFO("GattServerProfile::%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    if (eventData == nullptr) {
        return;
    }

    auto object = static_cast<GattServerProfile::impl *>(context);
    switch (event) {
        case ATT_EXCHANGE_MTU_REQUEST_ID:
            object->ExchangeMTUResponse(connectHandle, ((AttEventData *)eventData)->attExchangeMtuRequest.mtuSize);
            break;
        case ATT_FIND_INFORMATION_REQUEST_ID:
            object->DiscoverAllCharacteristicDescriptorResponsePostTask(connectHandle, ((AttEventData *)eventData));
            break;
        case ATT_FIND_BY_TYPE_VALUE_REQUEST_ID:
            object->DiscoverPrimaryServiceByUuidResponsePostTask(connectHandle, buffer, (AttEventData *)eventData);
            break;
        case ATT_READ_BY_TYPE_REQUEST_ID:
            object->SplitReadByTypeReqMessage(connectHandle, (AttEventData *)eventData);
            break;
        case ATT_READ_REQUEST_ID:
            object->ReadValueResponsePostTask(
                connectHandle, ((AttEventData *)eventData)->attReadRequest.readHandle.attHandle);
            break;
        case ATT_READ_BLOB_REQUEST_ID:
            object->ReadBlobValueResponsePostTask(connectHandle, (AttEventData *)eventData);
            break;
        case ATT_READ_MULTIPLE_REQUEST_ID:
            object->ReadMultipleCharacteristicValueResponsePostTask(connectHandle, (AttEventData *)eventData, buffer);
            break;
        case ATT_READ_BY_GROUP_TYPE_REQUEST_ID:
            object->DiscoverAllPrimaryServiceResponsePostTask(connectHandle, (AttEventData *)eventData);
            break;
        case ATT_WRITE_REQUEST_ID:
            object->WriteValueResponsePostTask(connectHandle, (AttEventData *)eventData, buffer);
            break;
        case ATT_WRITE_COMMAND_ID:
            object->WriteWithoutResponsePostTask(connectHandle, (AttEventData *)eventData, buffer);
            break;
        case ATT_PREPARE_WRITE_REQUEST_ID:
            object->WriteLongCharacteristicValueResponsePostTask(connectHandle, (AttEventData *)eventData, buffer);
            break;
        case ATT_EXECUTE_WRITE_REQUEST_ID:
            object->ExecuteWriteResponsePostTask(connectHandle, (AttEventData *)eventData);
            break;
        case ATT_HANDLE_VALUE_CONFIRMATION_ID:
            object->HandleValueConfirmationResponsePostTask(connectHandle, GATT_SUCCESS);
            break;
        case ATT_TRANSACTION_TIME_OUT_ID:
            object->HandleValueConfirmationResponsePostTask(connectHandle, GATT_FAILURE);
            break;
        default:
            AttError errorData = {*(uint8_t *)eventData, 0, ATT_REQUEST_NOT_SUPPORTED};
            ATT_ErrorResponse(connectHandle, &errorData);
            LOG_INFO("%{public}s: ATT_REQUEST_NOT_FOUND", __FUNCTION__);
            break;
    }
}
/**
 * @brief This sub-procedure is used by the server to process received att response result.
 *
 * @param connectHandle Indicates identify a connection.
 * @param result Indicates Response result.
 * @param context Indicates server callback function.
 * @since 6.0
 */
void GattServerProfile::impl::ReceiveResponseResult(uint16_t connectHandle, int result, void *context)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto object = static_cast<GattServerProfile::impl *>(context);
    object->dispatcher_->PostTask(std::bind(&impl::ReceiveResponseResultPostTask, object, connectHandle, result));
}

void GattServerProfile::impl::ReceiveResponseResultPostTask(uint16_t connectHandle, int result)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    auto iter = requestList_.begin();
    if (iter != requestList_.end()) {
        if (!result) {
            switch (iter->second.respType_) {
                case EXCHANGE_MTU:
                    SetMtuInformation(connectHandle, iter->second.value_);
                    pServerCallBack_->OnExchangeMtuEvent(connectHandle, iter->second.value_);
                    break;
                case SEND_INDICATION:
                case READ_LONG_CHARACTERISTIC_VALUE:
                    AddResponseList();
                    break;
                default:
                    break;
            }
        }
        requestList_.erase(iter);
    }
}
/**
 * @brief This sub-procedure is used by the server to set the ATT_MTU to the maximum possible value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param mtu Indicates att bearer transmission unit.
 * @since 6.0
 */
void GattServerProfile::impl::ExchangeMTUResponse(uint16_t connectHandle, uint16_t mtu)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    AttError errorData = {READ_BY_GROUP_TYPE_REQUEST, 0, ATT_REQUEST_NOT_SUPPORTED};

    if (mtu < GATT_DEFAULT_MTU) {
        ATT_ErrorResponse(connectHandle, &errorData);
        return;
    } else if (mtu > mtu_) {
        mtu = mtu_;
    }
    requestList_.emplace_back(connectHandle, GattResponesInfor(EXCHANGE_MTU, mtu));
    ATT_ExchangeMTUResponse(connectHandle, mtu_);
}
/**
 * @brief This sub-procedure is used by the servert to respond that discover all primary services.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::DiscoverAllPrimaryServiceResponsePostTask(uint16_t connectHandle, AttEventData *data)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    uint16_t startHandle = data->attReadByGroupTypeRequest.readGroupRequest.handleRange.startHandle,
             endHandle = data->attReadByGroupTypeRequest.readGroupRequest.handleRange.endHandle;
    dispatcher_->PostTask(
        std::bind(&impl::DiscoverAllPrimaryServiceResponse, this, connectHandle, startHandle, endHandle));
}

void GattServerProfile::impl::DiscoverAllPrimaryServiceResponse(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    uint8_t dataLen = 0;
    uint16_t groupSize = 0;
    uint16_t preUuidLen = 0;
    uint16_t serviceNum = 0;
    AttError errorData = {READ_BY_GROUP_TYPE_REQUEST, startHandle, 0};
    AttReadGoupAttributeData serviceList[GATT_VALUE_LEN_MAX] = {{0, 0, nullptr}};
    auto service = db_.GetServices().begin();

    if (CheckAttHandleParameter(connectHandle, startHandle, endHandle, READ_BY_GROUP_TYPE_REQUEST)) {
        return;
    }
    for (; service != db_.GetServices().end(); service++) {
        if (startHandle <= service->second.handle_ && service->second.isPrimary_) {
            uint16_t uuidLen = service->second.uuid_.GetUuidType();
            if (uuidLen != UUID_16BIT_LEN) {
                uuidLen = UUID_128BIT_LEN;
            }
            if (preUuidLen != 0 && preUuidLen != uuidLen) {
                break;
            }
            preUuidLen = uuidLen;
            dataLen = sizeof(startHandle) + sizeof(endHandle) + uuidLen;
            groupSize = groupSize + dataLen;
            if (groupSize <= GetMtuInformation(connectHandle) - sizeof(groupSize)) {
                AssembleAttReadByGroupTypeRspPackage(serviceList, service->second, serviceNum);
                serviceNum++;
            } else {
                break;
            }
        }
    }
    if (serviceNum) {
        ATT_ReadByGroupTypeResponse(connectHandle, dataLen, serviceList, serviceNum);
        for (int i = 0; i < serviceNum; i++) {
            free(serviceList[i].attributeValue);
        }
    } else {
        errorData.errorCode = ATT_ATTRIBUTE_NOT_FOUND;
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used by the server to respond that discover all primary services by uuid.
 *
 * @param connectHandle Indicates identify a connection.
 * @param value Indicates att data.
 * @param data Indicates att handle infor.
 * @since 6.0
 */
void GattServerProfile::impl::DiscoverPrimaryServiceByUuidResponsePostTask(
    uint16_t connectHandle, Buffer *value, AttEventData *data)
{
    Buffer *attData = BufferRefMalloc(value);
    uint16_t startHandle = data->attFindByTypeValueRequest.findByTypeValueRequest.handleRange.startHandle;
    uint16_t endHandle = data->attFindByTypeValueRequest.findByTypeValueRequest.handleRange.endHandle;

    dispatcher_->PostTask(
        std::bind(&impl::DiscoverPrimaryServiceByUuidResponse, this, connectHandle, attData, startHandle, endHandle));
}

void GattServerProfile::impl::DiscoverPrimaryServiceByUuidResponse(
    uint16_t connectHandle, Buffer *value, uint16_t startHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    Uuid uuid;
    uint16_t listNum = 0;
    uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};
    AttHandleInfo handleInfoList[GATT_VALUE_LEN_MAX] = {{0, 0}};
    AttError errorData = {FIND_BY_TYPE_VALUE_REQUEST, startHandle, ATT_ATTRIBUTE_NOT_FOUND};
    auto service = db_.GetServices().begin();

    if (CheckAttHandleParameter(connectHandle, startHandle, endHandle, FIND_BY_TYPE_VALUE_REQUEST)) {
        return;
    }
    (void)memcpy_s(uuid128Bit, UUID_128BIT_LEN, BufferPtr(value), BufferGetSize(value));
    if (BufferGetSize(value) == UUID_16BIT_LEN) {
        uuid = Uuid::ConvertFrom16Bits((uuid128Bit[0] | (uuid128Bit[1] << BIT_8)));
    } else if (BufferGetSize(value) == UUID_128BIT_LEN) {
        uuid = Uuid::ConvertFromBytesLE(uuid128Bit, UUID_128BIT_LEN);
    } else {
        goto ATT_ERROR_RESPONSE;
    }
    for (; service != db_.GetServices().end(); service++) {
        if (startHandle <= service->second.handle_ && service->second.uuid_.operator==(uuid) &&
            service->second.isPrimary_) {
            handleInfoList->attHandle = service->second.handle_;
            handleInfoList->groupEndHandle = service->second.endHandle_;
            if ((uint16_t)((listNum + sizeof(uint8_t)) * (sizeof(startHandle) + sizeof(endHandle))) >
                GetMtuInformation(connectHandle) - 1) {
                break;
            }
            listNum++;
        }
    }
    if (listNum) {
        ATT_FindByTypeValueResponse(connectHandle, handleInfoList, listNum);
    } else {
    ATT_ERROR_RESPONSE:
        ATT_ErrorResponse(connectHandle, &errorData);
    }
    BufferFree(value);
}
/**
 * @brief This sub-procedure is used by the server to respond that find include 16bit uuid services.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::FindIncludedServiceResponsePostTask(uint16_t connectHandle, AttEventData *data)
{
    uint16_t startHandle = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.startHandle;
    uint16_t endHandle = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.endHandle;

    dispatcher_->PostTask(std::bind(&impl::FindIncludedServiceResponse, this, connectHandle, startHandle, endHandle));
}

void GattServerProfile::impl::FindIncludedServiceResponse(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    uint8_t offset = 0;
    AttReadByTypeRspDataList valueList[GATT_VALUE_LEN_MAX] = {{{0}, nullptr}};
    AttError errorData = {READ_BY_TYPE_REQUEST, startHandle, 0};

    if (CheckAttHandleParameter(connectHandle, startHandle, endHandle, READ_BY_TYPE_REQUEST)) {
        return;
    }
    for (; startHandle <= endHandle; startHandle++) {
        auto isvc = db_.GetIncludeService(startHandle);
        if (isvc != nullptr) {
            uint8_t len = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t);
            valueList[0].attHandle.attHandle = isvc->handle_;
            valueList[0].attributeValue = (uint8_t *)malloc(len);
            if (valueList[0].attributeValue == nullptr) {
                LOG_ERROR("%{public}s, malloc fail", __FUNCTION__);
                return;
            }
            AssembleDataPackage(
                valueList[0].attributeValue, len, &offset, (uint8_t *)&(isvc->startHandle_), sizeof(uint16_t));
            AssembleDataPackage(
                valueList[0].attributeValue, len, &offset, (uint8_t *)&(isvc->endHandle_), sizeof(uint16_t));
            if (isvc->uuid_.GetUuidType() == UUID_16BIT_LEN) {
                uint16_t uuid16bit = isvc->uuid_.ConvertTo16Bits();
                AssembleDataPackage(
                    valueList[0].attributeValue, len, &offset, (uint8_t *)&(uuid16bit), sizeof(uint16_t));
                len = len + UUID_16BIT_LEN;
            }
            ATT_ReadByTypeResponse(connectHandle, len, valueList, sizeof(uint8_t));
            free(valueList[0].attributeValue);
            return;
        }
        if (FindServiceEndingHandle(startHandle)) {
            break;
        }
    }
    errorData.errorCode = ATT_ATTRIBUTE_NOT_FOUND;
    ATT_ErrorResponse(connectHandle, &errorData);
}
/**
 * @brief This sub-procedure is used by the server to respond that discover all characteristics.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::DiscoverCharacteristicResponsePostTask(uint16_t connectHandle, AttEventData *data)
{
    uint16_t startHandle = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.startHandle;
    uint16_t endHandle = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.endHandle;

    dispatcher_->PostTask(
        std::bind(&impl::DiscoverCharacteristicResponse, this, connectHandle, startHandle, endHandle));
}

void GattServerProfile::impl::DiscoverCharacteristicResponse(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    uint8_t dataLen = 0;
    uint16_t groupSize = 0;
    uint16_t valueNum = 0;
    uint16_t preUuidLen = 0;
    AttReadByTypeRspDataList valueList[GATT_VALUE_LEN_MAX] = {{{0}, nullptr}};

    if (CheckAttHandleParameter(connectHandle, startHandle, endHandle, READ_BY_TYPE_REQUEST)) {
        return;
    }
    for (; startHandle <= endHandle; startHandle++) {
        uint8_t offset = 0;
        auto characteristic = db_.GetCharacteristic(startHandle + MIN_ATTRIBUTE_HANDLE);
        if (characteristic != nullptr) {
            uint16_t uuidLen = characteristic->uuid_.GetUuidType();
            if (uuidLen != UUID_16BIT_LEN) {
                uuidLen = UUID_128BIT_LEN;
            }
            if (preUuidLen != 0 && preUuidLen != uuidLen) {
                break;
            }
            preUuidLen = uuidLen;
            dataLen = sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t) + uuidLen;
            groupSize = groupSize + dataLen;
            if (characteristic->valueHandle_ <= endHandle &&
                groupSize <= GetMtuInformation(connectHandle) - sizeof(groupSize)) {
                AssembleAttReadByTypeRspCharacteristicPackage(
                    valueList, startHandle + MIN_ATTRIBUTE_HANDLE, valueNum, &offset);
                valueNum++;
            } else {
                break;
            }
        } else if (FindServiceEndingHandle(startHandle)) {
            break;
        }
    }
    SendAttReadByTypeResponse(connectHandle, startHandle, dataLen, valueList, valueNum);
}
/**
 * @brief This sub-procedure is used by the server to respond that discover all characteristic descriptors.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::DiscoverAllCharacteristicDescriptorResponsePostTask(
    uint16_t connectHandle, AttEventData *data)
{
    uint16_t startHandle = data->attFindInformationRequest.findInformationRequest.startHandle;
    uint16_t endHandle = data->attFindInformationRequest.findInformationRequest.endHandle;

    dispatcher_->PostTask(
        std::bind(&impl::DiscoverAllCharacteristicDescriptorResponse, this, connectHandle, startHandle, endHandle));
}

void GattServerProfile::impl::DiscoverAllCharacteristicDescriptorResponse(
    uint16_t connectHandle, uint16_t attHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    uint16_t pairNum = 0;
    uint16_t uuidLen = 0;
    uint16_t groupSize = 0;
    uint16_t preUuidLen = 0;
    AttHandleUuid handleUUIDPairs[GATT_VALUE_LEN_MAX] = {{0, {0, {0}}}};
    AttError errorData = {FIND_INFORMATION_REQUEST, attHandle, ATT_ATTRIBUTE_NOT_FOUND};

    if (CheckAttHandleParameter(connectHandle, attHandle, endHandle, FIND_INFORMATION_REQUEST)) {
        return;
    }
    for (; attHandle <= endHandle; attHandle++) {
        if (db_.GetService(attHandle) != nullptr) {
            AssembleAttFindInforRspSvcPackage(handleUUIDPairs, attHandle, pairNum, &uuidLen);
        } else if (db_.GetDescriptor(attHandle) != nullptr) {
            AssembleAttFindInforRspDescPackage(handleUUIDPairs, attHandle, pairNum, &uuidLen);
        } else if (db_.GetCharacteristic(attHandle) != nullptr) {
            AssembleAttFindInforRspCharacteristicValPackage(handleUUIDPairs, attHandle, pairNum, &uuidLen);
        } else if (db_.GetCharacteristic(attHandle + MIN_ATTRIBUTE_HANDLE) != nullptr) {
            AssembleAttFindInforRspCharacteristicPackage(handleUUIDPairs, attHandle, pairNum, &uuidLen);
        } else {
            if (FindServiceEndingHandle(attHandle)) {
                break;
            }
            continue;
        }
        if (preUuidLen != 0 && preUuidLen != uuidLen) {
            break;
        }
        preUuidLen = uuidLen;
        groupSize = groupSize + sizeof(uint16_t) + uuidLen;
        if (groupSize <= GetMtuInformation(connectHandle) - sizeof(groupSize)) {
            pairNum++;
        } else {
            break;
        }
    }

    if (pairNum) {
        ATT_FindInformationResponse(connectHandle, handleUUIDPairs->uuid.type, handleUUIDPairs, pairNum);
    } else {
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to respond that read value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param attHandle Indicates attribute handle.
 * @since 6.0
 */
void GattServerProfile::impl::ReadValueResponsePostTask(uint16_t connectHandle, uint16_t attHandle)
{
    dispatcher_->PostTask(std::bind(&impl::ReadValueResponse, this, connectHandle, attHandle));
}

void GattServerProfile::impl::ReadValueResponse(uint16_t connectHandle, uint16_t attHandle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    Buffer *value = nullptr;
    AttError errorData = {READ_REQUEST, attHandle, 0};

    if (db_.GetService(attHandle) != nullptr) {
        value = AssembleServicePackage(attHandle);
        if (value != nullptr) {
            ATT_ReadResponse(connectHandle, value);
            BufferFree(value);
        } else {
            errorData.errorCode = ATT_INVALID_HANDLE;
        }
    } else if (db_.GetDescriptor(attHandle) != nullptr) {
        if (DescriptorPropertyIsReadable(attHandle)) {
            value = AssembleDescriptorPackage(connectHandle, attHandle);
            if (value != nullptr) {
                ATT_ReadResponse(connectHandle, value);
                BufferFree(value);
            } else {
                pServerCallBack_->OnDescriptorReadEvent(connectHandle, attHandle);
            }
        } else {
            errorData.errorCode = ATT_READ_NOT_PERMITTED;
        }
    } else if (db_.GetCharacteristic(attHandle) != nullptr) {
        if (CharacteristicPropertyIsReadable(attHandle)) {
            pServerCallBack_->OnReadCharacteristicValueEvent(connectHandle, attHandle);
        } else {
            errorData.errorCode = ATT_READ_NOT_PERMITTED;
        }
    } else if (db_.GetCharacteristic(attHandle + MIN_ATTRIBUTE_HANDLE) != nullptr) {
        value = AssembleCharacteristicPackage(attHandle + MIN_ATTRIBUTE_HANDLE);
        if (value != nullptr) {
            ATT_ReadResponse(connectHandle, value);
            BufferFree(value);
        } else {
            errorData.errorCode = ATT_INVALID_HANDLE;
        }
    } else {
        errorData.errorCode = ATT_INVALID_HANDLE;
    }
    if (errorData.errorCode) {
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to respond that read a characteristic value by uuid.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::ReadUsingCharacteristicByUuidResponsePostTask(uint16_t connectHandle, AttEventData *data)
{
    Uuid uuid;
    uint16_t startHandle = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.startHandle;
    uint16_t endHandle = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.endHandle;
    if (CheckUuidType(connectHandle, &uuid, data)) {
        return;
    }
    dispatcher_->PostTask(std::bind(
        &impl::ReadUsingCharacteristicByUuidResponseStep1, this, connectHandle, startHandle, endHandle, uuid));
}

void GattServerProfile::impl::ReadUsingCharacteristicByUuidResponseStep1(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, Uuid uuid)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    uint8_t dataLen = 0;
    uint8_t preDataLen = 0;
    uint16_t groupSize = 0;
    uint16_t valueNum = 0;
    AttReadByTypeRspDataList valueList[GATT_VALUE_LEN_MAX] = {{{0}, nullptr}};

    if (CheckAttHandleParameter(connectHandle, startHandle, endHandle, READ_BY_TYPE_REQUEST)) {
        return;
    }
    for (; startHandle <= endHandle; startHandle++) {
        uint8_t offset = 0;
        RetVal ret =
            ReadUsingCharacteristicByUuidResponseStep2(connectHandle, startHandle, valueNum, valueList, uuid, &offset);
        if (ret == RET_RETURN) {
            return;
        } else if (ret == RET_BREAK) {
            break;
        } else if (ret == RET_CONTINUE) {
            continue;
        }
        groupSize = groupSize + offset;
        dataLen = offset + sizeof(uint16_t);
        if (preDataLen == 0 ||
            (preDataLen == dataLen && groupSize <= GetMtuInformation(connectHandle) - sizeof(groupSize))) {
            valueNum++;
            preDataLen = dataLen;
        } else {
            dataLen = preDataLen;
            break;
        }
    }

    SendAttReadByTypeResponse(connectHandle, startHandle, dataLen, valueList, valueNum);
}

RetVal GattServerProfile::impl::ReadUsingCharacteristicByUuidResponseStep2(uint16_t connectHandle, uint16_t startHandle,
    uint16_t num, AttReadByTypeRspDataList *list, Uuid uuid, uint8_t *offset)
{
    RetVal ret = RET_NORMAL;
    AttError errorData = {READ_BY_TYPE_REQUEST, startHandle, ATT_READ_NOT_PERMITTED};

    if (FindServiceByHandle(startHandle, uuid)) {
        AssembleAttReadByTypeRspSvcPackage(list, startHandle, num, offset);
    } else if (FindCharacteristicDescriptorByUuid(startHandle, uuid)) {
        if (DescriptorPropertyIsReadable(startHandle)) {
            if (AssembleAttReadByTypeRspDescPackage(list, connectHandle, startHandle, num, offset)) {
                pServerCallBack_->OnReadUsingCharacteristicUuidEvent(connectHandle, startHandle);
                ret = RET_RETURN;
            }
        } else {
            ATT_ErrorResponse(connectHandle, &errorData);
            ret = RET_RETURN;
        }
    } else if (FindCharacteristicValueByUuid(startHandle, uuid)) {
        if (CharacteristicPropertyIsReadable(startHandle)) {
            pServerCallBack_->OnReadUsingCharacteristicUuidEvent(connectHandle, startHandle);
        } else {
            ATT_ErrorResponse(connectHandle, &errorData);
        }
        ret = RET_RETURN;
    } else if (FindCharacteristicDeclarationByHandle(startHandle, uuid)) {
        AssembleAttReadByTypeRspCharacteristicPackage(list, startHandle + MIN_ATTRIBUTE_HANDLE, num, offset);
    } else {
        if (FindServiceEndingHandle(startHandle)) {
            ret = RET_BREAK;
        } else {
            ret = RET_CONTINUE;
        }
    }

    return ret;
}
/**
 * @brief This sub-procedure is used to respond that read long characteristic value.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::ReadBlobValueResponsePostTask(uint16_t connectHandle, AttEventData *data)
{
    uint16_t attHandle = data->attReadBlobRequest.readBlob.attHandle;
    uint16_t offset = data->attReadBlobRequest.readBlob.offset;
    dispatcher_->PostTask(std::bind(&impl::ReadBlobValueResponse, this, connectHandle, attHandle, offset));
}

void GattServerProfile::impl::ReadBlobValueResponse(uint16_t connectHandle, uint16_t attHandle, uint16_t offset)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    Buffer *buffer = nullptr;
    uint16_t bufSize = GetMtuInformation(connectHandle) - sizeof(uint8_t);
    AttError errorData = {READ_BLOB_REQUEST, attHandle, 0};

    auto iter = FindIteratorByResponesInfor(connectHandle, READ_LONG_CHARACTERISTIC_VALUE);
    if (iter != responseList_.end()) {
        uint16_t len = iter->second.value_;
        if (len < offset) {
            errorData.errorCode = ATT_INVALID_OFFSET;
        } else {
            if (len < bufSize + offset) {
                bufSize = len - offset;
            } else {
                requestList_.emplace_back(connectHandle,
                    GattResponesInfor(READ_LONG_CHARACTERISTIC_VALUE, iter->second.value_, iter->second.data_));
            }
            buffer = GattServiceBase::BuildBuffer(iter->second.data_->get(), (size_t)offset, (size_t)bufSize);
            responseList_.erase(iter);
        }
    } else if (offset != 0x00) {
        errorData.errorCode = ATT_INVALID_OFFSET;
    } else {
        if (db_.GetService(attHandle) != nullptr) {
            buffer = AssembleServicePackage(attHandle);
        } else if (db_.GetDescriptor(attHandle) != nullptr) {
            if (!DescriptorPropertyIsReadable(attHandle)) {
                errorData.errorCode = ATT_READ_NOT_PERMITTED;
            }
        } else if (db_.GetCharacteristic(attHandle) != nullptr) {
            if (!CharacteristicPropertyIsReadable(attHandle)) {
                errorData.errorCode = ATT_READ_NOT_PERMITTED;
            }
        } else if (db_.GetCharacteristic(attHandle + MIN_ATTRIBUTE_HANDLE) != nullptr) {
            buffer = AssembleCharacteristicPackage(attHandle + MIN_ATTRIBUTE_HANDLE);
        } else {
            errorData.errorCode = ATT_INVALID_HANDLE;
        }
    }
    if (errorData.errorCode) {
        ATT_ErrorResponse(connectHandle, &errorData);
    } else if (buffer != nullptr) {
        ATT_ReadBlobResponse(connectHandle, buffer);
        BufferFree(buffer);
    } else {
        pServerCallBack_->ReadBlobValueEvent(connectHandle, attHandle);
    }
}
/**
 * @brief This sub-procedure is used to respond that read multiple characteristic values.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattServerProfile::impl::ReadMultipleCharacteristicValueResponsePostTask(
    uint16_t connectHandle, AttEventData *data, Buffer *value)
{
    Buffer *attData = BufferRefMalloc(value);
    dispatcher_->PostTask(std::bind(&impl::ReadMultipleCharacteristicValueResponse, this, connectHandle, attData));
}

void GattServerProfile::impl::ReadMultipleCharacteristicValueResponse(uint16_t connectHandle, Buffer *value)
{
    auto dataPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), BufferGetSize(value));
    pServerCallBack_->OnMultipleCharacteristicValueEvent(connectHandle, dataPtr, BufferGetSize(value));
}
/**
 * @brief This sub-procedure is used to respond that write values.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param value Indicates value of the schedule settings.
 * @since 6.0
 */
void GattServerProfile::impl::WriteValueResponsePostTask(uint16_t connectHandle, AttEventData *data, Buffer *value)
{
    Buffer *attData = BufferRefMalloc(value);
    uint16_t attHandle = data->attWriteRequest.writeRequest.attHandle;

    dispatcher_->PostTask(std::bind(&impl::WriteValueResponse, this, connectHandle, attHandle, attData));
}

void GattServerProfile::impl::WriteValueResponse(uint16_t connectHandle, uint16_t attHandle, Buffer *value)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    AttError errorData = {WRITE_REQUEST, attHandle, 0};

    if (db_.GetService(attHandle) != nullptr) {
        errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
    } else if (db_.GetDescriptor(attHandle) != nullptr) {
        if (DescriptorPropertyIsWritable(attHandle)) {
            errorData = WriteDescriptorProcess(connectHandle, attHandle, value);
        } else {
            errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
        }
    } else if (db_.GetCharacteristic(attHandle) != nullptr) {
        if (CharacteristicPropertyIsWritable(attHandle)) {
            auto cccPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), BufferGetSize(value));
            pServerCallBack_->OnWriteCharacteristicValueEvent(connectHandle, attHandle, cccPtr, BufferGetSize(value));
        } else {
            errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
        }
    } else if (db_.GetCharacteristic(MIN_ATTRIBUTE_HANDLE + attHandle) != nullptr) {
        errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
    } else {
        errorData.errorCode = ATT_INVALID_HANDLE;
    }

    if (errorData.errorCode) {
        ATT_ErrorResponse(connectHandle, &errorData);
    }
    BufferFree(value);
}
/**
 * @brief This sub-procedure is used to respond that write without response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param value Indicates value of the schedule settings.
 * @since 6.0
 */
void GattServerProfile::impl::WriteWithoutResponsePostTask(uint16_t connectHandle, AttEventData *data, Buffer *value)
{
    Buffer *attData = BufferRefMalloc(value);
    uint16_t attHandle = data->attWriteCommand.writeCommand.attHandle;

    dispatcher_->PostTask(std::bind(&impl::WriteWithoutResponse, this, connectHandle, attHandle, attData));
}

void GattServerProfile::impl::WriteWithoutResponse(uint16_t connectHandle, uint16_t attHandle, Buffer *value)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (db_.GetDescriptor(attHandle) != nullptr) {
        if (DescriptorPropertyIsWritable(attHandle)) {
            auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), BufferGetSize(value));
            pServerCallBack_->OnWriteWithoutResponseEvent(connectHandle, attHandle, sharedPtr, BufferGetSize(value));
        }
    } else if (db_.GetCharacteristic(attHandle) != nullptr) {
        if (db_.GetCharacteristic(attHandle)->properties_ & CHARACTERISTIC_PROPERTIE_WRITE_WITHOUT_RESPONSE) {
            auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), BufferGetSize(value));
            pServerCallBack_->OnWriteWithoutResponseEvent(connectHandle, attHandle, sharedPtr, BufferGetSize(value));
        }
    }
    BufferFree(value);
}
/**
 * @brief This sub-procedure is used to respond that write long characteristic values.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param value Indicates value of the schedule settings.
 * @since 6.0
 */
void GattServerProfile::impl::WriteLongCharacteristicValueResponsePostTask(
    uint16_t connectHandle, AttEventData *data, Buffer *value)
{
    Buffer *attData = BufferRefMalloc(value);
    uint16_t attHandle = data->attPrepareWriteResponse.prepareWrite.handleValue.attHandle;
    uint16_t offset = data->attPrepareWriteResponse.prepareWrite.offset;

    dispatcher_->PostTask(
        std::bind(&impl::WriteLongCharacteristicValueResponse, this, connectHandle, attHandle, offset, attData));
}

void GattServerProfile::impl::WriteLongCharacteristicValueResponse(
    uint16_t connectHandle, uint16_t attHandle, uint16_t offset, Buffer *value)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    size_t len = BufferGetSize(value);
    AttError errorData = {PREPARE_WRITE_REQUEST, attHandle, 0};

    if (db_.GetService(attHandle) != nullptr) {
        errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
    } else if (db_.GetDescriptor(attHandle) != nullptr) {
        if (DescriptorPropertyIsWritable(attHandle)) {
            auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), len);
            pServerCallBack_->OnPrepareWriteValueEvent(connectHandle, attHandle, offset, sharedPtr, len);
        } else {
            errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
        }
    } else if (db_.GetCharacteristic(attHandle) != nullptr) {
        if (CharacteristicPropertyIsWritable(attHandle)) {
            auto sharedPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), len);
            pServerCallBack_->OnPrepareWriteValueEvent(connectHandle, attHandle, offset, sharedPtr, len);
        } else {
            errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
        }
    } else if (db_.GetCharacteristic(MIN_ATTRIBUTE_HANDLE + attHandle) != nullptr) {
        errorData.errorCode = ATT_WRITE_NOT_PERMITTED;
    } else {
        errorData.errorCode = ATT_INVALID_HANDLE;
    }

    if (errorData.errorCode) {
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to respond that execute write values.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @since 6.0
 */
void GattServerProfile::impl::ExecuteWriteResponsePostTask(uint16_t connectHandle, AttEventData *data)
{
    bool flag = data->attExecuteWriteRequest.excuteWrite.flag;
    dispatcher_->PostTask(std::bind(&impl::ExecuteWriteResponse, this, connectHandle, flag));
}

void GattServerProfile::impl::ExecuteWriteResponse(uint16_t connectHandle, bool flag)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    pServerCallBack_->OnExecuteWriteValueEvent(connectHandle, flag);
}
/**
 * @brief The Attribute Protocol Handle Value Indication is used to perform this subprocedure.
 *
 * @param connectHandle Indicates identify a connection.
 * @param ret Indicates result of respond.
 * @since 6.0
 */
void GattServerProfile::impl::HandleValueConfirmationResponsePostTask(uint16_t connectHandle, int ret)
{
    dispatcher_->PostTask(std::bind(&impl::HandleValueConfirmationResponse, this, connectHandle, ret));
}

void GattServerProfile::impl::HandleValueConfirmationResponse(uint16_t connectHandle, int ret)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    auto attResp = FindIteratorByResponesInfor(connectHandle, SEND_INDICATION);
    if (attResp == responseList_.end()) {
        return;
    }
    pServerCallBack_->OnIndicationEvent(connectHandle, attResp->second.value_, ret);
}
/**
 * @brief This sub-procedure is used to Send Att ReadByTypeResponse.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates handle of error response.
 * @param len Indicates value length.
 * @param value Indicates send value.
 * @param num Indicates handle value pair.
 * @since 6.0
 */
void GattServerProfile::impl::SendAttReadByTypeResponse(
    uint16_t connectHandle, uint16_t handle, uint8_t len, AttReadByTypeRspDataList *value, uint16_t num)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    AttError errorData = {READ_BY_TYPE_REQUEST, handle, ATT_ATTRIBUTE_NOT_FOUND};

    if (num) {
        ATT_ReadByTypeResponse(connectHandle, len, value, num);
        FreeDataPackage(value, num);
    } else {
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief Check the correctness of the attribute handle.
 *
 * @param connectHandle Indicates identify a connection.
 * @param startHandle Indicates attribute start handle.
 * @param endHandle Indicates attribute end handle.
 * @param requestId Indicates send request type.
 * @return Returns true if the parameter is invalid. Returns false if the parameter is valid.
 * @since 6.0
 */
bool GattServerProfile::impl::CheckAttHandleParameter(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, uint8_t requestId)
{
    bool result = false;
    AttError errorData = {requestId, startHandle, ATT_INVALID_HANDLE};

    if (startHandle == INVALID_ATTRIBUTE_HANDLE || startHandle > endHandle) {
        ATT_ErrorResponse(connectHandle, &errorData);
        result = true;
    }

    return result;
}
/**
 * @brief Check the correctness of the uuid type.
 *
 * @param connectHandle Indicates identify a connection.
 * @param uuid Indicates uuid of expectation.
 * @param data Indicates attribute data.
 * @return Returns true if the uuid parameter is invalid. Returns false if the parameter is valid.
 * @since 6.0
 */
bool GattServerProfile::impl::CheckUuidType(uint16_t connectHandle, Uuid *uuid, AttEventData *data)
{
    bool result = false;
    AttError errorData;

    if (data->attReadByTypeRequest.readHandleRangeUuid.uuid->type == BT_UUID_16) {
        *uuid = Uuid::ConvertFrom16Bits(data->attReadByTypeRequest.readHandleRangeUuid.uuid->uuid16);
    } else if (data->attReadByTypeRequest.readHandleRangeUuid.uuid->type == BT_UUID_128) {
        *uuid = Uuid::ConvertFromBytesLE(data->attReadByTypeRequest.readHandleRangeUuid.uuid->uuid128, UUID_128BIT_LEN);
    } else {
        errorData.reqOpcode = READ_BY_TYPE_REQUEST;
        errorData.errorCode = ATT_ATTRIBUTE_NOT_FOUND;
        errorData.attHandleInError = data->attReadByTypeRequest.readHandleRangeUuid.handleRange.startHandle;
        ATT_ErrorResponse(connectHandle, &errorData);
        result = true;
    }

    return result;
}
/**
 * @brief Check if the handle is an end handle.
 *
 * @param attHandle Indicates attribute start handle.
 * @return Returns true if the attHandle is ending handle.
 * @since 6.0
 */
bool GattServerProfile::impl::FindServiceEndingHandle(uint16_t attHandle)
{
    bool result = false;

    if (db_.GetServices().begin() == db_.GetServices().end() ||
        attHandle > db_.GetServices().rbegin()->second.endHandle_ || attHandle == MAX_ATTRIBUTE_HANDLE) {
        result = true;
    }

    return result;
}
/**
 * @brief Confirm through the handle that the handle belongs to a service handle.
 *
 * @param attHandle Indicates attribute start handle.
 * @param uuid Indicates 0x2800–UUID for «PrimaryService» OR 0x2801 for «Secondary Service».
 * @return Returns true if the handle belongs to a service handle.
 * @since 6.0
 */
bool GattServerProfile::impl::FindServiceByHandle(uint16_t attHandle, Uuid uuid)
{
    bool ret = false;
    if (db_.GetService(attHandle) != nullptr && uuid.operator==(Uuid::ConvertFrom16Bits(UUID_PRIMARY_SERVICE))) {
        ret = true;
    }
    return ret;
}
/**
 * @brief Confirm through the handle that the handle belongs to a characteristic declaration handle.
 *
 * @param attHandle Indicates attribute start handle.
 * @param uuid Indicates 0x2803–UUID for «Characteristic».
 * @return Returns true if the handle belongs to a characteristic declaration handle.
 * @since 6.0
 */
bool GattServerProfile::impl::FindCharacteristicDeclarationByHandle(uint16_t attHandle, Uuid uuid)
{
    bool ret = false;
    if (db_.GetCharacteristic(attHandle + MIN_ATTRIBUTE_HANDLE) != nullptr &&
        uuid.operator==(Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC))) {
        ret = true;
    }
    return ret;
}
/**
 * @brief Confirm through the handle that the handle belongs to a characteristic value handle.
 *
 * @param attHandle Indicates attribute start handle.
 * @param uuid Indicates 16-bit Bluetooth UUID or 128-bit UUID for characteristic value.
 * @return Returns true if the uuid belongs to a characteristic value.
 * @since 6.0
 */
bool GattServerProfile::impl::FindCharacteristicValueByUuid(uint16_t attHandle, Uuid uuid)
{
    bool ret = false;
    if (db_.GetDescriptor(attHandle) == nullptr && db_.GetCharacteristic(attHandle) != nullptr &&
        db_.GetCharacteristic(attHandle)->uuid_.operator==(uuid)) {
        ret = true;
    }
    return ret;
}
/**
 * @brief Confirm through the handle that the handle belongs to a service handle.
 *
 * @param attHandle Indicates attribute start handle.
 * @param uuid Indicates 16-bit Bluetooth UUID or 128-bit UUID for characteristic descriptor
 * @return Returns true if the uuid belongs to a characteristic descriptor.
 * @since 6.0
 */
bool GattServerProfile::impl::FindCharacteristicDescriptorByUuid(uint16_t attHandle, Uuid uuid)
{
    bool ret = false;
    if (db_.GetDescriptor(attHandle) != nullptr && db_.GetDescriptor(attHandle)->uuid_.operator==(uuid)) {
        ret = true;
    }
    return ret;
}
/**
 * @brief Assemble service data package.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns attribute value buffer.
 * @since 6.0
 */
Buffer *GattServerProfile::impl::AssembleServicePackage(uint16_t attHandle)
{
    uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};

    Buffer *value = BufferMalloc(db_.GetService(attHandle)->uuid_.GetUuidType());
    if (db_.GetService(attHandle)->uuid_.GetUuidType() == UUID_16BIT_LEN) {
        uint16_t uuid16Bit = db_.GetService(attHandle)->uuid_.ConvertTo16Bits();
        if (memcpy_s(BufferPtr(value), BufferGetSize(value), &uuid16Bit, UUID_16BIT_LEN) != EOK) {
            LOG_ERROR("%{public}s: memcpy_s uuid16Bit fail", __FUNCTION__);
            BufferFree(value);
            value = nullptr;
        }
    } else if (db_.GetService(attHandle)->uuid_.GetUuidType() == UUID_128BIT_LEN) {
        db_.GetService(attHandle)->uuid_.ConvertToBytesLE(uuid128Bit, UUID_128BIT_LEN);
        if (memcpy_s(BufferPtr(value), BufferGetSize(value), uuid128Bit, UUID_128BIT_LEN) != EOK) {
            LOG_ERROR("%{public}s: memcpy_s uuid128Bit fail", __FUNCTION__);
            BufferFree(value);
            value = nullptr;
        }
    } else {
        BufferFree(value);
        value = nullptr;
    }
    return value;
}
/**
 * @brief Assemble service data package.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns attribute value buffer.
 * @since 6.0
 */
Buffer *GattServerProfile::impl::AssembleCharacteristicPackage(uint16_t attHandle)
{
    Buffer *value = nullptr;
    uint8_t offset = 0;

    if (db_.GetCharacteristic(attHandle) != nullptr) {
        uint8_t properties = db_.GetCharacteristic(attHandle)->properties_;
        uint16_t valueHandle = db_.GetCharacteristic(attHandle)->valueHandle_;
        value =
            BufferMalloc(sizeof(uint8_t) + sizeof(uint16_t) + db_.GetCharacteristic(attHandle)->uuid_.GetUuidType());
        AssembleDataPackage((uint8_t *)BufferPtr(value), &offset, properties);
        AssembleDataPackage(
            (uint8_t *)BufferPtr(value), BufferGetSize(value), &offset, (uint8_t *)&valueHandle, sizeof(uint16_t));
        if (db_.GetCharacteristic(attHandle)->uuid_.GetUuidType() == UUID_16BIT_LEN) {
            uint16_t uuid16Bit = db_.GetCharacteristic(attHandle)->uuid_.ConvertTo16Bits();
            AssembleDataPackage(
                (uint8_t *)BufferPtr(value), BufferGetSize(value), &offset, (uint8_t *)&uuid16Bit, UUID_16BIT_LEN);
        } else if (db_.GetCharacteristic(attHandle)->uuid_.GetUuidType() == UUID_128BIT_LEN) {
            uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};
            db_.GetCharacteristic(attHandle)->uuid_.ConvertToBytesLE(uuid128Bit, UUID_128BIT_LEN);
            AssembleDataPackage(
                (uint8_t *)BufferPtr(value), BufferGetSize(value), &offset, uuid128Bit, UUID_128BIT_LEN);
        } else {
            BufferFree(value);
            value = nullptr;
        }
    }

    return value;
}
/**
 * @brief Assemble client characteristic configuration descriptor data package.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns attribute value buffer.
 * @since 6.0
 */
Buffer *GattServerProfile::impl::AssembleDescriptorPackage(uint16_t connectHandle, uint16_t attHandle)
{
    Uuid uuid = Uuid::ConvertFrom16Bits(UUID_CLIENT_CHARACTERISTIC_CONFIGURATION);
    if (db_.GetDescriptor(attHandle) != nullptr &&
        db_.GetDescriptor(attHandle)->uuid_.operator==(uuid)) {
        uint8_t data[2] = {0};
        uint16_t val = GetCccdValue(connectHandle, attHandle);
        (void)memcpy_s(data, sizeof(uint16_t), &val, sizeof(uint16_t));
        return GattServiceBase::BuildBuffer(data, sizeof(uint16_t));
    }
    return nullptr;
}
/**
 * @brief Assemble AttReadByGroupTypeRsp Package.
 *
 * @param list Indicates AttReadGroupAttributeData list.
 * @param service Indicates Services found.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttReadByGroupTypeRspPackage(
    AttReadGoupAttributeData *list, const GattDatabase::Service &service, uint8_t num)
{
    uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};

    list[num].attHandle = service.handle_;
    list[num].groupEndHandle = service.endHandle_;

    if (service.uuid_.GetUuidType() == UUID_16BIT_LEN) {
        list[num].attributeValue = (uint8_t *)malloc(UUID_16BIT_LEN);
        uint16_t uuid16Bit = service.uuid_.ConvertTo16Bits();
        (void)memcpy_s(list[num].attributeValue, UUID_16BIT_LEN, &uuid16Bit, UUID_16BIT_LEN);
    } else {
        list[num].attributeValue = (uint8_t *)malloc(UUID_128BIT_LEN);
        service.uuid_.ConvertToBytesLE(uuid128Bit, UUID_128BIT_LEN);
        (void)memcpy_s(list[num].attributeValue, UUID_128BIT_LEN, &uuid128Bit, UUID_128BIT_LEN);
    }
}
/**
 * @brief Assemble AttReadByTypeRspSvc Package.
 *
 * @param list Indicates AttReadByTypeRspDataList list.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttReadByTypeRspSvcPackage(
    AttReadByTypeRspDataList *list, uint16_t attHandle, uint8_t num, uint8_t *offset)
{
    list[num].attHandle.attHandle = db_.GetService(attHandle)->handle_;
    if (db_.GetService(attHandle)->uuid_.GetUuidType() == UUID_16BIT_LEN) {
        uint16_t uuid16Bit = db_.GetService(attHandle)->uuid_.ConvertTo16Bits();
        list[num].attributeValue = (uint8_t *)malloc(UUID_16BIT_LEN);
        AssembleDataPackage(list[num].attributeValue, UUID_16BIT_LEN, offset, (uint8_t *)&uuid16Bit, UUID_16BIT_LEN);
    } else {
        uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};
        db_.GetService(attHandle)->uuid_.ConvertToBytesLE(uuid128Bit, UUID_128BIT_LEN);
        list[num].attributeValue = (uint8_t *)malloc(UUID_128BIT_LEN);
        AssembleDataPackage(list[num].attributeValue, UUID_128BIT_LEN, offset, uuid128Bit, UUID_128BIT_LEN);
    }
}

/**
 * @brief Assemble AttReadByTypeRspCharacteris Package.
 *
 * @param list Indicates AttReadByTypeRspDataList list.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttReadByTypeRspCharacteristicPackage(
    AttReadByTypeRspDataList *list, uint16_t attHandle, uint8_t num, uint8_t *offset)
{
    uint8_t uuid128Bit[UUID_128BIT_LEN] = {0};
    uint8_t len = sizeof(uint8_t) + sizeof(uint16_t) + UUID_128BIT_LEN;

    if (db_.GetCharacteristic(attHandle) == nullptr) {
        return;
    }
    list[num].attHandle.attHandle = db_.GetCharacteristic(attHandle)->handle_;
    list[num].attributeValue = (uint8_t *)malloc(len);
    AssembleDataPackage(list[num].attributeValue, offset, db_.GetCharacteristic(attHandle)->properties_);
    AssembleDataPackage(list[num].attributeValue,
        len,
        offset,
        (uint8_t *)&db_.GetCharacteristic(attHandle)->valueHandle_,
        sizeof(uint16_t));
    if (db_.GetCharacteristic(attHandle)->uuid_.GetUuidType() == UUID_16BIT_LEN) {
        uint16_t uuid16Bit = db_.GetCharacteristic(attHandle)->uuid_.ConvertTo16Bits();
        AssembleDataPackage(list[num].attributeValue, len, offset, (uint8_t *)&uuid16Bit, UUID_16BIT_LEN);
    } else {
        db_.GetCharacteristic(attHandle)->uuid_.ConvertToBytesLE(uuid128Bit, UUID_128BIT_LEN);
        AssembleDataPackage(list[num].attributeValue, len, offset, uuid128Bit, UUID_128BIT_LEN);
    }
}
/**
 * @brief Assemble AttReadByTypeRspDesc Package.
 *
 * @param list Indicates AttReadByTypeRspDataList list.
 * @param connectHandle Indicates identify a connection.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @return Returns true if descriptor is not CCCD.
 * @since 6.0
 */
bool GattServerProfile::impl::AssembleAttReadByTypeRspDescPackage(
    AttReadByTypeRspDataList *list, uint16_t connectHandle, uint16_t attHandle, uint8_t num, uint8_t *offset)
{
    bool ret = true;
    Uuid uuid = Uuid::ConvertFrom16Bits(UUID_CLIENT_CHARACTERISTIC_CONFIGURATION);
    if (db_.GetDescriptor(attHandle) != nullptr) {
        list[num].attHandle.attHandle = db_.GetDescriptor(attHandle)->handle_;
        if (db_.GetDescriptor(attHandle)->uuid_.operator==(uuid)) {
            uint16_t val = GetCccdValue(connectHandle, attHandle);
            list[num].attributeValue = (uint8_t *)malloc(sizeof(val));
            AssembleDataPackage(list[num].attributeValue, sizeof(val), offset, (uint8_t *)&val, sizeof(val));
            ret = false;
        }
    }

    return ret;
}
/**
 * @brief Assemble AttFindInforRspSvc Package.
 *
 * @param pairs Indicates AttHandleUuid pairs.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttFindInforRspSvcPackage(
    AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len)
{
    pairs[num].attHandle = db_.GetService(attHandle)->handle_;
    if (db_.GetService(attHandle)->isPrimary_) {
        pairs[num].uuid.uuid16 = UUID_PRIMARY_SERVICE;
    } else {
        pairs[num].uuid.uuid16 = UUID_SECONDARY_SERVICE;
    }
    pairs[num].uuid.type = UUID_16BIT_FORMAT;
    *len = UUID_16BIT_LEN;
}
/**
 * @brief Assemble AttFindInforRspCharacteris Package.
 *
 * @param pairs Indicates AttHandleUuid pairs.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttFindInforRspCharacteristicPackage(
    AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len)
{
    if (db_.GetCharacteristic(attHandle + MIN_ATTRIBUTE_HANDLE) != nullptr) {
        pairs[num].attHandle = db_.GetCharacteristic(attHandle + MIN_ATTRIBUTE_HANDLE)->handle_;
        pairs[num].uuid.uuid16 = UUID_CHARACTERISTIC;
        pairs[num].uuid.type = UUID_16BIT_FORMAT;
        *len = UUID_16BIT_LEN;
    }
}
/**
 * @brief Assemble AttFindInforRspCharacterisVal Package.
 *
 * @param pairs Indicates AttHandleUuid pairs.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttFindInforRspCharacteristicValPackage(
    AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len)
{
    if (db_.GetCharacteristic(attHandle) == nullptr) {
        return;
    }
    pairs[num].attHandle = db_.GetCharacteristic(attHandle)->valueHandle_;
    if (db_.GetCharacteristic(attHandle)->uuid_.GetUuidType() == UUID_16BIT_LEN) {
        pairs[num].uuid.uuid16 = db_.GetCharacteristic(attHandle)->uuid_.ConvertTo16Bits();
        pairs[num].uuid.type = UUID_16BIT_FORMAT;
        *len = UUID_16BIT_LEN;
    } else {
        db_.GetCharacteristic(attHandle)->uuid_.ConvertToBytesLE(pairs[num].uuid.uuid128, UUID_128BIT_LEN);
        pairs[num].uuid.type = UUID_128BIT_FORMAT;
        *len = UUID_128BIT_LEN;
    }
}
/**
 * @brief Assemble AttFindInforRspDesc Package.
 *
 * @param pairs Indicates AttHandleUuid pairs.
 * @param attHandle Indicates attribute handle.
 * @param num Indicates list offset.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleAttFindInforRspDescPackage(
    AttHandleUuid *pairs, uint16_t attHandle, uint8_t num, uint16_t *len)
{
    if (db_.GetDescriptor(attHandle) != nullptr) {
        pairs[num].attHandle = db_.GetDescriptor(attHandle)->handle_;
        if (db_.GetDescriptor(attHandle)->uuid_.GetUuidType() == UUID_16BIT_LEN) {
            pairs[num].uuid.uuid16 = db_.GetDescriptor(attHandle)->uuid_.ConvertTo16Bits();
            pairs[num].uuid.type = UUID_16BIT_FORMAT;
            *len = UUID_16BIT_LEN;
        } else {
            db_.GetDescriptor(attHandle)->uuid_.ConvertToBytesLE(pairs[num].uuid.uuid128, UUID_128BIT_LEN);
            pairs[num].uuid.type = UUID_128BIT_FORMAT;
            *len = UUID_128BIT_LEN;
        }
    }
}
/**
 * @brief Assemble att data package.
 *
 * @param dest Indicates destination address.
 * @param destMax -The maximum length of destination buffer.
 * @param offset Indicates address offset.
 * @param src Indicates source address.
 * @param size Indicates data size.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleDataPackage(
    uint8_t *dest, uint8_t destMax, uint8_t *offset, uint8_t *src, uint8_t size)
{
    if (memcpy_s((dest + *offset), destMax, src, size) != EOK) {
        return;
    }
    *offset = *offset + size;
}
/**
 * @brief Assemble att data package.
 *
 * @param dest Indicates destination address.
 * @param offset Indicates address offset.
 * @param src Indicates source address.
 * @since 6.0
 */
void GattServerProfile::impl::AssembleDataPackage(uint8_t *dest, uint8_t *offset, uint8_t src)
{
    *(dest + *offset) = src;
    *offset = *offset + 1;
}

void GattServerProfile::impl::FreeDataPackage(AttReadByTypeRspDataList *list, uint8_t num)
{
    for (int i = 0; i < num; i++) {
        free(list[i].attributeValue);
    }
}
/**
 * @brief Get properties of Characteristic and value.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns true if characteristic is readable
 * @since 6.0
 */
bool GattServerProfile::impl::CharacteristicPropertyIsReadable(uint16_t attHandle)
{
    if ((db_.GetCharacteristic(attHandle) != nullptr) &&
        (db_.GetCharacteristic(attHandle)->properties_ & CHARACTERISTIC_PROPERTIE_READ) &&
        (db_.GetValueByHandle(attHandle).value().get().permissions_ & static_cast<int>(GattPermission::READABLE))) {
        return true;
    } else {
        return false;
    }
}
/**
 * @brief Get properties of descriptor and value.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns true if descriptor is readable
 * @since 6.0
 */
bool GattServerProfile::impl::DescriptorPropertyIsReadable(uint16_t attHandle)
{
    if ((db_.GetDescriptor(attHandle) != nullptr) &&
        (db_.GetDescriptor(attHandle)->permissions_ & static_cast<int>(GattPermission::READABLE))) {
        return true;
    } else {
        return false;
    }
}
/**
 * @brief Get properties of Characteristic and value.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns true if characteristic is writable
 * @since 6.0
 */
bool GattServerProfile::impl::CharacteristicPropertyIsWritable(uint16_t attHandle)
{
    if ((db_.GetCharacteristic(attHandle) != nullptr) &&
        (db_.GetCharacteristic(attHandle)->properties_ & CHARACTERISTIC_PROPERTIE_WRITE) &&
        (db_.GetValueByHandle(attHandle).value().get().permissions_ & static_cast<int>(GattPermission::WRITABLE))) {
        return true;
    } else {
        return false;
    }
}
/**
 * @brief Get properties of descriptor and value.
 *
 * @param attHandle Indicates attribute handle.
 * @return Returns true if descriptor is writable
 * @since 6.0
 */
bool GattServerProfile::impl::DescriptorPropertyIsWritable(uint16_t attHandle)
{
    if ((db_.GetDescriptor(attHandle) != nullptr) &&
        (db_.GetDescriptor(attHandle)->permissions_ & static_cast<int>(GattPermission::WRITABLE))) {
        return true;
    } else {
        return false;
    }
}
/**
 * @brief This sub-procedure is used to respond that write descriptor values.
 *
 * @param connectHandle Indicates identify a connection.
 * @param attHandle Indicates descriptor handle.
 * @param value Indicates value of the schedule settings.
 * @since 6.0
 */
AttError GattServerProfile::impl::WriteDescriptorProcess(uint16_t connectHandle, uint16_t attHandle, Buffer *value)
{
    uint16_t cccVal = 0;
    AttError errorData = {WRITE_REQUEST, attHandle, 0};
    Uuid uuid = Uuid::ConvertFrom16Bits(UUID_CLIENT_CHARACTERISTIC_CONFIGURATION);

    if (db_.GetDescriptor(attHandle) != nullptr &&
        db_.GetDescriptor(attHandle)->uuid_.operator==(uuid)) {
        if (BufferGetSize(value) > sizeof(uint16_t)) {
            errorData.errorCode = ATT_ATTRIBUTE_NOT_LONG;
        } else {
            if (memcpy_s(&cccVal, sizeof(uint16_t), BufferPtr(value), BufferGetSize(value)) != EOK) {
                LOG_ERROR("%{public}s: memcpy_s fail", __FUNCTION__);
                errorData.errorCode = ATT_UNLIKELY_ERROR;
                return errorData;
            }
            AddCccdValue(connectHandle, attHandle, cccVal);
            ATT_WriteResponse(connectHandle);
        }
    } else {
        auto descPtr = GattServiceBase::BuildGattValue((uint8_t *)BufferPtr(value), BufferGetSize(value));
        pServerCallBack_->OnDescriptorWriteEvent(connectHandle, attHandle, descPtr, BufferGetSize(value));
    }

    return errorData;
}
/**
 * @brief Get att mtu size.
 *
 * @param connectHandle Indicates identify a connection.
 * @return Returns mtu size.
 * @since 6.0
 */
uint16_t GattServerProfile::impl::GetMtuInformation(uint16_t connectHandle)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    uint16_t mtu = GATT_DEFAULT_MTU;
    auto it = mtuInfo_.find(connectHandle);
    if (it != mtuInfo_.end()) {
        mtu = it->second;
    }

    return mtu;
}
/**
 * @brief Save att mtu size.
 *
 * @param connectHandle Indicates identify a connection.
 * @param mtu Indicates mtu size.
 * @since 6.0
 */
void GattServerProfile::impl::SetMtuInformation(uint16_t connectHandle, uint16_t mtu)
{
    LOG_INFO("%{public}s: connectHandle is %hu, mtu is %hu.", __FUNCTION__, connectHandle, mtu);
    auto it = mtuInfo_.find(connectHandle);
    if (it == mtuInfo_.end()) {
        mtuInfo_.emplace(connectHandle, mtu);
    } else {
        it->second = mtu;
    }
}
/**
 * @brief Delete list of request and response when gatt is disconnected.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattServerProfile::impl::DeleteList(uint16_t connectHandle)
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
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
 * @brief Add new device to list.
 *
 * @param connectHandle Indicates identify a connection.
 * @param device Indicates new device.
 * @since 6.0
 */
void GattServerProfile::impl::AddDeviceList(uint16_t connectHandle, GattDevice device)
{
    LOG_INFO("%{public}s: entry", __FUNCTION__);
    for (auto iter = devList_.begin(); iter != devList_.end(); iter++) {
        if (iter->second.device_.addr_.operator==(device.addr_) &&
            iter->second.device_.transport_ == device.transport_) {
            iter->first = connectHandle;
            LOG_INFO("%{public}s: Device already exists", __FUNCTION__);
            return;
        }
    }
    devList_.emplace_back(connectHandle, DeviceInfo(device));
}
/**
 * @brief Set cccd value to list.
 *
 * @param connectHandle Indicates identify a connection.
 * @param attHandle Indicates cccd handle.
 * @param value Indicates cccd value.
 * @since 6.0
 */
void GattServerProfile::impl::AddCccdValue(uint16_t connectHandle, uint16_t attHandle, uint16_t value)
{
    LOG_INFO("%{public}s: connectHandle is %hu, attHandle is %hu, value is %hu.",
        __FUNCTION__, connectHandle, attHandle, value);
    auto iter = devList_.begin();
    for (; iter != devList_.end(); iter++) {
        if (connectHandle == iter->first) {
            break;
        }
    }
    if (iter == devList_.end()) {
        LOG_INFO("%{public}s: Device does not exist", __FUNCTION__);
        return;
    }
    for (uint8_t num = 0; num < GATT_CCCD_NUM_MAX; num++) {
        if (iter->second.cccd_[num].valHandle_ == attHandle) {
            iter->second.cccd_[num].value_ = value;
            LOG_INFO("cccd valid handle: %hu, value: %hu", attHandle, value);
            return;
        } else if (iter->second.cccd_[num].valHandle_ == INVALID_ATTRIBUTE_HANDLE) {
            iter->second.cccd_[num].valHandle_ = attHandle;
            iter->second.cccd_[num].value_ = value;
            LOG_INFO("cccd invalid handle: %hu, value: %hu", attHandle, value);
            return;
        } else {
            LOG_ERROR("%{public}s: CCCD does not exist", __FUNCTION__);
        }
    }
}
/**
 * @brief Get cccd value from list.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattServerProfile::impl::DeleteCccdValue(uint16_t connectHandle)
{
    for (auto iter = devList_.begin(); iter != devList_.end(); iter++) {
        if (connectHandle == iter->first) {
            for (uint8_t num = 0; num < GATT_CCCD_NUM_MAX; num++) {
                iter->second.cccd_[num].valHandle_ = INVALID_ATTRIBUTE_HANDLE;
                iter->second.cccd_[num].value_ = 0x00;
            }
        }
    }
}
/**
 * @brief Get cccd value from list.
 *
 * @param connectHandle Indicates identify a connection.
 * @param attHandle Indicates cccd handle.
 * @since 6.0
 */
uint16_t GattServerProfile::impl::GetCccdValue(uint16_t connectHandle, uint16_t attHandle)
{
    uint16_t ret = 0;

    for (auto iter = devList_.begin(); iter != devList_.end(); iter++) {
        if (connectHandle == iter->first) {
            for (uint8_t num = 0; num < GATT_CCCD_NUM_MAX; num++) {
                if (iter->second.cccd_[num].valHandle_ == attHandle) {
                    ret = iter->second.cccd_[num].value_;
                }
            }
        }
    }
    LOG_INFO("%{public}s: connectHandle is %hu, attHandle is %hu, CCCvalue is %hu.",
        __FUNCTION__, connectHandle, attHandle, ret);
    return ret;
}
/**
 * @brief Indicates connect or disconnect.
 *
 * @since 6.0
 */
class GattServerProfile::impl::GattConnectionObserverImplement : public GattConnectionObserver {
public:
    void OnConnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        LOG_INFO("%{public}s: gatt_server connect ret is %{public}d", __FUNCTION__, ret);
        if (ret == GATT_SUCCESS) {
            this->serverProfile_.pimpl->dispatcher_->PostTask(
                std::bind(&impl::AddDeviceList, serverProfile_.pimpl.get(), connectionHandle, device));
        }
    }

    void OnDisconnect(const GattDevice &device, uint16_t connectionHandle, int ret) override
    {
        LOG_INFO("%{public}s: gatt_server connect ret is %{public}d", __FUNCTION__, ret);
        if (device.isEncryption_ == false) {
            this->serverProfile_.pimpl->dispatcher_->PostTask(
                std::bind(&impl::DeleteCccdValue, serverProfile_.pimpl.get(), connectionHandle));
        }
        this->serverProfile_.pimpl->dispatcher_->PostTask(
            std::bind(&impl::DeleteList, serverProfile_.pimpl.get(), connectionHandle));
        this->serverProfile_.pimpl->dispatcher_->PostTask(
            std::bind(&impl::SetMtuInformation, serverProfile_.pimpl.get(), connectionHandle, GATT_DEFAULT_MTU));
    }

    GattConnectionObserverImplement(GattServerProfile &serverProfile) : serverProfile_(serverProfile)
    {}
    ~GattConnectionObserverImplement()
    {}

private:
    GattServerProfile &serverProfile_;
};
/**
 * @brief Register callback to ConnectManager.
 *
 * @since 6.0
 */
void GattServerProfile::impl::RegisterCallbackToConnectManager()
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
void GattServerProfile::impl::DeregisterCallbackToConnectManager()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    GattConnectionManager::GetInstance().DeregisterObserver(connectionObserverId_);
}

std::list<std::pair<uint16_t, GattResponesInfor>>::iterator GattServerProfile::impl::FindIteratorByResponesInfor(
    uint16_t handle, ResponesType respType)
{
    std::list<std::pair<uint16_t, GattResponesInfor>>::iterator iter;
    for (iter = responseList_.begin(); iter != responseList_.end(); iter++) {
        if (handle == iter->first && respType == iter->second.respType_) {
            break;
        }
    }
    return iter;
}

/**
 * @brief This sub-procedure is used to add services.
 *
 * @param service Indicates service list.
 * @since 6.0
 */
int GattServerProfile::AddService(Service &service) const
{
    int result = pimpl->db_.AddService(service);
    return result;
}
/**
 * @brief Check service from data base.
 *
 * @param service Indicates service list
 * @since 6.0
 */
int GattServerProfile::CheckLegalityOfServiceDefinition(Service &service) const
{
    return pimpl->db_.CheckLegalityOfServiceDefinition(service);
}
/**
 * @brief This sub-procedure is used to remove services.
 *
 * @param serviceHandle Indicates service handle.
 * @since 6.0
 */
int GattServerProfile::RemoveService(uint16_t serviceHandle) const
{
    int result = pimpl->db_.DeleteService(serviceHandle);
    return result;
}

const std::string GattServerProfile::GetDBHash() const
{
    return std::string();
}
/**
 * @brief This sub-procedure is used to get service from data base.
 *
 * @param serviceHandle Indicates service handle.
 * @since 6.0
 */
const GattDatabase::Service *GattServerProfile::GetService(uint16_t serviceHandle) const
{
    return pimpl->db_.GetService(serviceHandle);
}
/**
 * @brief This sub-procedure is used to get service from data base.
 *
 * @param serviceHandle Indicates service handle.
 * @since 6.0
 */
const std::map<uint16_t, GattDatabase::Service> &GattServerProfile::GetServices() const
{
    return pimpl->db_.GetServices();
}
/**
 * @brief This sub-procedure is used to get service characteristic from data base.
 *
 * @param valueHandle Indicates value handle.
 * @since 6.0
 */
GattDatabase::Characteristic *GattServerProfile::GetCharacteristic(uint16_t valueHandle) const
{
    return pimpl->db_.GetCharacteristic(valueHandle);
}
/**
 * @brief This sub-procedure is used to get service descriptor from data base.
 *
 * @param valueHandle Indicates value handle.
 * @since 6.0
 */
const GattDatabase::Descriptor *GattServerProfile::GetDescriptor(uint16_t valueHandle) const
{
    return pimpl->db_.GetDescriptor(valueHandle);
}
/**
 * @brief This sub-procedure is used to set value .
 *
 * @since 6.0
 */
void GattServerProfile::SetAttributeValue(uint16_t valueHandle, GattDatabase::AttributeValue &value) const
{
    pimpl->db_.SetValueByHandle(valueHandle, value);
}
/**
 * @brief This sub-procedure is used to respond that write values.
 *
 * @param connectHandle Indicates identify a connection.
 * @param data Indicates att data.
 * @param value Indicates value of the schedule settings.
 * @since 6.0
 */
Buffer *GattServerProfile::GetAttributeValue(uint16_t handle) const
{
    auto entity = pimpl->db_.GetValueByHandle(handle);
    if (entity.has_value()) {
        auto &value = entity.value().get();
        return GattServiceBase::BuildBuffer(value.value_.value_.get(), value.value_.length_);
    }

    return nullptr;
}
/**
 * @brief This sub-procedure is used to get attribute.
 *
 * @since 6.0
 */
const std::optional<std::reference_wrapper<GattDatabase::AttributeEntity>> GattServerProfile::GetAttributeEntity(
    uint16_t handle) const
{
    return pimpl->db_.GetValueByHandle(handle);
}
/**
 * @brief This sub-procedure is used to send notification.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @since 6.0
 */
void GattServerProfile::SendNotification(
    uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, handle is is %hu.", __FUNCTION__, connectHandle, handle);
    if (pimpl->GetCccdValue(connectHandle, handle + MIN_ATTRIBUTE_HANDLE) == GATT_NOTIFICATION_VALUE) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            ATT_HandleValueNotification(connectHandle, handle, buffer);
            BufferFree(buffer);
        }
    } else {
        LOG_ERROR("%{public}s: GATT_NOTIFICATION_VALUE is invalid", __FUNCTION__);
    }
}
/**
 * @brief This sub-procedure is used to send indication.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @since 6.0
 */
void GattServerProfile::SendIndication(
    uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const
{
    LOG_INFO("%{public}s: connectHandle is %hu.", __FUNCTION__, connectHandle);
    if (pimpl->GetCccdValue(connectHandle, handle + MIN_ATTRIBUTE_HANDLE) == GATT_INDICATION_VALUE) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            pimpl->requestList_.emplace_back(connectHandle, GattResponesInfor(SEND_INDICATION, handle));
            ATT_HandleValueIndication(connectHandle, handle, buffer);
            BufferFree(buffer);
        }
    } else {
        LOG_ERROR("%{public}s: GATT_INDICATION_VALUE is invalid", __FUNCTION__);
        pimpl->pServerCallBack_->OnIndicationEvent(connectHandle, handle, GATT_FAILURE);
    }
}
/**
 * @brief This sub-procedure is used to send read characteristic value response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendReadCharacteristicValueResp(
    uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const
{
    return SendReadDescriptorResp(connectHandle, handle, value, len, result);
}
/**
 * @brief This sub-procedure is used to send read using characteristic value response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendReadUsingCharacteristicValueResp(
    uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, result is %{public}d.", __FUNCTION__, connectHandle, result);
    AttReadByTypeRspDataList valueList[GATT_VALUE_LEN_MAX] = {{{0}, nullptr}};
    AttError errorData = {READ_REQUEST, handle, 0};

    if (result == GATT_SUCCESS) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            valueList->attHandle.attHandle = handle;
            valueList->attributeValue = (uint8_t *)BufferPtr(buffer);
            ATT_ReadByTypeResponse(connectHandle, len + sizeof(handle), valueList, sizeof(uint8_t));
            BufferFree(buffer);
        }
    } else {
        errorData.errorCode = ConvertResponseErrorCode(result);
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to send read blob value response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendReadBlobValueResp(
    uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, result is %{public}d.", __FUNCTION__, connectHandle, result);
    AttError errorData = {READ_BLOB_REQUEST, handle, 0};

    if (result == GATT_SUCCESS) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            ATT_ReadBlobResponse(connectHandle, buffer);
            BufferFree(buffer);
        }
    } else {
        errorData.errorCode = ConvertResponseErrorCode(result);
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to send write characteristic value response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendWriteCharacteristicValueResp(
    uint16_t connectHandle, uint16_t handle, int result) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, result is %{public}d.", __FUNCTION__, connectHandle, result);
    AttError errorData = {WRITE_REQUEST, handle, 0};
    if (result == GATT_SUCCESS) {
        ATT_WriteResponse(connectHandle);
    } else {
        errorData.errorCode = ConvertResponseErrorCode(result);
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to send read descriptor response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendReadDescriptorResp(
    uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, result is %{public}d.", __FUNCTION__, connectHandle, result);
    Buffer *buffer = nullptr;
    AttError errorData = {READ_REQUEST, handle, 0};
    uint16_t bufSize = pimpl->GetMtuInformation(connectHandle) - sizeof(uint8_t);

    if (result == GATT_SUCCESS) {
        if (len > bufSize) {
            pimpl->requestList_.emplace_back(
                connectHandle, GattResponesInfor(READ_LONG_CHARACTERISTIC_VALUE, len, value));
            len = bufSize;
        }
        buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            ATT_ReadResponse(connectHandle, buffer);
            BufferFree(buffer);
        }
    } else {
        errorData.errorCode = ConvertResponseErrorCode(result);
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to send write descriptor response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendWriteDescriptorResp(uint16_t connectHandle, uint16_t handle, int result) const
{
    AttError errorData = {WRITE_REQUEST, handle, 0};
    if (result == GATT_SUCCESS) {
        ATT_WriteResponse(connectHandle);
    } else {
        errorData.errorCode = ConvertResponseErrorCode(result);
        ATT_ErrorResponse(connectHandle, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to send prepare write value response.
 *
 * @param connectHandle Indicates identify a connection.
 * @param handle Indicates value handle.
 * @param value Indicates value of the schedule settings.
 * @param len Indicates size of value.
 * @param result Indicates send status.
 * @since 6.0
 */
void GattServerProfile::SendPrepareWriteValueResp(
    PrepareWriteParam param, const GattValue &value, size_t len, int result) const
{
    LOG_INFO("%{public}s: connectHandle is %hu, offset is %hu, result is %{public}d.",
        __FUNCTION__,
        param.connectHandle_,
        param.offset_,
        result);
    AttError errorData = {READ_BLOB_REQUEST, param.handle_, 0};
    AttReadBlobReqPrepareWriteValue attReadBlobObj = {param.handle_, param.offset_};

    if (result == GATT_SUCCESS) {
        Buffer *buffer = GattServiceBase::BuildBuffer(value->get(), len);
        if (buffer != nullptr) {
            ATT_PrepareWriteResponse(param.connectHandle_, attReadBlobObj, buffer);
            BufferFree(buffer);
        }
    } else {
        errorData.errorCode = ConvertResponseErrorCode(result);
        ATT_ErrorResponse(param.connectHandle_, &errorData);
    }
}
/**
 * @brief This sub-procedure is used to send execute write response.
 *
 * @param connectHandle Indicates identify a connection.
 * @since 6.0
 */
void GattServerProfile::SendExecuteWriteValueResp(uint16_t connectHandle)
{
    LOG_INFO("%{public}s: connectHandle is %hu", __FUNCTION__, connectHandle);
    ATT_ExecuteWriteResponse(connectHandle);
}
/**
 * @brief Convert the att error code to the service layer error code.
 *
 * @param errorCode Indicates gatt service's error code.
 * @return Returns att error code.
 * @since 6.0
 */
int GattServerProfile::ConvertResponseErrorCode(int errorCode)
{
    int ret;

    switch ((GattStatus)errorCode) {
        case INVALID_HANDLE:
            ret = ATT_INVALID_HANDLE;
            break;
        case INVALID_OFFSET:
            ret = ATT_INVALID_OFFSET;
            break;
        case HANDLE_NOT_FOUND:
            ret = ATT_ATTRIBUTE_NOT_FOUND;
            break;
        case READ_NOT_PERMITTED:
            ret = ATT_READ_NOT_PERMITTED;
            break;
        case WRITE_NOT_PERMITTED:
            ret = ATT_WRITE_NOT_PERMITTED;
            break;
        case INSUFFICIENT_ENCRYPTION:
            ret = ATT_INSUFFICIENT_ENCRYPTION;
            break;
        case INSUFFICIENT_AUTHENTICATION:
            ret = ATT_INSUFFICIENT_AUTHENTICATION;
            break;
        case INSUFFICIENT_AUTHORIZATION:
            ret = ATT_INSUFFICIENT_AUTHORIZATION;
            break;
        case INSUFFICIENT_ENCRYPTION_KEY_SIZE:
            ret = ATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE;
            break;
        case PREPARE_QUEUE_FULL:
            ret = ATT_PREPARE_QUEUE_FULL;
            break;
        case ATTRIBUTE_NOT_LONG:
            ret = ATT_ATTRIBUTE_NOT_LONG;
            break;
        case INVALID_ATTRIBUTE_VALUE_LENGTH:
            ret = ATT_INVALID_ATTRIBUTE_VALUE_LENGTH;
            break;
        case WRITE_REQUEST_REJECTED:
            ret = ATT_WRITE_REQUEST_REJECTED;
            break;
        default:
            ret = ATT_REQUEST_NOT_SUPPORTED;
            LOG_ERROR("%{public}s: Error code of error response is invalid. ErrorCode = %{public}d", __FUNCTION__, errorCode);
            break;
    }

    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS
