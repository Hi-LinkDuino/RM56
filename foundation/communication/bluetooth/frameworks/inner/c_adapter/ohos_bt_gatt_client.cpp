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

#include "ohos_bt_gatt_client.h"
#include <cstring>
#include <map>
#include "__config"
#include "__functional_base"

#include "bluetooth_gatt_characteristic.h"
#include "bluetooth_gatt_client.h"
#include "bluetooth_gatt_descriptor.h"
#include "bluetooth_gatt_service.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "bluetooth_remote_device.h"

#include "iosfwd"
#include "memory"
#include "new"
#include "ohos_bt_adapter_utils.h"
#include "ohos_bt_def.h"
#include "optional"
#include "string"
#include "type_traits"
#include "utility"
#include "uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

namespace OHOS {
namespace Bluetooth {
class GattClientCallbackWrapper;

struct GattClientWrapper {
    GattClient *gattClient;
    GattClientCallbackWrapper *gattClientCallback;
    string remoteAddr;
};

using ClientIterator = std::map<int, struct GattClientWrapper>::iterator;

static int g_clientIncrease = 0;
static std::map<int, struct GattClientWrapper> g_MapGattClient;

#define GATTCLIENT g_MapGattClient

int ConverWriteType(BtGattWriteType writeType)
{
    int outWriteType = GattCharacteristic::WriteType::DEFAULT;
    if (writeType == OHOS_GATT_WRITE_DEFAULT) {
        outWriteType = GattCharacteristic::WriteType::DEFAULT;
    } else if (writeType == OHOS_GATT_WRITE_NO_RSP) {
        outWriteType = GattCharacteristic::WriteType::NO_RESPONSE;
    } else if (writeType == OHOS_GATT_WRITE_SIGNED) {
        outWriteType = GattCharacteristic::WriteType::SIGNED;
    } else {
        HILOGE("write type: %{public}d is not supported and the default type is used.", writeType);
        outWriteType = GattCharacteristic::WriteType::DEFAULT;
    }
    return outWriteType;
}

static GattCharacteristic *GattcFindCharacteristic(int clientId, GattClient **client,
    BtGattCharacteristic characteristic)
{
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("clientId: %{public}d, has not been registered.", clientId);
        return nullptr;
    }

    *client = iter->second.gattClient;
    if (*client == nullptr) {
        HILOGE("client is null.");
        return nullptr;
    }

    string strUuidSvc(characteristic.serviceUuid.uuid);
    string strUuidChar(characteristic.characteristicUuid.uuid);
    HILOGI("execute, strUuidSvc: %{public}s, strUuidChar: %{public}s",
        strUuidSvc.c_str(), strUuidChar.c_str());
    if (!regex_match(strUuidSvc, uuidRegex) || (!regex_match(strUuidChar, uuidRegex))) {
        HILOGE("match the UUID faild.");
        return nullptr;
    }
    std::optional<std::reference_wrapper<GattService>> service = (*client)->GetService(UUID::FromString(strUuidSvc));
    if (service == std::nullopt) {
        HILOGE("find service fail.");
        return nullptr;
    }
    GattCharacteristic *charac = service->get().GetCharacteristic(UUID::FromString(strUuidChar));
    if (charac == nullptr) {
        HILOGE("find characteristic fail.");
    }
    return charac;
}


class GattClientCallbackWrapper : public GattClientCallback {
public:
    GattClientCallbackWrapper(BtGattClientCallbacks *callback, int clientId)
    {
        appCallback_ = callback;
        clientId_ = clientId;
    }

    void OnConnectionStateChanged(int connectionState, int ret)
    {
        if (appCallback_ == nullptr || appCallback_->ConnectionStateCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }

        HILOGI("clientId: %{public}d, connectionState: %{public}d, ret: %{public}d",
            clientId_, connectionState, ret);
        appCallback_->ConnectionStateCb(clientId_, connectionState, GetGattcResult(ret));
    }

    void OnConnectionParameterChanged(int interval, int latency, int timeout, int status)
    {
        if (appCallback_ == nullptr || appCallback_->connectParaUpdateCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }

        HILOGI("clientId: %{public}d, status: %{public}d, interval: %{public}d, latency: %{public}d, "
            "timeout: %{public}d", clientId_, status, interval, latency, timeout);
        appCallback_->connectParaUpdateCb(clientId_, interval, latency, timeout, GetGattcResult(status));
    }

    void OnServicesDiscovered(int status)
    {
        if (appCallback_ == nullptr || appCallback_->searchServiceCompleteCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }

        HILOGI("clientId: %{public}d, status: %{public}d", clientId_, status);
        appCallback_->searchServiceCompleteCb(clientId_, GetGattcResult(status));
    }

    void OnCharacteristicReadResult(const GattCharacteristic &characteristic, int ret)
    {
        if (appCallback_ == nullptr || appCallback_->readCharacteristicCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }
        if (characteristic.GetService() == nullptr) {
            HILOGE("get service null.");
            return;
        }

        BtGattReadData characData;
        string srvUuid = characteristic.GetService()->GetUuid().ToString();
        string charcUuid = characteristic.GetUuid().ToString();
        GattcBuildUuid(&characData.attribute.characteristic.serviceUuid, srvUuid);
        GattcBuildUuid(&characData.attribute.characteristic.characteristicUuid, charcUuid);
        size_t tmpLen = 0;
        characData.data = characteristic.GetValue(&tmpLen).get();
        characData.dataLen = (unsigned short)tmpLen;

        HILOGI("clientId: %{public}d, ret: %{public}d, dataLen: %{public}d",
            clientId_, ret, characData.dataLen);
        HILOGI("srvUuid: %{public}s, charcUuid: %{public}s",
            srvUuid.c_str(), charcUuid.c_str());
        appCallback_->readCharacteristicCb(clientId_, &characData, GetGattcResult(ret));
    }

    void OnCharacteristicWriteResult(const GattCharacteristic &characteristic, int ret)
    {
        if (appCallback_ == nullptr || appCallback_->writeCharacteristicCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }
        if (characteristic.GetService() == nullptr) {
            HILOGE("get service null.");
            return;
        }

        BtGattCharacteristic tmpCharac;
        string srvUuid = characteristic.GetService()->GetUuid().ToString();
        string charcUuid = characteristic.GetUuid().ToString();
        GattcBuildUuid(&tmpCharac.serviceUuid, srvUuid);
        GattcBuildUuid(&tmpCharac.characteristicUuid, charcUuid);

        HILOGI("clientId: %{public}d, ret: %{public}d, ", clientId_, ret);
        HILOGI("srvUuid: %{public}s, charcUuid: %{public}s", srvUuid.c_str(), charcUuid.c_str());
        appCallback_->writeCharacteristicCb(clientId_, &tmpCharac, GetGattcResult(ret));
    }

    void OnDescriptorReadResult(const GattDescriptor &descriptor, int ret)
    {
        if (appCallback_ == nullptr || appCallback_->readDescriptorCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }

        if (descriptor.GetCharacteristic() == nullptr ||
            descriptor.GetCharacteristic()->GetService() == nullptr) {
            HILOGE("get characteristic or service null.");
            return;
        }

        BtGattReadData descData;
        string srvUuid = descriptor.GetCharacteristic()->GetService()->GetUuid().ToString();
        string charcUuid = descriptor.GetCharacteristic()->GetUuid().ToString();
        string descUuid = descriptor.GetUuid().ToString();
        GattcBuildUuid(&descData.attribute.descriptor.characteristic.serviceUuid, srvUuid);
        GattcBuildUuid(&descData.attribute.descriptor.characteristic.characteristicUuid, charcUuid);
        GattcBuildUuid(&descData.attribute.descriptor.descriptorUuid, descUuid);
        size_t tmpLen = 0;
        descData.data = descriptor.GetValue(&tmpLen).get();
        descData.dataLen = (unsigned short)tmpLen;

        HILOGI("clientId: %{public}d, ret: %{public}d, dataLen: %{public}d", clientId_, ret, descData.dataLen);
        HILOGI("srvUuid: %{public}s, charcUuid: %{public}s, descUuid: %{public}s",
            srvUuid.c_str(), charcUuid.c_str(), descUuid.c_str());
        appCallback_->readDescriptorCb(clientId_, &descData, GetGattcResult(ret));
    }

    void OnDescriptorWriteResult(const GattDescriptor &descriptor, int ret)
    {
        if (appCallback_ == nullptr || appCallback_->writeDescriptorCb == NULL) {
            HILOGI("callback is null.");
            return;
        }

        if (descriptor.GetCharacteristic() == nullptr ||
            descriptor.GetCharacteristic()->GetService() == nullptr) {
            HILOGE("get characteristic or service null.");
            return;
        }

        BtGattDescriptor tmpDesc;
        string srvUuid = descriptor.GetCharacteristic()->GetService()->GetUuid().ToString();
        string charcUuid = descriptor.GetCharacteristic()->GetUuid().ToString();
        string descUuid = descriptor.GetUuid().ToString();
        GattcBuildUuid(&tmpDesc.characteristic.serviceUuid, srvUuid);
        GattcBuildUuid(&tmpDesc.characteristic.characteristicUuid, charcUuid);
        GattcBuildUuid(&tmpDesc.descriptorUuid, descUuid);

        HILOGI("clientId: %{public}d, ret: %{public}d", clientId_, ret);
        HILOGI("srvUuid: %{public}s, charcUuid: %{public}s, descUuid: %{public}s",
            srvUuid.c_str(), charcUuid.c_str(), descUuid.c_str());
        appCallback_->writeDescriptorCb(clientId_, &tmpDesc, GetGattcResult(ret));
    }

    void OnMtuUpdate(int mtu, int ret)
    {
        if (appCallback_ == nullptr || appCallback_->configureMtuSizeCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }

        HILOGI("clientId: %{public}d, mtu: %{public}d, ret: %{public}d", clientId_, mtu, ret);
        appCallback_->configureMtuSizeCb(clientId_, mtu, GetGattcResult(ret));
    }

    void OnSetNotifyCharacteristic(int status)
    {
        if (appCallback_ == nullptr || appCallback_->registerNotificationCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }

        HILOGI("clientId: %{public}d, status: %{public}d", clientId_, status);
        appCallback_->registerNotificationCb(clientId_, GetGattcResult(status));
    }

    void OnCharacteristicChanged(const GattCharacteristic &characteristic)
    {
        if (appCallback_ == nullptr || appCallback_->notificationCb == nullptr) {
            HILOGI("callback is null.");
            return;
        }
        if (characteristic.GetService() == nullptr) {
            HILOGE("get service null.");
            return;
        }

        BtGattReadData notificationData;
        string srvUuid = characteristic.GetService()->GetUuid().ToString();
        string charcUuid = characteristic.GetUuid().ToString();
        GattcBuildUuid(&notificationData.attribute.characteristic.serviceUuid, srvUuid);
        GattcBuildUuid(&notificationData.attribute.characteristic.characteristicUuid, charcUuid);
        size_t tmpLen = 0;
        notificationData.data = characteristic.GetValue(&tmpLen).get();
        notificationData.dataLen = (unsigned short)tmpLen;

        HILOGI("clientId: %{public}d, dataLen: %{public}d, ", clientId_, notificationData.dataLen);
        HILOGI("srvUuid: %{public}s, charcUuid: %{public}s", srvUuid.c_str(), charcUuid.c_str());
        appCallback_->notificationCb(clientId_, &notificationData, OHOS_BT_STATUS_SUCCESS);
    }
private:
    void GattcBuildUuid(BtUuid *desUuid, const std::string &srcUuid)
    {
        desUuid->uuid = (char *)srcUuid.c_str();
        desUuid->uuidLen = srcUuid.size();
    }

    BtGattClientCallbacks *appCallback_;
    int clientId_;
};

/**
 * @brief Registers a GATT client with a specified application UUID.
 *
 * @param appUuid Indicates the UUID of the application for which the GATT client is to be registered.
 * The UUID is defined by the application.
 * @return Returns the client ID.
 */
int BleGattcRegister(BtUuid appUuid)
{
    g_clientIncrease++;
    struct GattClientWrapper clientWrapper;
    clientWrapper.gattClient = nullptr;
    clientWrapper.gattClientCallback = nullptr;
    clientWrapper.remoteAddr = "";
    int clientId = g_clientIncrease;
    GATTCLIENT.insert(std::pair<int, struct GattClientWrapper>(clientId, clientWrapper));
    HILOGI("clientId: %{public}d", clientId);
    return clientId;
}

/**
 * @brief Unregisters a GATT client with a specified ID.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcUnRegister(int clientId)
{
    HILOGI("clientId: %{public}d", clientId);
    ClientIterator it = GATTCLIENT.find(clientId);
    if (it != GATTCLIENT.end()) {
        auto &clientWrapper = it->second;
        if (clientWrapper.gattClient != nullptr) {
            delete clientWrapper.gattClient;
            clientWrapper.gattClient = nullptr;
        }
        if (clientWrapper.gattClientCallback != nullptr) {
            delete clientWrapper.gattClientCallback;
            clientWrapper.gattClientCallback = nullptr;
        }
        GATTCLIENT.erase(it);
    }
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Set fastest connection of the Gatt connection, add address to the accelerate connection map
 *  before create a new connection.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param fastestConnFlag Indicates whether to enable the fastest connection.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcSetFastestConn(int clientId, bool fastestConnFlag)
{
    HILOGI("clientId: %{public}d, fastestConnFlag: %{public}d", clientId, fastestConnFlag);
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Create a Gatt connection to a remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param bdAddr Indicates the remote device's address.
 * @param isAutoConnect Indicates whether it is a direct connection(false) or a background connection(true).
 * @param transport Indicates the transport of Gatt client {@link BtTransportType}.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcConnect(int clientId, BtGattClientCallbacks *func, const BdAddr *bdAddr,
    bool isAutoConnect, BtTransportType transport)
{
    if (func == nullptr || bdAddr == nullptr) {
        HILOGE("func or bdAddr is null.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("clientId: %{public}d, has not been registered.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    string strAddress;
    ConvertAddr(bdAddr->addr, strAddress);
    HILOGI("clientId: %{public}d, addr: %{public}s, isAutoConnect: %{public}d",
        clientId, GetEncryptAddr(strAddress).c_str(), isAutoConnect);
    GattClient *client = nullptr;
    if (iter->second.gattClient != nullptr && iter->second.remoteAddr == strAddress) {
        HILOGI("connect to the same remote device again.");
        client = iter->second.gattClient;
        delete iter->second.gattClientCallback;
        iter->second.gattClientCallback = nullptr;
    } else {
        BluetoothRemoteDevice device(strAddress, transport);
        client = new GattClient(device);
    }

    GattClientCallbackWrapper *clientWrapper = new GattClientCallbackWrapper(func, clientId);
    int result = client->Connect(*(clientWrapper), isAutoConnect, transport);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    if (result == OHOS_BT_STATUS_SUCCESS) {
        iter->second.gattClient = client;
        iter->second.gattClientCallback = clientWrapper;
        iter->second.remoteAddr = strAddress;
        return OHOS_BT_STATUS_SUCCESS;
    } else {
        HILOGE("fail.");
        delete client;
        delete clientWrapper;
        iter->second.gattClient = nullptr;
        iter->second.gattClientCallback = nullptr;
        iter->second.remoteAddr = "";
        return OHOS_BT_STATUS_FAIL;
    }
}

/**
 * @brief Set priority of the Gatt connection.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param bdAddr Indicates the remote device's address.
 * @param priority Indicates the priority of Gatt client {@link BtGattPriority}.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcSetPriority(int clientId, const BdAddr *bdAddr, BtGattPriority priority)
{
    if (bdAddr == nullptr) {
        HILOGE("bdAddr is null.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("clientId: %{public}d, has not been registered.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    string strAddress;
    ConvertAddr(bdAddr->addr, strAddress);
    HILOGI("clientId: %{public}d, addr: %{public}s, priority: %{public}d",
        clientId, GetEncryptAddr(strAddress).c_str(), priority);
    if (iter->second.gattClient == nullptr || iter->second.remoteAddr != strAddress) {
        HILOGE("fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    GattClient *client = iter->second.gattClient;
    int result = client->RequestConnectionPriority(priority);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Disconnect a Gatt connection with a remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @Returns the operation result status {@link BtStatus}.
 */
int BleGattcDisconnect(int clientId)
{
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("clientId: %{public}d, has not been registered.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    GattClient *client = iter->second.gattClient;
    if (client == nullptr) {
        HILOGE("clientId: %{public}d, has not been connected.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    int result = client->Disconnect();
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Request a GATT service discovery on a remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcSearchServices(int clientId)
{
    HILOGI("clientId: %{public}d", clientId);
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("clientId: %{public}d, has not been registered.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    GattClient *client = iter->second.gattClient;
    if (client == nullptr) {
        HILOGE("clientId: %{public}d, has not been connected.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    HILOGI("DiscoverServices() called");
    int result = client->DiscoverServices();
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Check whether the expected service exists.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param serviceUuid Indicates the UUID of the service.
 * @return Returns true or false.
 */
bool BleGattcGetService(int clientId, BtUuid serviceUuid)
{
    HILOGI("clientId: %{public}d", clientId);
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("clientId has not been registered.");
        return false;
    }

    GattClient *client = iter->second.gattClient;
    if (client == nullptr) {
        HILOGE("gatt is not connected.");
        return false;
    }

    string strUuid(serviceUuid.uuid);
    if (!regex_match(strUuid, uuidRegex)) {
        HILOGE("match the UUID faild.");
        return false;
    }
    UUID uuid(UUID::FromString(strUuid));
    HILOGI("service uuid: %{public}s", strUuid.c_str());
    std::optional<std::reference_wrapper<GattService>> gattService = client->GetService(uuid);
    if (gattService == std::nullopt) {
        HILOGE("get service failed, gattService is null.");
        return false;
    }
    GattService service = gattService->get();
    if (service.GetUuid().Equals(uuid)) {
        HILOGI("get service success.");
        return true;
    } else {
        HILOGE("get service failed, the service uuid is not exist.");
        return false;
    }
}

/**
 * @brief Read characteristic value from the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The specified characteristic {@link BtGattCharacteristic} to be read.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcReadCharacteristic(int clientId, BtGattCharacteristic characteristic)
{
    HILOGI("clientId: %{public}d", clientId);
    GattClient *client = nullptr;
    GattCharacteristic *tmpCharac = GattcFindCharacteristic(clientId, &client, characteristic);
    if (tmpCharac == nullptr || client == nullptr) {
        HILOGE("find characteristic fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    int result = client->ReadCharacteristic(*tmpCharac);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Write characteristic value to the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The specified characteristic {@link BtGattCharacteristic} to be read.
 * @param writeType Indicates the characteristic write type.
 * @param value The value to be write.
 * @param len The length of the value.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcWriteCharacteristic(int clientId, BtGattCharacteristic characteristic,
    BtGattWriteType writeType, int len, const char *value)
{
    HILOGI("clientId:%{public}d, writeType:%{public}d, len:%{public}d", clientId, writeType, len);
    GattClient *client = nullptr;
    GattCharacteristic *tmpCharac = GattcFindCharacteristic(clientId, &client, characteristic);
    if (tmpCharac == nullptr || client == nullptr) {
        HILOGE("find characteristic fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    tmpCharac->SetValue((unsigned char *)value, len);
    int newWriteType = ConverWriteType(writeType);
    tmpCharac->SetWriteType(newWriteType);
    int result = client->WriteCharacteristic(*tmpCharac);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Read descriptor value from the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param descriptor The specified characteristic {@link BtGattDescriptor} to be read.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcReadDescriptor(int clientId, BtGattDescriptor descriptor)
{
    HILOGI("clientId: %{public}d", clientId);
    GattClient *client = nullptr;
    GattCharacteristic *tmpCharac = GattcFindCharacteristic(clientId, &client, descriptor.characteristic);
    if (tmpCharac == nullptr || client == nullptr) {
        HILOGE("find characteristic fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    string strUuidDesc(descriptor.descriptorUuid.uuid);
    if (!regex_match(strUuidDesc, uuidRegex)) {
        HILOGE("match the UUID faild.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }
    GattDescriptor *tmpDescriptor = tmpCharac->GetDescriptor(UUID::FromString(strUuidDesc));
    if (tmpDescriptor == nullptr) {
        HILOGE("find descriptor fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    int result = client->ReadDescriptor(*tmpDescriptor);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Write descriptor value to the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param descriptor The specified descriptor {@link BtGattDescriptor} to be read.
 * @param value The value to be write.
 * @param len The length of the value.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcWriteDescriptor(int clientId, BtGattDescriptor descriptor, int len, const char *value)
{
    HILOGI("clientId:%{public}d, len:%{public}d", clientId, len);
    GattClient *client = nullptr;
    GattCharacteristic *tmpCharac = GattcFindCharacteristic(clientId, &client, descriptor.characteristic);
    if (tmpCharac == nullptr || client == nullptr) {
        HILOGE("find characteristic fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    string strUuidDesc(descriptor.descriptorUuid.uuid);
    if (!regex_match(strUuidDesc, uuidRegex)) {
        HILOGE("match the UUID faild.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }
    GattDescriptor *tmpDescriptor = tmpCharac->GetDescriptor(UUID::FromString(strUuidDesc));
    if (tmpDescriptor == nullptr) {
        HILOGE("find descriptor fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    tmpDescriptor->SetValue((unsigned char *)value, len);
    int result = client->WriteDescriptor(*tmpDescriptor);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Configure the ATT MTU size.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param mtuSize The size of MTU.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcConfigureMtuSize(int clientId, int mtuSize)
{
    HILOGI("clientId:%{public}d, mtuSize:%{public}d", clientId, mtuSize);
    ClientIterator iter = GATTCLIENT.find(clientId);
    if (iter == GATTCLIENT.end()) {
        HILOGE("GattcFindCharacteristic, clientId: %{public}d, has not been registered.", clientId);
        return OHOS_BT_STATUS_FAIL;
    }

    GattClient *client = iter->second.gattClient;
    if (client == nullptr) {
        HILOGE("client is null.");
        return OHOS_BT_STATUS_FAIL;
    }

    int result = client->RequestBleMtuSize(mtuSize);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}

/**
 * @brief Enable or disable notifications for a given characteristic.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The specified characteristic {@link BtGattCharacteristic}.
 * @param enable True or false.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcRegisterNotification(int clientId, BtGattCharacteristic characteristic, bool enable)
{
    HILOGI("clientId:%{public}d, enable:%{public}d", clientId, enable);
    GattClient *client = nullptr;
    GattCharacteristic *tmpCharac = GattcFindCharacteristic(clientId, &client, characteristic);
    if (tmpCharac == nullptr || client == nullptr) {
        HILOGE("find characteristic fail.");
        return OHOS_BT_STATUS_FAIL;
    }

    int result = client->SetNotifyCharacteristic(*tmpCharac, enable);
    HILOGI("clientId: %{public}d, result: %{public}d", clientId, result);
    return GetGattcResult(result);
}
}  // namespace Bluetooth
}  // namespace OHOS
#ifdef __cplusplus
}
#endif
/** @} */