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

#include "napi_bluetooth_utils.h"
#include <functional>
#include <optional>
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_bluetooth_spp_client.h"
#include "securec.h"

namespace OHOS {
namespace Bluetooth {
using namespace std;

napi_value GetCallbackErrorValue(napi_env env, int errCode)
{
    HILOGE("errCode: %{public}d", errCode);
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    napi_create_int32(env, errCode, &eCode);
    napi_create_object(env, &result);
    napi_set_named_property(env, result, "code", eCode);
    return result;
}

std::shared_ptr<BluetoothCallbackInfo> GetCallbackInfoByType(const std::string type)
{
    std::lock_guard<std::mutex> lock(g_observerMutex);
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> observers = GetObserver();
    if (!observers[type]) {
        return nullptr;
    }
    return observers[type];
}

bool ParseString(napi_env env, string &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    if (valuetype != napi_string) {
        HILOGE("Wrong argument type(%{public}d). String expected.", valuetype);
        return false;
    }
    size_t size = 0;

    if (napi_get_value_string_utf8(env, args, nullptr, 0, &size) != napi_ok) {
        HILOGE("can not get string size");
        param = "";
        return false;
    }
    param.reserve(size + 1);
    param.resize(size);
    if (napi_get_value_string_utf8(env, args, param.data(), (size + 1), &size) != napi_ok) {
        HILOGE("can not get string value");
        param = "";
        return false;
    }
    return true;
}

bool ParseInt32(napi_env env, int32_t &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    if (valuetype != napi_number) {
        HILOGE("Wrong argument type(%{public}d). Int32 expected.", valuetype);
        return false;
    }
    napi_get_value_int32(env, args, &param);
    return true;
}

bool ParseBool(napi_env env, bool &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    if (valuetype != napi_boolean) {
        HILOGE("Wrong argument type(%{public}d). bool expected.", valuetype);
        return false;
    }
    napi_get_value_bool(env, args, &param);
    return true;
}


bool ParseArrayBuffer(napi_env env, uint8_t** data, size_t &size, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    if (valuetype != napi_object) {
        HILOGE("Wrong argument type(%{public}d). object expected.", valuetype);
        return false;
    }

    status = napi_get_arraybuffer_info(env, args, (void**)data, &size);
    if (status != napi_ok) {
        HILOGE("can not get arraybuffer, error is %{public}d", status);
        return false;
    }
    HILOGI("arraybuffer size is %{public}zu", size);
    return true;
}

void ConvertStringVectorToJS(napi_env env, napi_value result, std::vector<std::string>& stringVector)
{
    HILOGI("vector size: %{public}zu", stringVector.size());
    size_t idx = 0;

    if (stringVector.empty()) {
        return;
    }

    for (auto& str : stringVector) {
        napi_value obj = nullptr;
        napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &obj);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertGattServiceVectorToJS(napi_env env, napi_value result, vector<GattService>& services)
{
    HILOGI("enter");
    size_t idx = 0;

    if (services.empty()) {
        return;
    }
    HILOGI("size: %{public}zu", services.size());
    for (auto& service : services) {
        napi_value obj = nullptr;
        napi_create_object(env, &obj);
        ConvertGattServiceToJS(env, obj, service);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertOppTransferInformationToJS(napi_env env, napi_value result,
    BluetoothOppTransferInformation& transferInformation)
{
    HILOGI("ConvertOppTransferInformationToJS called");
    napi_value id;
    napi_create_int32(env, transferInformation.GetId(), &id);
    napi_set_named_property(env, result, "id", id);

    napi_value fileName;
    napi_create_string_utf8(env, transferInformation.GetFileName().c_str(), NAPI_AUTO_LENGTH, &fileName);
    napi_set_named_property(env, result, "fileName", fileName);

    napi_value filePath;
    napi_create_string_utf8(env, transferInformation.GetFilePath().c_str(), NAPI_AUTO_LENGTH, &filePath);
    napi_set_named_property(env, result, "filePath", filePath);

    napi_value mimeType;
    napi_create_string_utf8(env, transferInformation.GetMimeType().c_str(), NAPI_AUTO_LENGTH, &mimeType);
    napi_set_named_property(env, result, "mimeType", mimeType);

    napi_value deviceName;
    napi_create_string_utf8(env, transferInformation.GetDeviceName().c_str(), NAPI_AUTO_LENGTH, &deviceName);
    napi_set_named_property(env, result, "deviceName", deviceName);

    napi_value deviceAddress;
    napi_create_string_utf8(env, transferInformation.GetDeviceAddress().c_str(), NAPI_AUTO_LENGTH, &deviceAddress);
    napi_set_named_property(env, result, "deviceAddress", deviceAddress);

    napi_value direction;
    napi_create_int32(env, transferInformation.GetDirection(), &direction);
    napi_set_named_property(env, result, "direction", direction);

    napi_value status;
    napi_create_int32(env, transferInformation.GetStatus(), &status);
    napi_set_named_property(env, result, "status", status);

    napi_value failedReason;
    napi_create_int32(env, transferInformation.GetFailedReason(), &failedReason);
    napi_set_named_property(env, result, "failedReason", failedReason);

    napi_value timeStamp;
    napi_create_int64(env, transferInformation.GetTimeStamp(), &timeStamp);
    napi_set_named_property(env, result, "timeStamp", timeStamp);

    napi_value currentBytes;
    napi_create_int64(env, transferInformation.GetCurrentBytes(), &currentBytes);
    napi_set_named_property(env, result, "currentBytes", currentBytes);

    napi_value totalBytes;
    napi_create_int64(env, transferInformation.GetTotalBytes(), &totalBytes);
    napi_set_named_property(env, result, "totalBytes", totalBytes);
}

void ConvertGattServiceToJS(napi_env env, napi_value result, GattService& service)
{
    napi_value serviceUuid;
    napi_create_string_utf8(env, service.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &serviceUuid);
    napi_set_named_property(env, result, "serviceUuid", serviceUuid);

    napi_value isPrimary;
    napi_get_boolean(env, service.IsPrimary(), &isPrimary);
    napi_set_named_property(env, result, "isPrimary", isPrimary);
    HILOGI("uuid: %{public}s, isPrimary: %{public}d", service.GetUuid().ToString().c_str(), service.IsPrimary());

    napi_value characteristics;
    napi_create_array(env, &characteristics);
    ConvertBLECharacteristicVectorToJS(env, characteristics, service.GetCharacteristics());
    napi_set_named_property(env, result, "characteristics", characteristics);

    napi_value includedServices;
    napi_create_array(env, &includedServices);
    vector<GattService> services;
    vector<std::reference_wrapper<GattService>> srvs = service.GetIncludedServices();
    for (auto &srv : srvs) {
        services.push_back(srv.get());
    }
    ConvertGattServiceVectorToJS(env, includedServices, services);
    napi_set_named_property(env, result, "includedServices", includedServices);
}

void ConvertBLECharacteristicVectorToJS(napi_env env, napi_value result,
    vector<GattCharacteristic>& characteristics)
{
    HILOGI("size: %{public}zu", characteristics.size());
    size_t idx = 0;
    if (characteristics.empty()) {
        return;
    }

    for (auto &characteristic : characteristics) {
        napi_value obj = nullptr;
        napi_create_object(env, &obj);
        ConvertBLECharacteristicToJS(env, obj, characteristic);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertBLECharacteristicToJS(napi_env env, napi_value result, GattCharacteristic& characteristic)
{
    napi_value characteristicUuid;
    HILOGI("uuid: %{public}s", characteristic.GetUuid().ToString().c_str());
    napi_create_string_utf8(env, characteristic.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &characteristicUuid);
    napi_set_named_property(env, result, "characteristicUuid", characteristicUuid);

    if (characteristic.GetService() != nullptr) {
        napi_value serviceUuid;
        napi_create_string_utf8(env, characteristic.GetService()->GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH,
            &serviceUuid);
        napi_set_named_property(env, result, "serviceUuid", serviceUuid);
    }

    size_t valueSize = 0;
    uint8_t* valueData = characteristic.GetValue(&valueSize).get();
    if (valueSize != 0) {
        napi_value value = nullptr;
        uint8_t* bufferData = nullptr;
        napi_create_arraybuffer(env, valueSize, (void**)&bufferData, &value);
        if (memcpy_s(bufferData, valueSize, valueData, valueSize) != EOK) {
            HILOGE("memcpy_s failed");
            return;
        }
        napi_set_named_property(env, result, "characteristicValue", value);
    }
    napi_value descriptors;
    napi_create_array(env, &descriptors);
    ConvertBLEDescriptorVectorToJS(env, descriptors, characteristic.GetDescriptors());
    napi_set_named_property(env, result, "descriptors", descriptors);
}


void ConvertBLEDescriptorVectorToJS(napi_env env, napi_value result, vector<GattDescriptor>& descriptors)
{
    HILOGI("size: %{public}zu", descriptors.size());
    size_t idx = 0;

    if (descriptors.empty()) {
        return;
    }

    for (auto& descriptor : descriptors) {
        napi_value obj = nullptr;
        napi_create_object(env, &obj);
        ConvertBLEDescriptorToJS(env, obj, descriptor);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertBLEDescriptorToJS(napi_env env, napi_value result, GattDescriptor& descriptor)
{
    HILOGI("uuid: %{public}s", descriptor.GetUuid().ToString().c_str());

    napi_value descriptorUuid;
    napi_create_string_utf8(env, descriptor.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &descriptorUuid);
    napi_set_named_property(env, result, "descriptorUuid", descriptorUuid);

    if (descriptor.GetCharacteristic() != nullptr) {
        napi_value characteristicUuid;
        napi_create_string_utf8(env, descriptor.GetCharacteristic()->GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH,
            &characteristicUuid);
        napi_set_named_property(env, result, "characteristicUuid", characteristicUuid);

        if (descriptor.GetCharacteristic()->GetService() != nullptr) {
            napi_value serviceUuid;
            napi_create_string_utf8(env, descriptor.GetCharacteristic()->GetService()->GetUuid().ToString().c_str(),
                NAPI_AUTO_LENGTH, &serviceUuid);
            napi_set_named_property(env, result, "serviceUuid", serviceUuid);
        }
    }

    napi_value value;
    size_t valueSize;
    uint8_t* valueData = descriptor.GetValue(&valueSize).get();
    uint8_t* bufferData = nullptr;
    napi_create_arraybuffer(env, valueSize, (void**)&bufferData, &value);
    (void)memcpy_s(bufferData, valueSize, valueData, valueSize);
    napi_set_named_property(env, result, "descriptorValue", value);
}

void ConvertCharacteristicReadReqToJS(napi_env env, napi_value result, const std::string &device,
    GattCharacteristic &characteristic, int requestId)
{
    napi_value deviceId;
    napi_create_string_utf8(env, device.c_str(), NAPI_AUTO_LENGTH, &deviceId);
    napi_set_named_property(env, result, "deviceId", deviceId);

    napi_value transId;
    napi_create_int32(env, requestId, &transId);
    napi_set_named_property(env, result, "transId", transId);

    napi_value offset;
    napi_create_int32(env, 0, &offset);
    napi_set_named_property(env, result, "offset", offset);
    HILOGI("offset is %{public}d", 0);

    napi_value characteristicUuid;
    napi_create_string_utf8(env, characteristic.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &characteristicUuid);
    napi_set_named_property(env, result, "characteristicUuid", characteristicUuid);
    HILOGI("characteristicUuid is %{public}s", characteristic.GetUuid().ToString().c_str());

    if (characteristic.GetService() != nullptr) {
        napi_value serviceUuid;
        napi_create_string_utf8(env, characteristic.GetService()->GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH,
            &serviceUuid);
        napi_set_named_property(env, result, "serviceUuid", serviceUuid);
    }
}

void ConvertDescriptorReadReqToJS(napi_env env, napi_value result, const std::string &device,
    GattDescriptor& descriptor, int requestId)
{
    napi_value deviceId;
    napi_create_string_utf8(env, device.c_str(), NAPI_AUTO_LENGTH, &deviceId);
    napi_set_named_property(env, result, "deviceId", deviceId);

    napi_value transId;
    napi_create_int32(env, requestId, &transId);
    napi_set_named_property(env, result, "transId", transId);

    napi_value offset;
    napi_create_int32(env, 0, &offset);
    napi_set_named_property(env, result, "offset", offset);
    HILOGI("offset is %{public}d", 0);

    napi_value descriptorUuid;
    napi_create_string_utf8(env, descriptor.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &descriptorUuid);
    napi_set_named_property(env, result, "descriptorUuid", descriptorUuid);
    HILOGI("descriptorUuid is %{public}s", descriptor.GetUuid().ToString().c_str());

    if (descriptor.GetCharacteristic() != nullptr) {
    napi_value characteristicUuid;
    napi_create_string_utf8(env, descriptor.GetCharacteristic()->GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH,
        &characteristicUuid);
    napi_set_named_property(env, result, "characteristicUuid", characteristicUuid);

        if (descriptor.GetCharacteristic()->GetService() != nullptr) {
            napi_value serviceUuid;
            napi_create_string_utf8(env, descriptor.GetCharacteristic()->GetService()->GetUuid().ToString().c_str(),
                NAPI_AUTO_LENGTH, &serviceUuid);
            napi_set_named_property(env, result, "serviceUuid", serviceUuid);
        }
    }
}

void ConvertCharacteristicWriteReqToJS(napi_env env, napi_value result, const std::string &device,
    GattCharacteristic& characteristic, int requestId)
{
    napi_value deviceId;
    napi_create_string_utf8(env, device.c_str(), NAPI_AUTO_LENGTH, &deviceId);
    napi_set_named_property(env, result, "deviceId", deviceId);

    napi_value transId;
    napi_create_int32(env, requestId, &transId);
    napi_set_named_property(env, result, "transId", transId);

    napi_value offset;
    napi_create_int32(env, 0, &offset);
    napi_set_named_property(env, result, "offset", offset);
    HILOGI("offset is %{public}d", 0);

    napi_value isPrep;
    napi_get_boolean(env, false, &isPrep);
    napi_set_named_property(env, result, "isPrep", isPrep);

    napi_value needRsp;
    napi_get_boolean(env, true, &needRsp);
    napi_set_named_property(env, result, "needRsp", needRsp);

    napi_value value;
    size_t valueSize;
    uint8_t* valueData = characteristic.GetValue(&valueSize).get();
    uint8_t* bufferData = nullptr;
    napi_create_arraybuffer(env, valueSize, (void**)&bufferData, &value);
    (void)memcpy_s(bufferData, valueSize, valueData, valueSize);
    napi_set_named_property(env, result, "value", value);

    napi_value characteristicUuid;
    napi_create_string_utf8(env, characteristic.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &characteristicUuid);
    napi_set_named_property(env, result, "characteristicUuid", characteristicUuid);
    HILOGI("characteristicUuid is %{public}s", characteristic.GetUuid().ToString().c_str());

    if (characteristic.GetService() != nullptr) {
        napi_value serviceUuid;
        napi_create_string_utf8(env, characteristic.GetService()->GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH,
            &serviceUuid);
        napi_set_named_property(env, result, "serviceUuid", serviceUuid);
    }
}

void ConvertDescriptorWriteReqToJS(napi_env env, napi_value result, const std::string &device,
    GattDescriptor &descriptor, int requestId)
{
    napi_value deviceId;
    napi_create_string_utf8(env, device.c_str(), NAPI_AUTO_LENGTH, &deviceId);
    napi_set_named_property(env, result, "deviceId", deviceId);

    napi_value transId;
    napi_create_int32(env, requestId, &transId);
    napi_set_named_property(env, result, "transId", transId);

    napi_value offset;
    napi_create_int32(env, 0, &offset);
    napi_set_named_property(env, result, "offset", offset);
    HILOGI("offset is %{public}d", 0);

    napi_value isPrep;
    napi_get_boolean(env, false, &isPrep);
    napi_set_named_property(env, result, "isPrep", isPrep);

    napi_value needRsp;
    napi_get_boolean(env, true, &needRsp);
    napi_set_named_property(env, result, "needRsp", needRsp);

    napi_value value;
    size_t valueSize;
    uint8_t* valueData = descriptor.GetValue(&valueSize).get();
    uint8_t* bufferData = nullptr;
    napi_create_arraybuffer(env, valueSize, (void**)&bufferData, &value);
    (void)memcpy_s(bufferData, valueSize, valueData, valueSize);
    napi_set_named_property(env, result, "value", value);

    napi_value descriptorUuid;
    napi_create_string_utf8(env, descriptor.GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH, &descriptorUuid);
    napi_set_named_property(env, result, "descriptorUuid", descriptorUuid);
    HILOGI("descriptorUuid is %{public}s", descriptor.GetUuid().ToString().c_str());

    if (descriptor.GetCharacteristic() != nullptr) {
        napi_value characteristicUuid;
        napi_create_string_utf8(env, descriptor.GetCharacteristic()->GetUuid().ToString().c_str(), NAPI_AUTO_LENGTH,
            &characteristicUuid);
        napi_set_named_property(env, result, "characteristicUuid", characteristicUuid);

        if (descriptor.GetCharacteristic()->GetService() != nullptr) {
            napi_value serviceUuid;
            napi_create_string_utf8(env, descriptor.GetCharacteristic()->GetService()->GetUuid().ToString().c_str(),
                NAPI_AUTO_LENGTH, &serviceUuid);
            napi_set_named_property(env, result, "serviceUuid", serviceUuid);
        }
    }
}

void ConvertStateChangeParamToJS(napi_env env, napi_value result, const std::string &device, int state)
{
    napi_value deviceId = nullptr;
    napi_create_string_utf8(env, device.c_str(), NAPI_AUTO_LENGTH, &deviceId);
    napi_set_named_property(env, result, "deviceId", deviceId);

    napi_value profileState = nullptr;
    napi_create_int32(env, GetProfileConnectionState(state), &profileState);
    napi_set_named_property(env, result, "state", profileState);
}

void ConvertScoStateChangeParamToJS(napi_env env, napi_value result, const std::string &device, int state)
{
    napi_value deviceId = nullptr;
    napi_create_string_utf8(env, device.c_str(), NAPI_AUTO_LENGTH, &deviceId);
    napi_set_named_property(env, result, "deviceId", deviceId);

    napi_value profileState = nullptr;
    napi_create_int32(env, GetScoConnectionState(state), &profileState);
    napi_set_named_property(env, result, "state", profileState);
}

bool GetServiceVectorFromJS(napi_env env, napi_value object, vector<GattService>& services,
    std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client)
{
    size_t idx = 0;
    bool hasElement = false;
    napi_has_element(env, object, idx, &hasElement);
    while (hasElement) {
        napi_value result = nullptr;
        napi_create_object(env, &result);
        napi_get_element(env, object, idx, &result);
        GattService* service = GetServiceFromJS(env, result, nullptr, nullptr);
        if (service == nullptr) {
            HILOGE("characteristic is nullptr");
            return false;
        }
        services.push_back(*service);
        delete service;
        idx++;
        napi_has_element(env, object, idx, &hasElement);
    }
    HILOGI("services size is %{public}zu", services.size());
    return true;
}

GattService* GetServiceFromJS(napi_env env, napi_value object, std::shared_ptr<GattServer> server,
    std::shared_ptr<GattClient> client)
{
    string serviceUuid;
    bool isPrimary = false;

    napi_value propertyNameValue = nullptr;
    napi_value value = nullptr;
    bool hasProperty = false;

    napi_create_string_utf8(env, "serviceUuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool isSuccess = ParseString(env, serviceUuid, value);
    if (!isSuccess || (!regex_match(serviceUuid, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }

    napi_create_string_utf8(env, "isPrimary", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseBool(env, isPrimary, value);
    HILOGI("serviceUuid: %{public}s, isPrimary: %{public}d", serviceUuid.c_str(), isPrimary);

    GattServiceType serviceType = GattServiceType::PRIMARY;
    if (!isPrimary) {
        serviceType = GattServiceType::SECONDARY;
    }

    GattService* service = nullptr;
    if (server == nullptr && client == nullptr) {
        service =  new GattService(UUID::FromString(serviceUuid), serviceType);

        napi_create_string_utf8(env, "characteristics", NAPI_AUTO_LENGTH, &propertyNameValue);
        napi_has_property(env, object, propertyNameValue, &hasProperty);
        if (!hasProperty) {
            HILOGE("no characteristics field");
            return nullptr;
        }
        napi_get_property(env, object, propertyNameValue, &value);
        vector<GattCharacteristic> characteristics;
        bool ret = GetCharacteristicVectorFromJS(env, value, characteristics, server, client);
        if (!ret) {
            HILOGI("GetCharacteristicVectorFromJS faild");
            return nullptr;
        }
        for (auto& characteristic : characteristics) {
            service->AddCharacteristic(characteristic);
        }

        napi_create_string_utf8(env, "includeServices", NAPI_AUTO_LENGTH, &propertyNameValue);
        napi_has_property(env, object, propertyNameValue, &hasProperty);
        if (hasProperty) {
            napi_get_property(env, object, propertyNameValue, &value);
            vector<GattService> services;
            bool ret = GetServiceVectorFromJS(env, value, services, server, client);
            if (!ret) {
                HILOGI("GetServiceVectorFromJS faild");
                return nullptr;
            }
            for (auto& serv : services) {
                service->AddService(serv);
            }
        }
    } else {
        std::optional<std::reference_wrapper<GattService>> obtainedService;
        if (server != nullptr) {
            obtainedService = server->GetService(UUID::FromString(serviceUuid), isPrimary);
        } else if (client != nullptr) {
            if (client->DiscoverServices()) {
                obtainedService = client->GetService(UUID::FromString(serviceUuid));
            } else {
                return nullptr;
            }
        }

        if (obtainedService == std::nullopt) {
            return nullptr;
        } else {
            service = &(obtainedService->get());
        }
    }
    return service;
}

bool GetCharacteristicVectorFromJS(napi_env env, napi_value object, vector<GattCharacteristic>& characteristics,
    std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client)
{
    size_t idx = 0;

    bool hasElement = false;
    napi_has_element(env, object, idx, &hasElement);
    while (hasElement) {
        napi_value result = nullptr;
        napi_create_object(env, &result);
        napi_get_element(env, object, idx, &result);
        GattCharacteristic* characteristic = GetCharacteristicFromJS(env, result, server, client);
        if (characteristic ==nullptr) {
            HILOGE("characteristic is nullptr");
            return false;
        }
        characteristics.push_back(*characteristic);
        if (server == nullptr && client == nullptr) {
            delete characteristic;
        }
        idx++;
        napi_has_element(env, object, idx, &hasElement);
    };
    HILOGI("characteristics size is %{public}zu", characteristics.size());
    return true;
}

GattCharacteristic* GetCharacteristicFromJS(napi_env env, napi_value object, std::shared_ptr<GattServer> server,
    std::shared_ptr<GattClient> client)
{
    string serviceUuid;
    string characteristicUuid;
    uint8_t *characteristicValue = nullptr;
    size_t characteristicValueSize = 0;

    napi_value propertyNameValue = nullptr;
    napi_value value = nullptr;
    bool hasProperty = false;

    napi_create_string_utf8(env, "serviceUuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool parseServUuid = ParseString(env, serviceUuid, value);
    if (!parseServUuid || (!regex_match(serviceUuid, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }
    HILOGI("serviceUuid is %{public}s", serviceUuid.c_str());

    napi_create_string_utf8(env, "characteristicUuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool parseCharacUuid = ParseString(env, characteristicUuid, value);
    if (!parseCharacUuid || (!regex_match(characteristicUuid, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }
    HILOGI("characteristicUuid is %{public}s", characteristicUuid.c_str());

    GattCharacteristic* characteristic = nullptr;
    std::optional<std::reference_wrapper<GattService>> service = nullopt;

    if (server == nullptr && client == nullptr) {
        characteristic = new GattCharacteristic(UUID::FromString(characteristicUuid),
            (GattCharacteristic::Permission::READABLE | GattCharacteristic::Permission::WRITEABLE),
                GattCharacteristic::Propertie::NOTIFY | GattCharacteristic::Propertie::READ
                | GattCharacteristic::Propertie::WRITE);

        napi_create_string_utf8(env, "descriptors", NAPI_AUTO_LENGTH, &propertyNameValue);
        napi_has_property(env, object, propertyNameValue, &hasProperty);
        if (hasProperty) {
            napi_get_property(env, object, propertyNameValue, &value);
            vector<GattDescriptor> descriptors;
            bool ret = GetDescriptorVectorFromJS(env, value, descriptors);
            if (!ret) {
                HILOGE("GetDescriptorVectorFromJS faild");
                return nullptr;
            }
            for (auto& descriptor : descriptors) {
                characteristic->AddDescriptor(descriptor);
            }
        } else {
            HILOGE("descriptor is nullptr");
            return nullptr;
        }
    } else {
        if (server != nullptr) {
            service = server->GetService(UUID::FromString(serviceUuid), true);
            if (service == std::nullopt) {
                service = server->GetService(UUID::FromString(serviceUuid), false);
            }
        } else if (client != nullptr) {
            service = client->GetService(UUID::FromString(serviceUuid));
        }
        if (service == std::nullopt) {
            HILOGI("service is null");
            return nullptr;
        } else {
            characteristic = service->get().GetCharacteristic(UUID::FromString(characteristicUuid));
        }
    }

    if (characteristic == nullptr) {
        HILOGI("characteristic is null");
        return nullptr;
    }
    napi_create_string_utf8(env, "characteristicValue", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    if (!ParseArrayBuffer(env, &characteristicValue, characteristicValueSize, value)) {
        HILOGE("ParseArrayBuffer failed");
        return nullptr;
    }
    characteristic->SetValue(characteristicValue, characteristicValueSize);
    return characteristic;
}

void SetNamedPropertyByInteger(napi_env env, napi_value dstObj, int32_t objName, const char *propName)
{
    napi_value prop = nullptr;
    if (napi_create_int32(env, objName, &prop) == napi_ok) {
        napi_set_named_property(env, dstObj, propName, prop);
    }
}

napi_value NapiGetNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

napi_value NapiGetBooleanFalse(napi_env env)
{
    napi_value result = nullptr;
    napi_get_boolean(env, false, &result);
    return result;
}

napi_value NapiGetBooleanTrue(napi_env env)
{
    napi_value result = nullptr;
    napi_get_boolean(env, true, &result);
    return result;
}

napi_value NapiGetBooleanRet(napi_env env, bool ret)
{
    napi_value result = nullptr;
    napi_get_boolean(env, ret, &result);
    return result;
}

napi_value NapiGetUndefinedRet(napi_env env)
{
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value RegisterObserver(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    HILOGI("argc: %{public}zu", argc);
    if (argc == expectedArgsCount + 1) {
        NapiSppClient::On(env, info);
    } else {
        NAPI_ASSERT(env, argc == expectedArgsCount, "Requires 2 arguments.");
        std::string type;
        ParseString(env, type, argv[PARAM0]);
        std::shared_ptr<BluetoothCallbackInfo> pCallbackInfo = std::make_shared<BluetoothCallbackInfo>();
        pCallbackInfo->env_ = env;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valueType);
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &pCallbackInfo->callback_);
        std::lock_guard<std::mutex> lock(g_observerMutex);
        g_Observer[type] = pCallbackInfo;
        HILOGI("%{public}s is registered", type.c_str());
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

static void RemoveObserver(std::string type)
{
    std::lock_guard<std::mutex> lock(g_observerMutex);
    if (g_Observer.find(type) != g_Observer.end()) {
        g_Observer[type] = nullptr;
        HILOGI("%{public}s is deregistered", type.c_str());
    } else {
        HILOGE("%{public}s has not been registered", type.c_str());
    }
}

static std::map<std::string, int32_t> registerTypeMap = {
    {REGISTER_DEVICE_FIND_TYPE, BLUETOOTH_DEVICE_FIND_TYPE},
    {REGISTER_STATE_CHANGE_TYPE, STATE_CHANGE_TYPE},
    {REGISTER_PIN_REQUEST_TYPE, PIN_REQUEST_TYPE},
    {REGISTER_BONE_STATE_TYPE, BOND_STATE_CHANGE_TYPE},
    {REGISTER_BLE_FIND_DEVICE_TYPE, BLE_DEVICE_FIND_TYPE}
};

napi_value CallbackObserver(int32_t typeNum, std::shared_ptr<BluetoothCallbackInfo> pCallbackInfo)
{
    napi_value result = 0;
    napi_value deviceId = nullptr;
    napi_value state = nullptr;
    napi_value pinCode = nullptr;
    napi_value value = 0;

    switch (typeNum) {
        case BLUETOOTH_DEVICE_FIND_TYPE:
            napi_create_array(pCallbackInfo->env_, &result);
            napi_create_string_utf8(pCallbackInfo->env_, INVALID_DEVICE_ID.c_str(), NAPI_AUTO_LENGTH, &value);
            napi_set_element(pCallbackInfo->env_, result, 0, value);
            break;
        case STATE_CHANGE_TYPE:
            napi_create_int32(pCallbackInfo->env_, static_cast<int32_t>(BluetoothState::STATE_OFF), &result);
            break;
        case PIN_REQUEST_TYPE:
            napi_create_object(pCallbackInfo->env_, &result);
            napi_create_string_utf8(pCallbackInfo->env_, INVALID_DEVICE_ID.c_str(), NAPI_AUTO_LENGTH, &deviceId);
            napi_set_named_property(pCallbackInfo->env_, result, "deviceId", deviceId);
            napi_create_string_utf8(pCallbackInfo->env_, INVALID_PIN_CODE.c_str(), NAPI_AUTO_LENGTH, &pinCode);
            napi_set_named_property(pCallbackInfo->env_, result, "pinCode", pinCode);
            break;
        case BOND_STATE_CHANGE_TYPE:
            napi_create_object(pCallbackInfo->env_, &result);
            napi_create_string_utf8(pCallbackInfo->env_, INVALID_DEVICE_ID.c_str(), NAPI_AUTO_LENGTH, &deviceId);
            napi_set_named_property(pCallbackInfo->env_, result, "deviceId", deviceId);
            napi_create_int32(pCallbackInfo->env_, static_cast<int32_t>(BondState::BOND_STATE_INVALID), &state);
            napi_set_named_property(pCallbackInfo->env_, result, "state", state);
            break;
        case BLE_DEVICE_FIND_TYPE:
            napi_create_array(pCallbackInfo->env_, &result);
            napi_create_string_utf8(pCallbackInfo->env_, INVALID_DEVICE_ID.c_str(), NAPI_AUTO_LENGTH, &value);
            napi_set_element(pCallbackInfo->env_, result, 0, value);
            break;
        default:
            result = NapiGetNull(pCallbackInfo->env_);
            break;
    }
    return result;
}

napi_value DeregisterObserver(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    HILOGI("argc: %{public}zu", argc);
    if (argc == ARGS_SIZE_ONE) {
        std::string type;
        ParseString(env, type, argv[PARAM0]);
        RemoveObserver(type);
    } else if (argc == expectedArgsCount + 1) {
        NapiSppClient::Off(env, info);
    } else {
        NAPI_ASSERT(env, argc == expectedArgsCount, "Requires 2 argument.");
        std::string type;
        ParseString(env, type, argv[PARAM0]);

        std::shared_ptr<BluetoothCallbackInfo> pCallbackInfo = std::make_shared<BluetoothCallbackInfo>();
        pCallbackInfo->env_ = env;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valueType);
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &pCallbackInfo->callback_);
        if (pCallbackInfo != nullptr) {
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(pCallbackInfo->env_, &undefined);
            int32_t typeNum = 0;
            if (registerTypeMap.find(type) != registerTypeMap.end()) {
                typeNum = registerTypeMap[type];
            }
            napi_value result = CallbackObserver(typeNum, pCallbackInfo);
            napi_get_reference_value(pCallbackInfo->env_, pCallbackInfo->callback_, &callback);
            napi_call_function(pCallbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
        }

        RemoveObserver(type);
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> GetObserver()
{
    return g_Observer;
}

const sysBLEMap &GetSysBLEObserver()
{
    return g_sysBLEObserver;
}

bool GetDescriptorVectorFromJS(napi_env env, napi_value object, vector<GattDescriptor>& descriptors)
{
    size_t idx = 0;
    bool hasElement = false;
    napi_has_element(env, object, idx, &hasElement);

    while (hasElement) {
        napi_value result = nullptr;
        napi_create_object(env, &result);
        napi_get_element(env, object, idx, &result);
        GattDescriptor* descriptor = GetDescriptorFromJS(env, result, nullptr, nullptr);
        if (descriptor == nullptr) {
            HILOGE("descriptor is nullptr");
            return false;
        }
        descriptors.push_back(*descriptor);
        delete descriptor;
        idx++;
        napi_has_element(env, object, idx, &hasElement);
    }
    HILOGI("descriptors size: %{public}zu", descriptors.size());
    return true;
}

GattDescriptor* GetDescriptorFromJS(napi_env env, napi_value object, std::shared_ptr<GattServer> server,
    std::shared_ptr<GattClient> client)
{
    string serviceUuid;
    string characteristicUuid;
    string descriptorUuid;
    uint8_t *descriptorValue = nullptr;
    size_t descriptorValueSize = 0;

    napi_value propertyNameValue = nullptr;
    napi_value value = nullptr;

    napi_create_string_utf8(env, "serviceUuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool parseServUuid = ParseString(env, serviceUuid, value);
    if (!parseServUuid || (!regex_match(serviceUuid, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }
    HILOGI("serviceUuid is %{public}s", serviceUuid.c_str());

    napi_create_string_utf8(env, "characteristicUuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool parseCharacUuid = ParseString(env, characteristicUuid, value);
    if (!parseCharacUuid || (!regex_match(characteristicUuid, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }
    HILOGI("characteristicUuid is %{public}s", characteristicUuid.c_str());

    napi_create_string_utf8(env, "descriptorUuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool parseDescUuid = ParseString(env, descriptorUuid, value);
    if (!parseDescUuid || (!regex_match(descriptorUuid, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }
    HILOGI("descriptorUuid is %{public}s", descriptorUuid.c_str());

    GattDescriptor* descriptor = nullptr;
    GattCharacteristic* characteristic = nullptr;
    std::optional<std::reference_wrapper<GattService>> service = nullopt;

    if (server == nullptr && client == nullptr) {
        descriptor = new GattDescriptor(UUID::FromString(descriptorUuid),
            GattCharacteristic::Permission::READABLE | GattCharacteristic::Permission::WRITEABLE);
    } else {
        if (server != nullptr) {
            service = server->GetService(UUID::FromString(serviceUuid), true);
            if (service == std::nullopt) {
                service = server->GetService(UUID::FromString(serviceUuid), false);
            }
        } else if (client != nullptr) {
            service = client->GetService(UUID::FromString(serviceUuid));
        }

        if (service == std::nullopt) {
            return nullptr;
        } else {
            characteristic = service->get().GetCharacteristic(UUID::FromString(characteristicUuid));
        }

        if (characteristic == nullptr) {
            return nullptr;
        } else {
            descriptor = characteristic->GetDescriptor(UUID::FromString(descriptorUuid));
        }
    }

    if (descriptor == nullptr) {
        HILOGE("descriptor is nullptr");
        return nullptr;
    }

    napi_create_string_utf8(env, "descriptorValue", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    if (!ParseArrayBuffer(env, &descriptorValue, descriptorValueSize, value)) {
        HILOGE("ParseArrayBuffer failed");
        return nullptr;
    } else {
        HILOGI("descriptorValue is %{public}d", descriptorValue[0]);
    }
    descriptor->SetValue(descriptorValue, descriptorValueSize);

    return descriptor;
}

ServerResponse GetServerResponseFromJS(napi_env env, napi_value object)
{
    ServerResponse serverResponse;
    napi_value propertyNameValue = nullptr;
    napi_value value = nullptr;
    uint8_t *values = nullptr;
    size_t valuesSize = 0;

    napi_create_string_utf8(env, "deviceId", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseString(env, serverResponse.deviceId, value);

    napi_create_string_utf8(env, "transId", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseInt32(env, serverResponse.transId, value);

    napi_create_string_utf8(env, "status", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseInt32(env, serverResponse.status, value);
    HILOGI("deviceId: %{public}s, transId: %{public}d, status: %{public}d",
        GetEncryptAddr(serverResponse.deviceId).c_str(), serverResponse.transId, serverResponse.status);

    napi_create_string_utf8(env, "offset", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseInt32(env, serverResponse.offset, value);

    napi_create_string_utf8(env, "value", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    if (!ParseArrayBuffer(env, &values, valuesSize, value)) {
        HILOGE("ParseArrayBuffer failed");
    }
    serverResponse.SetValue(values, valuesSize);
    return serverResponse;
}

std::shared_ptr<SppOption> GetSppOptionFromJS(napi_env env, napi_value object)
{
    std::shared_ptr<SppOption> sppOption = std::make_shared<SppOption>();
    napi_value propertyNameValue = nullptr;
    napi_value value = nullptr;

    napi_create_string_utf8(env, "uuid", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    bool isSuccess = ParseString(env, sppOption->uuid_, value);
    if (!isSuccess || (!regex_match(sppOption->uuid_, uuidRegex))) {
        HILOGE("Parse UUID faild.");
        return nullptr;
    }
    HILOGI("uuid is %{public}s", sppOption->uuid_.c_str());

    napi_create_string_utf8(env, "secure", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseBool(env, sppOption->secure_, value);
    HILOGI("secure is %{public}d", sppOption->secure_);

    int type = 0;
    napi_create_string_utf8(env, "type", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_get_property(env, object, propertyNameValue, &value);
    ParseInt32(env, type, value);
    sppOption->type_ = SppSocketType(type);
    HILOGI("uuid: %{public}s, secure: %{public}d, type: %{public}d",
        sppOption->uuid_.c_str(), sppOption->secure_, sppOption->type_);
    return sppOption;
}

void SetGattClientDeviceId(const std::string &deviceId)
{
    deviceAddr = deviceId;
}

std::string GetGattClientDeviceId()
{
    return deviceAddr;
}

void SetRssiValueCallbackInfo(std::shared_ptr<GattGetRssiValueCallbackInfo> &callback)
{
    callbackInfo = callback;
}

std::shared_ptr<GattGetRssiValueCallbackInfo> GetRssiValueCallbackInfo()
{
    return callbackInfo;
}

int GetProfileConnectionState(int state)
{
    int32_t profileConnectionState = ProfileConnectionState::STATE_DISCONNECTED;
    switch (state) {
        case static_cast<int32_t>(BTConnectState::CONNECTING):
            HILOGD("STATE_CONNECTING(1)");
            profileConnectionState = ProfileConnectionState::STATE_CONNECTING;
            break;
        case static_cast<int32_t>(BTConnectState::CONNECTED):
            HILOGD("STATE_CONNECTED(2)");
            profileConnectionState = ProfileConnectionState::STATE_CONNECTED;
            break;
        case static_cast<int32_t>(BTConnectState::DISCONNECTING):
            HILOGD("STATE_DISCONNECTING(3)");
            profileConnectionState = ProfileConnectionState::STATE_DISCONNECTING;
            break;
        case static_cast<int32_t>(BTConnectState::DISCONNECTED):
            HILOGD("STATE_DISCONNECTED(0)");
            profileConnectionState = ProfileConnectionState::STATE_DISCONNECTED;
            break;
        default:
            break;
    }
    return profileConnectionState;
}

uint32_t GetProfileId(int profile)
{
    uint32_t profileId = 0;
    switch (profile) {
        case static_cast<int32_t>(ProfileId::PROFILE_A2DP_SINK):
            HILOGD("PROFILE_ID_A2DP_SINK");
            profileId = PROFILE_ID_A2DP_SINK;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_A2DP_SOURCE):
            HILOGD("PROFILE_ID_A2DP_SRC");
            profileId = PROFILE_ID_A2DP_SRC;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_AVRCP_CT):
            HILOGD("PROFILE_ID_AVRCP_CT");
            profileId = PROFILE_ID_AVRCP_CT;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_AVRCP_TG):
            HILOGD("PROFILE_ID_AVRCP_TG");
            profileId = PROFILE_ID_AVRCP_TG;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_HANDS_FREE_AUDIO_GATEWAY):
            HILOGD("PROFILE_ID_HFP_AG");
            profileId = PROFILE_ID_HFP_AG;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_HANDS_FREE_UNIT):
            HILOGD("PROFILE_ID_HFP_HF");
            profileId = PROFILE_ID_HFP_HF;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_PBAP_CLIENT):
            HILOGD("PROFILE_ID_PBAP_PCE");
            profileId = PROFILE_ID_PBAP_PCE;
            break;
        case static_cast<int32_t>(ProfileId::PROFILE_PBAP_SERVER):
            HILOGD("PROFILE_ID_PBAP_PSE");
            profileId = PROFILE_ID_PBAP_PSE;
            break;
        default:
            break;
    }
    return profileId;
}

int GetScoConnectionState(int state)
{
    int32_t scoState = ScoState::SCO_DISCONNECTED;
    switch (state) {
        case static_cast<int32_t>(HfpScoConnectState::SCO_CONNECTING):
            HILOGD("SCO_CONNECTING(1)");
            scoState = ScoState::SCO_CONNECTING;
            break;
        case static_cast<int32_t>(HfpScoConnectState::SCO_CONNECTED):
            HILOGD("SCO_CONNECTED(3)");
            scoState = ScoState::SCO_CONNECTED;
            break;
        case static_cast<int32_t>(HfpScoConnectState::SCO_DISCONNECTING):
            HILOGD("SCO_DISCONNECTING(2)");
            scoState = ScoState::SCO_DISCONNECTING;
            break;
        case static_cast<int32_t>(HfpScoConnectState::SCO_DISCONNECTED):
            HILOGD("SCO_DISCONNECTED(0)");
            scoState = ScoState::SCO_DISCONNECTED;
            break;
        default:
            break;
    }
    return scoState;
}

void SetCurrentAppOperate(const bool &isCurrentApp)
{
    isCurrentAppOperate.store(isCurrentApp);
}

bool GetCurrentAppOperate()
{
    return isCurrentAppOperate.load();
}

void RegisterSysBLEObserver(
    const std::shared_ptr<BluetoothCallbackInfo> &info, int32_t callbackIndex, const std::string &type)
{
    if (callbackIndex >= ARGS_SIZE_THREE) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_sysBLEObserverMutex);
    HILOGI("type: %{public}s, index: %{public}d", type.c_str(), callbackIndex);
    g_sysBLEObserver[type][callbackIndex] = info;
}

void UnregisterSysBLEObserver(const std::string &type)
{
    std::lock_guard<std::mutex> lock(g_sysBLEObserverMutex);
    auto itor = g_sysBLEObserver.find(type);
    if (itor != g_sysBLEObserver.end()) {
        g_sysBLEObserver.erase(itor);
    }
}
}  // namespace Bluetooth
}  // namespace OHOS
